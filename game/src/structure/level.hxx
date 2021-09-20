#pragma once

#include "file_library.hxx"
#include "gltf/gltf_material_accessor_library_iface.hxx"
#include "room.hxx"
#include "level_exception.hxx"
#include "space_resolver.hxx"

struct LevelRoomResolver;

class Level : public SpaceResolver
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

        /** Get a POV if a portal is crossed. This is used to draw in the right POV, as draw POV is higher (head) than
         * the player's sphere radius.
         * @param origin Is the original POV
         * @param destination is the desintation position in the origin's POV space
         * @result the final POV. It may be different than the one passed as origin.
         */
        virtual PointOfView getDestinationPov(const PointOfView& origin, const glm::vec3& destination) const override;

        /** @brief Check if a wall is reached. It checks if it goes through a portal,
         *         and makes the appropriate POV translation. So that the POV is returned and
         *         must always be considered as the "good" position
         * @param origin is the original position, ie. start of trajectory
         * @param destination is the end of trajectory
         * @param radius is the size of the object
         * @param endPoint is ALWAYS filled with the destination point, either where the wall is reached or the end portal.
         * @param normal is filled with the normal of the wall
         * @param distance is filled with the distance, if a wall is reached
         * @param face face that was hit
         * @return True if a wall was reached, if ever
        */
        virtual bool isWallReached(
            const PointOfView& origin,
            const glm::vec3& destination,
            const float radius,
            PointOfView& endPoint,
            PointOfView& destinationEndPoint,
            glm::vec3& normal,
            float& distance,
            FaceHard*& face
            ) const override;

    private:

        /** Pass RoomResolver to rooms */
        std::unique_ptr<LevelRoomResolver> room_resolver;

        /** Identified by their IDs */
        std::map<std::string, std::shared_ptr<Room>> rooms;

        /** Kept for the duration of the level. This forces to keep texturing */
        GltfMaterialLibraryIfacePtr materialLibrary;
};

