#ifndef GLTF_DATA_ACCESSOR_IFACE_HXX
#define GLTF_DATA_ACCESSOR_IFACE_HXX

#include "file_library.hxx"
#include <nlohmann/json.hpp>

#include <memory>

/**
 * accessor is intended to return a given data block offset.
 * interlacing, and so on ... must be managed from this offset.
 * the goal of this file is to provide an abstraction to external file access
 * so that it can be cached when multiple accesses to the same files are done
 */
class GltfDataAccessorIFace
{
    public:

    /** access a given Id, return the first value of the data block.
     * This block may be a copy or original data.
     * Internal data should not be accessed after loading has ending.
     */
    virtual std::shared_ptr<uint8_t*> accessId(uint32_t index) = 0;
    virtual ~GltfDataAccessorIFace() = 0;
};

#endif