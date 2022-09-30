
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common.hxx"
#include "gl_init.hxx"
#include "fonts.hxx"

#include "menu.hxx"

using namespace std;

static auto console = getConsole("menu");

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void Menu::setStatus(bool _running_game)
{
    running_game = _running_game;
}

Menu::Menu(std::shared_ptr<ModelRegistry> _model_registry, std::shared_ptr<FileLibrary> _file_library)
: file_library(_file_library), model_registry(_model_registry)
{
    running_game = false;
    sub_menu = MENU_LEVEL;
    menu_index = 0;
    menu_pos = 0;
    sub_menu = MenuType::MENU_LEVEL;

    auto levels_ref = file_library->searchFile("level.json");
    for (auto ref : levels_ref)
    {
        levels_ref_array.push_back(LevelInfo(ref));
        console->info("Found level {}", ref.getPath());
    }
}

void Menu::printMenu()
{
    bool key_back = (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) && !was_key_back;
    bool key_enter = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && !was_key_enter;
    bool key_up = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && !was_key_up;
    bool key_down = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && !was_key_down;

    was_key_back = (glfwGetKey(window, GLFW_KEY_BACKSPACE) != GLFW_RELEASE);
    was_key_enter = (glfwGetKey(window, GLFW_KEY_ENTER) != GLFW_RELEASE);
    was_key_up = (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE);
    was_key_down = (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE);

    switch (sub_menu)
    {
    case MenuType::MENU_MAIN:
    {
        break;
    }
    case MenuType::MENU_LEVEL:
    {
        //console->info("Level");
        fontRenderTextBorder("regular", "Selectionnez un niveau", 80.0f, 550.0f, 1.0f, 2, glm::vec3(1.0, 0.8f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
        for (auto i = 0; i != 10; i++)
        {
            int index = i + menu_index;
            //console->info("Level {}, {}", index, levels_ref_array.size());
            if ((index < levels_ref_array.size()) && (index >= 0))
            {
                auto level_def = levels_ref_array[index];
                //console->info("Level {}, {}", index, levels_ref_array.size());
                fontRenderTextBorder("regular", level_def.section,                          50.0f, 500.0f - i * 50, 0.5f, 2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
                fontRenderTextBorder("regular", std::to_string(level_def.recommended_time), 150.0f, 500.0f - i * 50, 0.5f, 2, glm::vec3(0.1, 0.9f, 0.0f), glm::vec3(0.1, 0.1f, 0.1f));
                fontRenderTextBorder("regular", level_def.name,                             200.0f, 500.0f - i * 50, 0.5f, 2, glm::vec3(0.8, 0.7f, 0.3f), glm::vec3(0.1, 0.1f, 0.1f));
                fontRenderTextBorder("regular", level_def.ref.getPath(),                    700.0f, 500.0f - i * 50, 0.5f, 2, glm::vec3(0.1, 0.9f, 0.0f), glm::vec3(0.1, 0.1f, 0.1f));
            }
        }
        fontRenderTextBorder("regular", ">", 20.0f, 500.0f - menu_pos * 50, 0.5f, 2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
        if (key_up) {
            if (menu_pos > 0)
                menu_pos--;
            console->info("Up {}", menu_pos);
        }
        if (key_down) {
            if (menu_pos < levels_ref_array.size() -1)
                menu_pos++;
            console->info("Down {}", menu_pos);
        }
        if (key_enter) {
            selected_level = menu_pos;
            has_selected_level = true;
        }
        break;
    }
    case MenuType::MENU_CREDITS:
    {
        break;
    }
    }
}

bool Menu::get_selected_level(FileLibrary::UriReference& ref)
{
    if (has_selected_level) {
        has_selected_level = false;
        ref = levels_ref_array[selected_level].ref;
        return true;
    }
    return false;
}
