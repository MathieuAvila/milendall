#pragma once

#include "gltf_data_accessor_iface.hxx"
#include "file_library.hxx"
#include <nlohmann/json.hpp>

#include <memory>
#include <vector>

/**
 * accessor is intended to return a given data block.
 * interlacing, and so on ... must be managed from this offset.
 * the goal of this file is to provide an abstraction to external file access
 * so that it can be cached when multiple accesses to the same files are done
 */
class GltfDataAccessor: public GltfDataAccessorIFace
{
    std::vector<FileContentPtr> loaded_buffers;
    nlohmann::json& json;

    public:

    /** @brief Needs the JSON model file being used, as well as the library content */
    GltfDataAccessor(nlohmann::json& _json, const FileLibrary::UriReference ref);

    virtual std::shared_ptr<DataBlock> accessId(uint32_t index);
    virtual ~GltfDataAccessor();
};
