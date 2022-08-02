#pragma once

#include "viewables_registrar.hxx"
#include "map"
#include "mutex"

class ViewableObject;

/** @brief Real implementation of an viewable object registrar. This one should be used by Level. */
class ViewablesRegistrarImpl : public ViewablesRegistrar
{

    /** @brief Storing information for a viewable object. ID is in the enclosing map */
    typedef struct ViewableObjectInfo_s
    {
        /** @brief the object itself */
        ViewableObjectPtr object;
        /** @brief the list of room it is implied in with the associated positions. Must be in sync with mapRoomObject */
        std::list<PointOfView> povList;

    } ViewableObjectInfo;

    /** @brief keep this up to date to have a way to quickly access the list of objects associated to a room */
    std::map<std::string, std::list<viewableId>> mapRoomObject;

    /** @brief Full list of objects */
    std::map<viewableId, ViewableObjectInfo> objects;

    /** @brief object counter */
    viewableId counter;

    /** @brief global mutex */
    std::mutex access_mutex;

    public:

        ViewablesRegistrarImpl();
        virtual ~ViewablesRegistrarImpl();

        virtual viewableId appendViewable(ViewableObjectPtr _object) override;
        virtual void updateViewable(viewableId id, std::list<PointOfView> positions) override;
        virtual void removeViewable(viewableId id) override;

        /** @brief Get objects in a room ready to be drawn */
        virtual std::list<ViewableObjectPtr> getViewables(std::string room);

        /** @brief for debug purposes */
        void dump();
};

