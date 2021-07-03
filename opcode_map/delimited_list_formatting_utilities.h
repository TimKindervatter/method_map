#pragma once

#include "compile_time_join.h"

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


template<const std::string_view& Delimiter, const std::string_view& First, const std::string_view&... Rest>
struct delimited_list
{
private:
    template<const std::string_view& D, const std::string_view&... R>
    constexpr static std::string_view impl()
    {
        if constexpr (sizeof...(Rest) > 0)
        {
            return join_v<First, D, delimited_list<D, R...>::value>;
        }
        else
        {
            return First;
        }
    }

public:
    constexpr static inline std::string_view value = impl<Delimiter, Rest...>();
};