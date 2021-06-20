#include "SubopcodeParser.h"

SubopcodeParser::SubopcodeParser()
{
	// MSVC is apparently a buggy piece of shit; it won't correctly print the pretty-printed string for std::vector<std::string> 
	// unless we do the following:
	pretty_print_type_name_v<std::variant_alternative_t<4, SubopcodeParser::map_pointer_types>>;

	// Maybe this is forcing it to instantiate the pretty_print type trait? It should be doing that anyway. GCC does it. Fuck Microsoft.
}

void SubopcodeParser::choose_member(int opcode, int subopcode)
{
	choose_member_impl(opcode, subopcode);
}

void SubopcodeParser::choose_member(int opcode, int subopcode, const char* parameters)
{
	choose_member_impl(opcode, subopcode, parameters);
}

void SubopcodeParser::choose_member(int opcode, int subopcode, int parameters)
{
	choose_member_impl(opcode, subopcode, parameters);
}

void SubopcodeParser::choose_member(int opcode, int subopcode, const std::vector<int>& parameters)
{
	choose_member_impl(opcode, subopcode, parameters);
}

void SubopcodeParser::choose_member(int opcode, int subopcode, const std::vector<std::string>& parameters)
{
	choose_member_impl(opcode, subopcode, parameters);
}

void SubopcodeParser::choose_member(int opcode, int subopcode, double d, const std::string& s)
{
	choose_member_impl(opcode, subopcode, d, s);
}