
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include "level.hxx"

#include <memory>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;


// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl_init.hxx"

using namespace glm;

#include "common.hxx"

int main( void )
{
    set_level(level::debug);
    auto fl = FileLibrary();
    milendall_gl_init(fl);
    milendall_gl_close();

	return 0;
}

