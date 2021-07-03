#pragma once

#include "pretty_print_type_traits.h"
#include "variant_type_traits.h"
#include "subopcode_parser_formatting_utilities.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <functional>
#include <sstream>
#include <variant>
#include <type_traits>

class SubopcodeParser
{
public:
    SubopcodeParser();

private:
    template<typename... Ts>
    using member_ptr_t = void (SubopcodeParser::*)(Ts...);

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
    void choose_member(int opcode, int subopcode);
    void choose_member(int opcode, int subopcode, const char* parameters);
    void choose_member(int opcode, int subopcode, int parameters);
    void choose_member(int opcode, int subopcode, const std::vector<int>& parameters);
    void choose_member(int opcode, int subopcode, const std::vector<std::string>& parameters);
    void choose_member(int opcode, int subopcode, double d, const std::string& s);

private:
    void mf1(int i) { std::cout << "mf1(" << i << ")\n\n"; }
    void mf2(const char* s) { std::cout << "mf2(" << (s ? s : "nullptr") << ")\n\n"; }
    void mf3() { std::cout << "mf3()\n\n"; }
    void mf4(std::vector<int> v) { std::cout << "mf4(" << v << ")\n\n"; }
    void mf5(std::vector<std::string> v) { std::cout << "mf5(" << v << ")\n\n"; }
    void mf6(double d, std::string s) { std::cout << "mf6(" << d << ", " << s << ")\n\n"; }
    void mf7() { std::cout << "mf7()\n\n"; }
    void mf8(int i) { std::cout << "mf8(" << i << ")\n\n"; }

    template<typename... Ts, typename = std::enable_if_t<is_variant_alternative<map_pointer_types, member_ptr_t<Ts...>>>>
    void choose_member_impl(int opcode, int subopcode, Ts... parameters);

    template<typename... Ts>
    void print_invalid_types_error_message(int opcode, int subopcode, Ts... parameters);

    template<typename... Ts>
    void print_invalid_opcode_subopcode_pair_message(int opcode, int subopcode, Ts... parameters);

    inline static const std::multimap<std::pair<int, int>, map_pointer_types> opcode_map
    {
        { {1, 2}, &SubopcodeParser::mf1 },
        { {1, 2}, &SubopcodeParser::mf8 },
        { {1, 2}, &SubopcodeParser::mf7 },
        { {2, 4}, &SubopcodeParser::mf2 },
        { {3, 2}, &SubopcodeParser::mf3 },
        { {4, 1}, &SubopcodeParser::mf4 },
        { {5, 3}, &SubopcodeParser::mf5 },
        { {6, 1}, &SubopcodeParser::mf6 },
    };
};


template<typename... Ts, typename>
void SubopcodeParser::choose_member_impl(int opcode, int subopcode, Ts... parameters)
{
    if constexpr (is_variant_alternative<map_pointer_types, member_ptr_t<Ts...>>)
    {
        // equal_range returns a pair of iterators denoting a list of all members that have equivalent keys. 
        // The first iterator points to the first element whose key is not less than the one passed to equal_range,
        // and the second iterator points to the first element whose key is greater than the one passed to equal_range.
        auto variant_range = opcode_map.equal_range({ opcode, subopcode });

        if (variant_range.first == std::end(opcode_map) && variant_range.second == std::end(opcode_map))
        {
            // If both iterators returned from equal_range are the end iterator, it means that no keys
            // in the map were equivalent to the one passed to equal_range. 
            // That is, the passed opcode/subopcode pair has no corresponding values in the map.

            print_invalid_opcode_subopcode_pair_message(opcode, subopcode, parameters...);
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
            // opcode map. If so, it invokes that value (which is a member function pointer) 
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
            // to the passed opcode/subopcode pair take arguments whose types 
            // match Ts (i.e the types of the parameters passed to choose_member_impl). 
            // Issue an error message to indicate this failure. 
            print_invalid_types_error_message(opcode, subopcode, parameters...);
        }
    }
    else
    {
        // Theoretically, this branch should never be taken, because SFINAE will prevent instantiation of an overload if
        // the passed parameters' types do not match any of the members of the map.
        // Just to be safe, keep this here as an extra precaution.
        print_invalid_types_error_message(opcode, subopcode, parameters...);
    }
}


template<typename... Ts>
void SubopcodeParser::print_invalid_types_error_message(int opcode, int subopcode, Ts... parameters)
{
    print_opcode_signature(opcode, subopcode, parameters...);

    std::vector<std::string_view> type_names{ pretty_print_type_name_v<Ts>... };
    if constexpr (sizeof...(Ts) > 0)
        std::cout << "Error: Invalid argument types. The types passed were: " << type_names << ".\n";
    else
        std::cout << "Error: Invalid argument types. The types passed were: {void}.\n";

    std::cout << "Argument types must match one of the following function signatures:\n";

    auto variant_range = opcode_map.equal_range({ opcode, subopcode });

    for (auto variant_iter = variant_range.first; variant_iter != variant_range.second; ++variant_iter)
    {
        print_variant_types(variant_iter->second);
    }

    std::cout << "\n";
}


template<typename... Ts>
void SubopcodeParser::print_invalid_opcode_subopcode_pair_message(int opcode, int subopcode, Ts... parameters)
{
    print_opcode_signature(opcode, subopcode, parameters...);
    std::cout << "Error: Invalid Opcode/Subopcode pair.\n\n";
}