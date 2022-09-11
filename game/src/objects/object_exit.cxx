#include "common.hxx"
#include "json_helper_accessor.hxx"

#include "gltf_model.hxx"
#include "model_registry.hxx"

#include "viewable_object.hxx"
#include "object_exit.hxx"

#include "object_type_register.hxx"

// To animate option.
#include <glm/gtc/matrix_transform.hpp>

// TODO manage elsewhere.
#include <chrono>

#include "object_exception.hxx"

static auto console = getConsole("object_exit");

class ObjectExitViewable : public ViewableObject
{
    /** @brief own ref to the model */
    std::shared_ptr<GltfModel> model;

    /** @brief own instance */
    std::unique_ptr<GltfInstance> instance;

    // TODO: provide time from elsewhere
    std::chrono::steady_clock::time_point begin;

public:
    ObjectExitViewable(
        ModelRegistry *registry,
        FileLibrary *library)
    {
        model = registry->getModel(library->getRoot().getSubPath("/common/objects/unicorn/unicorn.gltf"));
        instance = make_unique<GltfInstance>(model->getInstanceParameters());
        begin = std::chrono::steady_clock::now();
    }

    virtual ~ObjectExitViewable() = default;

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
        auto final2 = glm::scale(final, glm::vec3(1.0/35.0f, 1.0/35.0f, 1.0/35.0f));

        model->applyDefaultTransform(instance.get(), final2);
        model->draw(instance.get());
    }
};

ObjectExit::ObjectExit(
    ModelRegistry *registry,
    FileLibrary *library,
    nlohmann::json *root)
{
    console->info("load exit");

    viewable = std::make_shared<ObjectExitViewable>(registry, library);
}

bool ObjectExit::checkEol() const
{
    return false;
}

MovementWish ObjectExit::getRequestedMovement() const
{
    return MovementWish();
}

glm::mat4x4 ObjectExit::getOwnMatrix() const
{
    return glm::mat4x4();
}

static MovableObjectDefinition option_def(false, MovableObjectDefinition::InteractionLevel::LOW); // don't need to compute move

const MovableObjectDefinition &ObjectExit::getObjectDefinition() const
{
    return option_def;
}

std::shared_ptr<ViewableObject> ObjectExit::getViewable() const
{
    return viewable;
}

void ObjectExit::interact(ManagedObject *second_object)
{
    console->info("Touched exit");
}
