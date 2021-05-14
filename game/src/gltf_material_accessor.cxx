#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"

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

class GltfMaterialAccessorImpl : public GltfMaterialAccessorIFace {

    GltfMaterialLibraryImpl* libImpl;
    vector<GltfTextureReferencePtr> textureList;

    public:

    GltfMaterialAccessorImpl(GltfMaterialLibraryImpl* _libImpl, nlohmann::json& file) :
        libImpl(_libImpl)
    {
    }

    void loadId(uint32_t index) override
    {
        if (index >= textureList.size())
            throw GltfException("No such material with index ");
        console->debug("Apply texture {}", index);
        textureList[index]->apply();
    }

    ~GltfMaterialAccessorImpl() override
    {

    };
};

class GltfMaterialLibraryImpl : public GltfMaterialLibraryIface {

    map<const FileLibrary::UriReference, GltfTextureReferenceWPtr> textureMap;

public:

    GltfMaterialAccessorIFacePtr getMaterialAccessor(nlohmann::json& file) override {
        return make_shared<GltfMaterialAccessorImpl>(this, file);
    };

    void releaseTexture(const FileLibrary::UriReference ref)
    {
        textureMap.erase(ref);
    }

    GltfTextureReferencePtr getTexture(const FileLibrary::UriReference ref)
    {
        if (!textureMap.count(ref)) {
            GltfTextureReferencePtr tex_ref = make_shared<GltfTextureReference>(this, ref);
            textureMap.insert(make_pair(ref, tex_ref));
        }
        return textureMap[ref].lock();
    }

    GltfMaterialLibraryImpl(): GltfMaterialLibraryIface() {};
};

GltfMaterialLibraryIfacePtr GltfMaterialLibraryIface::getMaterialLibray()
{
    return make_shared<GltfMaterialLibraryImpl>();
}

GltfMaterialLibraryIface::~GltfMaterialLibraryIface()
{}



GltfTextureReference::~GltfTextureReference()
{
    lib->releaseTexture(ref);
    console->debug("Release texture {}", ref.getPath().c_str());
}

GltfMaterialAccessorIFace::~GltfMaterialAccessorIFace() {}

