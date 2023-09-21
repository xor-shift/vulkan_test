#pragma once

#include <stuff/core.hpp>
#include <stuff/expected.hpp>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.h>

#include <expected>
#include <string_view>

struct vk_dynamic_loader {
    static auto make() -> std::expected<vk_dynamic_loader, std::string_view>;

    ~vk_dynamic_loader() {
        close_dl(m_dl);
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;

    template<typename Fn>
    auto getProcAddress(const char* name) const -> Fn {
        auto* res = get_dl_symbol<Fn>(name);

        if (!res) {
            spdlog::warn("requested vulkan function \"{}\" returned nullptr", name);
        }

        return res;
    }

private:
#if defined(__linux__) || defined(__APPLE__)
    using dl_type = void*;
#elifdef _WIN32
    using dl_type = HMODULE;
#endif

    dl_type m_dl;

    static constexpr auto get_candidate_library_names() -> std::span<const char* const> {
#ifdef __linux__
        return (const char* const[]){
          "libvulkan.so.1",  //
          "libvulkan.so"     //
        };
#elifdef __APPLE__
        return (const char* const[]){
          "libvulkan.1.dylib",
          "libvulkan.dylib",
        };
#elifdef _WIN32
        return (const char* const[]){
          "vulkan-1.dll",
        };
#else
        return {};
#endif
    }

    static auto open_dl(const char* lib_name) -> std::optional<dl_type>;

    static void close_dl(dl_type library);

    template<typename Sym>
    auto get_dl_symbol(const char* symbol_name) const -> Sym {
        return reinterpret_cast<Sym>(get_dl_symbol_impl(symbol_name));
    }

    auto get_dl_symbol_impl(const char* symbol_name) const -> void*;
};
