#pragma once

#include <memory>
#include <vector>
#include <list>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "gltf/gltf_data_accessor_iface.hxx"

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

            struct PointInfo {
                unsigned int index;
                glm::vec4 plane;
            };

            std::shared_ptr<PointsBlock> points;
            std::vector<PointInfo> indices;
            glm::vec3 normal;
            glm::vec4 plane;

            Face(std::shared_ptr<PointsBlock> _points, std::vector<unsigned int>&& _indices);

            /** @brief Check a given point is inside volume defined by normals of all points
             */
            bool checkInVolume(glm::vec3 p) const;

            /** @brief Check a given trajectory crosses face
             */
            bool checkTrajectoryCross(glm::vec3 p0, glm::vec3 p1, glm::vec3& impact, float& distance, glm::vec3& normal) const;

            /** @brief Check a given sphere trajectory crosses face
             */
            bool checkSphereTrajectoryCross(glm::vec3 p0, glm::vec3 p1, float radius, glm::vec3& impact, float& distance, glm::vec3& normal) const;

    };

    protected:

        std::list<Face> faces;
        std::shared_ptr<PointsBlock> points;

    public:

        FaceList(std::shared_ptr<PointsBlock>, std::unique_ptr<GltfDataAccessorIFace::DataBlock>);

        // for UT purposes
        const std::list<Face>& getFaces() const;
};


