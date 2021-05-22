#pragma once

#include <cstdint>
#include <memory>

/**
 * @class GltfMaterialAccessorIFace is intended to return a given texture to be used for direct injection
 * during @class GltfMesh creation. All game is to load a given texture only once.
 * This interface is the only one presented to @class GltfMesh.
 * See @file gltf_material_accessor.hxx to get the whole picture.
 */
class GltfMaterialAccessorIFace
{
    public:

    /**
     * @brief access a given Id, access and apply.
     *        It is expected to apply only if not already applied, and not to load multiple
     *        times.
     */
    virtual void loadId(uint32_t index) = 0;
    virtual ~GltfMaterialAccessorIFace() = 0;
};

using UGltfMaterialAccessorIFace = std::unique_ptr<GltfMaterialAccessorIFace>;
using WGltfMaterialAccessorIFace = std::weak_ptr<GltfMaterialAccessorIFace>;
using SGltfMaterialAccessorIFace = std::shared_ptr<GltfMaterialAccessorIFace>;
