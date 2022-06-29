#pragma once

#include <memory>
#include <map>
#include "managed_object.hxx"

#include "point_of_view.hxx"

class SpaceResolver;
class GravityProvider;
class ManagedObjectInstance;

class ObjectManager
{
    public:

        using ObjectUid = int;

        ObjectManager(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider
            );

        /** @brief Insert any object in the list of managed objects.
         * @param obj The bject to insert
         * @param pos Where to insert it (main position)
         * @return a UID that can be used later on to reference objects
         */
        ObjectUid insertObject(std::shared_ptr<ManagedObject> obj, PointOfView pos);

        /** @brief Retrieve an object's position. Mainly used to know where the user is,
         * so that drawing is made from that position.
         * @return true if object is found, false otherwise
         */
        bool getObjectPosition(ObjectUid objectId, PointOfView& pos);

        /** @brief update for the given time lapse. Also check EOL for objects
         * NOTE: total_time gives time from start of game play
        */
        void update(float total_time);

        /** manadatory due to fwd decslaration */
        virtual ~ObjectManager();

    private:

        SpaceResolver* spaceResolver;
        GravityProvider* gravityProvider;

        /** mapping unique ID to objects */
        std::map<ObjectUid, std::unique_ptr<ManagedObjectInstance>> managed_objects;

};

