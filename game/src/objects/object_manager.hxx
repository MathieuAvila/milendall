#pragma once

#include <memory>
#include <map>
#include "managed_object.hxx"

#include "point_of_view.hxx"

#include "iface_object_loader.hxx"

class SpaceResolver;
class GravityProvider;
class ManagedObjectInstance;
class ViewablesRegistrar;

class ObjectManager: public IObjectLoader
{
    public:

        using ObjectUid = int;

        ObjectManager(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar = nullptr
            );

        ObjectManager();

        virtual void setReferences(
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar = nullptr) override;

        /** @brief Insert any object in the list of managed objects.
         * @param obj The bject to insert
         * @param pos Where to insert it (main position)
         * @param mesh_name mesh to insert it in, if any.
         * @return a UID that can be used later on to reference objects
         */
        ObjectUid insertObject(
            std::shared_ptr<ManagedObject> obj,
            PointOfView pos,
            std::string mesh_name = "");

        /** @brief Retrieve an object's position. Mainly used to know where the user is,
         * so that drawing is made from that position.
         * @return true if object is found, false otherwise
         */
        bool getObjectPosition(ObjectUid objectId, PointOfView& pos);

        /** @brief update for the given time lapse. Also check EOL for objects
         * NOTE: total_time gives time from start of game play
        */
        void update(float total_time);

        /** @brief From IObjectLoader */
        virtual void loadObject(std::string room_name, std::string mesh_name, nlohmann::json& root) override;

        /** @brief manadatory due to fwd decslaration */
        virtual ~ObjectManager();

    private:

        SpaceResolver* spaceResolver;
        GravityProvider* gravityProvider;
        ViewablesRegistrar* viewables_registrar;

        /** mapping unique ID to objects */
        std::map<ObjectUid, std::unique_ptr<ManagedObjectInstance>> managed_objects;

};

