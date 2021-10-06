#pragma once

#include <memory>
#include <set>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
#include "face_hard.hxx"
#include "room_resolver.hxx"

#include <gtest/gtest_prod.h>

#include "gl_init.hxx"

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

    RoomNode(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        RoomResolver* _room_resolver,
        const std::string& room_name);

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
};
