#include <gtest/gtest.h>

#include "viewables_registrar_impl.hxx"
#include "viewable_object.hxx"

#include "common.hxx"

using namespace std;

static auto console = getConsole("ut_viewables_registrar_impl");

class ViewablesRegistrarImplTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        spdlog::get("ut_viewables_registrar_impl")->set_level(spdlog::level::debug);
        spdlog::get("viewables_registrar_impl")->set_level(spdlog::level::debug);
    }
};

class TestViewable : public ViewableObject
{
public:
    float radius;
    ViewablesRegistrar::viewableId assigned_id;

    TestViewable(float _radius) : radius(_radius){};

    virtual float getRadius() const override
    {
        return radius;
    }

    virtual ~TestViewable(){};

    virtual void outputObject(glm::vec4 masterMatrix) const override{

    };
};

TEST_F(ViewablesRegistrarImplTest, init)
{
    std::unique_ptr<ViewablesRegistrarImpl> registrar = make_unique<ViewablesRegistrarImpl>();
    auto obj_list = registrar->getViewables("room1");
    ASSERT_EQ(obj_list.size(), 0);
    registrar->dump();
}

std::list<PointOfView> getPovList(std::list<std::string> rooms)
{
    std::list<PointOfView> result;
    for (auto& r: rooms) {
        result.push_back(PointOfView(glm::vec3(), glm::mat3x3(), r));
    }
    return result;
}

bool checkContent(ViewablesRegistrarImpl* registrar, std::string room, std::list<ViewablesRegistrar::viewableId> list)
{
    auto content = registrar->getViewables(room);
    if (list.size() != content.size()) {
        registrar->dump();
        return false;
    }
    for (auto id : list) {
        bool found = false;
        for (auto sid : content) {
            TestViewable* tv = dynamic_cast<TestViewable*>(sid.get());
            if (tv->assigned_id == id) {
                found = true;
            }
        }
        if (found == false) {
            registrar->dump();
            return false;
        }
    }
    return true;
}

TEST_F(ViewablesRegistrarImplTest, add)
{
    std::unique_ptr<ViewablesRegistrarImpl> registrar = make_unique<ViewablesRegistrarImpl>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto my_obj_3 = make_shared<TestViewable>(3.0);
    auto id_1 = registrar->appendViewable(my_obj_1);
    registrar->updateViewable(id_1, getPovList({"room1"}));
    my_obj_1->assigned_id = id_1;
    auto id_2 = registrar->appendViewable(my_obj_2);
    registrar->updateViewable(id_2, getPovList({"room2", "room1"}));
    my_obj_2->assigned_id = id_2;
    auto id_3 = registrar->appendViewable(my_obj_3);
    my_obj_3->assigned_id = id_3;
    registrar->updateViewable(id_3, getPovList({"room2"}));

    registrar->dump();

    ASSERT_TRUE(checkContent(registrar.get(), "room_none", {}));
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {id_2, id_3}));

}

TEST_F(ViewablesRegistrarImplTest, remove)
{
    std::unique_ptr<ViewablesRegistrarImpl> registrar = make_unique<ViewablesRegistrarImpl>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto id_1 = registrar->appendViewable(my_obj_1);
    registrar->updateViewable(id_1, getPovList({"room1"}));
    my_obj_1->assigned_id = id_1;
    auto id_2 = registrar->appendViewable(my_obj_2);
    registrar->updateViewable(id_2, getPovList({"room1"}));
    my_obj_2->assigned_id = id_2;

    registrar->dump();

    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));

    // remove 1
    registrar->removeViewable(id_1);
    ASSERT_TRUE(checkContent(registrar.get(), "room1", { id_2}));

    // remove 2
    registrar->removeViewable(id_2);
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {}));
}

TEST_F(ViewablesRegistrarImplTest, update)
{
    std::unique_ptr<ViewablesRegistrarImpl> registrar = make_unique<ViewablesRegistrarImpl>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto my_obj_3 = make_shared<TestViewable>(3.0);
    auto id_1 = registrar->appendViewable(my_obj_1);
    registrar->updateViewable(id_1, getPovList({"room1"}));
    my_obj_1->assigned_id = id_1;
    auto id_2 = registrar->appendViewable(my_obj_2);
    registrar->updateViewable(id_2, getPovList({"room1"}));
    my_obj_2->assigned_id = id_2;
    auto id_3 = registrar->appendViewable(my_obj_3);
    registrar->updateViewable(id_3, getPovList({"room2"}));
    my_obj_3->assigned_id = id_3;

    registrar->dump();


    // move object3 to room3
    registrar->updateViewable(id_3, getPovList({"room3"}));
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {}));
    ASSERT_TRUE(checkContent(registrar.get(), "room3", {id_3}));

    // move object3 to room1 and room2
    registrar->updateViewable(id_3, getPovList({"room1", "room2"}));
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2, id_3}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {id_3}));
    ASSERT_TRUE(checkContent(registrar.get(), "room3", {}));
}
