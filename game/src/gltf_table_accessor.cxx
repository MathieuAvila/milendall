#include "gltf_table_accessor.hxx"

#include "gltf_exception.hxx"

nlohmann::json& gltfGetElementByIndex(nlohmann::json& file, std::string element, int index)
{
    if (!file.contains(element))
        throw GltfException(std::string("No such element:") + element);
    auto array_element = file[element];
    if (array_element.size() <= index)
        throw GltfException(std::string("Index out of range (") + std::to_string(index) + ") for element:" + element);
    return file[element][index]; //[index];
}
