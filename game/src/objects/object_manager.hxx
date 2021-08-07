#pragma once

#include <memory>
#include "managed_object.hxx"

struct RoomResolver;

class ObjectManager
{
    public:

        ObjectManager(RoomResolver* _roomResolver);

        // Insert any object in the list of managed objects.
        void insertObject(std::shared_ptr<ManagedObject>);

        // update for the given time lapse.
        // also check EOL for objects
        void update(float time_delta);

    private:

        RoomResolver* roomResolver;
        std::list<std::shared_ptr<ManagedObject>> managed_objects;

};

