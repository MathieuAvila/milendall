#include "viewables_registrar_impl.hxx"
#include "viewable_object.hxx"
#include "common.hxx"

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

void ViewablesRegistrarImpl::updateViewable(ViewablesRegistrar::viewableId id, std::list<PointOfView> positions)
{
    std::unique_lock<std::mutex> lock(access_mutex);

    if (!objects.count(id))
        return;

    auto& obj = objects[id];
    // first remove where it appears
    for (auto& pov: obj.povList) {
        if (mapRoomObject.count(pov.room)) {
            auto& subr = mapRoomObject[pov.room];
            subr.remove(id);
        }
    }
    // replace
    obj.povList = positions;
    // then append in new position
    for (auto& pov: obj.povList) {
        if (mapRoomObject.count(pov.room) == 0) {
            std::list<ViewablesRegistrar::viewableId> ids = {counter};
            mapRoomObject.insert(std::pair<std::string, std::list<ViewablesRegistrar::viewableId>>(pov.room, ids));
        } else {
            mapRoomObject[pov.room].push_back(counter);
        }
    }
}

void ViewablesRegistrarImpl::removeViewable(ViewablesRegistrar::viewableId id)
{
    updateViewable(id, std::list<PointOfView>());

    std::unique_lock<std::mutex> lock(access_mutex);
    if (!objects.count(id))
        return;

    objects.erase(id);
}

std::list<ViewablesRegistrar::ViewableObjectPtr> ViewablesRegistrarImpl::getViewables(std::string room)
{
    std::unique_lock<std::mutex> lock(access_mutex);
    std::list<ViewablesRegistrar::ViewableObjectPtr> result;
    if (mapRoomObject.count(room) != 0) {
        auto listIds = mapRoomObject[room];
        for (auto id : listIds) {
            result.push_back(objects[id].object);
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
            console->debug("ID={}", id);
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