#include "impl_room_node_portal_register.hxx"
#include "common.hxx"

static auto console = getConsole("impl_room_node_portal_register");

static void dump(ImplRoomNodePortalRegister *reg)
{
    for (auto const & g: reg->map) {
        console->info("Dump {} {} => {}", g.first.gate, g.first.connect, (void*)g.second);
    }
}

void ImplRoomNodePortalRegister::registerPortal(GateIdentifier gate, RoomNode* room_node)
{
    console->info("Register({}): {}, {}, at {}", (void*)this, gate.gate, gate.connect, (void*)room_node);
    map[gate] = room_node;
    dump(this);
}

RoomNode* ImplRoomNodePortalRegister::getPortal(GateIdentifier gate)
{
    /*console->info("Get({}): {}, {}", (void*)this, gate.gate, gate.connect);
    dump(this);
    for (auto const & g: map) {
        console->info("Cmp {} {} <=> {} {}", g.first.gate, g.first.connect, gate.gate, gate.connect);
        if (g.first == gate) {
            console->info("Found");
            return g.second;
        }
    }*/
    return map[gate];
}
