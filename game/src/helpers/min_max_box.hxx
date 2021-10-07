#pragma once

#include <glm/vec3.hpp>
#include <string>

class MinMaxBox {

    glm::vec3 minP;
    glm::vec3 maxP;

    bool defined; // If Min and Max are defined somehow
    bool full;    // If not defined, isInt returns either always TRUE or FALSE

    public:
        MinMaxBox();
        MinMaxBox(bool _full);
        MinMaxBox(glm::vec3 _minP, glm::vec3 _maxP);
        bool isIn(glm::vec3 p);
        std::string toString();
};