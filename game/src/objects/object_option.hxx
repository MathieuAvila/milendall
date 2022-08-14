#pragma once

#include "managed_object.hxx"

#include <nlohmann/json.hpp>

/** @brief All options objects
 */
class ObjectOption : public ManagedObject
{

    public:

        // to be derived
        ObjectOption() = default;

        /** @brief Load from file */
        ObjectOption(std::string room_name, std::string mesh_name, nlohmann::json& root);

        /** @brief from ManagedObject */
        virtual bool checkEol() override;

        /** @brief from ManagedObject */
        virtual MovementWish getRequestedMovement() override;

        /** @brief from ManagedObject */
        virtual glm::mat4x4 getOwnMatrix() override;

        /** @brief from ManagedObject */
        virtual MovableObjectDefinition& getObjectDefinition() override;

        // needs too be derivable.
        virtual ~ObjectOption() = default;
};
