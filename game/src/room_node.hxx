#pragma once

#include <memory>
#include <set>

#include "file_library.hxx"
#include "gltf_model.hxx"
#include "face_list.hxx"
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

    struct FaceHard
    {
        /** @brief Should contain 1 face only */
        FaceList face; // Should contain 1 face only

        FaceHard(
            std::shared_ptr<PointsBlock>,
            std::unique_ptr<GltfDataAccessorIFace::DataBlock>,
            nlohmann::json& json);
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
};
