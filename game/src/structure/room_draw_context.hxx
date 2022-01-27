#pragma once

#include "point_of_view.hxx"
#include "gl_init.hxx"

struct RoomNodePortalProvider
{
    virtual std::tuple<std::string,RoomNode*,GltfNodeInstanceIface*> getPortal(std::string gateId, std::string connectId) = 0;
    virtual ~RoomNodePortalProvider() = default;
};

struct DrawContext {
    PointOfView pov;
    RoomNodePortalProvider* portal_provider;
    int recurse_level;
    FboIndex fbo;
};
