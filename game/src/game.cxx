
#include <vector>
#include <memory>
#define GLM_FORCE_MESSAGES
#include <glm/glm.hpp>

#include "game.hxx"

#include "common.hxx"
#include "file_library.hxx"
#include "model_registry.hxx"
#include "object_manager.hxx"
#include "level.hxx"
#include "player.hxx"
#include "fonts.hxx"

using namespace std;

static auto console = getConsole("game");

Game::Game(
    std::shared_ptr<ModelRegistry> _model_registry,
    std::shared_ptr<FileLibrary> _file_library,
    FileLibrary::UriReference &levelRef):
    file_library(_file_library), model_registry(_model_registry),
    exit_enter_pressed(false)
{
    object_manager = make_unique<ObjectManager>(model_registry, _file_library);
    level = make_unique<Level>(levelRef, object_manager.get());
    auto room_ids = level.get()->getRoomNames();
    level.get()->update(0.0);
    PointOfView currentPov{
        level->getDefinition().start_position,
        glm::mat4(1.0f),
        level->getDefinition().start_room};
    console->info("Set start room to {}", currentPov.room);
    console->info("Set start position to {}", vec3_to_string(level->getDefinition().start_position));

    player = make_shared<Player>();
    player_id = object_manager->insertObject(player,
                                             PointOfView(currentPov.position, currentPov.local_reference, currentPov.room));
    player->addTime(level->getDefinition().recommended_time);
    current_time = std::chrono::steady_clock::now();
}

bool Game::manageGame(bool inMenu)
{
    PointOfView player_position;
    Player::ActionSet actionSet;

    int time_left;
    bool exited;
    bool exit_game = false;
    player->getGameStatus(time_left, exited);
    actionSet.horizontalAngle = horizontalAngle;
    actionSet.verticalAngle = verticalAngle;
    if (!exited) {
        actionSet.forward = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        actionSet.backward = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        actionSet.left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        actionSet.right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        actionSet.jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            exit_enter_pressed = true;
            console->debug("End game, enter was pressed");
        }
        if ((exit_enter_pressed)&&(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)) {
            console->debug("End game, enter was pressed and released");
            exit_game = true;
        }
    }
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
    if (!exited) {
        fontRenderTextBorder("regular", std::string("Temps:") + std::to_string(time_left), 550.0f, 720.0f, 1.0f, 2, glm::vec3(0.9, 0.7f, 0.3f), glm::vec3(0.1, 0.1f, 0.1f));
    }
    else {
        fontRenderTextBorder(
            "candy",
            std::string("Gagne !"),
            300.0f, 350.0f, 3.0f, 2, glm::vec3(0.2, 0.7f, 0.2f), glm::vec3(0.8, 0.8f, 0.8f)
        );
        fontRenderTextBorder(
            "candy",
            std::string("Temps restant: ") + std::to_string(time_left),
            200.0f, 200.0f, 1.5f, 2, glm::vec3(0.5, 0.7f, 0.5f), glm::vec3(0.5, 0.5f, 0.5f)
        );
    }
    return exit_game;
}

Game::~Game()
{
}
