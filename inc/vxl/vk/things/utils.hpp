#pragma once

#include <stuff/core.hpp>

#include <limits>
#include <span>

namespace vxl::vk::detail {

struct always_true_constraint {
    static constexpr bool operator()(auto const&) noexcept {
        return true;
    }
};

template<typename T, usize N, typename U, usize M, typename ConstraintFn = always_true_constraint, typename TransformFn = std::identity>
constexpr auto pick_with_preference_and_constraint(
  std::span<T, N> range_mut,
  std::span<const U, M> preferences,
  TransformFn&& pre_transform = {},
  ConstraintFn&& constraint = {}
) -> std::optional<T> {
    if constexpr (N == 0) {
        return std::nullopt;
    }

    auto index_of = [preferences, &pre_transform](T const& v) -> usize {
        auto const& transformed = std::invoke(pre_transform, v);
        const auto it = std::ranges::find(preferences, transformed);
        const auto dist = std::distance(preferences.begin(), it);

        return dist;
    };

    if constexpr (std::is_same_v<ConstraintFn, always_true_constraint>) {
        auto range = range_mut | std::views::transform(index_of);
        auto min = std::ranges::min_element(range);
        return range_mut[std::distance(std::ranges::begin(range), min)];
    }

    std::ranges::sort(range_mut, [&index_of](T const& lhs, T const& rhs) {
        return index_of(lhs) < index_of(rhs);
    });

    for (auto& v : range_mut) {
        if (!constraint(v)) {
            continue;
        }

        return v;
    }

    return std::nullopt;
}

}  // namespace vxl::vk::detail
