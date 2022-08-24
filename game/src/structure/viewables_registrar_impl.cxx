#include "viewables_registrar_impl.hxx"
#include "viewable_object.hxx"
#include "common.hxx"
#include "helper_math.hxx"

static auto console = getConsole("viewables_registrar_impl");

ViewablesRegistrarImpl::ViewablesRegistrarImpl() : counter(0)
{
}

ViewablesRegistrarImpl::~ViewablesRegistrarImpl()
{
}

ViewablesRegistrar::viewableId ViewablesRegistrarImpl::appendViewable(ViewableObjectPtr _object)
{
    std::unique_lock<std::mutex> lock(access_mutex);
    counter++;
    ViewableObjectInfo info { _object, std::list<PointOfView>()  };
    objects.insert(std::pair<viewableId, ViewableObjectInfo>(counter, info));

    return counter;
}

void ViewablesRegistrarImpl::updateViewableSolved(ViewablesRegistrar::viewableId id, std::list<PointOfView> positions)
{
    std::unique_lock<std::mutex> lock(access_mutex);

    if (!objects.count(id))
        return;

    auto& obj = objects[id];
    // first remove where it appears
    for (auto& pov: obj.povList) { // NOTE: one room may be accessed multiple times, but this is costless.
        if (mapRoomObject.count(pov.room)) {
            auto& subr = mapRoomObject[pov.room];
            subr.erase(id);
        }
    }
    // replace
    obj.povList = positions;
    // then append in new position
    for (auto& pov: obj.povList) {
        if (mapRoomObject.count(pov.room) == 0) {
            MapIdPovList voidList;
            mapRoomObject.insert(std::pair<std::string, MapIdPovList>(pov.room, voidList));
        };
        MapIdPovList& room_ids = mapRoomObject[pov.room];
        if (room_ids.count(id) == 0) {
            room_ids.insert(std::pair<viewableId, PovList>(id, { pov }));
        }
         else {
            room_ids[id].push_back(pov);
        }
    }
}

void ViewablesRegistrarImpl::updateViewable(viewableId id, PointOfView position)
{
    auto positions = solvePosition(position);
    updateViewableSolved(id, positions);
}

void ViewablesRegistrarImpl::removeViewable(ViewablesRegistrar::viewableId id)
{
    updateViewableSolved(id, std::list<PointOfView>());

    std::unique_lock<std::mutex> lock(access_mutex);
    if (!objects.count(id))
        return;

    objects.erase(id);
}

std::list<ViewablesRegistrar::ViewableLocation> ViewablesRegistrarImpl::getViewables(std::string room)
{
    std::unique_lock<std::mutex> lock(access_mutex);
    std::list<ViewablesRegistrar::ViewableLocation> result;
    if (mapRoomObject.count(room) != 0) {
        auto listIds = mapRoomObject[room];
        for (auto id : listIds) {
            auto objptr = objects[id.first].object;
            ViewablesRegistrar::ViewableLocation loc = { objptr , id.second };
            result.push_back(loc);
        }
    }
    return result;
}

void ViewablesRegistrarImpl::dump()
{
    console->debug("===== DEBUG MAP");
    for (auto kv : mapRoomObject)
    {
        console->debug("ROOM={}", kv.first);
        for (auto id : kv.second) {
            console->debug("ID={}", id.first);
            for (auto pov : id.second) {
               console->debug("P={}", vec3_to_string(pov.position));
            }
        }
    }
    console->debug("===== DEBUG IDS");
    for (auto kv : objects)
    {
        console->debug("ID={}", kv.first);
        for (auto pov : kv.second.povList) {
            console->debug("ROOM={}", pov.room);
        }
    }

}