#pragma once

#include "pretty_print_type_traits.h"
#include "variant_type_traits.h"
#include "delimited_list_formatting_utilities.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "{" << format_as_comma_separated_list(v) << "}";

    return os;
}


template<typename... Ts>
void print_opcode_signature(int opcode, int subopcode, Ts... parameters)
{
    std::cout << "Opcode: " << opcode << ", SubOpcode: " << subopcode << ", Parameters: ";
    if constexpr (sizeof...(parameters) == 0)
        std::cout << "None";
    else
    {
        std::cout << format_as_comma_separated_list(parameters...);
    }
    std::cout << "\n";
}


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