#include "impl_room_node_portal_register.hxx"


void ImplRoomNodePortalRegister::registerPortal(GateIdentifier gate, RoomNode* room_node)
{
    map[gate] = room_node;
}

RoomNode* ImplRoomNodePortalRegister::getPortal(GateIdentifier gate)
{
    return map[gate];
}
