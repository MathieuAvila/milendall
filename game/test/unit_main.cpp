#include "common.hxx"

#include <GL/glew.h>

#include <stdint.h>

void glewGenerateMipmap(uint32_t target){};
PFNGLGENERATEMIPMAPPROC __glewGenerateMipmap = glewGenerateMipmap;

struct ConsoleInit {
    ConsoleInit()
    {
        set_level(level::debug);
    }
};

ConsoleInit consoleInit;