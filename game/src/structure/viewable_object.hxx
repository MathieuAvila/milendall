#pragma once

#include "glm/mat4x4.hpp"

/** @brief Interface class for any object that needs to be drawn */
class ViewableObject
{
    public:

        /** @brief need this property to detect portal crossing. In that case the object may need to be drawn in
        multiple different rooms */
        virtual float getRadius() const = 0;

        /** @brief Render object, in the given position. Other matrices must NOT be changed */
        virtual void outputObject(glm::vec4 masterMatrix) const = 0;
};

