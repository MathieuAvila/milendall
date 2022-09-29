
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "common.hxx"
#include "level.hxx"
#include "gl_init.hxx"
#include "fonts.hxx"

#include <stdlib.h>
#include <unistd.h>

#include "objects/player.hxx"
#include "objects/object_manager.hxx"
#include "objects/model_registry.hxx"

#include "helper_math.hxx"
#include "fps_counter.hxx"

#include "menu.hxx"

using namespace std;

static auto console = getConsole("Milendall");

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void help()
{
    console->info("preview_room [-h] -f model_file [-d path ...]");
    exit(1);
}

tuple<string, vector<string>> readParams(int argc, char *argv[])
{
    string modelPath;
    vector<string> libPaths;

    int c;
    while ((c = getopt(argc, argv, "f:d:h")) != -1)
    {
        switch (c)
        {
        case 'f':
            modelPath = optarg;
            break;
        case 'd':
            libPaths.push_back(optarg);
            break;
        case 'h':
            help();
            break;
        default:
            console->error("Unknown option {}", char(c));
            help();
        }
    }
    return tuple{modelPath, libPaths};
}

int main(int argc, char *argv[])
{
    std::string binpath = std::filesystem::path(argv[0]).parent_path();

    set_level(level::debug);

    auto [modelPath, libPaths] = readParams(argc, argv);

    auto flp = make_shared<FileLibrary>();
    auto &fl = *flp.get();

    /* Add some default directories */
    fl.addRootFilesystem(binpath + "/../data/");
    fl.addRootFilesystem(binpath + "/data/");
    /* Add cmd-line directories */
    for (auto p : libPaths)
        fl.addRootFilesystem(p);

    milendall_gl_init(fl);

    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();

    auto model_registry = make_shared<ModelRegistry>(materialLibrary);

    auto fontRegular = fl.getRoot().getSubPath("/common/fonts/fredoka-one.one-regular.ttf");
    auto fontCandy = fl.getRoot().getSubPath("/common/fonts/emilyscandy/EmilysCandy-Regular.ttf");
    fontLoadFont("regular", fontRegular);
    fontLoadFont("candy", fontCandy);

    auto current_time = std::chrono::steady_clock::now();
    auto total_time = 0.0f;

    FpsCounter fpsCounter;

    std::shared_ptr<Player> player;
    ObjectManager::ObjectUid player_id;
    std::unique_ptr<Level> level;
    std::unique_ptr<ObjectManager> object_manager;

    unique_ptr<Menu> menu = make_unique<Menu>(model_registry, flp);

    bool running_game = false;
    bool menu_mode = true;

    do
    {
        unlockAllFbo();

        // Use our shader
        activateDefaultDrawingProgram();

        if (!modelPath.empty())
        {
            console->info("Start level ... {}", modelPath);

            auto ref = fl.getRoot().getSubPath(modelPath);
            object_manager = make_unique<ObjectManager>(model_registry, flp);
            level = make_unique<Level>(ref, object_manager.get());
            auto room_ids = level.get()->getRoomNames();
            level.get()->update(0.0);
            PointOfView currentPov{
                level->getDefinition().start_position,
                mat4(1.0f),
                *room_ids.begin()};
            console->info("Set current room to {}", currentPov.room);

            player = make_shared<Player>();
            player_id = object_manager->insertObject(player,
                                                     PointOfView(currentPov.position, currentPov.local_reference, currentPov.room));
            player->addTime(level->getDefinition().recommended_time);

            running_game = true;
            menu_mode = false;
            menu->setStatus(running_game);
            modelPath = "";
        }

        if (running_game)
        {
            if (!menu_mode)
                updatePlayerInputs();

            PointOfView player_position;
            Player::ActionSet actionSet;
            actionSet.horizontalAngle = horizontalAngle;
            actionSet.verticalAngle = verticalAngle;
            actionSet.forward = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
            actionSet.backward = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
            actionSet.left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
            actionSet.right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
            actionSet.jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
            player->setActionSet(actionSet);

            bool found = object_manager->getObjectPosition(player_id, player_position);
            if (found == false)
                throw system_error();

            level.get()->draw(player_position);

            auto new_time = std::chrono::steady_clock::now();
            auto elapsed = float(std::chrono::duration_cast<std::chrono::microseconds>(new_time - current_time).count()) / (1000.0f * 1000.0f);
            total_time += elapsed;
            object_manager->update(total_time);
            level->update(elapsed);
            current_time = new_time;

            fontRenderTextBorder("regular", player_position.room, 25.0f, 720.0f, 1.0f, 2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
            fontRenderTextBorder("regular", std::string("Temps restant: ") + std::to_string(player->getLeftTime()), 550.0f, 720.0f, 1.0f, 2, glm::vec3(0.9, 0.7f, 0.3f), glm::vec3(0.1, 0.1f, 0.1f));
            fontRenderTextBorder("regular", vec3_to_string(player_position.position), 25.0f, 50.0f, 0.5f, 2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
        }

        if (menu_mode)
        {
            menu->printMenu();
            FileLibrary::UriReference ref = flp->getRoot();
            if (menu->get_selected_level(ref))
            {
                modelPath = ref.getPath();
            }
        }

        fpsCounter.update();
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    milendall_gl_close();

    return 0;
}
