#include "object_option.hxx"
#include "viewable_object.hxx"

#include "common.hxx"

static auto console = getConsole("object_option");

class ObjectOptionViewable : public ViewableObject {

    public:

        ObjectOptionViewable()  = default;

        virtual ~ObjectOptionViewable() = default;

        virtual float getRadius() const override {
            return 0.5f;
        }

        virtual void outputObject() const override {
            console->info("print object");
        }
};

ObjectOption::ObjectOption(nlohmann::json* root) : viewable(std::make_shared<ObjectOptionViewable>())
{
    console->info("load object");
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

static MovableObjectDefinition option_def(0.5, 0.0, 0.0, 0.0);

const MovableObjectDefinition &ObjectOption::getObjectDefinition() const
{
    return option_def;
}

std::shared_ptr<ViewableObject> ObjectOption::getViewable() const
{
    return viewable;
}
