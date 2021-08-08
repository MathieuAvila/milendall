
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "common.hxx"
#include "level.hxx"
#include "gl_init.hxx"

#include <stdlib.h>
#include <unistd.h>

#include "objects/player.hxx"
#include "objects/object_manager.hxx"

using namespace std;

static auto console = spdlog::stdout_color_mt("room_preview");

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
    //auto room = make_unique<Room>(materialLibrary, ref);
    //room->applyTransform();
    auto level = make_unique<Level>(ref);
    auto room_ids = level.get()->getRoomNames();
    level.get()->update(0.0);
    auto current_room = room_ids.begin();
    console->info("Set current room to {}", *current_room);

    auto new_space_state = GLFW_RELEASE;
    auto space_state = GLFW_RELEASE;

    auto player = make_shared<Player>();
    auto object_manager = make_unique<ObjectManager>(level->getRoomResolver());
    auto player_id = object_manager ->insertObject(player,
        MovableObjectPosition{
            glm::vec3( 2, 2, 2 ),
            glm::vec3( 1.0, 0, 0),
            glm::vec3( 0.0, 0, 0),
            glm::vec3( 1.0, 0, 0),
            *room_ids.begin()}
    );

    auto current_time = std::chrono::steady_clock::now();

	do{
        unlockAllFbo();

		// Use our shader
		activateDefaultDrawingProgram();

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();

		setMeshMatrix(glm::mat4(1.0));

        MovableObjectPosition player_position;
        bool found = object_manager->getObjectPosition(player_id, player_position);
        if (found == false)
            throw system_error();

        level.get()->draw(*current_room, position, direction, up);

        auto new_time = std::chrono::steady_clock::now();
        auto elapsed = float(std::chrono::duration_cast<std::chrono::microseconds>(new_time - current_time).count())
            /(1000.0f*1000.0f);
        console->info("Elapsed time={}", elapsed);
        object_manager->update(elapsed);
        current_time = new_time;

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        // iterate through rooms with SPACE
        new_space_state = glfwGetKey(window, GLFW_KEY_SPACE);
        if ((new_space_state == GLFW_PRESS) && (space_state != new_space_state))
        {
            current_room++;
            if (current_room == room_ids.end())
                current_room = room_ids.begin();
            console->info("Current room set to {}", *current_room);
        }
        space_state = new_space_state;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    milendall_gl_close();

	return 0;
}

