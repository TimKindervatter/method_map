#include "SubopcodeParser.h"

SubopcodeParser::SubopcodeParser()
{
	// Nevermind, it was a static initialization order fiasco problem. Fixed by constructing all strings at compile time.
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