// regroups include and global variables
#ifndef SINGULAR_HPP
#define SINGULAR_HPP

#include <custom/external.hpp>

#include <custom/camera.hpp>
#include <custom/image.hpp>
#include <custom/light.hpp>
#include <custom/model.hpp>
#include <custom/shader.hpp>
//

//
namespace fs = std::filesystem;

fs::path current_dir = fs::current_path();
fs::path shaderDirPath = current_dir / "media" / "shaders";
fs::path textureDirPath = current_dir / "media" / "textures";
fs::path plyPath = current_dir / "media" / "ply";
fs::path capturePath = current_dir / "media" / "capture";
fs::path modelPath = current_dir / "media" / "model";

// initialization code

const unsigned int WINWIDTH = 800;
const unsigned int WINHEIGHT = 600;
const float ASPECT_RATIO = (float)WINWIDTH / WINHEIGHT;

// camera related

float lastX = WINWIDTH / 2.0f;
float lastY = WINHEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 2.3f));

// time related
float deltaTime = 0.0f;
float lastTime = 0.0f;

// screen capture
unsigned int captCounter = 0;

// movement related
glm::mat4 modelMat(1.0f);
glm::vec3 transVec(0.0f);
glm::vec3 scale(1.0f);
float angle = 20.0f;
glm::vec3 xaxis(1, 0, 0);
glm::vec3 yaxis(0, 1, 0);
glm::vec3 zaxis(0, 0, 1);

// light related
glm::vec3 lightPos = glm::vec3(0.2f, 1.0f, 0.5f);
float lightIntensity = 1.0f;
bool inTangent = false;
DirectionalLight dirLight(glm::vec3(lightIntensity), glm::vec3(0, 1, 0));
PointLight pointLight(glm::vec3(lightIntensity), lightPos);
SpotLight spotLight(glm::vec3(lightIntensity), lightPos, glm::vec3(0, 1, 0));

#endif
