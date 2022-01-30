#include "impl_room_node_portal_register.hxx"
#include "common.hxx"
#include "level_exception.hxx"

static auto console = getConsole("impl_room_node_portal_register");

/*
static void dump(ImplRoomNodePortalRegister *reg)
{
    for (auto const & g: reg->map) {
        console->info("Dump {} {} => {}", g.first.gate, g.first.connect, (void*)g.second);
    }
}*/

void ImplRoomNodePortalRegister::registerPortal(GateIdentifier gate, RoomNode* room_node)
{
    if (map.count(gate) != 0) {
        throw LevelException("Register already contains gate: " + to_string(gate));
    }
    console->info("Register({}): {}, {}, at {}", (void*)this, gate.gate, gate.connect, (void*)room_node);
    map[gate] = room_node;
    //dump(this);
}

RoomNode* ImplRoomNodePortalRegister::getPortal(GateIdentifier gate)
{
    //dump(this);
    if (map.count(gate) == 0) {
        throw LevelException("Register hs no gate: " + to_string(gate));
    }
    return map[gate];
}
