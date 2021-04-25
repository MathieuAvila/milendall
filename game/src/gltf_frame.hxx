#ifndef GLTF_FRAME_HXX
#define GLTF_FRAME_HXX

#include <memory>
#include <nlohmann/json.hpp>

#include "gltf_data_accessor_iface.hxx"
#include "gltf_material_accessor_iface.hxx"

class GltfFrame
{
    protected:

    virtual void parseApplicationData(nlohmann::json& json);


    public:

    GltfFrame(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor,
        GltfMaterialAccessorIFace* material_accessor
        );

    virtual ~GltfFrame();

};

#endif