#include <camera.hpp>

#include <glm/gtc/type_ptr.hpp>

void Camera::processInput(GLFWwindow *window, float deltaTime) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = static_cast<float>(2.5 * deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

glm::mat4 Camera::getProgectionMatrix() {
  return glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f,
                          100.0f);
}

glm::mat4 Camera::getView() {
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}