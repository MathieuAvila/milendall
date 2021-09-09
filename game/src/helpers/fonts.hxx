#pragma once
#pragma once

#include <glm/vec3.hpp>
#include <stdexcept>

#include "file_library.hxx"

/** @brief Font exception when a errors happens */
class FontException: public std::runtime_error
{
    public:

        FontException(const std::string err): runtime_error(err) {};
};

int fontInit();
void fontLoadFont(std::string id, FileLibrary::UriReference& font_name);
void fontRenderText(std::string ID, std::string text, float x, float y, float scale, glm::vec3 color);
void fontRenderTextBorder(std::string ID, std::string text, float x, float y, float scale, int border, glm::vec3 color, glm::vec3 borderColor);
