#pragma once

#include <memory>
#include <map>
#include "managed_object.hxx"

struct RoomResolver;
class ManagedObjectInstance;

class ObjectManager
{
    public:

        ObjectManager(RoomResolver* _roomResolver);

        /** @brief Insert any object in the list of managed objects.
         * @param obj The bject to insert
         * @param pos Where to insert it (main position)
         * @return a UID that can be used later on to reference objects
         */
        int insertObject(std::shared_ptr<ManagedObject> obj, MovableObjectPosition pos);

        /** @brief Retrieve an object's position. Mainly used to know where the user is,
         * so that drawing is made from that position.
         * @return true if object is found, false otherwise */
        bool getObjectPosition(int objectId, MovableObjectPosition& pos);

        /** @brief update for the given time lapse. Also check EOL for objects
        */
        void update(float time_delta);

        /** manadatory due to fwd decslaration */
        virtual ~ObjectManager();

    private:

        RoomResolver* roomResolver;

        /** mapping unique ID to objects */
        std::map<int, std::unique_ptr<ManagedObjectInstance>> managed_objects;

};

