#pragma once

#include "glm/mat4x4.hpp"

class ViewableObject
{
    public:

        virtual float getRadius() = 0;
        virtual void outputObject(glm::vec4 masterMatrix) = 0;
};

