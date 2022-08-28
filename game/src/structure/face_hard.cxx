#include <common.hxx>
#include "face_hard.hxx"

static auto console = getConsole("face_hard");

FaceHard::FaceHard(
    std::shared_ptr<PointsBlock> points,
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> accessor,
    nlohmann::json& json) : face(points, move(accessor))
{
    console->debug("Hard walls");
}
