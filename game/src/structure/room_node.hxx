#pragma once

#include <memory>
#include <set>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
#include "face_hard.hxx"
#include "room_resolver.hxx"
#include "structure_object_type.hxx"
#include "trigger.hxx"

#include <gtest/gtest_prod.h>

#include "gl_init.hxx"

#include "room_node_gravity.hxx"

struct DrawContext;
class Script;

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
        std::unique_ptr<FaceList> face; // Should contain 1 face only

        /** @brief triangles, ready to be passed to GL */
        std::unique_ptr<TrianglesBufferInfo> portal_triangles;

        FacePortal(
            std::shared_ptr<PointsBlock>,
            std::unique_ptr<GltfDataAccessorIFace::DataBlock>,
            nlohmann::json& json,
            const std::string& room_name);
    };

    std::shared_ptr<PointsBlock> points;
    std::list<FacePortal> portals;
    std::list<FaceHard> walls;
    RoomResolver* room_resolver;
    std::unique_ptr<RoomNodeGravity> gravity;
    std::list<Trigger> triggers;

    RoomNode(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        RoomResolver* _room_resolver,
        Script* _roomScript,
        const std::string& room_name,
        StatesList* _states_list = nullptr);

    /** internal method to check if a gate is drawable, and return new draw context.
     * exposed for testability
     */
    bool checkDrawGate(
        GltfNodeInstanceIface * currentNodeInstance,
        const DrawContext& currentDrawContext,
        const FacePortal& portal,
        const FaceList::Face& face,
        DrawContext& newDrawContext) const;

    /** specialized recursive draw for portals */
    void draw(GltfNodeInstanceIface * nodeInstance, DrawContext& roomContext);

    /** Accessor to the list of portals it connects. */
    std::list<GateIdentifier> getPortalNameList();

    /** @brief compute destination point if given portal is crossed
     * @param origin start position in Room referential
     * @param destination end position in Room referential
     * @param gate Which gate must be checked
     * @param changePoint At which poistion is portal is crossed
     * @param distance At which distance from the start the portal is crossed
     * @return true if portal is crossed
     */
    bool checkPortalCrossing(
            const glm::vec3& origin,
            const glm::vec3& destination,
            std::string& roomTarget, GateIdentifier& gate,
            glm::vec3& changePoint,
            float& distance
            );

    /** @brief Check if a wall is reached
    * @param origin is the original position, ie. start of trajectory
    * @param destination is the end of trajectory
    * @param radius is the size of the object
    * @param hitPoint is filled with the point where the wall is reached
    * @param normal is filled with the normal of the wall
    * @param distance is filled with the distance, if a wall is reached
    * @param face definition of the wall that was reached.
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
     * @param position position in local reference frame
     * @param speed speed vector in local reference frame
     * @param weight obvious
     * @param radius obvious
     * @param total_time total elapsed time
     * @param[out] gravity filled if returns true
     * @return TRUE if gravity is changed, FALSE otherwise
     */
    bool getGravity(glm::vec3 position, glm::vec3 speed, float weight, float radius, float total_time, GravityInformation& gravity);

    /** @brief Apply trigger changes. See SpaceResolver and Level */
    void applyTrigger(
            const glm::vec3& previous_position,
            const glm::vec3& next_position,
            const STRUCTURE_OBJECT_TYPE object_type,
            const bool activated) const;
};
