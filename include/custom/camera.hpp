#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};
struct Segment {
  glm::vec3 origin;
  glm::vec3 direction;
  float size;
};

// default values for the camera
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.00001f;
const float ZOOM = 45.0f;

// Abstract camera class
class Camera {
public:
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;
  // euler angles
  float yaw;
  float pitch;

  // camera options
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  // Constructor 1
  Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f), float Yaw = YAW,
         float Pitch = PITCH, float Zoom = ZOOM,
         glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f), float Speed = SPEED,
         float Sens = SENSITIVITY);

  // Constructor 2
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float Yaw, float Pitch, glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
         float Speed = SPEED, float Sens = SENSITIVITY, float Zoom = ZOOM);
  void processKeyBoardRotate(Camera_Movement direction, float deltaTime);
  virtual void processKeyboard(Camera_Movement direction, float deltaTime);
  glm::mat4 getViewMatrix();
  virtual void processMouseMovement(float xoffset, float yoffset,
                                    GLboolean pitchBound = true);
  void processMouseScroll(float yoffset);
  Ray getRayToPosV4Perspective(glm::vec4 posn);
  Ray getRayToPosV4Ortho(glm::vec4 posn);
  Segment getSegmentToPosV4Perspective(glm::vec4 posn);
  Segment getSegmentToPosV4Ortho(glm::vec4 posn);
  glm::vec3 getPosToPosV4Perspective(glm::vec4 posn);
  glm::vec3 getPosToPosV4Ortho(glm::vec4 posn);
  void setYaw(float nyaw) {
    yaw = nyaw;
    updateCameraVectors();
  }
  void setPitch(float npitch) {
    pitch = npitch;
    updateCameraVectors();
  }
  void setZoom(float nzoom) { zoom = nzoom; }

private:
  void updateCameraVectors();
};

// first constructor
Camera::Camera(glm::vec3 Position, glm::vec3 Up, float Yaw, float Pitch,
               float Zoom, glm::vec3 Front, float Speed, float Sens) {
  this->pos = Position;
  this->worldUp = Up;
  this->pitch = Pitch;
  this->yaw = Yaw;
  this->movementSpeed = Speed;
  this->mouseSensitivity = Sens;
  this->front = Front;
  this->zoom = Zoom;
  this->updateCameraVectors();
}

// second constructor
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float Yaw, float Pitch, glm::vec3 Front, float Speed,
               float Sens, float Zoom) {
  this->pos = glm::vec3(posX, posY, posZ);
  this->worldUp = glm::vec3(upX, upY, upZ);
  this->yaw = Yaw;
  this->pitch = Pitch;
  this->movementSpeed = Speed;
  this->mouseSensitivity = Sens;
  this->zoom = Zoom;
  this->front = Front;
  this->updateCameraVectors();
}
void Camera::updateCameraVectors() {
  glm::vec3 front;
  // compute new front
  front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  front.y = sin(glm::radians(this->pitch));
  front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front = glm::normalize(front);

  // compute right and up
  this->right = glm::normalize(glm::cross(this->front, this->worldUp));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}
void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = this->movementSpeed * deltaTime;
  switch (direction) {
  case FORWARD:
    this->pos += this->front * velocity;
    break;
  case BACKWARD:
    this->pos -= this->front * velocity;
    break;
  case RIGHT:
    this->pos += this->right * velocity;
    break;
  case LEFT:
    this->pos -= this->right * velocity;
    break;
  }
}

glm::mat4 Camera::getViewMatrix() {
  glm::vec3 target = this->pos + this->front;
  glm::vec3 upvec = this->up;
  glm::vec3 cameraDirection = glm::normalize(this->pos - target);
  glm::vec3 right = glm::normalize(glm::cross(upvec, cameraDirection));
  glm::vec3 realUp = glm::normalize(glm::cross(cameraDirection, right));
  //
  glm::mat4 trans(1.0f);
  trans[3][0] = -this->pos.x;
  trans[3][1] = -this->pos.y;
  trans[3][2] = -this->pos.z;

  //
  glm::mat4 rotation(1.0f);
  rotation[0][0] = right.x;
  rotation[1][0] = right.y;
  rotation[2][0] = right.z;
  rotation[0][1] = realUp.x;
  rotation[1][1] = realUp.y;
  rotation[2][1] = realUp.z;
  rotation[0][2] = cameraDirection.x;
  rotation[1][2] = cameraDirection.y;
  rotation[2][2] = cameraDirection.z;
  return rotation * trans;
}

void Camera::processMouseMovement(float xoffset, float yoffset,
                                  GLboolean pitchBound) {
  xoffset *= this->mouseSensitivity;
  yoffset *= this->mouseSensitivity;

  this->yaw += xoffset;
  this->pitch += yoffset;

  if (pitchBound) {
    if (this->pitch > 89.0f) {
      this->pitch = 89.0f;
    }
    if (this->pitch < -89.0f) {
      this->pitch = -89.0f;
    }
  }

  this->updateCameraVectors();
}
void Camera::processKeyBoardRotate(Camera_Movement direction, float deltaTime) {

  deltaTime *= this->movementSpeed;
  switch (direction) {
  case FORWARD:
    this->pitch += deltaTime;
    break;
  case BACKWARD:
    this->pitch -= deltaTime;
    break;
  case RIGHT:
    this->yaw += deltaTime;
    break;
  case LEFT:
    this->yaw -= deltaTime;
    break;
  }
  this->updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
  float zoom = this->zoom;

  if (this->zoom >= 1.0f && this->zoom <= 45.0f) {
    this->zoom -= yoffset;
  }
  if (this->zoom <= 1.0f) {
    this->zoom = 1.0f;
  }
  if (this->zoom >= 45.0f) {
    this->zoom = 45.0f;
  }
}
glm::vec3 Camera::getPosToPosV4Perspective(glm::vec4 pos) {
  glm::vec3 posPers = glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
  return posPers - this->pos;
}
glm::vec3 Camera::getPosToPosV4Ortho(glm::vec4 pos) {
  glm::vec3 posPers = glm::vec3(pos.x, pos.y, pos.z);
  return posPers - this->pos;
}
Segment Camera::getSegmentToPosV4Perspective(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  glm::vec3 segment = this->getPosToPosV4Perspective(pos);
  Segment s;
  s.origin = segment;
  s.size = glm::length(segment);
  s.direction = glm::normalize(segment);
  return s;
}
Segment Camera::getSegmentToPosV4Ortho(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  glm::vec3 segment = this->getPosToPosV4Ortho(pos);
  Segment s;
  s.origin = segment;
  s.direction = glm::normalize(segment);
  s.size = glm::length(segment);
  return s;
}
Ray Camera::getRayToPosV4Perspective(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  Segment s = this->getSegmentToPosV4Perspective(pos);
  Ray r;
  r.origin = s.origin;
  r.direction = s.direction;
  return r;
}
Ray Camera::getRayToPosV4Ortho(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  Segment s = this->getSegmentToPosV4Ortho(pos);
  Ray r;
  r.origin = s.origin;
  r.direction = s.direction;
  return r;
}

class FpsCamera : public Camera {
  void processKeyboard(Camera_Movement direction, float deltaTime) override;
};

void FpsCamera::processKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = this->movementSpeed * deltaTime;
  switch (direction) {
  case FORWARD:
    this->pos += this->front * velocity;
    break;
  case BACKWARD:
    this->pos -= this->front * velocity;
    break;
  case RIGHT:
    this->pos += this->right * velocity;
    break;
  case LEFT:
    this->pos -= this->right * velocity;
    break;
  }
  this->pos.y = 0.0f;
}

#endif
