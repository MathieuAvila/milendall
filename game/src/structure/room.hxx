#pragma once

#include <memory>
#include <set>
#include <list>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
#include "point_of_view.hxx"
#include "room_node.hxx"
#include "gravity_information.hxx"
#include "room_animation.hxx"
#include "structure_object_type.hxx"
#include "gate_id.hxx"

#include <gtest/gtest_prod.h>

struct RoomResolver;
struct DrawContext;
class Script;
class StatesList;
struct IRoomNodePortalRegister;


/** @brief to be able to load script before the GltfModel
 * so that RoomNodes children can get a reference to the script */
class RoomScriptLoader {
        std::unique_ptr<Script> loadedScript;
    public:
        RoomScriptLoader(FileLibrary::UriReference& roomRef);
        Script* getScript();
        virtual ~RoomScriptLoader();
};

/** The interface provided to register portals, this is provided to the room. */
struct IRoomPortalRegister
{
    virtual void registerPortal(const Room* room, const RoomNode* roomNode, const std::string gateId, const std::string connectId) = 0;
    virtual ~IRoomPortalRegister() = default;
};


/** @brief A room is both a Model (through inheritance) and an instance (through a class field) */
class Room : private RoomScriptLoader, public GltfModel
{

    protected:

        /** @brief My name, as defined by Level */
        std::string room_name;

        /** @brief my very own instance */
        std::unique_ptr<GltfInstance> instance;

        /**
         * @brief Provides rooms when drawing needs to go through a portal.
         * Level defines this.
         * No ownership, should be valid for the life duration of the object.
         */
        RoomResolver* room_resolver;

        /** @brief Holds the states list provided by Level.
         * Level defines this.
         * No ownership, should be valid for the life duration of the object.
         */
        StatesList* states_list;

        IRoomNodePortalRegister* portal_register;

        /** @brief List of animation code specific to the room. It relies on
         * the animations defined by the model and specific application data
         * to manage it
         * WARNING: not to be confused with animations from the model. This
         * is a wrapper*/
        std::list<std::unique_ptr<RoomAnimation>> room_animations;

        /** @brief inherited from GltfModel to load specific data */
        virtual void parseApplicationData(nlohmann::json& json) override;

        /** @brief Overload recursive method to draw things, will draw portals */
        virtual void draw(GltfInstance*, int, void* context = nullptr) override;

        /** @brief Full context information for drawing. Used by Room for bootstrapping, and RoomNode for recursing.
         * Draws all: room itself (all nodes), objects, portals.
         * Pass position, direction, up of the room. Those are set in GL context
         * (caller must set them back)
         * recurse_level is used to indicate depth of tree of gates. Clip to a given
         * limit as it may not be useful to go too deep.*/
        void draw(DrawContext& draw_context);

        /** @brief local gravity computation for a given node, that manages recursion. see ::getGravity for
         * the entry-point
         */
        bool _getGravity(
            unsigned int instance,
            glm::vec3 position, glm::vec3 speed, float weight, float radius,
            float total_time,
            GravityInformation& result);

    public:

        /** build the room from the directory path */
        Room(
            std::string _room_name,
            GltfMaterialLibraryIfacePtr materialLibrary,
            FileLibrary::UriReference& ref,
            RoomResolver*  _room_resolver = nullptr,
            IRoomNodePortalRegister* portal_register = nullptr,
            StatesList* _states_list = nullptr);

        /** @brief full draw entry point
        */
        void draw(PointOfView pov);

        /** @brief apply transformation to instance
        */
        void updateRoom(float delta_time);

        /** @brief get an updated POV if it goes through a portal.
         * @param origin is the original POV, ie. start of trajectory
         * @param destination is the end of trajectory, all other parameters are kept
         * @param changePoint is filled with the point where the portal is reached, if a portal is reached
         * @param distance is filled with the distance, if a portal is reached
         * @param newPovChangePoint is filled with the position of where portal is reached, in NEW POV, if portal is reached
         * @param newPovDestination is filled with the position of destination, in NEW POV, if portal is reached
         * @param gate is the ID of the portal that was reached
         * @return True if a portal was reached
          */
        bool getDestinationPov(
            const PointOfView& origin,
            const PointOfView& destination,
            glm::vec3& changePoint,
            float& distance,
            PointOfView& newPovChangePoint,
            PointOfView& newPovDestination,
            GateIdentifier& gate
            );

        /** @brief Check if a wall is reached
         * @param origin is the original position, ie. start of trajectory
         * @param destination is the end of trajectory
         * @param radius is the size of the object
         * @param hitPoint is filled with the point where the wall is reached
         * @param normal is filled with the normal of the wall
         * @param distance is filled with the distance, if a wall is reached
         * @param face face that was hit
         * @return True if a wall was reached
        */
        bool isWallReached(
            const glm::vec3& origin,
            const glm::vec3& destination,
            float radius,
            glm::vec3& hitPoint,
            glm::vec3& normal,
            float& distance,
            FaceHard*& face
            );

        /** @brief compute gravity values
         * @param position position in room
         * @param speed current speed
         * @param weight obvious
         * @param radius obvious
         * @param total_time
         * @return gravity information
        */
        GravityInformation getGravity(
            glm::vec3 position, glm::vec3 speed, float weight, float radius,
            float total_time);

        /** @brief Apply trigger changes. See SpaceResolver and Level */
        void applyTrigger(
            const glm::vec3& previous_position,
            const glm::vec3& next_position,
            const STRUCTURE_OBJECT_TYPE object_type,
            const bool activated) const;

        virtual ~Room() = default;

        friend struct RoomNode;

    FRIEND_TEST(RoomTest, LoadLevel2Rooms1Gate_Room1);
    FRIEND_TEST(RoomTest, animations);
};


