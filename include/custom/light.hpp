#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <custom/external.hpp>

using vec3 = glm::vec3;
using point3 = vec3;
using color = vec3;

// default values for the camera
const float LYAW = -90.0f;
const float LPITCH = 0.0f;
const float LSPEED = 2.5f;

//

enum LightMovement { L_FORWARD, L_BACKWARD, L_LEFT, L_RIGHT };

class Light {
public:
  color emitColor;

  virtual bool emitted(color &emits) const {
    emits = emitColor;
    return true;
  }
  Light(color lightColor) : emitColor(lightColor) {}
};

class DirectionalLight : public Light {
public:
  vec3 front;
  vec3 up;
  vec3 right;
  vec3 worldUp;
  float yaw, pitch;
  DirectionalLight(color lightColor, vec3 wup, float y = LYAW, float p = LPITCH)
      : Light(lightColor), yaw(y), pitch(p), worldUp(wup) {
    updateDirection();
  }
  void setYaw(float val) {
    yaw = val;
    updateDirection();
  }
  void setPitch(float val) {
    pitch = val;
    updateDirection();
  }

protected:
  void updateDirection() {
    //
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};
class PointLight : public Light {
public:
  point3 position;
  PointLight(color lightColor, point3 pos) : Light(lightColor), position(pos) {}
};

class SpotLight : public DirectionalLight, public PointLight {
public:
  float cutOff;
  float outerCutoff;
  float movementSpeed;
  SpotLight(color lightColor, point3 pos, vec3 wup, float y = LYAW,
            float p = LPITCH, float cutOffAngleDegree = 0.91,
            float outerCut = 0.82, float mspeed = LSPEED)

      : DirectionalLight(lightColor, wup, y, p), PointLight(lightColor, pos),
        cutOff(glm::radians(cutOffAngleDegree)), outerCutoff(outerCut),
        movementSpeed(mspeed) {}
  glm::mat4 getViewMatrix() {
    vec3 target = position - front;
    vec3 lightDir = glm::normalize(position - target);
    vec3 rightDir = glm::normalize(glm::cross(up, lightDir));
    glm::vec3 realUp = glm::normalize(glm::cross(lightDir, rightDir));
    //
    glm::mat4 trans(1.0f);
    trans[3][0] = -position.x;
    trans[3][1] = -position.y;
    trans[3][2] = -position.z;

    //
    glm::mat4 rotation(1.0f);
    rotation[0][0] = rightDir.x;
    rotation[1][0] = rightDir.y;
    rotation[2][0] = rightDir.z;
    rotation[0][1] = realUp.x;
    rotation[1][1] = realUp.y;
    rotation[2][1] = realUp.z;
    rotation[0][2] = lightDir.x;
    rotation[1][2] = lightDir.y;
    rotation[2][2] = lightDir.z;
    return rotation * trans;
  }
  void processKeyBoardRotate(LightMovement direction, float deltaTime) {

    deltaTime *= movementSpeed;
    switch (direction) {
    case L_FORWARD:
      pitch += deltaTime;
      break;
    case L_BACKWARD:
      pitch -= deltaTime;
      break;
    case L_RIGHT:
      yaw += deltaTime;
      break;
    case L_LEFT:
      yaw -= deltaTime;
      break;
    }
    updateDirection();
  }
};

#endif
