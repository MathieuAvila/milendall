#include "object_manager.hxx"

#include "space_resolver.hxx"
#include <glm/gtx/projection.hpp>

#include "common.hxx"

#include "managed_object_instance.hxx"

#define GLM_ENABLE_EXPERIMENTAL 1

static auto console = getConsole("object_manager");

ObjectManager::ObjectManager(
    SpaceResolver* _spaceResolver,
    GravityProvider* _gravityProvider) :
spaceResolver(_spaceResolver), gravityProvider(_gravityProvider)
{

}

int ObjectManager::insertObject(std::shared_ptr<ManagedObject> object, PointOfView pos)
{
    static int ID = 0;
    managed_objects.insert(std::pair<int, std::unique_ptr<ManagedObjectInstance>>(++ID,
        std::make_unique<ManagedObjectInstance>(object, pos, spaceResolver, gravityProvider)));
    return ID;
}

void ObjectManager::update(float total_time)
{
    for (auto& _obj: managed_objects) {
        auto p = _obj.second.get();
        p->computeNextPosition(total_time);
    }
}

bool ObjectManager::getObjectPosition(int objectId, PointOfView& pos)
{
    if (managed_objects.count(objectId) != 1)
        return false;
    auto obj = (managed_objects.find(objectId)->second.get());
    pos = obj->getObjectPosition();
    return true;
}

ObjectManager::~ObjectManager()
{
}
