
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "common.hxx"
#include "gl_init.hxx"
#include "fonts.hxx"
#include "keyboard.hxx"

#include <stdlib.h>
#include <unistd.h>

#include "menu.hxx"
#include "game.hxx"

#include "gltf_material_accessor_library_iface.hxx"
#include "model_registry.hxx"
#include "fps_counter.hxx"

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

    FpsCounter fpsCounter;

    unique_ptr<Menu> menu = make_unique<Menu>(model_registry, flp);
    unique_ptr<Game> game = nullptr;

    bool menu_mode = true;

    do
    {
        unlockAllFbo();

        // Use our shader
        activateDefaultDrawingProgram();

        manageKeyboard();

        if (!modelPath.empty())
        {
            console->info("Start level ... {}", modelPath);

            auto ref = fl.getRoot().getSubPath(modelPath);
            game = std::make_unique<Game>(model_registry, flp, ref);

            menu_mode = false;
            menu->setStatus(true);
            modelPath = "";
        }

        if (game.get())
        {
            if (!menu_mode)
                updatePlayerInputs();

            if (game->manageGame(menu_mode)) // exited
            {
                console->info("Finished game");
                game = nullptr;
                menu_mode = true;
                menu->setStatus(true);
            }
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
