#include "level.hxx"

#include <memory>

int main(int argc, char** argv)
{
    auto fl = FileLibrary();
    Level* level = new Level(fl.getRoot());

    return 0;
}