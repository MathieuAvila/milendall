#pragma once

#include <glm/vec3.hpp>
#include <string>

class MinMaxBox {

    glm::vec3 minP;
    glm::vec3 maxP;

    public:
        MinMaxBox(glm::vec3 _minP, glm::vec3 _maxP);
        bool isIn(glm::vec3 p);
        std::string toString();
};