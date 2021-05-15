#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"
#include "json_helper_accessor.hxx"

#include "common.hxx"
#include <map>

class GltfMaterialLibraryImpl;
using GltfMaterialLibraryImplPtr = std::shared_ptr<GltfMaterialLibraryImpl>;

static auto console = spdlog::stdout_color_mt("texture");


class GltfTextureReference {

    const FileLibrary::UriReference ref;
    GltfMaterialLibraryImpl* lib;

    public:

    GltfTextureReference(GltfMaterialLibraryImpl* _lib, const FileLibrary::UriReference _ref) :
        ref(_ref), lib(_lib)
    {
        console->debug("Load texture {}", ref.getPath().c_str());
    };

    void apply() {
        console->debug("Apply texture {}", ref.getPath().c_str());
    }

    ~GltfTextureReference();
};

using GltfTextureReferencePtr = shared_ptr<GltfTextureReference>;
using GltfTextureReferenceWPtr = weak_ptr<GltfTextureReference>;



class GltfMaterialLibraryImpl : public GltfMaterialLibraryIface {

    map<const FileLibrary::UriReference, GltfTextureReferenceWPtr> textureMap;

public:

    GltfMaterialAccessorIFacePtr getMaterialAccessor(
        const FileLibrary::UriReference& dir,
        nlohmann::json& file) override;

    void releaseTexture(const FileLibrary::UriReference ref);
    GltfTextureReferencePtr getTexture(const FileLibrary::UriReference ref);

    GltfMaterialLibraryImpl();
    virtual ~GltfMaterialLibraryImpl() override;
};

class GltfMaterialAccessorImpl : public GltfMaterialAccessorIFace {

    struct material {
        int textureIndex; // -1 if none
    };

    GltfMaterialLibraryImpl* libImpl;
    vector<struct material> materialList;
    vector<GltfTextureReferencePtr> textureList;

    public:

    GltfMaterialAccessorImpl(GltfMaterialLibraryImpl* _libImpl,
        const FileLibrary::UriReference& dir,
        nlohmann::json& file) :
        libImpl(_libImpl)
    {
        std::vector<int> texture_to_image;
        jsonExecuteAllIfElement(file, "images", [this, dir](nlohmann::json& node_image, int image_index) {
            if (!node_image.contains("uri"))
                throw GltfException(string("Image has no URI. Index=") + to_string(image_index));
            string uri = node_image["uri"].get<string>();
            console->debug("Image {} has URI={}", image_index, uri);
            FileLibrary::UriReference imageUri = dir.getSubPath(uri);
            textureList.push_back(libImpl->getTexture(imageUri));
        });
        jsonExecuteAllIfElement(file, "textures", [this, &texture_to_image](nlohmann::json& node_texture, int texture_index) {
            if (!node_texture.contains("source"))
                throw GltfException(string("Texture has no source. Index=") + to_string(texture_index));
            int source_index = node_texture["source"].get<int>();
            texture_to_image.push_back(source_index);
            console->debug("texture {} is using image {}", texture_index, source_index);
        });
        jsonExecuteAllIfElement(file, "materials", [this, &texture_to_image](nlohmann::json& child, int node_index) {
            string name;
            string textureFileName;
            struct material mat;
            mat.textureIndex = -1;
            jsonExecuteIfElement(child, "name", [&name](nlohmann::json& name_child) {
                name = name_child.get<string>();
            });
            jsonExecuteIfElement(child, "pbrMetallicRoughness", [this, &texture_to_image, node_index, &mat](nlohmann::json& pbr_child) {
                jsonExecuteIfElement(pbr_child, "baseColorTexture", [this, &texture_to_image, node_index, &mat](nlohmann::json& baseColorTexture) {
                    jsonExecuteIfElement(baseColorTexture, "index", [this, &texture_to_image, node_index, &mat](nlohmann::json& index) {
                        int texture_index = index.get<int>();
                        if (texture_index < 0 || texture_index >=texture_to_image.size())
                            throw GltfException(string("Invaild texture index {} for material {}")
                                    + to_string(texture_index)+ to_string(node_index));
                        int image_index = texture_to_image[texture_index];
                        mat.textureIndex = image_index;
                        console->debug("material {} is using texture index {}, with image index was {}",
                                    node_index, texture_index, image_index);
                    });
                });
            });
            console->info("found material:{} with name '{}' with texture ID={}", to_string(node_index), name, mat.textureIndex);
            //children.push_back(child.get<int>());
    });
    }

    void loadId(uint32_t index) override
    {
        if (index >= textureList.size())
            throw GltfException("No such material with index ");
        console->debug("Apply texture {}", index);
        //textureList[index]->apply();
    }

    ~GltfMaterialAccessorImpl() override
    {

    };
};

GltfMaterialAccessorIFacePtr GltfMaterialLibraryImpl::getMaterialAccessor(
    const FileLibrary::UriReference& dir,
    nlohmann::json& file)
{
        return make_shared<GltfMaterialAccessorImpl>(this, dir, file);
};

void GltfMaterialLibraryImpl::releaseTexture(const FileLibrary::UriReference ref)
{
    textureMap.erase(ref);
}

GltfTextureReferencePtr GltfMaterialLibraryImpl::getTexture(const FileLibrary::UriReference ref)
{
    // implementation note: order is important, as we must keep the shared_ptr and return it
    // and keep the weak ptr. If shared_ref is created in the sub context it will be released
    // too early.
    GltfTextureReferencePtr tex_ref;
    if (!textureMap.count(ref)) {
        tex_ref = make_shared<GltfTextureReference>(this, ref);
        textureMap.insert(make_pair(ref, tex_ref));
    }
    return textureMap[ref].lock();
}

GltfMaterialLibraryImpl::~GltfMaterialLibraryImpl()
{
    console->debug("End material library, textures left: {}", textureMap.size());
}

GltfMaterialLibraryImpl::GltfMaterialLibraryImpl(): GltfMaterialLibraryIface()
{
};

GltfMaterialLibraryIfacePtr GltfMaterialLibraryIface::getMaterialLibray()
{
    return make_shared<GltfMaterialLibraryImpl>();
}

GltfMaterialLibraryIface::~GltfMaterialLibraryIface()
{
}

GltfTextureReference::~GltfTextureReference()
{
    lib->releaseTexture(ref);
    console->debug("Release texture {}", ref.getPath().c_str());
}

GltfMaterialAccessorIFace::~GltfMaterialAccessorIFace() {}

