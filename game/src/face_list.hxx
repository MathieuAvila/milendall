#pragma once

#include <memory>
#include <vector>
#include <list>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
            std::shared_ptr<PointsBlock> points;
            std::vector<unsigned int> indices;
            glm::vec3 normal;
            glm::vec4 plane;

            Face(std::shared_ptr<PointsBlock> _points, std::vector<unsigned int>&& _indices);
    };

    protected:

        std::list<Face> faces;
        std::shared_ptr<PointsBlock> points;

    public:

        FaceList(std::shared_ptr<PointsBlock>, std::unique_ptr<GltfDataAccessorIFace::DataBlock>);

        // for UT purposes
        const std::list<Face>& getFaces() const;
};


