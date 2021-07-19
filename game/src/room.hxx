#pragma once

#include <memory>
#include <set>

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

struct DrawContext;

struct GateIdentifier {
    std::string gate;
    bool from;
    bool operator< (const GateIdentifier& b) const;
    bool operator== (const GateIdentifier& b) const;
};

struct RoomNode : public GltfNode
{

    struct FacePortal
    {
        /** @brief Which rooms does it connect. Order tells the direction of the normal */
        std::string connect[2]; // in / out

        /** If connect[0] == room_name, we're IN */
        bool in;

        /** @brief gate ID. Needed to differentiate gates, as 2 gates may connect the same rooms. */
        std::string gate;

        /** @brief Should contain 1 face only */
        FaceList face; // Should contain 1 face only

        FacePortal(
            std::shared_ptr<PointsBlock>,
            std::unique_ptr<GltfDataAccessorIFace::DataBlock>,
            nlohmann::json& json,
            const std::string& room_name);
    };

    std::shared_ptr<PointsBlock> points;
    std::list<FacePortal> portals;
    RoomResolver* room_resolver;

    RoomNode(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        RoomResolver* _room_resolver,
        const std::string& room_name);

    /** specialized recursive draw for portals */
    void draw(GltfNodeInstanceIface * nodeInstance, DrawContext& roomContext);

    /** Accessor to the list of portals it connects. */
    std::list<GateIdentifier> getPortalNameList();
};

/** @brief A room is both a Model (through inheritance) and an instance (through a class field) */
class Room : public GltfModel
{

    protected:

        /** My name, as defined by Level */
        std::string room_name;

        virtual void parseApplicationData(nlohmann::json& json) override;

        /** my very own instance */
        std::unique_ptr<GltfInstance> instance;

        /**
         * Provides rooms when drawing needs to go through a portal.
         * Level defines this.
         * No ownership, should be valid for the life duration of the object.
         */
        RoomResolver* room_resolver;

        /** Overload recursive method to draw things, will draw portals */
        virtual void draw(GltfInstance*, int, void* context = nullptr) override;

        /** @brief Mapping a list of portals and in/out to indices
         * WARNING: this is not a map, as there can be portals connecting a room to itself.
        */
        std::map<GateIdentifier, int> portalsIndices;

    public:

        /** build the room from the directory path */
        Room(
            std::string _room_name,
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

        /** get the list of handled portals */
        std::set<GateIdentifier> getGateNameList() const;

        /** get the objects for a given portal */
        std::pair<RoomNode*, GltfNodeInstanceIface*> getGateNode(const GateIdentifier& gate) const;

        virtual ~Room() = default;

    FRIEND_TEST(Room, LoadLevel2Rooms1Gate_Room1);
};


