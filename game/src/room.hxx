#pragma once

#include <memory>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"

#include <gtest/gtest_prod.h>

class Room;

struct RoomResolver
{
    virtual Room* getRoom(std::string) = 0;
    virtual ~RoomResolver() = default;
};

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
    RoomResolver* room_resolver;

    public:

    RoomNode(nlohmann::json& json, GltfDataAccessorIFace* data_accessor, RoomResolver* _room_resolver);

    /** specialized recursive draw for portals */
    void draw(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
};

/** @brief A room is both a Model (through inheritance) and an instance (through a class field) */
class Room : public GltfModel
{

    protected:

        virtual void parseApplicationData(nlohmann::json& json);

        /** my very own instance */
        std::unique_ptr<GltfInstance> instance;

        /**
         * Provides rooms when drawing needs to go through a portal.
         * Level defines this.
         * No ownership, should be valid for the life duration of the object.
         */
        RoomResolver* room_resolver;

    public:

        /** build the room from the directory path */
        Room(
            GltfMaterialLibraryIfacePtr materialLibrary,
            FileLibrary::UriReference& ref,
            RoomResolver*  _room_resolver = nullptr);

        /** @brief full draw: room itself (all nodes), objects, portals.
         * Pass position, direction, up of the room. Those are set in GL context
         * (caller must set them back)
        */
        void draw(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

        /** @brief apply transformation to instance
        */
        void applyTransform();

    FRIEND_TEST(Room, LoadLevel2Rooms1Gate_Room1);
};


