#include <min_max_box.hxx>

MinMaxBox::MinMaxBox(glm::vec3 _minP, glm::vec3 _maxP): minP(_minP), maxP(_maxP)
{
}

bool MinMaxBox::isIn(glm::vec3 p)
{
    return
    ((p.x >= minP.x) && (p.y >= minP.y) && (p.z >= minP.z)
    && (p.x <= maxP.x) && (p.y <= maxP.y) && (p.z <= maxP.z));
}
