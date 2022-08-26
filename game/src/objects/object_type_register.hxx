#pragma once

#include <functional>
#include <nlohmann/json.hpp>

/** @brief a global function to register object types. This one is used by
 * objects but defined and used by @file object_manager.cxx */
class ModelRegistry;
class FileLibrary;
class ManagedObject;
typedef std::function<std::shared_ptr<ManagedObject>(ModelRegistry* registry, FileLibrary* library, nlohmann::json* root)>  ObjectTypeRegisterMethod;

void registerObjectType(
    std::string type_name,
    ObjectTypeRegisterMethod method
    );
void registerAllObjectTypes();
