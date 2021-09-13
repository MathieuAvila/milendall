#pragma once

#include <memory>
#include <map>
#include "managed_object.hxx"

#include "../point_of_view.hxx"

struct RoomResolver;
class SpaceResolver;
class ManagedObjectInstance;

class ObjectManager
{
    public:

        ObjectManager(RoomResolver* _roomResolver, SpaceResolver* _spaceResolver);

        /** @brief Insert any object in the list of managed objects.
         * @param obj The bject to insert
         * @param pos Where to insert it (main position)
         * @return a UID that can be used later on to reference objects
         */
        int insertObject(std::shared_ptr<ManagedObject> obj, PointOfView pos);

        /** @brief Retrieve an object's position. Mainly used to know where the user is,
         * so that drawing is made from that position.
         * @return true if object is found, false otherwise
         */
        bool getObjectPosition(int objectId, PointOfView& pos);

        /** @brief update for the given time lapse. Also check EOL for objects
         * NOTE: time_delta is passed so that it can be UT.
        */
        void update(float time_delta);

        /** manadatory due to fwd decslaration */
        virtual ~ObjectManager();

    private:

        RoomResolver* roomResolver;
        SpaceResolver* spaceResolver;

        /** mapping unique ID to objects */
        std::map<int, std::unique_ptr<ManagedObjectInstance>> managed_objects;

};

