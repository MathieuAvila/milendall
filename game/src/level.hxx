#pragma once

#include "file_library.hxx"
#include "gltf/gltf_material_accessor_library_iface.hxx"
#include "room.hxx"
#include "level_exception.hxx"

struct LevelRoomResolver;

class Level
{
    public:

        /** build the level from the path to the final level file.
         * It's up to the called to have a naming scheme for levels */
        Level(FileLibrary::UriReference ref);

        /** To be able to preview a given room, give a list of them */
        std::list<std::string> getRoomNames();

        /** Rendering the level, from the position of a given room */
        void draw(PointOfView pov);

        /** Update, given an elapsed time */
        void update(float elapsed_time);

        /** mandatory for dtor of unique_ptr of fwd declaration LevelRoomResolver */
        virtual ~Level();

        /** get room resolver, for services that need access to particular rooms, like objet_manager */
        RoomResolver* getRoomResolver();

    private:

        /** Pass RoomResolver to rooms */
        std::unique_ptr<LevelRoomResolver> room_resolver;

        /** Identified by their IDs */
        std::map<std::string, std::shared_ptr<Room>> rooms;

        /** Kept for the duration of the level. This forces to keep texturing */
        GltfMaterialLibraryIfacePtr materialLibrary;
};

