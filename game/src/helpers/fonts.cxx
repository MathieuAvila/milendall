#include "fonts.hxx"

#include <GL/glew.h>

#include <map>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "gl_init.hxx"
#include "common.hxx"

using namespace glm;

static auto console = getConsole("font");

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int texturedTextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

struct FontData {
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
};

std::map<std::string, FontData> fontDataMap;

void fontLoadFont(std::string id, FileLibrary::UriReference& font_name)
{
    FT_Library ft;
    FontData fontData;

    if (FT_Init_FreeType(&ft))
        throw FontException("Unable to init freetype");

    FT_Face face;
    FileContentPtr content = font_name.readContent();

    if (FT_New_Memory_Face(ft, content.get()->data(), content.get()->size(), 0, &face))
        throw FontException("ERROR::FREETYPE: Failed to load font" + font_name.getPath());

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
       // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            console->warn("ERROR::FREETYTPE: Failed to load Glyph {}", c);
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        fontData.Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenBuffers(1, &fontData.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, fontData.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    fontDataMap.insert(std::pair<std::string, FontData>(id, fontData));
}

void fontRenderText(std::string ID, std::string text, float x, float y, float scale, glm::vec3 color)
{
    FontData& fd = fontDataMap[ID];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // activate corresponding render state
    unsigned int fontProgram = activateFontDrawingProgram();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1024), 0.0f, static_cast<float>(768));
    glUniformMatrix4fv(glGetUniformLocation(fontProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3f(glGetUniformLocation(fontProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    //glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, fd.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = fd.Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texturedTextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, fd.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    //glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void fontRenderTextBorder(std::string ID, std::string text, float x, float y, float scale, int border, glm::vec3 color, glm::vec3 borderColor)
{
    for (auto i = -border; i <= border ; i++)
            for (auto j = -border; j <= border ; j++) if (i != 0 && j != 0)
                fontRenderText(ID, text, x + i,  y + j, scale, borderColor);
    fontRenderText(ID, text, x, y, scale, color);
}
