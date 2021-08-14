#include "point_of_view.hxx"

#include "helper_math.hxx"

PointOfView PointOfView::changeCoordinateSystem(std::string newRoom, const glm::mat4& newMatrix) const
{
    return PointOfView {
        newMatrix * positionToVec4(position),
        newMatrix * vectorToVec4(direction),
        newMatrix * vectorToVec4(up),
        newRoom
    };
}