#pragma once

#include "pretty_print_type_traits.h"
#include "variant_type_traits.h"
#include "parser_formatting_utilities.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <functional>
#include <sstream>
#include <variant>
#include <type_traits>

class PairParser
{
public:
    PairParser();

private:
    template<typename... Ts>
    using member_ptr_t = void (PairParser::*)(Ts...);

    using map_pointer_types = std::variant
    <
        member_ptr_t<int>,
        member_ptr_t<const char*>,
        member_ptr_t<>,
        member_ptr_t<std::vector<int>>,
        member_ptr_t<std::vector<std::string>>,
        member_ptr_t<double, std::string>
    >;

public:
    void choose_member(int first, int second);
    void choose_member(int first, int second, const char* parameters);
    void choose_member(int first, int second, int parameters);
    void choose_member(int first, int second, const std::vector<int>& parameters);
    void choose_member(int first, int second, const std::vector<std::string>& parameters);
    void choose_member(int first, int second, double d, const std::string& s);

private:
    void member_function_1(int i) { std::cout << "member_function_1(" << i << ")\n\n"; }
    void member_function_2(const char* s) { std::cout << "member_function_2(" << (s ? s : "nullptr") << ")\n\n"; }
    void member_function_3() { std::cout << "member_function_3()\n\n"; }
    void member_function_4(std::vector<int> v) { std::cout << "member_function_4(" << v << ")\n\n"; }
    void member_function_5(std::vector<std::string> v) { std::cout << "member_function_5(" << v << ")\n\n"; }
    void member_function_6(double d, std::string s) { std::cout << "member_function_6(" << d << ", " << s << ")\n\n"; }
    void member_function_7() { std::cout << "member_function_7()\n\n"; }
    void member_function_8(int i) { std::cout << "member_function_8(" << i << ")\n\n"; }

    template<typename... Ts, typename = std::enable_if_t<is_variant_alternative<map_pointer_types, member_ptr_t<Ts...>>>>
    void choose_member_impl(int first, int second, Ts... parameters);

    template<typename... Ts>
    void print_invalid_types_error_message(int first, int second, Ts... parameters);

    template<typename... Ts>
    void print_invalid_first_second_pair_message(int first, int second, Ts... parameters);

    inline static const std::multimap<std::pair<int, int>, map_pointer_types> first_map
    {
        { {1, 2}, &PairParser::member_function_1 },
        { {1, 2}, &PairParser::member_function_8 },
        { {1, 2}, &PairParser::member_function_7 },
        { {2, 4}, &PairParser::member_function_2 },
        { {3, 2}, &PairParser::member_function_3 },
        { {4, 1}, &PairParser::member_function_4 },
        { {5, 3}, &PairParser::member_function_5 },
        { {6, 1}, &PairParser::member_function_6 },
    };
};


template<typename... Ts, typename>
void PairParser::choose_member_impl(int first, int second, Ts... parameters)
{
    if constexpr (is_variant_alternative<map_pointer_types, member_ptr_t<Ts...>>)
    {
        // equal_range returns a pair of iterators denoting a list of all members that have equivalent keys. 
        // The first iterator points to the first element whose key is not less than the one passed to equal_range,
        // and the second iterator points to the first element whose key is greater than the one passed to equal_range.
        auto variant_range = first_map.equal_range({ first, second });

        if (variant_range.first == variant_range.second)
        {
            // If both iterators returned from equal_range are the same, it means that no keys
            // in the map were equivalent to the one passed to equal_range. 
            // That is, the passed pair has no corresponding values in the map.

            print_invalid_first_second_pair_message(first, second, parameters...);
            return;
        }

        bool matching_method_found = false;

        for (auto variant_iter = variant_range.first; variant_iter != variant_range.second; ++variant_iter)
        {
            auto current_variant_alternative = variant_iter->second;

            // Information on std::variant and std::visit: https://arne-mertz.de/2018/05/modern-c-features-stdvariant-and-stdvisit/
            
            // std::visit is essentially a switch statement that switches on each alternative of a std::variant.
            // The first argument is the visitor - a callable that polymorphically chooses its behavior depending 
            // on the type of the variant alternative that is passed.
            // 
            // In this case, we use a lambda that checks whether the current variant alternative's value is an element in the
            // first map. If so, it invokes that value (which is a member function pointer) 
            // with the parameters passed into choose_member_impl.

            std::visit(
                [&, this](auto member_pointer)
                {
                    using T = decltype(member_pointer);
                    if constexpr (std::is_same_v<T, member_ptr_t<Ts...>>)
                    {
                        std::invoke(member_pointer, this, parameters...);
                        matching_method_found = true;
                    }
                }, current_variant_alternative);
        }
            
        if (!matching_method_found)
        {
            // If we've gotten here, it means that none of the member functions corresponding 
            // to the passed pair take arguments whose types 
            // match Ts (i.e the types of the parameters passed to choose_member_impl). 
            // Issue an error message to indicate this failure. 
            print_invalid_types_error_message(first, second, parameters...);
        }
    }
    else
    {
        // Theoretically, this branch should never be taken, because SFINAE will prevent instantiation of an overload if
        // the passed parameters' types do not match any of the members of the map.
        // Just to be safe, keep this here as an extra precaution.
        print_invalid_types_error_message(first, second, parameters...);
    }
}


template<typename... Ts>
void PairParser::print_invalid_types_error_message(int first, int second, Ts... parameters)
{
    print_first_signature(first, second, parameters...);

    std::vector<std::string_view> type_names{ pretty_print_type_name_v<Ts>... };
    if constexpr (sizeof...(Ts) > 0)
        std::cout << "Error: Invalid argument types. The types passed were: " << type_names << ".\n";
    else
        std::cout << "Error: Invalid argument types. The types passed were: {void}.\n";

    std::cout << "Argument types must match one of the following function signatures:\n";

    auto variant_range = first_map.equal_range({ first, second });

    for (auto variant_iter = variant_range.first; variant_iter != variant_range.second; ++variant_iter)
    {
        print_variant_types(variant_iter->second);
    }

    std::cout << "\n";
}


template<typename... Ts>
void PairParser::print_invalid_first_second_pair_message(int first, int second, Ts... parameters)
{
    print_first_signature(first, second, parameters...);
    std::cout << "Error: Invalid pair.\n\n";
}