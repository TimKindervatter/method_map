#pragma once

#include "variant_type_traits.h"
#include "delimited_list_formatting_utilities.h"
#include "compile_time_join.h"

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <string_view>

class PairParser;


constexpr inline static std::string_view delimiter = ", ";

template<typename T>
struct pretty_print_type_name
{
    constexpr inline static std::string_view value = "UNHANDLED TYPE";
};

template<typename T>
constexpr inline static std::string_view pretty_print_type_name_v = pretty_print_type_name<T>::value;

template<>
struct pretty_print_type_name<void>
{
    constexpr inline static std::string_view value = "void";
};

template<>
struct pretty_print_type_name<int>
{
    constexpr inline static std::string_view value = "int";
};

template<>
struct pretty_print_type_name<double>
{
    constexpr inline static std::string_view value = "double";
};

template<>
struct pretty_print_type_name<const char*>
{
    constexpr inline static std::string_view value = "const char*";
};

template<>
struct pretty_print_type_name<std::string>
{
    constexpr inline static std::string_view value = "std::string";
};

template<typename T>
struct pretty_print_type_name<std::vector<T>>
{
    constexpr inline static std::string_view std_vector = "std::vector<";
    constexpr inline static std::string_view closing_bracket = ">";
    constexpr inline static std::string_view value = join_v<std_vector, pretty_print_type_name_v<T>, closing_bracket>;
};

template<>
struct pretty_print_type_name<PairParser>
{
    constexpr inline static std::string_view value = "PairParser";
};

template<typename R, typename Object, typename... Ts>
struct pretty_print_type_name<R(Object::*)(Ts...)>
{
    constexpr inline static std::string_view open_paren = " (";
    constexpr inline static std::string_view pointer_to_member = "::*)(";
    constexpr inline static std::string_view close_paren = ")";
    constexpr inline static std::string_view value = join_v<pretty_print_type_name_v<R>, open_paren, pretty_print_type_name_v<Object>, pointer_to_member, delimited_list<delimiter, pretty_print_type_name_v<Ts>...>::value, close_paren>;
};

template<typename R, typename Object>
struct pretty_print_type_name<R(Object::*)()>
{
    constexpr inline static std::string_view open_paren = " (";
    constexpr inline static std::string_view pointer_to_member = "::*)";
    constexpr inline static std::string_view void_arg = "(void)";
    constexpr inline static std::string_view value = join_v<pretty_print_type_name_v<R>, open_paren, pretty_print_type_name_v<Object>, pointer_to_member, void_arg>;;
};
