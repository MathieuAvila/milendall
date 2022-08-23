#pragma once

#include <memory>
#include <map>

#include "file_library.hxx"

class GltfModel;
class GltfMaterialLibraryIface;

class ModelRegistry
{
        /** @brief mapping ref and objects */
        std::map<FileLibrary::UriReference, std::shared_ptr<GltfModel>> models;

        /** @brief requested to load models */
        std::shared_ptr<GltfMaterialLibraryIface> materialLibrary;

    public:

        ModelRegistry(std::shared_ptr<GltfMaterialLibraryIface> _materialLibrary);

        /** @brief Get a reference to a GltfModel from a URI */
        std::shared_ptr<GltfModel> getModel(const FileLibrary::UriReference ref);

        /** @brief manadatory due to fwd decslaration */
        ~ModelRegistry();

};

