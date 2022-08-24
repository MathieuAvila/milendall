#pragma once

/** @brief Interface class for any object that needs to be drawn */
class ViewableObject
{
    public:

        /** @brief need this property to detect portal crossing. In that case the object may need to be drawn in
        multiple different rooms */
        virtual float getRadius() const = 0;

        /** @brief Render object. Only gltf_mesh can be changed, other matrices must NOT be changed */
        virtual void outputObject(glm::mat4& rel_pos) const = 0;
};

