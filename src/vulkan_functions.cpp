#include <vulkan_functions.hpp>

#include <dlfcn.h>

auto vk_dynamic_loader::make() -> std::expected<vk_dynamic_loader, std::string_view> {
    auto ret = vk_dynamic_loader{};

    for (auto names = get_candidate_library_names(); const auto* name : names) {
        spdlog::debug("attempting to open shared object: {}", name);
        if (const auto res = open_dl(name); !res) {
            continue;
        } else {
            ret.m_dl = *res;
            break;
        }
    }

    if (ret.m_dl == dl_type{}) {
        return std::unexpected{"failed to open the vulkan shared object"};
    }

    spdlog::debug("the vulkan shared object is at 0x{:08X}", reinterpret_cast<std::uintptr_t>(ret.m_dl));

    ret.vkGetInstanceProcAddr = ret.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

    if (ret.vkGetInstanceProcAddr == nullptr) {
        return std::unexpected{"failed to load vkGetInstanceProcAddr"};
    }

    spdlog::debug("vkGetInstanceProcAddr is at 0x{:08X}", reinterpret_cast<std::uintptr_t>(ret.vkGetInstanceProcAddr));

    return ret;
}

auto vk_dynamic_loader::open_dl(const char* lib_name) -> std::optional<dl_type> {
#if defined(__linux__) || defined(__APPLE__)
    auto* res = dlopen(lib_name, RTLD_NOW | RTLD_LOCAL);
#elifdef _WIN32
    auto res = LoadLibrary(TEXT("vulkan-1.dll"));
#endif

    if (res == dl_type{}) {
        return std::nullopt;
    }

    return res;
}

void vk_dynamic_loader::close_dl(dl_type library) {
#if defined(__linux__) || defined(__APPLE__)
    dlclose(library);
#elifdef _WIN32
    FreeLibrary(library);
#endif
}

auto vk_dynamic_loader::get_dl_symbol_impl(const char* symbol_name) const -> void* {
#if defined(__linux__) || defined(__APPLE__)
    return reinterpret_cast<void*>(dlsym(m_dl, symbol_name));
#elifdef _WIN32
    return reinterpret_cast<void*>(GetPRocAddress(m_dl, symbol_name));
#endif
}
