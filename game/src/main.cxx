
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include "level.hxx"

#include <memory>
#include <iostream>

int main(int argc, char** argv)
{
    auto fl = FileLibrary();
    Level* level = new Level(fl.getRoot());
    std::cout << level;
    return 0;
}