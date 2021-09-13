#include "point_of_view.hxx"

#include "helper_math.hxx"

PointOfView PointOfView::changeCoordinateSystem(std::string newRoom, const glm::mat4& newMatrix) const
{
    //glm::mat4x4 no_trans(newMatrix);
    //no_trans[3] = glm::vec4(0, 0, 0, 1);
    glm::mat3x3 no_trans(newMatrix);
    return PointOfView(
        newMatrix * positionToVec4(position),
        no_trans * direction,
        no_trans * up,
        no_trans * local_reference,
        newRoom
    );
}

PointOfView PointOfView::prependCoordinateSystem(const glm::mat3& newMatrix) const
{
    //glm::mat4x4 no_trans(newMatrix);
    //no_trans[3] = glm::vec4(0, 0, 0, 1);
    glm::mat3x3 no_trans(newMatrix);
    return PointOfView(
        position,
        no_trans * direction,
        no_trans * up,
        local_reference * no_trans,
        room
    );
}

bool PointOfView::operator==(const PointOfView& p) const
{
    return (position == p.position)
        && (direction == p.direction)
        && (up == p.up)
        && (local_reference == p.local_reference)
        && (room == p.room);
}

std::string to_string(const PointOfView& p)
{
    return std::string(
      "\nr:") + p.room
    + "\np: " + vec3_to_string(p.position)
    + "\nd: " + vec3_to_string(p.direction)
    + "\nu: " + vec3_to_string(p.up)
    + "\nr: " + mat4x4_to_string(p.local_reference);
}

PointOfView::PointOfView(
    const glm::vec3 _position,
    const float vertical, const float horizontal,
    const glm::mat3x3 _local_reference,
    const std::string _room) :
    position(_position), local_reference(_local_reference), room(_room)
{
	direction = glm::vec3(
		cos(vertical) * sin(horizontal),
		sin(vertical),
		cos(vertical) * cos(horizontal)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontal - 3.14f/2.0f),
		0,
		cos(horizontal - 3.14f/2.0f)
	);

	// Up vector
	up = glm::cross( right, direction );

    direction = local_reference*direction;
    up = local_reference*up;
}

glm::vec3 PointOfView::getDirection()
{
    return local_reference * glm::vec3(0.0, 0.0, 1.0);
}

glm::vec3 PointOfView::getUp()
{
    return local_reference * glm::vec3(0.0, 1.0, 0.0);
}

glm::vec3 PointOfView::getRight()
{
    return local_reference * glm::vec3(1.0, 0.0, 0.0);
}


PointOfView::PointOfView(
    const glm::vec3 _position,
    const glm::vec3 _direction,
    const glm::vec3 _up,
    const glm::mat3x3 _local_reference,
    const std::string _room):
position(_position), direction(_direction), up(_up), local_reference(_local_reference), room(_room)
{
}

PointOfView::PointOfView(const PointOfView& obj):
position(obj.position), direction(obj.direction), up(obj.up), local_reference(obj.local_reference), room(obj.room)
{

}
