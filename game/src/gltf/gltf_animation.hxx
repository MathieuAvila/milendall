#pragma once

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

#include "gltf_data_accessor_iface.hxx"
#include "gltf_material_accessor_iface.hxx"
#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_animation_target_iface.hxx"

class GltfAnimation
{

    class GltfAnimationFrame;

    protected:

    std::vector< std::unique_ptr<GltfAnimationFrame> > frames;
    std::string name;


    public:

    /** @brief Loads a GLTF animation object
     * @param json the JSON animation object
     * @param data_accessor helper to access data efficiently
     * @note may throw a GltfException
     */
    GltfAnimation(
        nlohmann::json& json,
        GltfDataAccessorIFace* data_accessor
        );

    /** @brief Apply the animation at a given time
     * @param time at which timeframe the animation must be applied
     * @param instance to which instance it must be applied
     */
    void apply(float time, GltfAnimationTargetIface* instance);

    virtual ~GltfAnimation();

};

