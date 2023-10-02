#include <vxl/vk/things/device.hpp>

namespace vxl::vk {

auto device_constraints::check_device(VkPhysicalDevice device, vulkan_functions const& vk_fns) const -> std::expected<std::optional<device_check_result>, error> {
    const auto device_extensions = TRYX(layer_extension_store<>::make(
      [&vk_fns, &device](u32* out_count, VkLayerProperties* out_layers) { return vk_fns.enumerate_device_layer_properties(device, out_count, out_layers); },
      [&vk_fns, &device](const char* name, u32* out_count, VkExtensionProperties* out_extensions) {
          return vk_fns.enumerate_device_extension_properties(device, name, out_count, out_extensions);
      }
    ));

    for (auto layers_range = device_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
        spdlog::debug("Device Layer #{}", layer_no);
        vxl::log(layer, 1);

        for (auto extensions_range = *device_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("\tExtension #{}", extension_no);
            vxl::log(extension, 2);
        }
    }

    for (auto extensions_range = device_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
        spdlog::debug("Device Extension #{}", extension_no);
        vxl::log(extension, 2);
    }

    auto available_layers = m_desired_layers | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.layer(ext.first); });
    auto layers_to_enable = ({
        auto res = device_extensions.pick_stuff(m_desired_layers, available_layers, "layer");

        if (!res) {
            return std::nullopt;
        }

        *res;
    });

    auto available_exts = m_desired_extensions | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.extension(ext.first); });
    auto exts_to_enable = ({
        auto res = device_extensions.pick_stuff(m_desired_extensions, available_exts, "extension");

        if (!res) {
            return std::nullopt;
        }

        *res;
    });

    return device_check_result{
      .m_layers_to_enable = std::move(layers_to_enable),
      .m_extensions_to_enable = std::move(exts_to_enable),
    };
}

auto device_constraints::check(VkPhysicalDevice device, vulkan_functions const& vk_fns) const -> std::expected<std::optional<device_check_result>, error> {
    // TODO?: queue merging

    auto ret = TRYX(TRYX(check_device(device, vk_fns)));

    const auto properties = vk_fns.get_physical_device_properties(device);

    const auto queue_family_properties = get_vector([&vk_fns, &device](u32* out_count, VkQueueFamilyProperties* out_properties) {
        vk_fns.get_physical_device_queue_family_properties(device, out_count, out_properties);
    });

    static constexpr auto sentinel_family = std::numeric_limits<u32>::max();

    auto&& occupancies = std::vector<usize>(queue_family_properties.size(), 0);
    auto&& assignments = std::vector<u32>(m_desired_queues.size(), sentinel_family);

    auto for_each_available_family = [this, &device, &vk_fns, &queue_family_properties, &occupancies, &assignments]<typename Fn>(Fn&& fn) -> std::expected<void, error> {
        for (auto [assignment, constraints] : std::views::zip(assignments, m_desired_queues) | std::views::filter([](auto p) { return p.first == sentinel_family; })) {
            for (auto [index, elem] : std::views::zip(queue_family_properties, occupancies) | std::views::enumerate) {
                auto& [props, occupancy] = elem;

                if (occupancy >= props.queueCount) {  // could use a std::views::filter but the line gets too long
                    continue;
                }

                const auto make_assignment = TRYX(std::invoke(std::forward<Fn>(fn), constraints, props, index));

                if (!make_assignment) {
                    continue;
                }

                ++occupancy;
                assignment = index;
            }
        }

        return {};
    };

    // first, get rid of exact matches
    TRYX(for_each_available_family([&device, &vk_fns](queue_constraints const& constraints, VkQueueFamilyProperties props, u32 family_index) -> std::expected<bool, error> {
        const auto pass_kind = constraints.check(props, family_index, device, vk_fns);
        return pass_kind == queue_constraints::pass_kind::barely_passes;
    }));

    // over-matched ones next
    TRYX(for_each_available_family([&device, &vk_fns](queue_constraints const& constraints, VkQueueFamilyProperties props, u32 family_index) -> std::expected<bool, error> {
        const auto pass_kind = constraints.check(props, family_index, device, vk_fns);
        return pass_kind == queue_constraints::pass_kind::overshoots;
    }));

    if (std::ranges::contains(assignments, sentinel_family)) {
        return std::nullopt;
    }

    ret.m_queue_families_for_queues = std::move(assignments);

    return ret;
}

device_things::device_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns)
    : m_vk_fns(std::move(vk_fns))
    , m_vk_instance(std::move(vk_instance)) {}

device_things::~device_things() {
    for (auto& queue : m_queues) {
        if (queue.command_buffer == nullptr) {
            continue;
        }

        const auto pool = std::ranges::find_if(m_command_pools, [&queue](auto p) { return p.first == queue.family_no; })->second;
        m_vk_fns->free_command_buffers(m_device, pool, 1, &queue.command_buffer);
        queue.command_buffer = nullptr;
    }

    for (auto& [family_index, pool] : m_command_pools) {
        if (pool == nullptr) {
            continue;
        }
        m_vk_fns->destroy_command_pool(m_device, pool);
        pool = nullptr;
    }

    if (m_device != nullptr) {
        m_vk_fns->destroy_device(m_device);
        m_device = nullptr;
    }
}

auto device_things::init(std::span<const VkPhysicalDeviceType> device_preference, device_constraints const& device_constraints) -> std::expected<void, error> {
    auto&& physical_devices = TRYX(error::from_vk(
      get_vector([this](u32* out_count, VkPhysicalDevice* out_devices) { return m_vk_fns->enumerate_physical_devices(*m_vk_instance, out_count, out_devices); }),
      "could not enumerate physical devices"
    ));

    std::ranges::sort(physical_devices, [this, device_preference](auto const& lhs, auto const& rhs) {
        auto lhs_it = std::ranges::find(device_preference, m_vk_fns->get_physical_device_properties(lhs).deviceType);
        auto lhs_idx = lhs_it == std::ranges::end(device_preference) ? std::numeric_limits<usize>::max() : std::distance(std::ranges::begin(device_preference), lhs_it);

        auto rhs_it = std::ranges::find(device_preference, m_vk_fns->get_physical_device_properties(rhs).deviceType);
        auto rhs_idx = rhs_it == std::ranges::end(device_preference) ? std::numeric_limits<usize>::max() : std::distance(std::ranges::begin(device_preference), rhs_it);

        return lhs_idx < rhs_idx;
    });

    for (auto const& device : physical_devices) {
        const auto check_result = TRYX(device_constraints.check(device, *m_vk_fns));

        if (!check_result) {
            continue;
        }

        TRYX(real_init(device, *check_result));

        return {};
    }

    return std::unexpected{error::make("no suitable physical device was found")};
}

auto device_things::real_init(VkPhysicalDevice physical_device, device_check_result const& check_result) -> std::expected<void, error> {
    m_physical_device = physical_device;

    m_queues = std::vector<queue_information>(check_result.m_queue_families_for_queues.size());
    const auto [min_family_index, max_family_index] = std::ranges::minmax(check_result.m_queue_families_for_queues);

    auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
    auto command_buffer_sets = std::vector<std::vector<VkCommandBuffer>>{};

    for (u32 family_index = 0; family_index <= max_family_index; family_index++) {
        const auto num_queues_for_family = std::ranges::count(check_result.m_queue_families_for_queues, family_index);

        if (num_queues_for_family == 0) {
            continue;
        }

        // create family infos

        auto priorities = std::vector<float>(num_queues_for_family, 0.f);

        auto&& queue_create_info = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = family_index,
          .queueCount = static_cast<u32>(num_queues_for_family),
          .pQueuePriorities = priorities.data(),
        };

        queue_create_infos.emplace_back(queue_create_info);

        auto&& queue_iter = std::views::zip(m_queues, check_result.m_queue_families_for_queues)              //
                          | std::views::filter([family_index](auto p) { return p.second == family_index; })  //
                          | std::views::keys                                                                 //
                          | std::views::enumerate;

        for (auto [no, queue] : queue_iter) {
            queue.family_no = family_index;
            queue.queue_no = no;
        }
    }

    const auto device_create_info = VkDeviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<u32>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledLayerCount = static_cast<u32>(check_result.m_layers_to_enable.size()),
      .ppEnabledLayerNames = check_result.m_layers_to_enable.data(),
      .enabledExtensionCount = static_cast<u32>(check_result.m_extensions_to_enable.size()),
      .ppEnabledExtensionNames = check_result.m_extensions_to_enable.data(),
      .pEnabledFeatures = nullptr,
    };

    m_device = TRYX(error::from_vk(m_vk_fns->create_device(physical_device, &device_create_info), "could not create a VkDevice"));

    // command pools
    for (auto [vector_index, family_index] : std::views::iota(min_family_index) | std::views::take(max_family_index - min_family_index + 1) | std::views::enumerate) {
        const auto num_queues_for_family = queue_create_infos[vector_index].queueCount;

        const auto command_pool_create_info = VkCommandPoolCreateInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
          .queueFamilyIndex = family_index,
        };

        auto&& command_pool = TRYX(error::from_vk(m_vk_fns->create_command_pool(m_device, &command_pool_create_info), "could not create a VkCommandPool"));
        m_command_pools.emplace_back(family_index, command_pool);

        auto&& command_buffers = std::vector<VkCommandBuffer>(num_queues_for_family, nullptr);
        const auto command_buffer_allocate_info = VkCommandBufferAllocateInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          .pNext = nullptr,
          .commandPool = command_pool,
          .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          .commandBufferCount = static_cast<u32>(num_queues_for_family),
        };

        TRYX(error::from_vk(m_vk_fns->allocate_command_buffers(m_device, &command_buffer_allocate_info, command_buffers.data()), "could not create a vector of vk::CommandBuffer"));

        command_buffer_sets.emplace_back(std::move(command_buffers));
    }

    for (auto [n, queue] : m_queues | std::views::enumerate) {
        queue.queue_handle = m_vk_fns->get_device_queue(m_device, queue.family_no, queue.queue_no);
        queue.command_buffer = command_buffer_sets[n][queue.queue_no];
    }

    return {};
}

}  // namespace vxl::vk
