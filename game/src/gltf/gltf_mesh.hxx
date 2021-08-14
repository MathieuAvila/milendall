#pragma once

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

#include "gltf_data_accessor_iface.hxx"
#include "gltf_material_accessor_iface.hxx"
#include "gltf_material_accessor_library_iface.hxx"

class GltfMesh
{
    protected:

    class GltfPrimitive;

    std::vector< std::unique_ptr<GltfPrimitive> > primitives;

    virtual void parseApplicationData(nlohmann::json& json);

    public:

    /** @brief Loads a GLTF frame object
     * @param json the JSON mesh
     * @param index index of the object to load
     * @param data_accessor helper to access data efficiently
     * @param material_accessor helper to load a texture
     * @note may throw a GltfException
     */
    GltfMesh(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        SGltfMaterialAccessorIFace material_accessor
        );

    void Draw();

    virtual ~GltfMesh();

};

