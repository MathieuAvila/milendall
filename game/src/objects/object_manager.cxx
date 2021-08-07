#include "object_manager.hxx"

struct LevelRoomResolver;

ObjectManager::ObjectManager(RoomResolver* _roomResolver) : roomResolver(_roomResolver)
{

}

void ObjectManager::insertObject(std::shared_ptr<ManagedObject> object)
{
    managed_objects.push_back(object);
}

void ObjectManager::update(float time_delta)
{

}
