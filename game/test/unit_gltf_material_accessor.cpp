#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"

using namespace nlohmann;
using namespace std;

TEST(GLTF_MATERIAL, READ_MATERIAL ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto material_uri = fl.getRoot().getSubPath("material/valid_3_3_3.gltf");
    auto json_content = json::parse(material_uri.readStringContent());

    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    auto accessor = materialLibrary->getMaterialAccessor(material_uri.getDirPath(), json_content);

}




