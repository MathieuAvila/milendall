#pragma once

#include "file_library.hxx"
#include "objects/model_registry.hxx"
#include <vector>
#include <memory>

class Menu
{
    bool running_game;
    typedef enum
    {
        MENU_MAIN,
        MENU_LEVEL,
        MENU_CREDITS
    } MenuType;
    MenuType sub_menu;

    int menu_index = 0; // in multi entry sub-menu, start. Eg: level list
    int menu_pos = 0;   // in simple sub-menu, position
    bool was_key_back;
    bool was_key_enter;
    bool was_key_up;
    bool was_key_down;

    std::shared_ptr<FileLibrary> file_library;
    std::vector<FileLibrary::UriReference> levels_ref_array;
    std::shared_ptr<ModelRegistry> model_registry;

    int selected_level;
    bool has_selected_level;

    public:

    Menu(std::shared_ptr<ModelRegistry> _model_registry, std::shared_ptr<FileLibrary> _file_library);

    void setStatus(bool _running_game);

    void printMenu();

    bool get_selected_level(FileLibrary::UriReference& ref);

};
