#include "object_type_register.hxx"

#include "object_option.hxx"
#include "object_exit.hxx"

void registerAllObjectTypes()
{
    registerObjectType("option",
    [](ModelRegistry* registry, FileLibrary* library, nlohmann::json* root) {
        return std::make_shared<ObjectOption>(registry, library, root);
    });
    registerObjectType("exit",
    [](ModelRegistry* registry, FileLibrary* library, nlohmann::json* root) {
        return std::make_shared<ObjectExit>(registry, library, root);
    });
}
