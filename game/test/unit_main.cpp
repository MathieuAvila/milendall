#include "common.hxx"

struct ConsoleInit {
    ConsoleInit()
    {
        set_level(level::debug);
    }
};

ConsoleInit consoleInit;