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
    using member_ptr_t = void (SubopcodeParser::*)(Ts...); //std::function<void(SubopcodeParser*, Ts...)>;

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

    inline static const std::map<std::pair<int, int>, map_pointer_types> opcode_map
    {
        { {1, 2}, &SubopcodeParser::mf1 },
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
    try
    {
        map_pointer_types variant = opcode_map.at({ opcode, subopcode });
        auto member_pointer = std::get<member_ptr_t<Ts...>>(variant);
        std::invoke(member_pointer, this, parameters...);
    }
    catch (std::out_of_range&)
    {
        print_opcode_signature(opcode, subopcode, parameters...);
        std::cout << "Error: Invalid Opcode/Subopcode pair.\n\n";
    }
    catch (std::bad_variant_access&)
    {
        print_opcode_signature(opcode, subopcode, parameters...);

        std::vector<std::string> type_names{ pretty_print_type_name_v<Ts>... };
        std::cout << "Error: Invalid argument types. The types passed were: " << type_names << ".\n";

        std::cout << "Argument types must match one of the following function signatures:\n";
        print_variant_types<map_pointer_types>();
        std::cout << "\n";
    }
}

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<int>> opcode_map<int>
    //{
    //    { {1, 2}, & SubopcodeParser::mf1 }
    //};

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<const char*>> opcode_map<const char*>
    //{
    //    { {2, 4}, & SubopcodeParser::mf2 }
    //};

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<>> opcode_map<>
    //{
    //    { {3, 2}, & SubopcodeParser::mf3 }
    //};

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<std::vector<int>>> opcode_map<std::vector<int>>
    //{
    //    { {4, 1}, & SubopcodeParser::mf4 }
    //};

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<std::vector<std::string>>> opcode_map<std::vector<std::string>>
    //{
    //    { {5, 3}, & SubopcodeParser::mf5 }
    //};

    //template<>
    //inline const std::map<std::pair<int, int>, member_ptr_t<double, std::string>> opcode_map<double, std::string>
    //{
    //    { {1, 1}, & SubopcodeParser::mf6 }
    //};


