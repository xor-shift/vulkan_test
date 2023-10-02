#pragma once

#include <vxl/vk/error.hpp>
#include <vxl/vk/vector_helpers.hpp>

#include <stuff/core.hpp>

#include <spdlog/spdlog.h>

#include <optional>
#include <ranges>

namespace vxl::vk {

template<typename LayerType = VkLayerProperties, typename ExtensionType = VkExtensionProperties>
struct layer_extension_store {
    using layer_type = std::pair<LayerType, std::pmr::vector<ExtensionType>>;

    static auto make(auto&& layer_enumerator, auto&& extension_enumerator) -> std::expected<layer_extension_store, error> {
        auto ret = layer_extension_store{};

        auto layers = TRYX(error::from_vk(get_vector(layer_enumerator), "failed to enumerate instance layers"));

        ret.m_vk_instance_layers.reserve(layers.size());
        for (auto const& layer_props : layers) {
            const auto enumerator = [&extension_enumerator, &layer_props](u32* out_size, ExtensionType* out_extensions) {
                return extension_enumerator(layer_props.layerName, out_size, out_extensions);
            };

            auto&& layer_extensions = TRYX(error::from_vk(get_vector(enumerator), "failed to enumerate instance layer properties"));

            ret.m_vk_instance_layers.emplace_back(layer_props, std::move(layer_extensions));
        }

        auto const& layerless_extension_enumerator = [&extension_enumerator](u32* out_size, ExtensionType* out_extensions) {
            return extension_enumerator(nullptr, out_size, out_extensions);
        };

        ret.m_vk_instance_extensions = TRYX(error::from_vk(get_vector(layerless_extension_enumerator), "could not enumerate instance layer properties"));

        return ret;
    }

    constexpr auto layer(std::string_view layer_name) const -> std::optional<LayerType> {
        return layer_internal(layer_name).transform([](auto const& layer) { return layer.get().first; });
    }

    constexpr auto extension(std::string_view extension_name) const -> std::optional<ExtensionType> {
        auto iter = std::ranges::find_if(m_vk_instance_extensions, [extension_name](auto const& extension) { return extension_name == std::string_view(extension.extensionName); });

        if (iter == std::ranges::end(m_vk_instance_extensions)) {
            return std::nullopt;
        }

        return *iter;
    }

    constexpr auto extension(std::string_view extension_name, std::string_view layer_name) const -> std::optional<ExtensionType> {
        return layer_internal(layer_name).and_then([extension_name](auto const& layer) -> std::optional<ExtensionType> {
            auto iter = std::ranges::find_if(layer.get().second, [extension_name](auto const& layer) { return extension_name == std::string_view(layer.extensionName); });

            if (iter == std::ranges::end(layer.get().second)) {
                return std::nullopt;
            }

            return *iter;
        });
    }

    constexpr auto layers() const -> std::ranges::keys_view<std::pmr::vector<layer_type> const&> { return m_vk_instance_layers | std::views::keys; }

    constexpr auto extensions() const -> std::ranges::ref_view<const std::pmr::vector<ExtensionType>> { return m_vk_instance_extensions | std::views::all; }

    constexpr auto extensions(std::string_view layer_name) const -> std::optional<std::ranges::ref_view<const std::pmr::vector<ExtensionType>>> {
        return layer_internal(layer_name)  //
          .transform([](auto const& layer) { return layer.get().second | std::views::all; });
        //.value_or([] { return std::ranges::empty_view<ExtensionType>{}; });
    }

    static constexpr auto pick_stuff(auto&& desired, auto&& available, std::string_view name) -> std::expected<std::vector<const char*>, error> {
        static constexpr auto grouper = std::views::chunk_by([](auto const& lhs, auto const& rhs) { return lhs.second == rhs.second; });

        auto desired_groups = desired | grouper;
        const auto desired_group_count = std::ranges::distance(desired_groups);

        auto available_groups = available | grouper;
        const auto available_group_count = std::ranges::distance(available_groups);

        if (available_group_count != desired_group_count) {
            spdlog::error("not all desired {}s could be loaded, available_group_count ({}) != desired_group_count ({})", name, available_group_count, desired_group_count);

            spdlog::error("desired extensions (grouped):");
            for (auto const& [no, group] : desired_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            spdlog::error("available {}s (grouped):", name);
            for (auto const& [no, group] : available_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            return std::unexpected{error{}};
        }

        auto extension_names_range = available_groups | std::views::transform([](auto group) { return group | std::views::take(1); }) | std::views::join | std::views::keys;
        auto ret = std::vector<const char*>(std::ranges::distance(extension_names_range));
        std::ranges::copy(extension_names_range, ret.begin());

        spdlog::debug("enabling the following {}s:", name);
        for (auto const& [no, elem_name] : ret | std::views::enumerate) {
            spdlog::debug("\t{} #{}: {}", name, no, elem_name);
        }

        return ret;
    }

private:
    std::pmr::vector<layer_type> m_vk_instance_layers{};
    std::pmr::vector<ExtensionType> m_vk_instance_extensions{};

    constexpr auto layer_internal(std::string_view layer_name) const -> std::optional<std::reference_wrapper<const layer_type>> {
        auto it = std::ranges::find_if(m_vk_instance_layers, [layer_name](auto const& layer) { return layer_name == std::string_view(layer.first.layerName); });

        if (it == std::ranges::end(m_vk_instance_layers)) {
            return std::nullopt;
        }

        return *it;
    }
};

}
