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

std::string vec3_to_string(glm::vec3 v)
{
    return string("[ ") + to_string(v.x) + " , "  + to_string(v.y) + " , "  + to_string(v.z) + " ]" ;
}

std::string vec4_to_string(glm::vec4 v)
{
   return string("[ ") + to_string(v.x) + " , "  + to_string(v.y) + " , " + to_string(v.z)  + " , " + to_string(v.w) + " ]" ;
}

glm::vec4 getPlaneEquation(glm::vec3 p, glm::vec3 n)
{
    n = normalize(n);
    return glm::vec4(n.x, n.y, n.z, -glm::dot(p,n));
}

float pointPlaneProjection(glm::vec4 plane, glm::vec3 point)
{
    return glm::dot(plane, glm::vec4(point.x, point.y, point.z, 1.0));
}

glm::vec4 positionToVec4(glm::vec3 p)
{
    return glm::vec4(p.x, p.y, p.z, 1.0);
}

glm::vec4 vectorToVec4(glm::vec3 v)
{
   return glm::vec4(v.x, v.y, v.z, 0.0);
}