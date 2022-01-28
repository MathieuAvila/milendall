#pragma once

#include "gate_id.hxx"

struct RoomNode;

/** Provide this to the RoomNode at instantiation. This is its interface to register its portals */
struct IRoomNodePortalRegister
{
    virtual void registerPortal(GateIdentifier gate, RoomNode* room_node) = 0;
    virtual RoomNode* getPortal(GateIdentifier gate) = 0;
    virtual ~IRoomNodePortalRegister() = default;
};
