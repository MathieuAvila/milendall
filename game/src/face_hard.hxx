#pragma once

#include <memory>
#include <set>

#include "face_list.hxx"

#include <nlohmann/json.hpp>

struct FaceHard
{
    /** @brief Should contain 1 face only */
    FaceList face; // Should contain 1 face only

    FaceHard(
        std::shared_ptr<PointsBlock>,
        std::unique_ptr<GltfDataAccessorIFace::DataBlock>,
        nlohmann::json& json);
};
