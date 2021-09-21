
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

#include "helper_math.hxx"
#include "fps_counter.hxx"

using namespace std;

static auto console = getConsole("Milendall");

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void help()
{
    console->info("preview_room [-h] -d model_file [-f path ...]");
    exit(1);
}

tuple<string, vector<string> > readParams(int argc, char* argv[])
{
    string modelPath;
    vector<string> libPaths;

    int c;
    while ((c = getopt (argc, argv, "f:d:h")) != -1)
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
    if (modelPath.empty())
    {
        console->error("Need to specify 1 model path", c);
        help();
    }
    return tuple{modelPath, libPaths};
}

int main(int argc, char* argv[])
{
    set_level(level::debug);

    auto [modelPath, libPaths] = readParams(argc, argv);

    auto fl = FileLibrary();
    for (auto p: libPaths)
        fl.addRootFilesystem(p);

    milendall_gl_init(fl);

    auto ref = fl.getRoot().getSubPath(modelPath);
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();

    auto level = make_unique<Level>(ref);
    auto room_ids = level.get()->getRoomNames();
    level.get()->update(0.0);
    PointOfView currentPov{
            position,
            verticalAngle,
            horizontalAngle,
            mat4(1.0f), //glm::rotate(mat4(1.0f), 1.0f, glm::vec3(1.0, 0.0, 0.0) ),
            *room_ids.begin()};
    console->info("Set current room to {}", currentPov.room);

    auto fontRegular = fl.getRoot().getSubPath("/common/fonts/fredoka-one.one-regular.ttf");
    auto fontCandy = fl.getRoot().getSubPath("/common/fonts/emilyscandy/EmilysCandy-Regular.ttf");
    fontLoadFont("regular", fontRegular);
    fontLoadFont("candy", fontCandy);

    auto player = make_shared<Player>();
    auto object_manager = make_unique<ObjectManager>(level->getRoomResolver(), level.get());
    auto player_id = object_manager ->insertObject(player,
        PointOfView(currentPov.position, verticalAngle, horizontalAngle, currentPov.local_reference, currentPov.room)
    );

    auto current_time = std::chrono::steady_clock::now();

    FpsCounter fpsCounter;

	do{
        unlockAllFbo();

		// Use our shader
		activateDefaultDrawingProgram();

        computeMatricesFromInputs();

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

    	setMeshMatrix(glm::mat4(1.0));
        level.get()->draw(player_position);

        auto new_time = std::chrono::steady_clock::now();
        auto elapsed = float(std::chrono::duration_cast<std::chrono::microseconds>(new_time - current_time).count())
            /(1000.0f*1000.0f);
        object_manager->update(elapsed);
        current_time = new_time;

        fpsCounter.update();

        fontRenderTextBorder("regular", player_position.room, 25.0f, 720.0f,  1.0f,  2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));

        fontRenderTextBorder("regular", vec3_to_string(player_position.position), 25.0f, 50.0f,  0.5f,  2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
        fontRenderTextBorder("regular", player_position.room, 25.0f, 25.0f,  0.5f,  2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    milendall_gl_close();

	return 0;
}

