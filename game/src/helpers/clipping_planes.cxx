#include "common.hxx"
#include "helper_math.hxx"
#include "clipping_planes.hxx"

#include <numeric>

static auto console = getConsole("clipping_planes");

using namespace glm;

ClippingPlanes::ClippingPlanes() : ClippingPlanes(1.01, 1.01)
{
}

ClippingPlanes::ClippingPlanes(float width, float height)
{
    currentPoints = {
        glm::vec2(-width, -height),
        glm::vec2(-width, height),
        glm::vec2(width, height),
        glm::vec2(width, -height)
    };
}

glm::vec2 plane2DCutPoint(glm::vec3 plane, glm::vec2 p0, glm::vec2 p1)
{
    auto c0 = glm::dot(glm::vec3(p0.x, p0.y, 1.0f), plane);
    auto c1 = glm::dot(glm::vec3(p1.x, p1.y, 1.0f), plane);
    auto diff = (c1 - c0);
    if (diff == 0.0f) {
        console->error("Enable to find cutting line");
        return p0;
    }
    auto alpha = - c0 / (c1 - c0);
    auto p = p0 + alpha * (p1 - p0);
    return p;
}

void ClippingPlanes::clipByPlane(glm::vec3 plane)
{
    console->debug("clipByPlane, equation {}", vec3_to_string(plane));
    std::vector<glm::vec2> newCurrentPoints;
    // for every point, determine projection
    std::vector<float> projs;
    std::vector<bool> positives;
    int positive = 0;
    int total = currentPoints.size();
    for (auto& p: currentPoints) {
        auto proj = glm::dot(plane, glm::vec3(p.x, p.y, 1.0));
        projs.push_back(proj);
        console->debug("clipByPlane, proj {} = {}", vec3_to_string(glm::vec3(p.x, p.y, 1.0)), proj);
        positives.push_back(proj >= 0.0);
        if (proj >= 0.0f)
            positive++;
    }
    if (positive == 0) {
        currentPoints = newCurrentPoints;
        return;
    }
    if (positive == total) {
        return;
    }
    auto index_first_neg = 0;
    // find first negative
    while (positives[index_first_neg])
        index_first_neg++;
    console->debug("clipByPlane, index_first_neg {}", index_first_neg);
    // find first positive after first negative:
    auto index_first_pos = index_first_neg + 1;
    while (! positives[index_first_pos % total ])
        index_first_pos++;
    console->debug("clipByPlane, index_first_pos {}", index_first_pos);
    // now find the cutting point
    auto pPOS = currentPoints[ index_first_pos % total ];
    auto pNEG = currentPoints[ (index_first_pos-1) % total ];
    auto pCUT0 = plane2DCutPoint(plane, pPOS, pNEG);

    // advance to next negative
    auto index_next_neg = index_first_pos + 1;
    while (positives[ index_next_neg % total ])
        index_next_neg++;
    console->debug("clipByPlane, index_next_neg {}", index_next_neg);
    auto pNEG2 = currentPoints[ index_next_neg % total ];
    auto pPOS2 = currentPoints[ (index_next_neg-1) % total ];
    auto pCUT1 = plane2DCutPoint(plane, pNEG2, pPOS2);

    auto my_index = index_first_pos;

    while (positives[ my_index % total ]) {
        newCurrentPoints.push_back(currentPoints[my_index % total]);
        console->debug("clipByPlane, append index {}", my_index);
        my_index = my_index + 1;
    }
    newCurrentPoints.push_back(pCUT1);
    newCurrentPoints.push_back(pCUT0);

    // purge same points
    decltype(newCurrentPoints) purgedCurrentPoints;
    auto currentPoint = newCurrentPoints[0];
    purgedCurrentPoints.push_back(currentPoint);
    for (auto i = 1 ; i < newCurrentPoints.size(); i++) {
        auto newPoint = newCurrentPoints[i];
        if (newPoint != currentPoint) {
            currentPoint = newPoint;
            purgedCurrentPoints.push_back(newPoint);
            console->debug("clipByPlane, result {} = {},{}", i,
                newCurrentPoints[i].x,
                newCurrentPoints[i].y);
        }
    }
    // remove if first and last are same
    if (purgedCurrentPoints.front() == purgedCurrentPoints.back()) {
        purgedCurrentPoints.erase(purgedCurrentPoints.begin());
    }
    if (purgedCurrentPoints.size() < 3)
        purgedCurrentPoints.clear();
    currentPoints = purgedCurrentPoints;
}

void ClippingPlanes::clipByPolygon(std::vector<glm::vec3>& polygon)
{
    for (auto i = 0 ; i < polygon.size(); i++) {
        auto& p1 = polygon[i];
        auto& p2 = polygon[(i+1) % (polygon.size())];
        auto p0 = glm::vec3(0.0);
        auto cross = glm::cross(p1 - p0, p2 - p0);
        if (glm::length(cross) < 0.0001f) {
            currentPoints = {};
            return;
        }
        auto normal = glm::normalize(cross);
        auto plane = getPlaneEquation(p0, normal);
        clipByPlane(glm::vec3(plane.x, plane.y, plane.z));
    }
}

std::vector<glm::vec3> ClippingPlanes::getEquations()
{
    std::vector<glm::vec3> result;
    auto p0 = glm::vec3(0.0, 0.0, 0.0);
    int size = currentPoints.size();
    for (auto i = 0 ; i < size; i++) {
        auto& p1 = currentPoints[i];
        auto& p2 = currentPoints[(i+1) % size];
        glm::vec3 p1_3 = glm::vec3(p1.x, p1.y, 1.0);
        glm::vec3 p2_3 = glm::vec3(p2.x, p2.y, 1.0);
        glm::vec4 p = getPlaneEquation(p0, glm::cross(p1_3, p2_3));
        result.push_back(p);
    }
    return result;
}

std::string vec2toString(glm::vec2 a) {
    return std::to_string(a.x) + std::string(" , ") + std::to_string(a.y);
};

std::string ClippingPlanes::toString()
{
    if (currentPoints.size() == 0) return "";
    std::string delim, result;
    for (auto& i : currentPoints) {
        result += delim + vec2toString(i);
        delim = "\n";
    }
    return result;
}