#pragma once
#include <common.hpp>

class Camera {
  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

  FrustumPlane frustumPlanes[6];

  bool firstMouse = true;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float roll = 0.0f;

  float fov = 45.0f;

public:
  void processInput(GLFWwindow *, float);

  glm::mat4 getProgectionMatrix(float);

  glm::mat4 getView();

  Chunk getCurrentChunk();

  const glm::vec3 &getPosition() const { return cameraPos; }
  float getX() const { return cameraPos.x; }
  float getY() const { return cameraPos.y; }
  float getZ() const { return cameraPos.z; }

  bool isChunkInFrustum(Chunk);
  void updateFrustumPlanes(float aspect, float nearPlane, float farPlane);
};