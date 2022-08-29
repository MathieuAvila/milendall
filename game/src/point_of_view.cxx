#include "point_of_view.hxx"

#include "helper_math.hxx"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

PointOfView PointOfView::changeCoordinateSystem(std::string newRoom, const glm::mat4& newMatrix) const
{
    glm::mat3x3 no_trans(newMatrix);
    return PointOfView(
        newMatrix * positionToVec4(position),
        no_trans * local_reference,
        newRoom
    );
}

PointOfView PointOfView::changePositionAndCoordinateSystem(const glm::mat4& newMatrix) const
{
    auto new_pos = newMatrix * positionToVec4(position);
    glm::mat3x3 no_trans(newMatrix);

    return PointOfView(
        new_pos,
        no_trans * local_reference,
        room
    );
}

PointOfView PointOfView::prependCoordinateSystem(const glm::mat3& newMatrix) const
{
    glm::mat3x3 no_trans(newMatrix);
    return PointOfView(
        position,
        local_reference * no_trans,
        room
    );
}

bool PointOfView::operator==(const PointOfView& p) const
{
    static const float epsilon = 0.001f;
    auto diff = local_reference - p.local_reference;
    auto s = 0.0;
    for (auto i = 0; i<3; i++)
        for (auto j = 0; j<3; j++)
            s += std::abs(diff[i][j]);
    return (glm::all(glm::lessThan(glm::abs(position - p.position), glm::vec3(epsilon)))
        && (s < epsilon)
        && (room == p.room));
}

std::string to_string(const PointOfView& p)
{
    return std::string(
      "\nr:") + p.room
    + "\np: " + vec3_to_string(p.position)
    + "\nr: " + mat4x4_to_string(p.local_reference);
}

glm::vec3 PointOfView::getDirection() const
{
    return local_reference * glm::vec3(0.0, 0.0, 1.0);
}

glm::vec3 PointOfView::getUp() const
{
    return local_reference * glm::vec3(0.0, 1.0, 0.0);
}

glm::vec3 PointOfView::getRight() const
{
    return local_reference * glm::vec3(1.0, 0.0, 0.0);
}


PointOfView::PointOfView(
    const glm::vec3 _position,
    const glm::mat3x3 _local_reference,
    const std::string _room):
position(_position), local_reference(_local_reference), room(_room)
{
}

PointOfView::PointOfView(const PointOfView& obj):
position(obj.position), local_reference(obj.local_reference), room(obj.room)
{
}

glm::mat4x4 PointOfView::getViewMatrix() const
{
    return glm::lookAt(
					position,
					position+ getDirection(),
					getUp());
}

glm::mat4x4 PointOfView::getPosMatrix() const
{
    glm::mat4x4 ref4 = glm::mat4(local_reference);
    return glm::translate(glm::mat4(1.0f), position) * ref4;
    //return glm::mat4(1.0);
}
