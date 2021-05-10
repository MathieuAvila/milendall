#include "common.hxx"
#include "gltf_table_accessor.hxx"
#include "gltf_exception.hxx"

static auto console = spdlog::stdout_color_mt("json");

nlohmann::json& jsonGetElementByName(json& file, string element)
{
    if (!file.contains(element)) {
        console->error("JSON element has no sub-element:'{}'. Will throw", element);
        throw GltfException(string("No such element:") + element);
    }
    return file[element];
}

nlohmann::json& jsonGetElementByIndex(json& file, string element, int index)
{
    auto array_element = jsonGetElementByName(file, element);
    if (array_element.size() <= index) {
        console->error("JSON sub element '{}' has not element with index:'{}'. Max is '{}'. Will throw",
                        element, index, array_element.size());
        throw GltfException(string("Index out of range (") + to_string(index) + ") for element:" + element);
    }
    return file[element][index]; //[index];
}

void jsonExecuteIfElement(
    json& file,
    string element,
    function<void(json&)> exec)
{
    if (file.contains(element)) {
        auto elem = file[element];
        exec(elem);
    }
}

/**
 * Helper function to execute code for each element of node if it exists
 */
void jsonExecuteAllIfElement(
    json& file,
    string element,
    function<void(json&, int)> exec)
{
    if (file.contains(element)) {
        auto elem = file[element];
        int count = 0;
        for (auto sub: elem) {
            exec(sub, count++);
        }
    }
}
