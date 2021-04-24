#ifndef GLTF_FRAME_HXX
#define GLTF_FRAME_HXX

#include "file_library.hxx"
#include <nlohmann/json.hpp>

class GltfFrame
{
    protected:

    virtual void parseApplicationData(nlohmann::json& json);


    public:

    GltfFrame(nlohmann::json& json);

};

#endif