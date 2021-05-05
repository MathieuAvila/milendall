#include "gltf_table_accessor.hxx"

#include "gltf_exception.hxx"

nlohmann::json& gltfGetElementByName(nlohmann::json& file, std::string element)
{
    if (!file.contains(element))
        throw GltfException(std::string("No such element:") + element);
    return file[element];
}

nlohmann::json& gltfGetElementByIndex(nlohmann::json& file, std::string element, int index)
{
    auto array_element = gltfGetElementByName(file, element);
    if (array_element.size() <= index)
        throw GltfException(std::string("Index out of range (") + std::to_string(index) + ") for element:" + element);
    return file[element][index]; //[index];
}
