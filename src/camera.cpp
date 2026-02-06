#include <camera.hpp>

#include <glm/gtc/type_ptr.hpp>

void Camera::processInput(GLFWwindow *window, float deltaTime) {
  float cameraSpeed = 2.5f * deltaTime;
  float rotationSpeed = 60.0f * deltaTime;

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

  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    roll -= rotationSpeed;

  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    roll += rotationSpeed;

  // Clamp pitch (avoid flip)
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;
  if (roll > 360.0f)
    roll -= 360.0f;
  if (roll < -360.0f)
    roll += 360.0f;

  // -------- Update cameraFront --------
  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);

  // -------- Apply roll --------
  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 right = glm::normalize(glm::cross(cameraFront, worldUp));

  // Rotate up vector around forward axis
  glm::mat4 rollMat =
      glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);

  cameraUp = glm::normalize(glm::vec3(rollMat * glm::vec4(worldUp, 0.0f)));
}

glm::mat4 Camera::getProgectionMatrix(float aspect) {
  glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);

  proj[1][1] *= -1;

  return proj;
}

glm::mat4 Camera::getView() {
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}