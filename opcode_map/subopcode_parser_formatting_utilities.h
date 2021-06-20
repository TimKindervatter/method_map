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