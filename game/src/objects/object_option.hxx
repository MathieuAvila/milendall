#pragma once

#include "managed_object.hxx"

#include <nlohmann/json.hpp>


class ViewableObject;
class ModelRegistry;
class FileLibrary;
struct OptionDefinition;

/** @brief All options objects */
class ObjectOption : public ManagedObject
{

    /** @brief shared object for viewing */
    std::shared_ptr<ViewableObject> viewable;

    /** @brief End on player interaction */
    bool end;

    /** @brief based on subtype, time to append */
    float time_value;

    /** @brief Internal definition field */
    const OptionDefinition* def;

    public:

        // to be derived
        ObjectOption() = default;

        /** @brief Load from file
         * @param root parameters root, or nullptr
         * @param registry one shot usage, only during initialization
         * @param library one shot usage, only during initialization
        */
        ObjectOption(ModelRegistry* registry, FileLibrary* library, nlohmann::json* root);

        /** @brief from ManagedObject */
        virtual bool checkEol() const override;

        /** @brief from ManagedObject */
        virtual MovementWish getRequestedMovement() const  override;

        /** @brief from ManagedObject */
        virtual glm::mat4x4 getOwnMatrix() const override;

        /** @brief from ManagedObject */
        virtual const MovableObjectDefinition& getObjectDefinition() const override;

        /** @brief from ManagedObject */
        virtual std::shared_ptr<ViewableObject> getViewable() const override;

        /** @brief send messages about time and speed */
        virtual void interact(ManagedObject* second_object) override;

        // needs too be derivable.
        virtual ~ObjectOption() = default;
};
