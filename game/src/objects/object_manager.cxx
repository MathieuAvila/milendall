#include "object_manager.hxx"

#include "space_resolver.hxx"
#include <glm/gtx/projection.hpp>

#include "common.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"

#include "managed_object_instance.hxx"
#include "object_exception.hxx"
#include "object_option.hxx"

#define GLM_ENABLE_EXPERIMENTAL 1

static auto console = getConsole("object_manager");

ObjectManager::ObjectManager()
{

}

void ObjectManager::setReferences(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar)
{
    spaceResolver = _spaceResolver;
    gravityProvider = _gravityProvider;
    viewables_registrar = _viewables_registrar;
}

ObjectManager::ObjectManager(
    SpaceResolver *_spaceResolver,
    GravityProvider *_gravityProvider,
    ViewablesRegistrar* _viewables_registrar)
{
    setReferences(
            _spaceResolver,
            _gravityProvider,
            _viewables_registrar);
}

ObjectManager::ObjectUid ObjectManager::insertObject(
    std::shared_ptr<ManagedObject> object,
    PointOfView pos,
    std::string mesh_name )
{
    static ObjectUid ID = 0;
    managed_objects.insert(
        std::pair<
            ObjectUid,
            std::unique_ptr<ManagedObjectInstance>>(
                ++ID,
                std::make_unique<ManagedObjectInstance>(
                    object,
                    pos,
                    mesh_name,
                    spaceResolver,
                    gravityProvider,
                    viewables_registrar)));
    return ID;
}

void ObjectManager::update(float total_time)
{
    for (auto &_obj : managed_objects)
    {
        auto p = _obj.second.get();
        p->computeNextPosition(total_time);
    }
}

bool ObjectManager::getObjectPosition(ObjectManager::ObjectUid objectId, PointOfView &pos)
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

void ObjectManager::loadObject(std::string room_name, std::string mesh_name, nlohmann::json &root)
{
    auto type = jsonGetElementByName(root, "type").get<std::string>();

    PointOfView pov(jsonGetVec3(jsonGetElementByName(root, "position")), glm::mat3x3(1.0), room_name);

    console->info("Load from room={} node={} type={}", room_name, mesh_name, type);
    // TODO: need an anonymous loader for sanity of architecture
    if (type == "option")
    {
        nlohmann::json* parameters = nullptr;
        if (root.contains("parameters")) {
            parameters = &(root["parameters"]);
        }
        auto obj = make_shared<ObjectOption>(parameters);
        insertObject(obj, pov);
    }
    else
    {
        console->warn(
            "Unknown object type from room={} node={} type={}. IGNORE. Shall I assert ?",
            room_name, mesh_name, type);
    }
}
