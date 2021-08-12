#include <glm/glm.hpp>

#include "face_list.hxx"

#include "common.hxx"
#include "gltf_exception.hxx"

#include "helper_math.hxx"

static auto console = spdlog::stdout_color_mt("face_list");

PointsBlock::PointsBlock(std::unique_ptr<GltfDataAccessorIFace::DataBlock> data)
{
    points.reserve(data.get()->count);
    if (data->unit_type != data->FLOAT)
        throw GltfException("invalid data type, expect float");
    if (data->vec_type != data->VEC3)
        throw GltfException("invalid unit type, expect VEC3 for points");
    float* raw_data = (float*)(data->data);
    for (auto i = 0; i < data->count; i++)
    {
        console->debug("point {}, [ {} {} {} ]", i, raw_data[i*3], raw_data[i*3 + 1], raw_data[i*3 + 2]);
        points.push_back(glm::vec3(raw_data[i*3], raw_data[i*3 + 1], raw_data[i*3 + 2]));
    }
    console->debug("points {}", points.size());
}

const std::vector<glm::vec3>& PointsBlock::getPoints() const
{
    return points;
}

FaceList::FaceList::Face::Face(std::shared_ptr<PointsBlock> _points, vector<unsigned int>&& _indices)
: points(_points)
{
    if (_indices.size() < 3)
        throw GltfException("invalid number of points for a face: must be 3 at least");
    for (auto i : _indices) {
        if (i >= points->getPoints().size())
            throw GltfException("invalid array index: over points");
        indices.push_back(PointInfo{i});
    }

    auto & a = points.get()->getPoints()[indices[0].index];
    auto & b = points.get()->getPoints()[indices[1].index];
    auto & c = points.get()->getPoints()[indices[2].index];
    normal = glm::normalize(glm::cross(b - a, c - a));
    plane = getPlaneEquation(a, normal);
    for (int i=0; i < indices.size(); i++ ) {
        auto p0 = points.get()->getPoints()[indices[i].index];
        auto p1 = points.get()->getPoints()[indices[ (i + 1) % indices.size()].index];
        //auto p2 = p0 + normal;
        indices[i].plane = getPlaneEquation(p0, glm::normalize(glm::cross(normal, p1 - p0)));

        //console->info("{}", vec3_to_string(p0));
        //console->info("{}", vec3_to_string(p1));
        //console->info("{}", vec3_to_string(normal));
        //console->info("== > {}", vec4_to_string(indices[i].plane));
    }
}

FaceList::FaceList(std::shared_ptr<PointsBlock> _points, std::unique_ptr<GltfDataAccessorIFace::DataBlock> data)
{
    points = _points;
    if (data->unit_type != data->UNSIGNED_SHORT)
        throw GltfException("invalid data type, expect short");
    if (data->vec_type != data->SCALAR)
        throw GltfException("invalid unit type, expect SCALAR for faces");
    unsigned short * raw_data = (unsigned short*)(data->data);
    auto index = 0;
    while (index < data->count) {
        auto p_count = raw_data[index];
        if (p_count + index >= data->count)
            throw GltfException("Indices exceeds total size");
        console->debug("face, total={}, index {} ]", p_count, index);
        std::vector<unsigned int> index_points;
        for (auto i = index + 1; i < index + 1 + p_count; i++) {
            //console->debug("face, index point = {} ]", raw_data[i]);
            index_points.push_back(raw_data[i]);
        }
        faces.push_back(FaceList::Face(points, move(index_points)));

        index += p_count + 1;
    }
}

const std::list<FaceList::Face>& FaceList::getFaces() const
{
    return faces;
}

bool FaceList::Face::checkInVolume(glm::vec3 p)
{
    for (auto& i: indices) {
        console->info("valued {}", pointPlaneProjection(i.plane, p));
        if (pointPlaneProjection(i.plane, p) < 0.0)
            return false;
    }
    return true;
}

bool FaceList::Face::checkTrajectoryCross(glm::vec3 p0, glm::vec3 p1, glm::vec3& impact, float& distance)
{
    console->debug("p={}", (void*)this);

    return checkSphereTrajectoryCross(p0, p1, 0.0f, impact,distance);
}

bool FaceList::Face::checkSphereTrajectoryCross(glm::vec3 p0, glm::vec3 p1, float radius, glm::vec3& impact, float& distance)
{
    console->debug("p={}", intersectSphereTrajectoryPlane(
        p0, p1, radius,
        plane,
        impact,
        distance));
    if (!intersectSphereTrajectoryPlane(
        p0, p1, radius,
        plane,
        impact,
        distance))
        return false;
    return checkInVolume(impact);
}