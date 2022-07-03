#include "gltf_data_accessor.hxx"
#include "json_helper_accessor.hxx"
#include "gltf_exception.hxx"

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
                    auto buf_ref = ref.getSubPath(buffer_json["uri"]);
                    //auto content = buf_ref.readContent();
                    loaded_buffers.push_back(buf_ref.readContent());
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
   auto byteOffset = jsonGetElementByName(accessor, "byteOffset").get<int>();
   auto componentType = mapper_component_type[jsonGetElementByName(accessor, "componentType").get<int>()];
   auto count = jsonGetElementByName(accessor, "count").get<int>();
   auto type = mapper_type[jsonGetElementByName(accessor, "type").get<std::string>()];


   auto bufferView_json = jsonGetIndex(bufferViews, bufferView);
   auto bufferview_byteOffset = jsonGetElementByName(bufferView_json, "byteOffset").get<int>();
   auto bufferview_byteLength = jsonGetElementByName(bufferView_json, "byteLength").get<int>();
   auto bufferview_buffer = jsonGetElementByName(bufferView_json, "buffer").get<int>();

   if (bufferview_buffer >= loaded_buffers.size())
        throw(GltfException(std::string("Buffer index is too big. Has:")
        + std::to_string(bufferview_buffer)
        +" size is:"
        + std::to_string(loaded_buffers.size())));

   FileContentPtr& buffPtr = loaded_buffers[bufferview_buffer];

   auto endIndex = bufferview_byteOffset + byteOffset + bufferview_byteLength;
   if (endIndex > buffPtr->size())
        throw(GltfException(std::string("Data sizes exceeds buffer length for accessor ")
        + std::to_string(index)
        + " . endIndex=" + std::to_string(endIndex)
        + " . buff=" + std::to_string(buffPtr->size()) ));

   uint8_t* nptr = (uint8_t*)buffPtr->data() + bufferview_byteOffset + byteOffset;

   return make_unique<GltfDataAccessor::DataBlock>(
        type,
        componentType,
        count, nptr);
}

GltfDataAccessor::~GltfDataAccessor()
{
}

