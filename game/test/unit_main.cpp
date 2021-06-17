#include "common.hxx"

#include <GL/glew.h>

#include <stdint.h>

#include "gl_init.hxx"

void setMeshMatrix(glm::mat4 mat) {};

void glewGenerateMipmap(uint32_t target){};
PFNGLGENERATEMIPMAPPROC __glewGenerateMipmap = glewGenerateMipmap;

struct ConsoleInit {
    ConsoleInit()
    {
        set_level(level::debug);
    }
};

ConsoleInit consoleInit;