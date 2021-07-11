#pragma once

#include <memory>
#include <vector>
#include <list>

#include <glm/vec3.hpp>

#include "gltf_data_accessor_iface.hxx"

class PointsBlock {

        std::vector<glm::vec3> points;

    public:

        const std::vector<glm::vec3>& getPoints() const;
        PointsBlock(std::unique_ptr<GltfDataAccessorIFace::DataBlock>);
};

class FaceList
{

    public:

    struct Face {
            std::vector<int> points;
    };

    protected:

        std::list<Face> faces;
        std::shared_ptr<PointsBlock> points;

    public:

        FaceList(std::shared_ptr<PointsBlock>, std::unique_ptr<GltfDataAccessorIFace::DataBlock>);

        // for UT purposes
        const std::list<Face>& getFaces() const;
};


