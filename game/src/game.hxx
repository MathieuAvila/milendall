#pragma once

#include <memory>
#include "file_library.hxx"

#include <chrono>

class ModelRegistry;
class Player;
class Level;
class ObjectManager;

class Game
{

    std::shared_ptr<FileLibrary> file_library;
    std::shared_ptr<ModelRegistry> model_registry;
    std::shared_ptr<Player> player;
    int player_id;
    std::unique_ptr<Level> level;
    std::unique_ptr<ObjectManager> object_manager;
    std::chrono::_V2::steady_clock::time_point current_time;
    float total_time = 0.0f;
    bool exit_enter_pressed; // when finished, mark that enter is pressed. wait release before exit.

    public:

    Game(std::shared_ptr<ModelRegistry> _model_registry, std::shared_ptr<FileLibrary> _file_library, FileLibrary::UriReference& levelRef);

    void setStatus(bool _running_game);
    bool manageGame(bool inMenu);

    // need this for all the unique_ptrs declarations.
    virtual ~Game();
};
