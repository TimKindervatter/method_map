#pragma once

#include "pretty_print_type_traits.h"
#include "variant_type_traits.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

template<typename First, typename... Rest>
std::string format_as_comma_separated_list_impl(First first, Rest&&... rest)
{
    std::stringstream ss;
    std::string delimiter = ", ";

    if constexpr (std::is_pointer_v<First>)
    {
        if (first == nullptr)
            ss << "nullptr" << delimiter;
        else
            ss << first << delimiter;
    }
    else
    {
        ss << first << delimiter;
        if constexpr (sizeof...(rest) > 0)
            ss << format_as_comma_separated_list_impl(rest...);
    }

    return ss.str();
}


template<typename... Args>
std::string format_as_comma_separated_list(Args&&... parameters)
{
    std::stringstream ss;
    std::string delimiter = ", ";

    ss << format_as_comma_separated_list_impl(parameters...);
    std::string delimited_list = ss.str();
    delimited_list = delimited_list.substr(0, delimited_list.size() - delimiter.size());

    return delimited_list;
}


template<typename T>
std::string format_as_comma_separated_list(const std::vector<T>& v)
{
    std::string delimiter = ", ";
    std::stringstream ss;

    auto last_elem_iter = std::end(v) - 1;

    for (auto it = std::begin(v); it != last_elem_iter; ++it)
        ss << *it << delimiter;

    ss << *(last_elem_iter);

    return ss.str();
}


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "{" << format_as_comma_separated_list(v) << "}";

    return os;
}


template<typename... Ts>
void print_opcode_signature(int opcode, int subopcode, Ts... parameters)
{
    std::cout << "Opcode: " << opcode << ", SubOpcode: " << opcode << ", Parameters: ";
    if constexpr (sizeof...(parameters) == 0)
        std::cout << "None";
    else
    {
        std::cout << format_as_comma_separated_list(parameters...);
    }
    std::cout << "\n";
}