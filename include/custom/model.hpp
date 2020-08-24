// declare the include guard
#ifndef MODEL_HPP
#define MODEL_HPP

// declare libs
#include <cmath>
#include <custom/external.hpp>

//

// mesh shader
#include <custom/image.hpp>
#include <custom/mesh.hpp>
#include <custom/shader.hpp>
#include <sstream>

//
namespace fs = std::filesystem;

unsigned int getTextureFromFile(const char *path, bool gamma);
unsigned int getTextureFromFile(const char *path, fs::path dir, bool gamma);

class Model {
public:
  // some statistics about
  int max_ao_nb = 0;
  int max_metallic_nb = 0;
  int max_albedo_nb = 0;
  int max_diffuse_nb = 0;
  int max_specular_nb = 0;
  int max_normals_nb = 0;
  int max_height_nb = 0;
  int max_roughness_nb = 0;

  bool isVertexOnly = false;
  bool isHdr = false;

  unsigned int postProcessFlags;

  std::vector<Texture> loaded_textures;
  std::vector<Mesh> meshes;
  fs::path dirpath;

  bool isGammaCorrected;
  Model(fs::path fpath, unsigned int flags, bool gamma = false,
        bool vertex_only = false, bool hdr_only = false)
      : isGammaCorrected(gamma), postProcessFlags(flags),
        isVertexOnly(vertex_only), isHdr(hdr_only) {

    load_model(fpath);
    std::cout << "Some statistics about model:" << std::endl;
    std::cout << "maximum number of ambient occlusion maps: " << max_ao_nb
              << std::endl;
    std::cout << "maximum number of metallic maps: " << max_metallic_nb
              << std::endl;
    std::cout << "maximum number of albedo maps: " << max_albedo_nb
              << std::endl;
    std::cout << "maximum number of diffuse maps: " << max_diffuse_nb
              << std::endl;
    std::cout << "maximum number of specular maps: " << max_specular_nb
              << std::endl;
    std::cout << "maximum number of normal maps: " << max_normals_nb
              << std::endl;
    std::cout << "maximum number of height maps: " << max_height_nb
              << std::endl;
    std::cout << "maximum number of roughness maps: " << max_roughness_nb
              << std::endl;
    std::cout << "total texture count: " << loaded_textures.size() << std::endl;
    std::cout << "total mesh count: " << meshes.size() << std::endl;
    std::cout << "path: " << fpath.c_str() << std::endl;
  }

  void Draw(Shader shdr) {
    //
    for (int i = 0; i < meshes.size(); i++) {
      //
      meshes[i].draw(shdr);
    }
  }
  /**
  draw some of the meshes not all
 */
  void Draw(Shader shdr, const unsigned int &mstart, const unsigned int &mend,
            bool isSlice) {
    if (mend >= meshes.size()) {
      throw std::invalid_argument(
          "given end mesh count is bigger than available meshes: ");
    }
    for (int i = mstart; i < mend; i++) {
      //
      meshes[i].draw(shdr);
    }
  }
  /**
    Drawing to hdr fbo then to a quad shader
   */
  void Draw(Shader meshShader, const GLuint &fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (int i = 0; i < meshes.size(); i++) {
      //
      meshes[i].draw(meshShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

private:
  void load_model(fs::path model_path) {
    //
    Assimp::Importer import;
    const aiScene *scene =
        import.ReadFile(model_path.string(), postProcessFlags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      //
      std::cout << "ERROR::ASSIMP:: " << import.GetErrorString() << std::endl;
      return;
    }
    // get directory
    if (model_path.has_parent_path()) {
      dirpath = model_path.parent_path();
    }

    // process nodes
    processNode(scene->mRootNode, scene);
  }
  void processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
    }
  }
  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    // data of mesh
    std::vector<Vertex> vs = processVertex(mesh);
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> ts;
    if (isVertexOnly == false) {
      ts = processMaterial(mat);
    }
    std::vector<unsigned int> inds = processFaces(mesh);
    return Mesh(vs, ts, inds);
  }
  std::vector<unsigned int> processFaces(aiMesh *mesh) {
    //
    std::vector<unsigned int> inds;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        inds.push_back(face.mIndices[j]);
      }
    }
    return inds;
  }

  std::vector<Vertex> processVertex(aiMesh *mesh) {
    //
    std::vector<Vertex> vs;
    for (int i = 0; i < mesh->mNumVertices; i++) {
      //
      Vertex vrtx;
      // vertex positions
      glm::vec3 vect;
      vect.x = mesh->mVertices[i].x;
      vect.y = mesh->mVertices[i].y;
      vect.z = mesh->mVertices[i].z;
      vrtx.pos = vect;
      //
      vect.x = mesh->mNormals[i].x;
      vect.y = mesh->mNormals[i].y;
      vect.z = mesh->mNormals[i].z;
      vrtx.normal = vect;
      if (mesh->mTextureCoords[0]) {
        //
        glm::vec2 tv; // texture coords
        tv.x = mesh->mTextureCoords[0][i].x;
        tv.y = mesh->mTextureCoords[0][i].y;
        vrtx.texCoord = tv;
      } else {

        vrtx.texCoord = glm::vec2(0.0f);
      }
      glm::vec3 v2;
      if (mesh->HasTangentsAndBitangents()) {
        // tangent
        vect.x = mesh->mTangents[i].x;
        vect.y = mesh->mTangents[i].y;
        vect.z = mesh->mTangents[i].z;
        v2.x = mesh->mBitangents[i].x;
        v2.y = mesh->mBitangents[i].y;
        v2.z = mesh->mBitangents[i].z;

      } else {
        vect = glm::vec3(INFINITY);
        v2 = glm::vec3(INFINITY);
      }
      vrtx.tangent = vect;
      vrtx.bitangent = v2;

      // bitangent
      vs.push_back(vrtx);
    }
    return vs;
  }
  std::vector<Texture> processMaterial(const aiMaterial *mat) {
    // diffuse textures
    std::vector<Texture> ts;
    std::vector<Texture> diffuseMaps =
        loadMaterialTexture(mat, aiTextureType_DIFFUSE, TextureType::Diffuse);
    max_diffuse_nb = max_diffuse_nb < diffuseMaps.size() ? diffuseMaps.size()
                                                         : max_diffuse_nb;
    ts.insert(ts.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps =
        loadMaterialTexture(mat, aiTextureType_SPECULAR, TextureType::Specular);
    max_specular_nb = max_specular_nb < specularMaps.size()
                          ? specularMaps.size()
                          : max_specular_nb;

    ts.insert(ts.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps =
        loadMaterialTexture(mat, aiTextureType_NORMALS, TextureType::Normal); //
    // this HEIGHT should be changed for other formats than obj
    max_normals_nb =
        max_normals_nb < normalMaps.size() ? normalMaps.size() : max_normals_nb;
    ts.insert(ts.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps =
        loadMaterialTexture(mat, aiTextureType_HEIGHT, TextureType::Height);
    max_height_nb =
        max_height_nb < heightMaps.size() ? heightMaps.size() : max_height_nb;
    ts.insert(ts.end(), heightMaps.begin(), heightMaps.end());
    //
    std::vector<Texture> aoMaps =
        loadMaterialTexture(mat, aiTextureType_LIGHTMAP, TextureType::Ao);
    max_ao_nb = max_ao_nb < aoMaps.size() ? aoMaps.size() : max_ao_nb;
    ts.insert(ts.end(), aoMaps.begin(), aoMaps.end());
    //
    std::vector<Texture> roughMaps =
        loadMaterialTexture(mat, aiTextureType_UNKNOWN, TextureType::Roughness);
    max_roughness_nb = max_roughness_nb < roughMaps.size() ? roughMaps.size()
                                                           : max_roughness_nb;
    ts.insert(ts.end(), roughMaps.begin(), roughMaps.end());
    //
    std::vector<Texture> metalMaps =
        loadMaterialTexture(mat, aiTextureType_UNKNOWN, TextureType::Metallic);
    max_metallic_nb =
        max_metallic_nb < metalMaps.size() ? metalMaps.size() : max_metallic_nb;

    ts.insert(ts.end(), metalMaps.begin(), metalMaps.end());
    //
    std::vector<Texture> albedoMaps =
        loadMaterialTexture(mat, aiTextureType_UNKNOWN, TextureType::Albedo);
    max_albedo_nb =
        max_albedo_nb < albedoMaps.size() ? albedoMaps.size() : max_albedo_nb;
    ts.insert(ts.end(), albedoMaps.begin(), albedoMaps.end());

    return ts;
  }
  std::vector<Texture> loadMaterialTexture(const aiMaterial *mat,
                                           aiTextureType type,
                                           TextureType typeName) {
    //
    std::vector<Texture> txts;
    unsigned int tcount = mat->GetTextureCount(type);
    for (unsigned int i = 0; i < tcount; i++) {
      aiString str;
      if (AI_SUCCESS != mat->GetTexture(type, i, &str)) {
        std::cout << "ERROR::ASSIMP::MATERIAL: "
                  << "Loading of material"
                  << " failed for type " << typeName << std::endl;
        throw std::invalid_argument("unable to load textures with given: "
                                    "arguments");
      }

      bool skip = false;
      for (unsigned int j = 0; j < loaded_textures.size(); j++) {
        //
        if (std::strcmp(loaded_textures[j].path.c_str(), str.C_Str()) == 0) {
          txts.push_back(loaded_textures[j]);
          skip = true;
          break;
        }
      }
      if (skip == false) {
        Texture txt = makeTexture(typeName, str);
        txts.push_back(txt);
        loaded_textures.push_back(txt);
      }
    }
    return txts;
  }
  Texture makeTexture(TextureType type, aiString str) {
    // make texture from given
    Texture txt;
    txt.id = getTextureFromFile(str.C_Str(), dirpath, isGammaCorrected);
    txt.type = type;
    switch (txt.type) {
    case TextureType::Diffuse:
      txt.name = "diffuseMap";
      break;
    case TextureType::Specular:
      txt.name = "specularMap";
      break;
    case TextureType::Normal:
      txt.name = "normalMap";
      break;
    case TextureType::Height:
      txt.name = "heightMap";
      break;
    case TextureType::Displacement:
      txt.name = "displacementMap";
      break;
    case TextureType::Ao:
      txt.name = "aoMap";
      break;
    case TextureType::Roughness:
      txt.name = "roughnessMap";
      break;
    case TextureType::Albedo:
      txt.name = "albedoMap";
      break;
    case TextureType::Metallic:
      txt.name = "metallicMap";
      break;
    }
    txt.path = fs::path(str.C_Str());
    return txt;
  }
  void renderQuad() {
    // from learnopengl.com
    GLuint quadVAO;
    GLuint quadVBO;

    float screen[] = {//
                      // positions    texCoords //
                      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
                      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //

                      1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
                      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f}; //
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen, GL_STATIC_DRAW);
    // ------------- shader specific -------------
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
  }
};

unsigned int getTextureFromFile(const char *path, bool gamma) {
  GLuint tex;
  glGenTextures(1, &tex);
  //
  int width, height, nbChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nbChannels, 0);
  if (data) {
    GLenum format;
    GLenum informat;
    if (nbChannels == 1) {
      format = GL_RED;
    } else if (nbChannels == 3) {
      format = GL_RGB;
      informat = gamma ? GL_SRGB : GL_RGB;
    } else if (nbChannels == 4) {
      format = GL_RGBA;
      informat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // use nearest neighbor interpolation when zooming out
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // use nearest neighbor interpolation when zooming out
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return tex;
}
unsigned int getTextureFromFile(const char *fname, fs::path dir,
                                bool gamma = false) {
  //
  fs::path fnameStr = fs::path(std::string(fname));
  fs::path fpath = dir / fnameStr;
  return getTextureFromFile(fpath.c_str(), gamma);
}

#endif
