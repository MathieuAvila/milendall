#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "common.hxx"
#include "managed_object_instance.hxx"
#include "object_manager.hxx"
#include "gravity_provider.hxx"

#include <glm/gtx/string_cast.hpp>


using namespace testing;
using ::testing::_;

static auto console = getConsole("test_object_manager");

class ObjectManagerTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("object_manager")->set_level(spdlog::level::debug);
     spdlog::get("managed_object_instance")->set_level(spdlog::level::debug);
     spdlog::get("test_object_manager")->set_level(spdlog::level::debug);
  }
};

class FakeObject_ForObjectManager: public ManagedObject
{
    public:
    const MovableObjectDefinition def;
    const MovableObjectDefinition def_low;
    int interact_count;
    int interact_with_id;
    bool eol_ret;
    int id;
    bool low_interact;

    virtual const MovableObjectDefinition& getObjectDefinition() const override{
        if (low_interact)
            return def_low;
        return def;
    };
    virtual MovementWish getRequestedMovement() const override { return MovementWish(); };
    virtual glm::mat4x4 getOwnMatrix() const override { return glm::mat4x4(1.0f); };
    virtual bool checkEol() const override { return eol_ret; } ;

    FakeObject_ForObjectManager(int _id, bool _low_interact):ManagedObject(),
        def(2.0f, 3.0f, 4.0f, 0.5f),
        def_low(false, MovableObjectDefinition::InteractionLevel::LOW, 1.0),
        interact_count(0),
        interact_with_id(0),
        eol_ret(false),
        id(_id),
        low_interact(_low_interact)
    {}; // updates only 0.5 times as fast as time
    virtual ~FakeObject_ForObjectManager() {};

    virtual void interact(ManagedObject* second) override
    {
        interact_count++;
        interact_with_id = ((FakeObject_ForObjectManager*)second)->id;
        console->info("Interact {} with {} count {} ", id, interact_with_id, interact_count);
    }

};

class ObjectManagerTestClass : public ObjectManager
{
    public:
        ObjectManagerTestClass(
            std::shared_ptr<ModelRegistry> _model_registry,
            std::shared_ptr<FileLibrary> _library,
            SpaceResolver* _spaceResolver,
            GravityProvider* _gravityProvider,
            ViewablesRegistrar* _viewables_registrar = nullptr
            ) :
            ObjectManager(_model_registry, _library, _spaceResolver, _gravityProvider, _viewables_registrar)
        {};

        std::map<ObjectUid, std::unique_ptr<ManagedObjectInstance>>* get_managed_objects()
        {
            return &managed_objects;
        }

        virtual ~ObjectManagerTestClass() {};
};

std::tuple<std::shared_ptr<ManagedObjectInstance>, std::shared_ptr<FakeObject_ForObjectManager>> createObject(int id, bool low_interact = false)
{
    auto object = make_shared<FakeObject_ForObjectManager>(id, low_interact);
    PointOfView mainPosition(glm::vec3(1.0f, 2.0f, 3.0f), glm::mat3(1.0f), "room1");
    auto object_instance = std::make_shared<ManagedObjectInstance>(
        object,
        mainPosition,
        "",
        nullptr,
        nullptr,
        nullptr);
    return {object_instance, object};
}

std::shared_ptr<ObjectManagerTestClass> getObjectManager()
{
    return make_shared<ObjectManagerTestClass>(
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );
}

TEST_F(ObjectManagerTest, insertObject) {
    auto om = getObjectManager();
    auto [object_instance, object] = createObject(0);
    om->insertObject(object, PointOfView());
}

TEST_F(ObjectManagerTest, update__cleanup) {
    auto om = getObjectManager();
    auto [object_instance1, object1] = createObject(0);
    auto [object_instance2, object2] = createObject(1);
    auto [object_instance3, object3] = createObject(2);
    auto [object_instance4, object4] = createObject(3);
    om->insertObject(object1, PointOfView());
    auto id2 = om->insertObject(object2, PointOfView());
    om->insertObject(object3, PointOfView());
    auto id4 = om->insertObject(object4, PointOfView());
    om->update(1.0f);
    auto objs = om->get_managed_objects();
    ASSERT_EQ(objs->size(), 4);

    object1->eol_ret = true;
    object3->eol_ret = true;
    om->update(1.0f);
    ASSERT_EQ(objs->size(), 2);
    ASSERT_TRUE(objs->count(id2) == 1);
    ASSERT_TRUE(objs->count(id4) == 1);
}

TEST_F(ObjectManagerTest, update__interact) {
    auto om = getObjectManager();
    auto [object_instance1, object1] = createObject(1);
    auto [object_instance2, object2] = createObject(2);
    auto [object_instance3, object3] = createObject(3);
    auto [object_instance4, object4] = createObject(4);
    om->insertObject(object1, PointOfView(glm::vec3(0.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object2, PointOfView(glm::vec3(1.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object3, PointOfView(glm::vec3(5.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object3, PointOfView(glm::vec3(6.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->update(1.0f);
    auto objs = om->get_managed_objects();
    ASSERT_EQ(objs->size(), 4);

    ASSERT_EQ(object1->interact_count, 2);
    ASSERT_EQ(object1->interact_with_id, 2);

    ASSERT_EQ(object2->interact_count, 2);
    ASSERT_EQ(object2->interact_with_id, 1);
}

TEST_F(ObjectManagerTest, update__interact_different_room_no_interact) {
    auto om = getObjectManager();
    auto [object_instance1, object1] = createObject(1);
    auto [object_instance2, object2] = createObject(2);
    om->insertObject(object1, PointOfView(glm::vec3(0.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object2, PointOfView(glm::vec3(1.0, 0.0, 0.0), glm::mat3(1.0), "room2"));
    om->update(1.0f);
    auto objs = om->get_managed_objects();
    ASSERT_EQ(objs->size(), 2);

    ASSERT_EQ(object1->interact_count, 0);
    ASSERT_EQ(object2->interact_count, 0);
}


TEST_F(ObjectManagerTest, update__interact_low_interact) {
    auto om = getObjectManager();
    auto [object_instance1, object1] = createObject(1);
    auto [object_instance2, object2] = createObject(2, true);
    auto [object_instance3, object3] = createObject(3, true);
    auto [object_instance4, object4] = createObject(4, true);
    om->insertObject(object1, PointOfView(glm::vec3(0.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object2, PointOfView(glm::vec3(1.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object3, PointOfView(glm::vec3(5.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->insertObject(object4, PointOfView(glm::vec3(6.0, 0.0, 0.0), glm::mat3(1.0), "room1"));
    om->update(1.0f);
    auto objs = om->get_managed_objects();
    ASSERT_EQ(objs->size(), 4);

    ASSERT_EQ(object1->interact_count, 1);
    ASSERT_EQ(object2->interact_count, 1);

    ASSERT_EQ(object3->interact_count, 0);
    ASSERT_EQ(object4->interact_count, 0);
}



