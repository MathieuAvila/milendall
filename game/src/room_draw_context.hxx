#pragma once

#include "point_of_view.hxx"
#include "gl_init.hxx"
#include "room_resolver.hxx"

struct DrawContext {
    PointOfView pov;
    RoomResolver* room_resolver;
    int recurse_level;
    FboIndex fbo;
};
