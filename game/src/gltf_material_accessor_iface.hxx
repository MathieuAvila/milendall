#ifndef GLTF_MATERIAL_IFACE_HXX
#define GLTF_MATERIAL_IFACE_HXX

#include "file_library.hxx"
#include <nlohmann/json.hpp>

#include <memory>

class GltfMaterialIface
{
   GltfMaterialIface();
};

/**
 * accessor is intended to return a given texture to be used for direct injection
 * during object creation.
 * the goal of this file is to provide an abstraction to external texture access
 */
class GltfMaterialAccessorIFace
{
    public:

    /** access a given Id, return a pointer to the material definition
     * caller doesn't hold ownership. It is destroyed when Accessor is destroyed
     * after all its references have been released.
     */
    virtual const GltfMaterialIface* accessId(uint32_t index) = 0;
    virtual ~GltfMaterialAccessorIFace() = 0;
};

#endif