#pragma once

#include "variant_type_traits.h"
#include "delimited_list_formatting_utilities.h"

#include <string>
#include <iostream>
#include <vector>
#include <functional>

class SubopcodeParser;

template<typename T>
struct pretty_print_type_name
{
    inline static std::string value = typeid(T).name();
};

template<typename T>
inline static std::string pretty_print_type_name_v = pretty_print_type_name<T>::value;

template<>
struct pretty_print_type_name<void>
{
    inline static std::string value = "void";
};

template<>
struct pretty_print_type_name<std::string>
{
    inline static std::string value = "std::string";
};

template<typename T>
struct pretty_print_type_name<std::vector<T>>
{
    inline static std::string value = "std::vector<" + pretty_print_type_name_v<T> + ">";
};

template<typename T>
struct pretty_print_type_name<std::function<T>>
{
    inline static std::string value = "std::function<" + pretty_print_type_name_v<T> + ">";
};

template<>
struct pretty_print_type_name<SubopcodeParser>
{
    inline static std::string value = "SubopcodeParser";
};

template<typename R, typename Object, typename... Ts>
struct pretty_print_type_name<R(Object::*)(Ts...)>
{
    inline static std::string value = pretty_print_type_name_v<R> +" (" + pretty_print_type_name_v<Object> +"::*)(" + format_as_comma_separated_list(pretty_print_type_name_v<Ts>...) + ")";
};

template<typename R, typename Object>
struct pretty_print_type_name<R(Object::*)()>
{
    inline static std::string value = pretty_print_type_name_v<R> +" (" + pretty_print_type_name_v<Object> +"::*)" + "(void)";
};


template<typename Variant, typename T, T... index>
void print_variant_types_impl(std::integer_sequence<T, index...> sequence)
{
    static_assert(is_variant_v<Variant>, "The supplied type must be a variant.");
    ((std::cout << "\n\t" << pretty_print_type_name_v<std::variant_alternative_t<index, Variant>> << '\n'), ...);
    std::cout << "\n";
}


template<typename Variant>
void print_variant_types()
{
    static_assert(is_variant_v<Variant>, "The supplied type must be a variant.");
    auto indices = std::make_index_sequence<std::variant_size_v<Variant>>{};
    print_variant_types_impl<Variant>(indices);
}