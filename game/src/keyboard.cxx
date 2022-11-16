#include <map>

#include "common.hxx"
#include "gl_init.hxx"
#include "keyboard.hxx"

static auto console = getConsole("keyboard");

typedef struct
{
    bool was_pressed;
    bool is_pressed;
} KeyInfo;

static std::map<int, KeyInfo> keyMap;


void manageKeyboard()
{
    for (auto& key: keyMap) {
        bool pressed = (glfwGetKey(window, key.first) == GLFW_PRESS);
        key.second.is_pressed = pressed && !key.second.was_pressed;
        if (key.second.is_pressed)
            console->debug("Pressed: {} was:{}", key.first, key.second.was_pressed);
        key.second.was_pressed = pressed;
    }
}

bool getKeyPressedOnce(int key)
{
    if (!keyMap.count(key)) {
        console->debug("Add key listener: {}", key);
        keyMap.insert({key, KeyInfo{false, false} });
    }
    return keyMap[key].is_pressed;
}
