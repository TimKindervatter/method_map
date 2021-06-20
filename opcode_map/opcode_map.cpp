#include "SubopcodeParser.h"

int main()
{
    SubopcodeParser a;
    a.choose_member(1, 2, 34);                      // Works, binds to choose_member(int, int, int)
    a.choose_member(1, 2, 3.1415926);               // Works, binds to choose_member(int, int, int) since double is convertible to int in last argument
    a.choose_member(1, 2, true);                    // Works, binds to choose_member(int, int, int) since bool is convertible to int in last argument

    a.choose_member(1, 2);                          // Works, binds to choose_member(int, int)

    a.choose_member(2, 4, "Hello");                 // Works, binds to choose_member(int, int, const char*)
    //a.choose_member(2, 4, std::string("World"));  // Compilation failure, no overload of choose_member takes std::string&&
    std::string s = "World";
    //a.choose_member(2, 4, s);                     // Compilation failure, no overload of choose_member takes std::string
    a.choose_member(2, 4, nullptr);                 // Works, binds to choose_member(int, int, const char*). Note that the member function that ultimately gets called must be able to handle a pointer argument with null value!

    a.choose_member(3, 2);                          // Works, binds to choose_member(int, int)

    a.choose_member(4, 1, std::vector<int>{1, 2, 3, 5, 7});  // Works, binds to choose_member(int, int, const std::vector<int>&)

    std::vector<std::string> vec = std::vector<std::string>{"H", "e", "l", "l", "o"};
    a.choose_member(5, 3, vec);  // Works, binds to choose_member(int, int, const std::vector<std::string>&)

    a.choose_member(6, 1, 3.1415926, "Hello");   // Works, binds to choose_member(int, int, double, const std::string&)

    a.choose_member(6, 3, 42);  // Fails, because opcode/subopcode pair (6, 3) does not correspond to an entry in the member pointer map. Fails gracefully at runtime and prints error message.
    a.choose_member(1, 2, "Hello");  // Fails, because the the only member pointer that is mapped to by the opcode/subopcode pair (1, 2) takes argument int, not const char*. Fails gracefully at runtime and prints error message.
    a.choose_member(1, 2, nullptr);  // Fails, because the overload const char Fails gracefully at runtime and prints error message.
    a.choose_member(1, 2, 3.1415926, "Hello"); // Fails, because opcode / subopcode pair(6, 3) does not correspond to an entry in the member pointer map.Fails gracefully at runtime and prints error message.

    a.choose_member(2, 4, 42);
    a.choose_member(2, 4, 3.1415926, "Hello");

    return 0;
}