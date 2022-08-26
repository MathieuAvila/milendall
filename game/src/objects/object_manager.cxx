#include "object_manager.hxx"

#include "space_resolver.hxx"
#include <glm/gtx/projection.hpp>

#include "common.hxx"
#include "json_helper_accessor.hxx"
#include "json_helper_math.hxx"

#include "object_type_register.hxx"
#include "managed_object_instance.hxx"
#include "object_exception.hxx"
#include "object_type_register.hxx"

static auto console = getConsole("object_manager");

ObjectManager::ObjectManager(
    std::shared_ptr<ModelRegistry> _model_registry,
    std::shared_ptr<FileLibrary> _library) : model_registry(_model_registry),
                                             library(_library)
{
}

void ObjectManager::setReferences(
    SpaceResolver *_spaceResolver,
    GravityProvider *_gravityProvider,
    ViewablesRegistrar *_viewables_registrar)
{
    spaceResolver = _spaceResolver;
    gravityProvider = _gravityProvider;
    viewables_registrar = _viewables_registrar;
}

static std::map<std::string, ObjectTypeRegisterMethod> objectTypeRegistry;

void registerObjectType(
    std::string type_name,
    ObjectTypeRegisterMethod method
    )
{
    console->debug("Register object type {}", type_name);
    objectTypeRegistry.insert(std::pair<std::string, ObjectTypeRegisterMethod>(type_name, method));
}

static struct __inited_objects {
    __inited_objects() {
        console->debug("Registering objects");
        registerAllObjectTypes();
    }
} __inited;

ObjectManager::ObjectManager(
    std::shared_ptr<ModelRegistry> _model_registry,
    std::shared_ptr<FileLibrary> _library,
    SpaceResolver *_spaceResolver,
    GravityProvider *_gravityProvider,
    ViewablesRegistrar *_viewables_registrar) : model_registry(_model_registry),
                                                library(_library)
{
    setReferences(
        _spaceResolver,
        _gravityProvider,
        _viewables_registrar);
}

ObjectManager::ObjectUid ObjectManager::insertObject(
    std::shared_ptr<ManagedObject> object,
    PointOfView pos,
    std::string mesh_name)
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
        p->updateViewable();
    }
    // compute interaction
    // TODO: check per room based on computed object list in a room.
    for (auto &obj : managed_objects)
    {
        float radius;
        MovableObjectDefinition::InteractionLevel level;
        PointOfView pos;
        obj.second->getInteractionParameters(radius, level, pos);
        if (level == MovableObjectDefinition::InteractionLevel::ALL)
        {
            for (auto &second_obj : managed_objects)
            {
                float second_radius;
                MovableObjectDefinition::InteractionLevel second_level;
                PointOfView second_pos;
                second_obj.second->getInteractionParameters(second_radius, second_level, second_pos);
                if ((obj.first != second_obj.first) && (second_level >= MovableObjectDefinition::InteractionLevel::LOW))
                {
                    if (second_pos.room == pos.room)
                    {
                        float delta = glm::distance(second_pos.position, pos.position) - radius - second_radius;
                        if (delta < 0.0f)
                        {
                            console->info("Interaction {} {}", obj.first, second_obj.first);
                            // interact on each other.
                            second_obj.second->interact(obj.second.get());
                            obj.second->interact(second_obj.second.get());
                        }
                    }
                }
            }
        }
    }

    // remove dead objects
    auto finish = false;
    while (!finish)
    {
        finish = true;
        for (auto &obj : managed_objects)
        {
            if (obj.second->checkEol())
            {
                managed_objects.erase(obj.first);
                finish = false;
                break;
            }
        }
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
    nlohmann::json *parameters = nullptr;
    if (root.contains("parameters"))
        {
            parameters = &(root["parameters"]);
        }
    if (objectTypeRegistry.count(type)) {
        console->info("Request to create option");
        auto obj = objectTypeRegistry[type](model_registry.get(), library.get(), parameters);
        insertObject(obj, pov);
    }
    else
    {
        console->warn(
            "Unknown object type from room={} node={} type={}. IGNORE. Shall I assert ?",
            room_name, mesh_name, type);
    }
}
