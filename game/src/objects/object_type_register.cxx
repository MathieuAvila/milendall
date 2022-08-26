#include "object_type_register.hxx"

#include "object_option.hxx"

void registerAllObjectTypes()
{
    registerObjectType("option",
    [](ModelRegistry* registry, FileLibrary* library, nlohmann::json* root) {
        return std::make_shared<ObjectOption>(registry, library, root);
    });
}
