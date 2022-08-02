#pragma once

#include "memory"
#include "point_of_view.hxx"
#include "list"

class ViewableObject;

/** @brief A container for Viewable objects, this allows to separate:
 * - the management of objects themselves: interaction with the world and
 *   between them
 * - outputting them on screen.
 *
 * Doing so, it can be computed and output in separate threads, with minimal
 * interaction.
 * Viewable Objects must be provided as intermediate objects that have a
 * different life cycle from their management counter-parts.
 */
class ViewablesRegistrar
{

    public:

        typedef unsigned int viewableId;
        typedef std::shared_ptr<ViewableObject> ViewableObjectPtr;

        virtual viewableId appendViewable(ViewableObjectPtr _object) = 0;
        virtual void updateViewable(viewableId id, std::list<PointOfView> positions) = 0;
        virtual void removeViewable(viewableId id) = 0;

        virtual ~ViewablesRegistrar();
};

