#pragma once

#include "viewables_registrar.hxx"
#include "list"
#include "map"
#include "mutex"

class ViewableObject;

class ViewablesRegistrarImpl : public ViewablesRegistrar
{

    typedef struct ViewableObjectInfo_s
    {
        /** @brief the object itself */
        ViewableObjectPtr object;
        /** @brief the list of room it is implied in. Must be in sync with mapRoomObject */
        std::list<std::string> roomList;

    } ViewableObjectInfo;

    /** @brief keep this up to date to have a way to quickly access the list of objects associated to a room */
    std::map<std::string, std::string> mapRoomObject;

    /** @brief Full list of objects */
    std::map<viewableId, ViewableObjectInfo> objects;

    /** @brief object counter */
    viewableId counter;

    /** @brief global mutex */
    std::mutex access_mutex;

    /** @brief global lock */
    std::unique_lock<std::mutex> access_lock;

    public:

        ViewablesRegistrarImpl();
        virtual ~ViewablesRegistrarImpl();

        virtual viewableId appendViewable(ViewableObjectPtr _object, PointOfView _pov) override;
        virtual void updateViewable(viewableId id, PointOfView newPov) override;
        virtual void removeViewable(viewableId id) override;

        /** @brief Get objects in a room ready to be drawn */
        std::list<ViewableObjectPtr> getViewables(std::string room);

};

