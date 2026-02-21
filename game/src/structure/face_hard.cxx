#include <common.hxx>
#include <utility>
#include "face_hard.hxx"

static auto console = getConsole("face_hard");

FaceHard::FaceHard(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json& json) : face(points, std::move(accessor))
{
    console->debug("Hard walls");
}
