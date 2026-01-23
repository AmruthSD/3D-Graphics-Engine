#include <camera.hpp>

#include <glm/gtc/type_ptr.hpp>

void Camera::processInput(GLFWwindow *window, float deltaTime) {
  float cameraSpeed = 2.5f * deltaTime;
  float rotationSpeed = 60.0f * deltaTime; // degrees per second

  // -------- Movement --------
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;

  glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= cameraRight * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += cameraRight * cameraSpeed;

  // Up / Down (world up)
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos += cameraUp * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    cameraPos -= cameraUp * cameraSpeed;

  // -------- Rotation --------
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    yaw -= rotationSpeed;

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    yaw += rotationSpeed;

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    pitch += rotationSpeed;

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    pitch -= rotationSpeed;

  // Clamp pitch (avoid flip)
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  // -------- Update cameraFront --------
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  cameraFront = glm::normalize(front);
}

glm::mat4 Camera::getProgectionMatrix() {
  return glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f,
                          100.0f);
}

glm::mat4 Camera::getView() {
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}