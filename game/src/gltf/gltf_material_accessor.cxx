#include <map>
#include <iostream>
#include <FreeImagePlus.h>
#include <GL/glew.h>

#include "gltf_material_accessor_library_iface.hxx"
#include "gltf_exception.hxx"
#include "json_helper_accessor.hxx"

#include "common.hxx"
#include "gl_init.hxx"

class GltfMaterialLibraryImpl;
using SGltfMaterialLibraryImpl = std::shared_ptr<GltfMaterialLibraryImpl>;
using WGltfMaterialLibraryImpl = std::weak_ptr<GltfMaterialLibraryImpl>;
using PGltfMaterialLibraryImpl = GltfMaterialLibraryImpl*;

static auto console = getConsole("texture");


class GltfTextureReference {

    const FileLibrary::UriReference ref;
    PGltfMaterialLibraryImpl lib;
    GLuint gl_texture;

    public:

    GltfTextureReference(PGltfMaterialLibraryImpl _lib, const FileLibrary::UriReference _ref);
    void apply();
    ~GltfTextureReference();

    /** in case library leaves before, but should not happen */
    void leaveLibrary();
};

using GltfTextureReferencePtr = shared_ptr<GltfTextureReference>;
using GltfTextureReferenceWPtr = weak_ptr<GltfTextureReference>;



class GltfMaterialLibraryImpl : public GltfMaterialLibraryIface {

    map<const FileLibrary::UriReference, GltfTextureReferenceWPtr> textureMap;

public:

    UGltfMaterialAccessorIFace getMaterialAccessor(
        const FileLibrary::UriReference& dir,
        nlohmann::json& file) override;

    void releaseTexture(const FileLibrary::UriReference ref);
    GltfTextureReferencePtr getTexture(const FileLibrary::UriReference ref);

    GltfMaterialLibraryImpl();
    virtual ~GltfMaterialLibraryImpl() override;
};

class GltfMaterialAccessorImpl : public GltfMaterialAccessorIFace {

    struct material {
        int textureIndex; // -1 if none, then it's a color
        float colors[3];
        float metallicFactor;
        float roughnessFactor;
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
        jsonExecuteAllIfElement(file, "textures", [&texture_to_image](nlohmann::json& node_texture, int texture_index) {
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
            mat.metallicFactor = 1.0;
            mat.roughnessFactor = 1.0;

            jsonExecuteIfElement(child, "name", [&name](nlohmann::json& name_child) {
                name = name_child.get<string>();
            });
            jsonExecuteIfElement(child, "pbrMetallicRoughness", [this, &texture_to_image, node_index, &mat](nlohmann::json& pbr_child) {
                jsonExecuteIfElement(pbr_child, "metallicFactor", [&mat](nlohmann::json& metallicFactor) {
                    mat.metallicFactor = metallicFactor.get<float>();
                });
                jsonExecuteIfElement(pbr_child, "roughnessFactor", [&mat](nlohmann::json& roughnessFactor) {
                    mat.roughnessFactor = roughnessFactor.get<float>();
                });
                jsonExecuteIfElement(pbr_child, "baseColorTexture", [&texture_to_image, node_index, &mat](nlohmann::json& baseColorTexture) {
                    jsonExecuteIfElement(baseColorTexture, "index", [&texture_to_image, node_index, &mat](nlohmann::json& index) {
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
                jsonExecuteIfElement(pbr_child, "baseColorFactor", [node_index, &mat](nlohmann::json& baseColorMaterial) {
                    mat.textureIndex = -1;
                    mat.colors[0] = baseColorMaterial[0];
                    mat.colors[1] = baseColorMaterial[1];
                    mat.colors[2] = baseColorMaterial[2];
                    console->debug("material {} is using color with RGB={},{},{}",
                                    node_index, mat.colors[0], mat.colors[1], mat.colors[2]);
                });
                materialList.push_back(mat);
            });
            console->debug("found material:{} with name '{}' with texture ID={}", to_string(node_index), name, mat.textureIndex);
    });
    }

    void loadId(uint32_t index) override
    {
        if (index >= materialList.size())
            throw GltfException("No such material with index: " + std::to_string(index));
        auto& mat = materialList[index];
        setPBR(mat.metallicFactor, mat.roughnessFactor);
        if (mat.textureIndex != -1) {
            console->debug("Apply texture {} from material {}", mat.textureIndex, index);
            textureList[mat.textureIndex]->apply();
        } else {
            console->debug("Apply colored material {}", index);
            setColoredMode(mat.colors);
        }
    }

    ~GltfMaterialAccessorImpl() override
    {

    };
};

UGltfMaterialAccessorIFace GltfMaterialLibraryImpl::getMaterialAccessor(
    const FileLibrary::UriReference& dir,
    nlohmann::json& file)
{
        return make_unique<GltfMaterialAccessorImpl>(this, dir, file);
};

void GltfMaterialLibraryImpl::releaseTexture(const FileLibrary::UriReference ref)
{
    console->debug("Remove texture from list: {}", ref.getPath());
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
    for (auto tex: textureMap) {
        if (auto stex=tex.second.lock()) {
            stex->leaveLibrary();
        }
    }
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

GltfMaterialAccessorIFace::~GltfMaterialAccessorIFace() {}


GltfTextureReference::GltfTextureReference(PGltfMaterialLibraryImpl _lib, const FileLibrary::UriReference _ref) :
        ref(_ref), lib(_lib)
{
    string f_path = ref.getPath().c_str();
    console->debug("Load texture {}", f_path);
    FileContentPtr textureContent = ref.readContent();
    fipImage img;
    int format;

    fipMemoryIO mem((BYTE*)(textureContent->data()), (DWORD)textureContent->size());
    bool result = img.loadFromMemory(mem);
    if (!result)
        throw GltfException(string("Error loading texture file: ") + f_path);
    console->debug("Load texture {} returned {} size {}x{}, image type={}, BPP={}", f_path, result,
        img.getWidth(), img.getHeight(), img.getImageType(), img.getBitsPerPixel());
    switch (img.getBitsPerPixel())
    {
        case 32: format = GL_RGBA; break;
        case 24: format = GL_RGB; break;
        default: throw GltfException("Unhandled pixel size: " + to_string(img.getBitsPerPixel()));
    }
    glGenTextures(1, &gl_texture);
    gltf_check_gl_error("glGenTextures " + f_path);
    glBindTexture(GL_TEXTURE_2D, gl_texture);
	gltf_check_gl_error("glBindTexture " + f_path);
    glTexImage2D(GL_TEXTURE_2D, 0, format, img.getWidth(), img.getHeight(), 0, GL_BGR,  GL_UNSIGNED_BYTE, (*img).data);
	gltf_check_gl_error("glTexImage2D " + f_path);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
    gltf_check_gl_error("glGenerateMipmap " + f_path);
};

void GltfTextureReference::apply()
{
    console->debug("Apply texture {} {}", ref.getPath().c_str(), gl_texture);
    setTextureMode(gl_texture);
}

GltfTextureReference::~GltfTextureReference()
{
    if (lib) {
        console->debug("Report to release texture {}", ref.getPath().c_str());
        lib->releaseTexture(ref);
        glDeleteTextures(1, &gl_texture);
    } else {
        console->debug("Unable to report to release texture {}, library left", ref.getPath().c_str());
    }
}

void GltfTextureReference::leaveLibrary()
{
    lib = nullptr;
}