#include <gtest/gtest.h>

#include "viewables_registrar_impl.hxx"
#include "viewable_object.hxx"

#include "common.hxx"

using namespace std;

static auto console = getConsole("ut_viewables_registrar_impl");

class ViewablesRegistrarImplUnitTest : public ::testing::Test
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

std::list<PointOfView> getPovList(std::list<std::string> rooms)
{
    std::list<PointOfView> result;
    for (auto &r : rooms)
    {
        result.push_back(PointOfView(glm::vec3(), glm::mat3x3(), r));
    }
    return result;
}

class ViewablesRegistrarImplTest : public ViewablesRegistrarImpl
{
    std::list<PointOfView> nextPositionsCall;

public:
    void setNextPositions(std::list<PointOfView> positions)
    {
        nextPositionsCall = positions;
    };

    virtual std::list<PointOfView> solvePosition(PointOfView mainPos) const override
    {
        return nextPositionsCall;
    };

    ViewablesRegistrarImplTest(){};
    virtual ~ViewablesRegistrarImplTest(){};

    ViewablesRegistrar::viewableId helperSetObjectId(std::shared_ptr<TestViewable> _object, std::list<std::string> rooms)
    {
        auto id = appendViewable(_object);
        setNextPositions(getPovList(rooms));
        updateViewable(id, PointOfView());
        _object->assigned_id = id;
        return id;
    }

    void helperUpdateObjectId(ViewablesRegistrar::viewableId id, std::list<std::string> rooms)
    {
        setNextPositions(getPovList(rooms));
        updateViewable(id, PointOfView());
    }
};

TEST_F(ViewablesRegistrarImplUnitTest, init)
{
    std::unique_ptr<ViewablesRegistrarImplTest> registrar = make_unique<ViewablesRegistrarImplTest>();
    auto obj_list = registrar->getViewables("room1");
    ASSERT_EQ(obj_list.size(), 0);
    registrar->dump();
}

bool checkContent(ViewablesRegistrarImpl *registrar, std::string room, std::list<ViewablesRegistrar::viewableId> list)
{
    auto content = registrar->getViewables(room);
    if (list.size() != content.size())
    {
        registrar->dump();
        return false;
    }
    for (auto id : list)
    {
        bool found = false;
        for (auto sid : content)
        {
            TestViewable *tv = dynamic_cast<TestViewable *>(sid.get());
            if (tv->assigned_id == id)
            {
                found = true;
            }
        }
        if (found == false)
        {
            registrar->dump();
            return false;
        }
    }
    return true;
}

TEST_F(ViewablesRegistrarImplUnitTest, add)
{
    std::unique_ptr<ViewablesRegistrarImplTest> registrar = make_unique<ViewablesRegistrarImplTest>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto my_obj_3 = make_shared<TestViewable>(3.0);
    auto id_1 = registrar->helperSetObjectId(my_obj_1, {"room1"});
    auto id_2 = registrar->helperSetObjectId(my_obj_2, {"room1", "room2"});
    auto id_3 = registrar->helperSetObjectId(my_obj_3, {"room2"});
    registrar->updateViewable(id_3, PointOfView());

    registrar->dump();

    ASSERT_TRUE(checkContent(registrar.get(), "room_none", {}));
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {id_2, id_3}));
}

TEST_F(ViewablesRegistrarImplUnitTest, remove)
{
    std::unique_ptr<ViewablesRegistrarImplTest> registrar = make_unique<ViewablesRegistrarImplTest>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto id_1 = registrar->helperSetObjectId(my_obj_1, {"room1"});
    auto id_2 = registrar->helperSetObjectId(my_obj_2, {"room1"});

    registrar->dump();

    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));

    // remove 1
    registrar->removeViewable(id_1);
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_2}));

    // remove 2
    registrar->removeViewable(id_2);
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {}));
}

TEST_F(ViewablesRegistrarImplUnitTest, update)
{
    std::unique_ptr<ViewablesRegistrarImplTest> registrar = make_unique<ViewablesRegistrarImplTest>();
    auto my_obj_1 = make_shared<TestViewable>(1.0);
    auto my_obj_2 = make_shared<TestViewable>(2.0);
    auto my_obj_3 = make_shared<TestViewable>(3.0);
    auto id_1 = registrar->helperSetObjectId(my_obj_1, {"room1"});
    auto id_2 = registrar->helperSetObjectId(my_obj_2, {"room1"});
    auto id_3 = registrar->helperSetObjectId(my_obj_3, {"room2"});

    registrar->dump();

    // move object3 to room3
    registrar->helperUpdateObjectId(id_3, {"room3"});
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {}));
    ASSERT_TRUE(checkContent(registrar.get(), "room3", {id_3}));

    // move object3 to room1 and room2
    registrar->helperUpdateObjectId(id_3, {"room1", "room2"});
    ASSERT_TRUE(checkContent(registrar.get(), "room1", {id_1, id_2, id_3}));
    ASSERT_TRUE(checkContent(registrar.get(), "room2", {id_3}));
    ASSERT_TRUE(checkContent(registrar.get(), "room3", {}));
}
