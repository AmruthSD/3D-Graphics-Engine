#include <camera.hpp>

#include <glm/gtc/type_ptr.hpp>

void Camera::processInput(GLFWwindow *window, float deltaTime) {
  float cameraSpeed = 25.0f * deltaTime;
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
  glm::mat4 proj =
      glm::perspective(glm::radians(fov), aspect, NEAR_PLANE, FAR_PLANE);

  proj[1][1] *= -1;

  return proj;
}

glm::mat4 Camera::getView() {
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

bool Camera::isChunkInFrustum(Chunk chunk) {

  glm::vec3 min;
  min.x = chunk.x * CHUNK_SIZE;
  min.y = 0;
  min.z = chunk.z * CHUNK_SIZE;

  glm::vec3 max = min + glm::vec3(CHUNK_SIZE, MAX_HEIGHT, CHUNK_SIZE);

  // --- Frustum test ---
  for (int i = 0; i < 6; i++) {
    const glm::vec3 &n = frustumPlanes[i].normal;

    glm::vec3 p;
    p.x = (n.x >= 0.0f) ? max.x : min.x;
    p.y = (n.y >= 0.0f) ? max.y : min.y;
    p.z = (n.z >= 0.0f) ? max.z : min.z;

    // Plane test
    if (glm::dot(n, p) + frustumPlanes[i].d < 0.0f)
      return false;
  }

  return true;
}

void Camera::updateFrustumPlanes(float aspect, float nearPlane,
                                 float farPlane) {
  glm::mat4 view = getView();

  glm::mat4 proj = getProgectionMatrix(aspect);

  glm::mat4 VP = proj * view;

  // Left
  frustumPlanes[0].normal.x = VP[0][3] + VP[0][0];
  frustumPlanes[0].normal.y = VP[1][3] + VP[1][0];
  frustumPlanes[0].normal.z = VP[2][3] + VP[2][0];
  frustumPlanes[0].d = VP[3][3] + VP[3][0];

  // Right
  frustumPlanes[1].normal.x = VP[0][3] - VP[0][0];
  frustumPlanes[1].normal.y = VP[1][3] - VP[1][0];
  frustumPlanes[1].normal.z = VP[2][3] - VP[2][0];
  frustumPlanes[1].d = VP[3][3] - VP[3][0];

  // Bottom
  frustumPlanes[2].normal.x = VP[0][3] + VP[0][1];
  frustumPlanes[2].normal.y = VP[1][3] + VP[1][1];
  frustumPlanes[2].normal.z = VP[2][3] + VP[2][1];
  frustumPlanes[2].d = VP[3][3] + VP[3][1];

  // Top
  frustumPlanes[3].normal.x = VP[0][3] - VP[0][1];
  frustumPlanes[3].normal.y = VP[1][3] - VP[1][1];
  frustumPlanes[3].normal.z = VP[2][3] - VP[2][1];
  frustumPlanes[3].d = VP[3][3] - VP[3][1];

  // Near
  frustumPlanes[4].normal.x = VP[0][3] + VP[0][2];
  frustumPlanes[4].normal.y = VP[1][3] + VP[1][2];
  frustumPlanes[4].normal.z = VP[2][3] + VP[2][2];
  frustumPlanes[4].d = VP[3][3] + VP[3][2];

  // Far
  frustumPlanes[5].normal.x = VP[0][3] - VP[0][2];
  frustumPlanes[5].normal.y = VP[1][3] - VP[1][2];
  frustumPlanes[5].normal.z = VP[2][3] - VP[2][2];
  frustumPlanes[5].d = VP[3][3] - VP[3][2];

  // Normalize all planes
  for (int i = 0; i < 6; i++) {
    float len = glm::length(frustumPlanes[i].normal);
    frustumPlanes[i].normal /= len;
    frustumPlanes[i].d /= len;
  }
}