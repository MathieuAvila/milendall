#include "model_registry.hxx"
#include "gltf_model.hxx"


ModelRegistry::ModelRegistry(
    std::shared_ptr<GltfMaterialLibraryIface> _materialLibrary):
    materialLibrary(_materialLibrary)
{
}

std::shared_ptr<GltfModel> ModelRegistry::getModel(const FileLibrary::UriReference ref)
{
    if (!models.count(ref)) {
        models[ref] = std::make_shared<GltfModel>(materialLibrary, ref);
    }
    return models[ref];
}

ModelRegistry::~ModelRegistry()
{
}
