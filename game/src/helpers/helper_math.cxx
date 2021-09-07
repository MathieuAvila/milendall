#include "common.hxx"
#include "helper_math.hxx"
#include "gltf_exception.hxx"

#include <glm/gtx/intersect.hpp>

static auto console = spdlog::stdout_color_mt("math");

std::string mat4x4_to_string(glm::mat4x4 mat)
{
    string result("[");
    for (auto i: {0,1,2,3}) {
        result+="\n[";
        for (auto j: {0,1,2,3}) {
            if (j!=0)
                (result+=", ");
            result+= to_string(mat[i][j]);
        }
        result+=']';
    }
    result+="\n]";
    return result;
}

std::string vec3_to_string(glm::vec3 v)
{
    return string("[ ") + to_string(v.x) + " , "  + to_string(v.y) + " , "  + to_string(v.z) + " ]" ;
}

std::string vec4_to_string(glm::vec4 v)
{
   return string("[ ") + to_string(v.x) + " , "  + to_string(v.y) + " , " + to_string(v.z)  + " , " + to_string(v.w) + " ]" ;
}

glm::vec4 getPlaneEquation(glm::vec3 p, glm::vec3 n)
{
    n = normalize(n);
    return glm::vec4(n.x, n.y, n.z, -glm::dot(p,n));
}

float pointPlaneProjection(glm::vec4 plane, glm::vec3 point)
{
    return glm::dot(plane, glm::vec4(point.x, point.y, point.z, 1.0));
}

glm::vec4 positionToVec4(glm::vec3 p)
{
    return glm::vec4(p.x, p.y, p.z, 1.0);
}

glm::vec4 vectorToVec4(glm::vec3 v)
{
   return glm::vec4(v.x, v.y, v.z, 0.0);
}

bool intersectSphereTrajectoryPlane(
    glm::vec3 position1, glm::vec3 position2, float radius,
    glm::vec4 plane,
    glm::vec3& intersect_center,
    float& distance,
    bool reversed_face
    )
{
    auto plane_v = glm::vec3(plane);

    // compute factor if reversed
    float factor = (reversed_face ? -1.0f : 1.0f);

    console->debug("factor {}", factor);
    console->debug("glm::dot(plane_v, position2 - position1) {}", glm::dot(plane_v, position2 - position1));
    console->debug("plane_v {}", vec3_to_string(plane_v));
    console->debug("plane {}", vec4_to_string(plane));
    console->debug("position1 {}", vec3_to_string(position1));
    console->debug("position2 {}", vec3_to_string(position2));

    // only if i'm going against the plane, otherwise let it flow through
    if (factor * glm::dot(plane_v, position2 - position1) > 0)
        return false;

    // OK, i *may* intersect

    glm::vec3 plane_orig = glm::vec3(plane) * (radius -plane.w) ;
    glm::vec3 plane_vec = glm::vec3(plane);

    glm::vec3 vec_direction = glm::normalize( position2 - position1);

    console->debug("plane_orig {}", vec3_to_string(plane_orig));
    console->debug("plane_vec {}", vec3_to_string(plane_vec));
    console->debug("vec_direction {}", vec3_to_string(vec_direction));

    bool result = glm::intersectRayPlane(
        position1,
        vec_direction,
        plane_orig, plane_vec, distance);
    if (result) {
        intersect_center = position1 + vec_direction * distance;
        console->debug("intersect_center {}", vec3_to_string(intersect_center));
    }
    if ((distance <= 0.0f)||(distance > glm::length(position2 - position1)) )
        return false;
    return result;
};

#define square(a) (dot(a,a))

bool intersectSphereTrajectorySegment(
    glm::vec3 position1, glm::vec3 position2, float radius,
    glm::vec3 A, glm::vec3 B,
    glm::vec3& intersect_center, float& distance, glm::vec3& normal)
{
    glm::vec3& M = position1;
    glm::vec3& N = position2;
    glm::vec3 AB = B - A;
    glm::vec3 MN = N - M;
    glm::vec3 AM = M - A;

    console->debug("AB={}", vec3_to_string(AB));
    console->debug("MN={}", vec3_to_string(MN));

    float square_ab  = square(AB);
    float x1 = dot(AB, MN) / square_ab;
    float x2 = dot(AB, AM) / square_ab;

    float a = x1*x1*square_ab - square(MN);
    float b = 2.0f *square_ab * x1 * x2 - 2.0f * dot(AM,MN);
    float c = radius*radius + x2*x2*square_ab - square(AM) ;

    float delta = b*b - 4.0f*a*c;

    console->debug("a={}, b={}, c={}, delta={} -b/2a ={}", a,b,c,delta, -b/(2.0f*a) );

    if (delta < 0)
        return false;

    auto sqrt_delta = sqrt(delta);
    auto length = glm::length(MN);

    float alpha1, alpha2;

    if (a > 0) {
        alpha1 = (-b - sqrt_delta) / (2*a);
        alpha2 = (-b + sqrt_delta) / (2*a);
    } else {
        alpha2 = (-b - sqrt_delta) / (2*a);
        alpha1 = (-b + sqrt_delta) / (2*a);
    }

    // if alpha1 is past trajectory, no way we can intersect
    if (alpha1 > 1.0) {
        console->debug("alpha1 > 1.0 , no contact");
        return false;
    }

    // if alpha2 is before trajectory, no way we can intersect
    if (alpha2 < 0.0) {
        console->debug("alpha2 < 0.0 , no contact");
        return false;
    }

    // if we're in the center, it's clear we've already crossed the segment.
    // Treat this as immediate contact
    if ((alpha1 < 0.0)&& (alpha2 >= 0.0)) {
        console->debug(" alpha1={} alpha2={} , already in contact at origin", alpha1, alpha2);
        alpha1 = 0.0;
    }

    float alpha = alpha1;

    intersect_center = M + alpha * MN;
    //auto S2 = M + alpha2 * MN;

    auto beta = alpha * x1 + x2;
    // if beta is outside of segment, compute if there's a matching point further
    // TODO

    // compute impact point on segment
    auto impact_point = A + AB * beta;

    // get best solution inside trajectory if it exists

    distance = alpha * length;
    normal = glm::normalize(intersect_center - impact_point);

    return true;
}
