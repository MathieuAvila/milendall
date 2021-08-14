#pragma once

#include "gltf_material_accessor_iface.hxx"

#include <file_library.hxx>
#include <nlohmann/json.hpp>

/** @file Provide a material loader, common to all rooms for a levels
 *        and even to additional objects, so that real textures are
 *        loading only once when playing a level.
 *        When a model is released, reference counters are decremented
 *        so that textures can be automatically released, thus only
 *        useful content is kept, and content no more used is released.
 *
 *        Rationale:
 *
 *        - GltfMaterialLibraryIface is the entry-point of the
 *        whole program. It is only an interface, and the real implementation
 *        is provided through the
 *        GltfMaterialLibraryIface::getMeterialLibray().
 *        - GltfMaterialLibraryIface::getMaterialAccessor must be used
 *          for every newly model, and released only upon release of model
 *
 */

class GltfMaterialLibraryIface;
using GltfMaterialLibraryIfacePtr = std::shared_ptr<GltfMaterialLibraryIface>;

class GltfMaterialLibraryIface
{
    public:

    /** @brief Access to real instance impl */
    static GltfMaterialLibraryIfacePtr getMaterialLibray();

    /** @brief Access to real GltfMaterialAccessor impl. Keep the pointer
     *         during the life of the model. Releasing it decrements
     *         allocated textures ref-counting
     * @param dir Current GLTF file directory. External files are built
     *            from this reference point.
     * @param file Current GLTF file. Used for:
     *             1/ relative file name resolution
     *             2/ preloading all material upfront
    */
    virtual UGltfMaterialAccessorIFace getMaterialAccessor(
        const FileLibrary::UriReference& dir,
        nlohmann::json& file) = 0;

    virtual ~GltfMaterialLibraryIface() = 0;
};



