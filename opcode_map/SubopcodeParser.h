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

    template<typename... Ts, typename = std::enable_if_t<is_variant_alternative<map_pointer_types, member_ptr_t<Ts...>>>>
    void choose_member_impl(int opcode, int subopcode, Ts... parameters);

    template<typename... Ts>
    void print_invalid_types_error_message(int opcode, int subopcode, Ts... parameters);

    template<typename... Ts>
    void print_invalid_opcode_subopcode_pair_message(int opcode, int subopcode, Ts... parameters);

    inline static const std::multimap<std::pair<int, int>, map_pointer_types> opcode_map
    {
        { {1, 2}, &SubopcodeParser::mf1 },
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

        for (auto variant_iter = variant_range.first; variant_iter != variant_range.second; ++variant_iter)
        {
            try
            {
                auto member_pointer = std::get<member_ptr_t<Ts...>>(variant_iter->second); // Throws bad_variant_access upon failure
                std::invoke(member_pointer, this, parameters...);

                // If we've gotten here, it means that one of the members of the map has arguments which match the types
                // of the matched parameters. Return early to indicate success.
                return;
            }
            catch (const std::bad_variant_access&)
            {
                // Don't print an error message yet. If multiple opcode/subopcode pairs are in the map, then
                // all but one of them will cause a bad variant access. We only want to print an error message
                // if all of them cause a bad variant access because this means that the types passed parameters do not match
                // the parameter lists of any of the member pointers in the map for that opcode/subopcode pair.
            }
        }

        // If we've gotten here, it means we didn't return early, which can only happen if the passed parameters
        // don't match any of the members corresponding to the passed opcode/subopcode pair.
        // Issue an error message to indicate this failure. 
        print_invalid_types_error_message(opcode, subopcode, parameters...);
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

    std::vector<std::string> type_names{ pretty_print_type_name_v<Ts>... };
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