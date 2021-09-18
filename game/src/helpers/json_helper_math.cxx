#include "common.hxx"
#include "json_helper_math.hxx"
#include "gltf_exception.hxx"

static auto console = getConsole("json_math");

glm::mat4x4 jsonGetMatrix4x4(nlohmann::json& e)
{
    if (!e.is_array())
        throw GltfException("JSON matrix 4x4 element is not an array");
    if (e.size() != 16)
        throw GltfException("JSON matrix 4x4 element has not size of 16");
    return glm::mat4x4(
        e[0].get<float>(), e[1].get<float>(), e[2].get<float>(), e[3].get<float>(),
        e[4].get<float>(), e[5].get<float>(), e[6].get<float>(), e[7].get<float>(),
        e[8].get<float>(), e[9].get<float>(), e[10].get<float>(),e[11].get<float>(),
        e[12].get<float>(),e[13].get<float>(),e[14].get<float>(),e[15].get<float>()
        );
}

glm::vec3 jsonGetVec3(nlohmann::json& e)
{
    if (!e.is_array())
        throw GltfException("JSON vec 3 element is not an array");
    if (e.size() != 3)
        throw GltfException("JSON vec 3 element has not size of 3");
    return glm::vec3(
        e[0].get<float>(), e[1].get<float>(), e[2].get<float>());
}

glm::vec4 jsonGetVec4(nlohmann::json& e)
{
    if (!e.is_array())
        throw GltfException("JSON vec 4 element is not an array");
    if (e.size() != 4)
        throw GltfException("JSON vec 4 element has not size of 4");
    return glm::vec4(e[0].get<float>(), e[1].get<float>(), e[2].get<float>(), e[3].get<float>());
}
