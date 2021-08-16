#pragma once

#include <memory>
#include <set>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
#include "point_of_view.hxx"
#include "room_node.hxx"

#include <gtest/gtest_prod.h>


struct RoomResolver;
struct DrawContext;

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

        /** @brief Full context information for drawing. Used by Room for bootstrapping, and RoomNode for recursing.
         * Draws all: room itself (all nodes), objects, portals.
         * Pass position, direction, up of the room. Those are set in GL context
         * (caller must set them back)
         * recurse_level is used to indicate depth of tree of gates. Clip to a given
         * limit as it may not be useful to go too deep.*/
        void draw(DrawContext& draw_context);

    public:

        /** build the room from the directory path */
        Room(
            std::string _room_name,
            GltfMaterialLibraryIfacePtr materialLibrary,
            FileLibrary::UriReference& ref,
            RoomResolver*  _room_resolver = nullptr);

        /** @brief full draw entry point
        */
        void draw(PointOfView pov);

        /** @brief apply transformation to instance
        */
        void applyTransform();

        /** get the list of handled portals */
        std::set<GateIdentifier> getGateNameList() const;

        /** get the objects for a given portal */
        std::pair<RoomNode*, GltfNodeInstanceIface*> getGateNode(const GateIdentifier& gate) const;

        virtual ~Room() = default;

        friend struct RoomNode;

    FRIEND_TEST(Room, LoadLevel2Rooms1Gate_Room1);
};


