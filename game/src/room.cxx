#include "room.hxx"

#include <string>

#include <iostream>

#include <nlohmann/json.hpp>
//using namespace nlohmann;

#include "gltf_frame.hxx"

Room::Room(FileLibrary::UriReference& ref)
{
    FileLibrary::UriReference room_ref = ref.getSubPath("/room.gltf");
    GltfModel* model = new GltfModel(room_ref);
    //model.reset(new GltfModel(room_ref));
};
