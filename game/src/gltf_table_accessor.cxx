#include "common.hxx"
#include "gltf_table_accessor.hxx"
#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("json");

nlohmann::json& jsonGetElementByName(nlohmann::json& file, std::string element)
{
    if (!file.contains(element)) {
        console->error("JSON element has no element:'{}'. Will throw", element);
        throw GltfException(std::string("No such element:") + element);
    }
    return file[element];
}

nlohmann::json& jsonGetElementByIndex(nlohmann::json& file, std::string element, int index)
{
    auto array_element = jsonGetElementByName(file, element);
    if (array_element.size() <= index) {
        console->error("JSON sub element '{}' has not element with index:'{}'. Max is '{}'. Will throw",
                        element, index, array_element.size());
        throw GltfException(std::string("Index out of range (") + std::to_string(index) + ") for element:" + element);
    }
    return file[element][index]; //[index];
}
