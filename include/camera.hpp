#include <common.hpp>

class Camera {
public:
  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

  bool firstMouse = true;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float roll = 0.0f;

  float lastX = 800.0f / 2.0;
  float lastY = 600.0 / 2.0;
  float fov = 45.0f;

  void processInput(GLFWwindow *, float);

  glm::mat4 getProgectionMatrix(float);

  glm::mat4 getView();
};