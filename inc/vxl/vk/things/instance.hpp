#pragma once

#include <vxl/vk/functions.hpp>
#include <vxl/vk/layer_ext_store.hpp>
#include <vxl/vk/loggers.hpp>

#include <cstdio>

namespace vxl::vk {

struct instance_things {
    instance_things(VkApplicationInfo app_info, std::shared_ptr<vulkan_functions> vk_fns);

    ~instance_things();

    instance_things(instance_things const&) = delete;
    instance_things(instance_things&&) = delete;

    auto init() -> std::expected<void, error>;

    operator VkInstance() const { return m_instance; }

    auto instance() const -> VkInstance { return m_instance; }

private:
    VkApplicationInfo m_app_info;
    std::shared_ptr<vulkan_functions> m_vk_fns;

    layer_extension_store<> m_instance_extensions;
    VkInstance m_instance;
};

}
