#pragma once

#include "memory"
#include "point_of_view.hxx"

class ViewableObject;

class ViewablesRegistrar
{


    public:

        typedef unsigned int viewableId;
        typedef std::shared_ptr<ViewableObject> ViewableObjectPtr;

        virtual viewableId appendViewable(ViewableObjectPtr _object, PointOfView _pov) = 0;
        virtual void updateViewable(viewableId id, PointOfView newPov) = 0;
        virtual void removeViewable(viewableId id) = 0;

        virtual ~ViewablesRegistrar() = 0;
};

