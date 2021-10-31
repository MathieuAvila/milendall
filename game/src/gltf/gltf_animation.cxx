#include "common.hxx"
#include "gltf_animation.hxx"
#include "gltf_exception.hxx"
#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"
#include "helper_math.hxx"
#include <glm/ext/matrix_transform.hpp>

static auto console = getConsole("gltf_animation");

class GltfAnimation::GltfAnimationFrame {

    enum Kind { TRANSLATION, ROTATION } kind;
    unsigned int target_node;
    std::vector<float> input_data;
    std::unique_ptr<GltfDataAccessorIFace::DataBlock> output_data;

public:

    GltfAnimationFrame(
        json& j_anim,
        json& j_samplers,
        GltfDataAccessorIFace* data_accessor);
    void apply(float time, GltfAnimationTargetIface* instance);
    ~GltfAnimationFrame() {};
};

void GltfAnimation::GltfAnimationFrame::apply(float time, GltfAnimationTargetIface* instance)
{
    // find smallest key time;
    int index = 0;
    while (index < input_data.size() - 1) {
        if (input_data[index + 1] >= time)
            break;
        index++;
    }
    auto next_current = index + 1;
    if (next_current >= input_data.size())
        next_current = index;

    float ratio = 0.0f;
    if (input_data[next_current] != input_data[index])
        ratio = (time - input_data[index]) / (input_data[next_current] - input_data[index]);

    console->debug("Time {} key is between: {}={} and {}={}, with ratio={}",
        time,
        index, input_data[index],
        next_current, input_data[next_current],
        ratio);

    switch(kind) {
        case TRANSLATION:
        {
            glm::vec3 vec_current = *(glm::vec3*)&output_data.get()->data[index * sizeof(glm::vec3)];
            glm::vec3 vec_next = *(glm::vec3*)&output_data.get()->data[next_current * sizeof(glm::vec3)];
            glm::vec3 vec_value = vec_current + (vec_next - vec_current) * ratio;
            console->debug("translation current={} next={} applied={}",
                vec3_to_string(vec_current), vec3_to_string(vec_next), vec3_to_string(vec_value));
            glm::mat4x4 mat_translate = glm::translate(glm::mat4x4(1.0f), vec_value);
            instance->applyChange(target_node, mat_translate);
            break;
        }
        case ROTATION:
        {
            // XXX todo
            break;
        }
    }
}

GltfAnimation::GltfAnimationFrame::GltfAnimationFrame(
        json& j_anim,
        json& j_samplers,
        GltfDataAccessorIFace* data_accessor)
{
    console->debug("New animation frame");

    auto target = jsonGetElementByName(j_anim, "target");
    target_node = jsonGetElementByName(target, "node").get<int>();
    auto path = jsonGetElementByName(target, "path").get<string>();
    if (path == "translation")
        kind = TRANSLATION;
    else if (path == "rotation")
        kind = ROTATION;
    else
        throw GltfException("Unhandled animation path type:" + path);

    auto sampler_id = jsonGetElementByName(j_anim, "sampler").get<int>();
    auto sampler = jsonGetIndex(j_samplers, sampler_id);
    auto input = jsonGetElementByName(sampler, "input").get<int>();
    auto output = jsonGetElementByName(sampler, "output").get<int>();
    // NOTE: ignore interpolation ATM
    auto input_data_raw = data_accessor->accessId(input);
    auto total_keys = input_data_raw.get()->count;
    console->debug("Total keys {} total data size {}", total_keys, input_data_raw.get()->count);
    for (int index = 0; index < total_keys; index++) {
        float time = *(float*)&input_data_raw.get()->data[index * sizeof(float)];
        console->debug("Insert time key {}", time);
        input_data.push_back(time);
    }
    output_data = data_accessor->accessId(output);
}

GltfAnimation::GltfAnimation(
        json& j_animation,
        GltfDataAccessorIFace* data_accessor)
{
    console->debug("New animation");
    auto samplers = jsonGetElementByName(j_animation, "samplers");
    jsonExecuteAllIfElement(j_animation, "channels", [this, j_animation, data_accessor, &samplers](nlohmann::json& j_anim, int node_index) {
        frames.push_back(make_unique<GltfAnimationFrame>(j_anim, samplers, data_accessor));
    });
    name = jsonGetElementByName(j_animation, "name").get<string>();
    console->info("Animation has name {}", name);
}

void GltfAnimation::apply(float time, GltfAnimationTargetIface* instance)
{
    for (auto& frame: frames) {
        frame->apply(time, instance);
    }
}

std::string GltfAnimation::getName()
{
    return name;
}

GltfAnimation::~GltfAnimation()
{
}