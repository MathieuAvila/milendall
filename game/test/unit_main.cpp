#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch_amalgamated.hpp"

#include "common.hxx"

struct ConsoleInit {
    ConsoleInit()
    {
        set_level(level::debug);
    }
};

ConsoleInit consoleInit;