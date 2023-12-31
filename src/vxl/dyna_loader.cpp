#include <vxl/dyna_loader.hpp>

#include <dlfcn.h>

namespace vxl {

auto dynamic_loader::init(std::span<const char* const> names) -> std::expected<void, std::string_view> {
    for (const auto* name : names) {
        spdlog::debug("attempting to open shared object: {}", name);
        if (const auto res = open_dl(name); !res) {
            continue;
        } else {
            m_dl = *res;
            break;
        }
    }

    if (m_dl == dl_type{}) {
        return std::unexpected{"failed to open the vulkan shared object"};
    }

    spdlog::debug("the loaded shared object is at 0x{:08X}", reinterpret_cast<std::uintptr_t>(m_dl));

    return {};
}

auto dynamic_loader::get_dl_symbol(const char* symbol_name) const -> void* {
#if defined(__linux__) || defined(__APPLE__)
    return reinterpret_cast<void*>(dlsym(m_dl, symbol_name));
#elifdef _WIN32
    return reinterpret_cast<void*>(GetPRocAddress(m_dl, symbol_name));
#endif
}

auto dynamic_loader::open_dl(const char* lib_name) -> std::optional<dl_type> {
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

void dynamic_loader::close_dl(dl_type library) {
    if (library == dl_type{}) {
        return;
    }

#if defined(__linux__) || defined(__APPLE__)
    dlclose(library);
#elifdef _WIN32
    FreeLibrary(library);
#endif
}

}
