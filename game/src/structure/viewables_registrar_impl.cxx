#include "viewables_registrar_impl.hxx"
#include "viewable_object.hxx"

ViewablesRegistrarImpl::ViewablesRegistrarImpl()
{
}

ViewablesRegistrarImpl::~ViewablesRegistrarImpl()
{
}

ViewablesRegistrar::viewableId ViewablesRegistrarImpl::appendViewable(ViewableObjectPtr _object, PointOfView _pov)
{
    return 0;
}

void ViewablesRegistrarImpl::updateViewable(ViewablesRegistrar::viewableId id, PointOfView newPov)
{
}

void ViewablesRegistrarImpl::removeViewable(ViewablesRegistrar::viewableId id)
{
}
