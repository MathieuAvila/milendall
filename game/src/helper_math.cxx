#include "common.hxx"
#include "helper_math.hxx"
#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("math");

std::string mat4x4_to_string(glm::mat4x4 mat)
{
    string result("[");
    for (auto i: {0,1,2,3}) {
        result+="\n[";
        for (auto j: {0,1,2,3}) {
            if (j!=0)
                (result+=", ");
            result+= to_string(mat[i][j]);
        }
        result+=']';
    }
    result+="\n]";
    return result;
}
