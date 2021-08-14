#include <GL/glew.h>
#include "gltf_exception.hxx"

void gltf_check_gl_error(std::string context)
{
    if (auto err = glGetError()) {
        throw GltfException(
            context
        + " returned error="
        + std::to_string(err)
        + " == "
        + std::string((char*)gluErrorString(err)));
    }
}