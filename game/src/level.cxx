#include "level.hxx"
#include "room.hxx"

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;

Level::Level(FileLibrary::UriReference ref)
{
    auto file_content = ref.readStringContent();
    //std::cout << file_content << std::endl;
    json j_level = json::parse(file_content);

    auto rooms = j_level.find("rooms");
    if (rooms != j_level.end()) {
        for (json::iterator room_it = rooms->begin(); room_it != rooms->end(); ++room_it) {
            auto room_id = room_it->find("room_id");
            std::cout << "detected room: " << *room_id << "\n";
            auto ref_room = ref.getDirPath().getSubPath(*room_id);
            auto room = std::make_shared<Room>(ref_room);
        }
    } else{
        std::cerr << "Rooms tag not found !" << std::endl;
    }
}
