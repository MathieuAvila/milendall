#pragma once

#include "glm/mat4x4.hpp"

class GltfAnimationTargetIface
{
    public:

    /** @brief Apply a matrix to a given frame index
     * @param time at which timeframe the animation must be applied
     * @param instance to which instance it must be applied
     */
    virtual void applyChange(int index, glm::mat4x4 matrix) = 0;
};

