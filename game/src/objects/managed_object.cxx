#include "managed_object.hxx"

std::shared_ptr<ViewableObject> ManagedObject::getViewable() const
{
    return nullptr;
}

bool ManagedObject::addTime(float time)
{
    return false;
}

void ManagedObject::interact(ManagedObject* second_object)
{
}
