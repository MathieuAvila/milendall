#pragma once

#include <memory>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"

#include <gtest/gtest_prod.h>

struct RoomNode : public GltfNode
{

    struct FacePortal
    {
        std::string connect[2]; // in / out
        FaceList face; // Should contain 1 face only

        FacePortal(std::shared_ptr<PointsBlock>, std::unique_ptr<GltfDataAccessorIFace::DataBlock>, nlohmann::json& json);
    };

    std::shared_ptr<PointsBlock> points;
    std::list<FacePortal> portals;

    public:

    RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor);
};

class Room : public GltfModel
{

    protected:

        virtual void parseApplicationData(nlohmann::json& json);


    public:

        /** build the room from the directory path */
        Room(GltfMaterialLibraryIfacePtr materialLibrary, FileLibrary::UriReference& ref);

    FRIEND_TEST(Room, LoadLevel2Rooms1Gate_Room1);
};


