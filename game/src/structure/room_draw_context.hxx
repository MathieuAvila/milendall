#pragma once

#include "point_of_view.hxx"
#include "gl_init.hxx"
#include "clipping_planes.hxx"

struct IRoomNodePortalRegister;

struct DrawContext {
    PointOfView pov;
    IRoomNodePortalRegister* portal_provider;
    int recurse_level;
    FboIndex fbo;
    ClippingPlanes clipping;
};
