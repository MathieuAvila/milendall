#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "gltf_model.hxx"

TEST_CASE( "Load GLTF model", "[gltf]" ) {

    auto fl = FileLibrary();
    fl.addRootFilesystem(std::filesystem::current_path().c_str() + std::string("/../game/test/sample"));
    auto ref = fl.getRoot().getSubPath("/cube/room_preview.gltf");
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();
    GltfModel* model = new GltfModel(materialLibrary, ref);

    REQUIRE(model != nullptr);
}
