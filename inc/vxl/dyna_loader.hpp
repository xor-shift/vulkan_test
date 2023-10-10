#pragma once

#include <stuff/core.hpp>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.h>

#include <expected>
#include <string_view>

namespace vxl {

struct dynamic_loader {
    auto init(std::span<const char* const> names) -> std::expected<void, std::string_view>;

    dynamic_loader() = default;
    dynamic_loader(dynamic_loader const&) = delete;
    auto operator=(dynamic_loader const&) -> dynamic_loader& = delete;

    dynamic_loader(dynamic_loader&& other) noexcept
        : m_dl(other.m_dl) {
        other.m_dl = nullptr;
    }

    auto operator=(dynamic_loader&& other) noexcept -> dynamic_loader& {
        m_dl = other.m_dl;
        other.m_dl = nullptr;
        return *this;
    }

    ~dynamic_loader() { close_dl(m_dl); }

    template<typename Sym>
    auto get_dl_symbol(const char* symbol_name) const -> Sym {
        return reinterpret_cast<Sym>(get_dl_symbol(symbol_name));
    }

    auto get_dl_symbol(const char* symbol_name) const -> void*;

    constexpr operator bool() const { return m_dl != dl_type{}; }

private:
#if defined(__linux__) || defined(__APPLE__)
    using dl_type = void*;
#elifdef _WIN32
    using dl_type = HMODULE;
#endif

    dl_type m_dl = dl_type{};

    static auto open_dl(const char* lib_name) -> std::optional<dl_type>;

    static void close_dl(dl_type library);
};

}
