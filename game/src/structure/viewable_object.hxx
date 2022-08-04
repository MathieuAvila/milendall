#pragma once

/** @brief Interface class for any object that needs to be drawn */
class ViewableObject
{
    public:

        /** @brief need this property to detect portal crossing. In that case the object may need to be drawn in
        multiple different rooms */
        virtual float getRadius() const = 0;

        /** @brief Render object. Other matrices must NOT be changed */
        virtual void outputObject() const = 0;
};

