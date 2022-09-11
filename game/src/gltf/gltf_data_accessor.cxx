#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"
#include "helper_math.hxx"

#include "common.hxx"

using namespace std;
using namespace nlohmann;

static auto console = getConsole("gltf_accessor");

GltfDataAccessor::GltfDataAccessor(nlohmann::json& json, const FileLibrary::UriReference ref, FileContentPtr content)
{
        /** load all buffers in memory now */
        auto buffers_json = json["buffers"];
        for(auto buffer_json: buffers_json) {
                if (buffer_json.contains("uri")) {
                    auto str = buffer_json["uri"].get<string>();
                    if (str.rfind("data:application/octet-stream;base64,", 0) == 0) {
                        auto substr = str.substr(std::string("data:application/octet-stream;base64,").size());
                        std::vector<uint8_t> b64decoded;
                        decodeBase64(substr, b64decoded);
                        FileContentPtr content= std::make_shared<std::vector<uint8_t>>(b64decoded);
                        loaded_buffers.push_back(content);
                    } else {
                        auto buf_ref = ref.getSubPath(buffer_json["uri"]);
                        loaded_buffers.push_back(buf_ref.readContent());
                    }
                } else {
                    loaded_buffers.push_back(content);
                }
        }
        accessors = jsonGetElementByName(json, "accessors");
        bufferViews = jsonGetElementByName(json, "bufferViews");
}

std::unique_ptr<GltfDataAccessor::DataBlock> GltfDataAccessor::accessId(uint32_t index)
{
   std::map<std::string, GltfDataAccessor::DataBlock::VEC_TYPE> mapper_type = {
           {"VEC2", GltfDataAccessor::DataBlock::VEC2},
           {"VEC3", GltfDataAccessor::DataBlock::VEC3},
           {"SCALAR", GltfDataAccessor::DataBlock::SCALAR}
    };
   std::map<int, GltfDataAccessor::DataBlock::UNIT_TYPE> mapper_component_type = {
           {5126, GltfDataAccessor::DataBlock::FLOAT},
           {5125, GltfDataAccessor::DataBlock::UNSIGNED_INT},
           {5123, GltfDataAccessor::DataBlock::UNSIGNED_SHORT}
    };
   auto accessor = jsonGetIndex(accessors, index);

   auto bufferView = jsonGetElementByName(accessor, "bufferView").get<int>();
   auto byteOffset = 0;
   jsonExecuteIfElement(accessor, "byteOffset", [&byteOffset](nlohmann::json& elem) {
        byteOffset = elem.get<int>();
    });
   auto componentType_int = jsonGetElementByName(accessor, "componentType").get<int>();
   if (!mapper_component_type.count(componentType_int))
    throw(GltfException(std::string("Unknown accessor component type:") + std::to_string(componentType_int)));
   auto componentType = mapper_component_type[componentType_int];
   auto count = jsonGetElementByName(accessor, "count").get<int>();
   auto type_str = jsonGetElementByName(accessor, "type").get<std::string>();
   if (!mapper_type.count(type_str))
    throw(GltfException(std::string("Unknown accessor type:") + type_str));
   auto type = mapper_type[type_str];

   auto bufferView_json = jsonGetIndex(bufferViews, bufferView);
   int bufferview_byteOffset;
   jsonExecuteIfElement(bufferView_json, "byteOffset", [&bufferview_byteOffset](nlohmann::json& elem) {
       bufferview_byteOffset = elem.get<int>();
   });
   auto bufferview_buffer = jsonGetElementByName(bufferView_json, "buffer").get<int>();
   auto bufferview_stride = bufferView_json.contains("byteStride") ? jsonGetElementByName(bufferView_json, "byteStride").get<int>() : 0;

   if (bufferview_buffer >= loaded_buffers.size())
        throw(GltfException(std::string("Buffer index is too big. Has:")
        + std::to_string(bufferview_buffer)
        +" size is:"
        + std::to_string(loaded_buffers.size())));

   FileContentPtr& buffPtr = loaded_buffers[bufferview_buffer];

   console->info("Accessing index {} - stride {} - buferView offset={} accessor offset={} count={}",
                index, bufferview_stride, bufferview_byteOffset, byteOffset, count);

    uint8_t* nptr = (uint8_t*)buffPtr->data() + bufferview_byteOffset + byteOffset;
    unsigned long long size = buffPtr->size() - bufferview_byteOffset + byteOffset;

    return make_unique<GltfDataAccessor::DataBlock>(
        type,
        componentType,
        count,
        bufferview_stride,
        size,
        vector<uint8_t>(nptr, nptr + size));
}

GltfDataAccessor::~GltfDataAccessor()
{
}

