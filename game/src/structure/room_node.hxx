#pragma once

#include <memory>
#include <set>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
#include "face_hard.hxx"
#include "structure_object_type.hxx"
#include "trigger.hxx"

#include <gtest/gtest_prod.h>

#include "gl_init.hxx"

#include "room_node_gravity.hxx"
#include "gate_id.hxx"

struct DrawContext;
class Script;
struct IRoomNodePortalRegister;
class Room;
class IObjectLoader;

struct RoomNode : public GltfNode
{

    struct FacePortal
    {
        /** What it points at */
        GateIdentifier gate;

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
    std::unique_ptr<RoomNodeGravity> gravity;
    std::list<Trigger> triggers;
    Room* room;
    std::string room_name;
    GltfNodeInstanceIface* node_instance;
    IRoomNodePortalRegister* portal_register;

    RoomNode(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        IRoomNodePortalRegister* _portal_register,
        Script* _roomScript,
        const std::string& room_name,
        Room* room,
        StatesList* _states_list = nullptr,
        IObjectLoader* _object_loader = nullptr);

    /** internal method to check if a gate is drawable, and return new draw context.
     * exposed for testability
     */
    bool checkDrawGate(
        GltfNodeInstanceIface * currentNodeInstance,
        const DrawContext& currentDrawContext,
        const FacePortal& portal,
        const FaceList::Face& face,
        DrawContext& newDrawContext,
        Room*& newRoom) const;

    /** specialized recursive draw for portals */
    void draw(GltfNodeInstanceIface * nodeInstance, DrawContext& roomContext);

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
            GateIdentifier& gate,
            glm::vec3& changePoint,
            float& distance,
            std::string& dstRoom,
            RoomNode*& dstRoomNode,
            GltfNodeInstanceIface *& dstNodeInstance
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

    void setInstance(GltfNodeInstanceIface* _node_instance);
};