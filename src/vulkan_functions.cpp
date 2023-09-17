#include <vulkan_functions.hpp>

#include <dlfcn.h>

auto vulkan_functions::make() -> std::expected<vulkan_functions, std::string_view> {
    auto ret = vulkan_functions{};

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

    ret.vkGetInstanceProcAddr = ret.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

    if (ret.vkGetInstanceProcAddr == nullptr) {
        return std::unexpected{"failed to load vkGetInstanceProcAddr"};
    }

    return ret;
}

auto vulkan_functions::load_instanceless_functions() -> std::expected<usize, std::string_view> {
    auto loaded = 0uz;

#pragma push_macro("FACTORY")
#define FACTORY(_x)                                                                                                 \
    this->_x = reinterpret_cast<PFN_##_x>(this->vkGetInstanceProcAddr(nullptr, #_x));                               \
    if (this->_x == nullptr) {                                                                                      \
        return std::unexpected{"failed to load instanceless vulkan function " #_x};                                 \
    }                                                                                                               \
    spdlog::debug("instanceless function \"" #_x "\" is at: 0x{:08X}", reinterpret_cast<std::uintptr_t>(this->_x)); \
    loaded++

    FACTORY(vkEnumerateInstanceExtensionProperties);
    FACTORY(vkEnumerateInstanceLayerProperties);
    FACTORY(vkEnumerateInstanceVersion);
    FACTORY(vkCreateInstance);

#pragma pop_macro("FACTORY")

    spdlog::debug("loaded {} instanceless functions", loaded);
    return loaded;
}

auto vulkan_functions::load_instance_functions(VkInstance instance) -> std::expected<usize, std::string_view> {
    auto loaded = 0uz;
    spdlog::debug("loading instanced functions with an instance at: 0x{:08X}", reinterpret_cast<std::uintptr_t>(instance));

#pragma push_macro("FACTORY_CRITICAL")
#define FACTORY_CRITICAL(_x)                                                                                     \
    this->_x = reinterpret_cast<PFN_##_x>(this->vkGetInstanceProcAddr(instance, #_x));                           \
    if (this->_x == nullptr) {                                                                                   \
        return std::unexpected{"failed to load instanced vulkan function " #_x};                                 \
    }                                                                                                            \
    spdlog::debug("instanced function \"" #_x "\" is at: 0x{:08X}", reinterpret_cast<std::uintptr_t>(this->_x)); \
    loaded++

#pragma push_macro("FACTORY")
#define FACTORY(_x)                                                                                                  \
    this->_x = reinterpret_cast<PFN_##_x>(this->vkGetInstanceProcAddr(instance, #_x));                               \
    if (this->_x == nullptr) {                                                                                       \
        spdlog::warn("instanced function \"" #_x "\" could not be loaded");                                          \
    } else {                                                                                                         \
        loaded++;                                                                                                    \
        spdlog::debug("instanced function \"" #_x "\" is at: 0x{:08X}", reinterpret_cast<std::uintptr_t>(this->_x)); \
    }                                                                                                                \
    (void)0

    FACTORY_CRITICAL(vkDestroyInstance);

    FACTORY(vkEnumeratePhysicalDevices);
    FACTORY(vkGetPhysicalDeviceFeatures);
    FACTORY(vkGetPhysicalDeviceFeatures2);
    FACTORY(vkGetPhysicalDeviceFeatures2KHR);
    FACTORY(vkGetPhysicalDeviceFormatProperties);
    FACTORY(vkGetPhysicalDeviceImageFormatProperties);
    FACTORY(vkGetPhysicalDeviceProperties);
    FACTORY(vkGetPhysicalDeviceProperties2);
    FACTORY(vkGetPhysicalDeviceQueueFamilyProperties);
    FACTORY(vkGetPhysicalDeviceQueueFamilyProperties2);
    FACTORY(vkGetPhysicalDeviceMemoryProperties);
    FACTORY(vkGetPhysicalDeviceFormatProperties2);
    FACTORY(vkGetPhysicalDeviceMemoryProperties2);

    FACTORY(vkGetDeviceProcAddr);
    FACTORY(vkCreateDevice);
    FACTORY(vkEnumerateDeviceExtensionProperties);

    /*FACTORY(vkDestroySurfaceKHR);
    FACTORY(vkGetPhysicalDeviceSurfaceSupportKHR);
    FACTORY(vkGetPhysicalDeviceSurfaceFormatsKHR);
    FACTORY(vkGetPhysicalDeviceSurfacePresentModesKHR);*/

    FACTORY_CRITICAL(vkCreateDebugUtilsMessengerEXT);
    FACTORY_CRITICAL(vkDestroyDebugUtilsMessengerEXT);

#pragma pop_macro("FACTORY")
#pragma pop_macro("FACTORY_CRITICAL")

    spdlog::debug("loaded {} instanced functions", loaded);
    return loaded;
}

auto vulkan_functions::open_dl(const char* lib_name) -> std::optional<dl_type> {
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

void vulkan_functions::close_dl(dl_type library) {
#if defined(__linux__) || defined(__APPLE__)
    dlclose(library);
#elifdef _WIN32
    FreeLibrary(library);
#endif
}

auto vulkan_functions::get_dl_symbol_impl(const char* symbol_name) const -> void* {
#if defined(__linux__) || defined(__APPLE__)
    return reinterpret_cast<void*>(dlsym(m_dl, symbol_name));
#elifdef _WIN32
    return reinterpret_cast<void*>(GetPRocAddress(m_dl, symbol_name));
#endif
}
