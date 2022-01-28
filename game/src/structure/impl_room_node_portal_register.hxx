#pragma once

#include <map>

#include "iface_room_node_portal_register.hxx"
#include "gate_id.hxx"

/** The real implementation */
struct ImplRoomNodePortalRegister: public IRoomNodePortalRegister
{

    std::map<GateIdentifier, RoomNode*> map;

    virtual void registerPortal(GateIdentifier gate, RoomNode* room_node) override;
    virtual RoomNode* getPortal(GateIdentifier gate) override;
    virtual ~ImplRoomNodePortalRegister() = default;
};
