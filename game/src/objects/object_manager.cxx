#include "object_manager.hxx"

class ManagedObjectInstance
{
    public:

    std::shared_ptr<ManagedObject> object;

    /** This is the real position of the center of the object*/
    PointOfView mainPosition;

    /** This is other positions that the object crosses */
    std::list<PointOfView> otherPosition;

    ManagedObjectInstance(std::shared_ptr<ManagedObject> _object, PointOfView _mainPosition):
        object(_object), mainPosition(_mainPosition) {};

};

ObjectManager::ObjectManager(RoomResolver* _roomResolver) : roomResolver(_roomResolver)
{

}

int ObjectManager::insertObject(std::shared_ptr<ManagedObject> object, PointOfView pos)
{
    static int ID = 0;
    managed_objects.insert(std::pair<int, std::unique_ptr<ManagedObjectInstance>>(++ID,
        std::make_unique<ManagedObjectInstance>(object, pos)));
    return ID;
}

void ObjectManager::update(float time_delta)
{
    for (auto& _obj: managed_objects) {
        auto p = _obj.second.get();
        // ATM, accept whatever the object requests
        p->mainPosition.position += p->object.get()->getRequestedMovement().impulse;
    }
}

bool ObjectManager::getObjectPosition(int objectId, PointOfView& pos)
{
    if (managed_objects.count(objectId) != 1)
        return false;
    auto obj = (managed_objects.find(objectId)->second.get());
    pos = obj->mainPosition;
    return true;
}

ObjectManager::~ObjectManager()
{
}
