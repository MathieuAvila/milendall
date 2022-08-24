#include "common.hxx"
#include "json_helper_accessor.hxx"

#include "gltf_model.hxx"
#include "model_registry.hxx"

#include "viewable_object.hxx"
#include "object_option.hxx"

static auto console = getConsole("object_option");

class ObjectOptionViewable : public ViewableObject {

    /** @brief own ref to the model */
    std::shared_ptr<GltfModel> model;

    /** @brief own instance */
    std::unique_ptr<GltfInstance> instance;

    public:

        ObjectOptionViewable(
            ModelRegistry* registry,
            FileLibrary* library,
            std::string type)
        {
            // TODO switch on type
            model = registry->getModel(library->getRoot().getSubPath("/common/objects/time_plus__apple.glb"));
            instance = make_unique<GltfInstance>(model->getInstanceParameters());
        }

        virtual ~ObjectOptionViewable() = default;

        virtual float getRadius() const override {
            return 0.5f;
        }

        virtual void outputObject(glm::mat4& rel_pos) const override {
            //console->info("print option");
            model->applyDefaultTransform(instance.get(), rel_pos);
            model->draw(instance.get());
        }
};

ObjectOption::ObjectOption(
    ModelRegistry* registry,
    FileLibrary* library,
    nlohmann::json* root)
{
    auto subtype = jsonGetElementByName(*root, "subtype").get<std::string>();
    console->info("load option, subtype {}", subtype);
    viewable = std::make_shared<ObjectOptionViewable>(registry, library, subtype);
}

bool ObjectOption::checkEol() const
{
    return false;
}

MovementWish ObjectOption::getRequestedMovement() const
{
    return MovementWish();
}

glm::mat4x4 ObjectOption::getOwnMatrix() const
{
    return glm::mat4x4();
}

static MovableObjectDefinition option_def(0.2, 0.0, 0.0, 0.0);

const MovableObjectDefinition &ObjectOption::getObjectDefinition() const
{
    return option_def;
}

std::shared_ptr<ViewableObject> ObjectOption::getViewable() const
{
    return viewable;
}
