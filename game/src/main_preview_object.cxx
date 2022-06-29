
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "common.hxx"
#include "gltf_model.hxx"
#include "gl_init.hxx"
#include "fonts.hxx"

#include <stdlib.h>
#include <unistd.h>

#include "helper_math.hxx"

using namespace std;

static auto console = getConsole("object_preview");

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void help()
{
    console->info("preview_object [-h] -f model_file [-d path ...]");
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

    auto font1 = fl.getRoot().getSubPath("/common/fonts/fredoka-one.one-regular.ttf");
    auto font2 = fl.getRoot().getSubPath("/common/fonts/emilyscandy/EmilysCandy-Regular.ttf");
    fontLoadFont("1", font1);
    fontLoadFont("2", font2);

    auto ref = fl.getRoot().getSubPath(modelPath);
    GltfMaterialLibraryIfacePtr materialLibrary = GltfMaterialLibraryIface::getMaterialLibray();


    glm::mat4 position;

    GltfModel model(materialLibrary, ref);
    GltfInstance instance(model.getInstanceParameters());
    model.applyDefaultTransform(&instance, position);

	do{
        unlockAllFbo();

		activateDefaultDrawingProgram();

		updatePlayerInputs();

        //fontRenderTextBorder("1", std::string("Position: ") + vec3_to_string(position), 25.0f, 25.0f, 1.0f, 2, glm::vec3(0.5, 0.8f, 0.2f), glm::vec3(0.1, 0.1f, 0.1f));
        //fontRenderTextBorder("2", std::string("Watching room: ") + *current_room, 25.0f, 100.0f,  1.0f,  2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));

        model.draw(&instance);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    milendall_gl_close();

	return 0;
}

