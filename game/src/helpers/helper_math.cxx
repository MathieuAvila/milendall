#include "common.hxx"
#include "helper_math.hxx"
#include "gltf_exception.hxx"

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>

static auto console = getConsole("math");

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
    float& _distance,
    bool reversed_face
    )
{
    auto distance = _distance;

    auto plane_v = glm::vec3(plane);

    // compute factor if reversed
    float factor = (reversed_face ? -1.0f : 1.0f);

    // only if i'm going against the plane, otherwise let it flow through
    if (factor * glm::dot(plane_v, position2 - position1) > 0)
        return false;

    // OK, i *may* intersect

    glm::vec3 plane_orig = glm::vec3(plane) * (-plane.w) ;
    glm::vec3 plane_orig_sphere = glm::vec3(plane) * (radius-plane.w) ;
    glm::vec3 plane_vec = glm::vec3(plane);

    glm::vec3 vec_direction = glm::normalize( position2 - position1);

    float distance_origin; // will be thrashed.
    bool result_origin = glm::intersectRayPlane(
        position1,
        vec_direction,
        plane_orig, plane_vec, distance_origin);
    bool result_sphere = glm::intersectRayPlane(
        position1,
        vec_direction,
        plane_orig_sphere, plane_vec, distance);

    if (!result_origin)
        return false;

    if (result_origin && !result_sphere) {
        // we're on it.
        console->debug("success, sphere is inside");
        _distance = 0.0f;
        intersect_center = position1;
        return true;
    }

    if ((distance <= 0.0f)||(distance > glm::length(position2 - position1)) )
        return false;
    console->debug("success distance = {}", distance);

    intersect_center = position1 + vec_direction * distance;
    _distance = distance;

    return true;
};

#define square(a) (dot(a,a))

static bool solution_2nd_degree(float a, float b, float c, float& s1, float& s2)
{
    float delta = b*b - 4.0f*a*c;

    if (a == 0) {
        return false;
    }

    //console->info("a={}, b={}, c={}, delta={} -b/2a ={}", a,b,c,delta, -b/(2.0f*a) );

    if (delta < 0)
        return false;

    auto sqrt_delta = sqrt(delta);

    if (a > 0) {
        s1 = (-b - sqrt_delta) / (2*a);
        s2 = (-b + sqrt_delta) / (2*a);
    } else {
        s2 = (-b - sqrt_delta) / (2*a);
        s1 = (-b + sqrt_delta) / (2*a);
    }

    return true;
}

static void replace_if_valid(bool valid, float& alpha, float &new_alpha)
{
        if ((new_alpha < 0.0f) || (new_alpha > 1.0f))
            return;
        if ((!valid) || (new_alpha < alpha))
            alpha = new_alpha;
        valid = true;
}

bool intersectSphereTrajectorySegment(
    glm::vec3 position1, glm::vec3 position2, float radius,
    glm::vec3 A, glm::vec3 B,
    glm::vec3& intersect_center, float& _distance, glm::vec3& normal)
{
    glm::vec3& M = position1;
    glm::vec3& N = position2;
    glm::vec3 AB = B - A;
    glm::vec3 MN = N - M;
    glm::vec3 AM = M - A;

    float square_ab  = square(AB);
    float x1 = dot(AB, MN) / square_ab;
    float x2 = dot(AB, AM) / square_ab;

    float a = x1*x1*square_ab - square(MN);
    float b = 2.0f *square_ab * x1 * x2 - 2.0f * dot(AM,MN);
    float c = radius*radius + x2*x2*square_ab - square(AM) ;

    float alpha1, alpha2;

    if (!solution_2nd_degree(a, b, c, alpha1, alpha2))
        return false;

    // if alpha1 is past trajectory, no way we can intersect
    if (alpha1 > 1.0) {
        //console->debug("alpha1 > 1.0 , no contact");
        return false;
    }

    // if alpha2 is before trajectory, no way we can intersect
    if (alpha2 < 0.0) {
        //console->debug("alpha2 < 0.0 , no contact");
        return false;
    }

    auto beta1 = alpha1 * x1 + x2;
    //auto beta2 = alpha2 * x1 + x2;
    //if (beta2 < beta1) std::swap(beta1, beta2)
    //console->info("beta1={}, beta2={}", beta1, beta2);

    float alpha; // good alpha, if retained.

    if ((beta1 < 0.0f) || (beta1 > 1.0f)){
        //console->debug("beta1 out-of-bound, check contact on A or B");

        float alpha_a_1, alpha_a_2, alpha_b_1, alpha_b_2;

        glm::vec3 BM = M-B;

        bool has_valid_sol = false;

        // check A
        a = square(MN);
        b = 2 * dot(MN, AM);
        c = square(AM) - radius*radius;
        bool sol_A = solution_2nd_degree(a, b, c, alpha_a_1, alpha_a_2);

        // check B
        b = 2 * dot(MN, BM);
        c = square(BM) - radius*radius;
        bool sol_B = solution_2nd_degree(a, b, c, alpha_b_1, alpha_b_2);

        if (sol_A) {
            replace_if_valid(has_valid_sol, alpha, alpha_a_1);
            replace_if_valid(has_valid_sol, alpha, alpha_a_2);
        }
        if (sol_B) {
            replace_if_valid(has_valid_sol, alpha, alpha_b_1);
            replace_if_valid(has_valid_sol, alpha, alpha_b_2);
        }
        if (!has_valid_sol)
            return false;
        // alpha is set to nearest solution
    }
    else {
        alpha = alpha1;

        // if we're in the center, it's clear we've already crossed the segment.
        // Treat this as immediate contact
        if ((alpha1 < 0.0)&& (alpha2 >= 0.0)) {
            //console->info(" alpha={}, already in contact at origin", alpha);
            alpha = 0.0;
        }
    }



    intersect_center = M + alpha * MN;
    //auto S2 = M + alpha2 * MN;

    auto beta = alpha * x1 + x2;
    // if beta is outside of segment, compute if there's a matching point closer
    // TODO

    // compute impact point on segment
    auto impact_point = A + AB * beta;

    // get best solution inside trajectory if it exists

    auto length = glm::length(MN);
    _distance = alpha * length;
    normal = glm::normalize(intersect_center - impact_point);

    console->debug("A={}", vec3_to_string(A));
    console->debug("B={}", vec3_to_string(B));
    console->debug("Position1={}", vec3_to_string(position1));
    console->debug("Position2={}", vec3_to_string(position2));
    console->debug("radius={}", radius);

    console->debug("impact_point={}", vec3_to_string(impact_point));
    console->debug("distance={}", _distance);
    console->debug("beta1={}", beta1);
    console->debug("alpha1={}", alpha1);
    console->debug("alpha2={}", alpha2);
    console->debug("AB={}", vec3_to_string(AB));
    console->debug("MN={}", vec3_to_string(MN));
    console->debug("normal={}", vec3_to_string(normal));

    return true;
}

glm::mat3 getRotatedMatrix(float verticalAngle, float horizontalAngle)
{
//    auto rotateVertical = glm::rotate(glm::mat4(1.0f), verticalAngle, glm::vec3(0.0, 0.0, 1.0));
//    auto rotateHorizontal = glm::rotate(rotateVertical, horizontalAngle, glm::vec3(0.0, 1.0, 0.0));
//    return rotateHorizontal;

    auto rotateHorizontal = glm::rotate(glm::mat4(1.0f), horizontalAngle, glm::vec3(0.0, 1.0, 0.0));
    auto rotateVertical = glm::rotate(glm::mat4(1.0f), verticalAngle, glm::vec3(1.0, 0.0, 0.0));
    return rotateHorizontal * rotateVertical;
}
