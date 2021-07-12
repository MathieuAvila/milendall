#include <glm/glm.hpp>

#include "face_list.hxx"

#include "common.hxx"
#include "gltf_exception.hxx"

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
: points(_points), indices(_indices)
{
    if (indices.size() < 3)
        throw GltfException("invalid number of points for a face: must be 3 at least");
    auto & a = points.get()->getPoints()[indices[0]];
    auto & b = points.get()->getPoints()[indices[1]];
    auto & c = points.get()->getPoints()[indices[2]];
    normal = glm::normalize(glm::cross(c - a, b - a));
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
            console->debug("face, index point = {} ]", raw_data[i]);
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
