#include "object_manager.hxx"

#include "space_resolver.hxx"

struct FaceHard;

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

ObjectManager::ObjectManager(RoomResolver* _roomResolver, SpaceResolver* _spaceResolver) :
    roomResolver(_roomResolver), spaceResolver(_spaceResolver)
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
        auto impulse = p->object.get()->getRequestedMovement().impulse;
        auto timed_impulse = impulse * time_delta;
        auto newPos = p->mainPosition.position + p->mainPosition.local_reference * timed_impulse;

        PointOfView endPoint;
        glm::vec3 normal;
        float distance;
        FaceHard* face;

        // Stop if wall is reached
        bool hit = spaceResolver->isWallReached(
            p->mainPosition,
            newPos,
            p->object.get()->getObjectDefinition().radius,
            endPoint,
            normal,
            distance,
            face);
        p->mainPosition = endPoint;
        if (hit) {
            p->mainPosition.position += normal * 0.001f;
        }
    }
}

bool ObjectManager::getObjectPosition(int objectId, PointOfView& pos)
{
    if (managed_objects.count(objectId) != 1)
        return false;
    auto obj = (managed_objects.find(objectId)->second.get());
    pos = obj->mainPosition.prependCoordinateSystem(obj->object->getOwnMatrix());
    return true;
}

ObjectManager::~ObjectManager()
{
}
