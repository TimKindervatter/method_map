#pragma once

#include <variant>

template<typename T> struct is_variant : std::false_type {};

template<typename ...Args>
struct is_variant<std::variant<Args...>> : std::true_type {};

template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;


template <typename, typename>
constexpr bool is_variant_alternative = false;

template <typename... Ts, typename T>
constexpr bool is_variant_alternative<std::variant<Ts...>, T> = (std::is_convertible_v<T, Ts> || ...);