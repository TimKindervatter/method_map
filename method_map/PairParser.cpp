#include "PairParser.h"

PairParser::PairParser()
{
}

void PairParser::choose_member(int first, int second)
{
	choose_member_impl(first, second);
}

void PairParser::choose_member(int first, int second, const char* parameters)
{
	choose_member_impl(first, second, parameters);
}

void PairParser::choose_member(int first, int second, int parameters)
{
	choose_member_impl(first, second, parameters);
}

void PairParser::choose_member(int first, int second, const std::vector<int>& parameters)
{
	choose_member_impl(first, second, parameters);
}

void PairParser::choose_member(int first, int second, const std::vector<std::string>& parameters)
{
	choose_member_impl(first, second, parameters);
}

void PairParser::choose_member(int first, int second, double d, const std::string& s)
{
	choose_member_impl(first, second, d, s);
}