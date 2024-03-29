#include "common.hxx"
#include "json_helper_accessor.hxx"

#include "gltf_model.hxx"
#include "model_registry.hxx"

#include "viewable_object.hxx"
#include "object_option.hxx"

#include "object_type_register.hxx"

// To animate option.
#include <glm/gtc/matrix_transform.hpp>

// TODO manage elsewhere.
#include <chrono>

#include "object_exception.hxx"

static auto console = getConsole("object_option");

typedef struct OptionDefinition
{
    string ressource;
    float scale;
    float time_value;
} OptionDefinition;

static const std::map<string, OptionDefinition> definitions = {
    {{"time_+1", {.ressource = "time_plus_1__apple.glb", .scale = 1.0f, .time_value = 1.0f}},
     {"time_+2", {.ressource = "time_plus_2__broccoli.glb", .scale = 1.0f, .time_value = 2.0f}},
     {"time_+5", {.ressource = "time_plus_5__banana.glb", .scale = 1.0f, .time_value = 5.0f}},
     {"time_-1", {.ressource = "time_less_1__candy.glb", .scale = 1.0f, .time_value = -1.0f}},
     {"time_-2", {.ressource = "time_less_2__sundae.glb", .scale = 1.0f, .time_value = -2.0f}},
     {"time_-5", {.ressource = "time_less_5__burgercheesedouble.glb", .scale = 1.0f, .time_value = -5.0f}}}};

class ObjectOptionViewable : public ViewableObject
{
    /** @brief own ref to the model */
    std::shared_ptr<GltfModel> model;

    /** @brief own instance */
    std::unique_ptr<GltfInstance> instance;

    // TODO: provide time from elsewhere
    std::chrono::steady_clock::time_point begin;

    const OptionDefinition& def;

public:
    ObjectOptionViewable(
        ModelRegistry *registry,
        FileLibrary *library,
        const OptionDefinition& _def) : def(_def)
    {
        model = registry->getModel(library->getRoot().getSubPath("/common/objects/" + def.ressource));
        instance = make_unique<GltfInstance>(model->getInstanceParameters());
        begin = std::chrono::steady_clock::now();
    }

    virtual ~ObjectOptionViewable() = default;

    virtual float getRadius() const override
    {
        return 0.5f;
    }

    virtual void outputObject(glm::mat4 &rel_pos) const override
    {
        // console->info("print option");

        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        auto diff_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>)(current - begin).count() / 1000000.0f;

        auto rot = glm::rotate(rel_pos, diff_sec * glm::pi<float>() / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        auto final = glm::translate(rot, glm::vec3(0.0f, glm::cos(diff_sec * 2.0f) * 0.1f - 0.2, 0.0f));
        auto final2 = glm::scale(final, glm::vec3(def.scale, def.scale, def.scale));

        model->applyDefaultTransform(instance.get(), final2);
        model->draw(instance.get());
    }
};

ObjectOption::ObjectOption(
    ModelRegistry *registry,
    FileLibrary *library,
    nlohmann::json *root)
{
    end = false;
    auto subtype = jsonGetElementByName(*root, "subtype").get<std::string>();
    console->debug("load option, subtype {}", subtype);
    if (!definitions.count(subtype))
    {
        throw ObjectException("Unknown option type " + subtype);
    }
    def = &definitions.at(subtype);

    viewable = std::make_shared<ObjectOptionViewable>(registry, library, *def);

    time_value = def->time_value;
}

bool ObjectOption::checkEol() const
{
    return end;
}

MovementWish ObjectOption::getRequestedMovement() const
{
    return MovementWish();
}

glm::mat4x4 ObjectOption::getOwnMatrix() const
{
    return glm::mat4x4();
}

static MovableObjectDefinition option_def(false, MovableObjectDefinition::InteractionLevel::LOW); // don't need to compute move

const MovableObjectDefinition &ObjectOption::getObjectDefinition() const
{
    return option_def;
}

std::shared_ptr<ViewableObject> ObjectOption::getViewable() const
{
    return viewable;
}

void ObjectOption::interact(ManagedObject *second_object)
{
    console->info("Add time {} minute", time_value);
    end = second_object->addTime(time_value* 60.0f);
}
