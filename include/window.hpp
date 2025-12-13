#include "camera.hpp"
#include "common.hpp"

class WindowHandler {
public:
  GLFWwindow *window;
  unsigned int VBO, VAO;
  unsigned int shaderProgram;
  unsigned int texture1, texture2;
  WindowHandler();
  void startWindow();

  Camera *camera;

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  const char *vertexShaderSource =
      "#version 410 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "layout (location = 1) in vec2 aTexCoord;\n"
      "\n"
      "out vec2 TexCoord;\n"
      "\n"
      "uniform mat4 model;\n"
      "uniform mat4 view;\n"
      "uniform mat4 projection;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
      "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
      "}\n";

  const char *fragmentShaderSource =
      "#version 410 core\n"
      "out vec4 FragColor;\n"
      "\n"
      "in vec2 TexCoord;\n"
      "\n"
      "// texture samplers\n"
      "uniform sampler2D texture1;\n"
      "uniform sampler2D texture2;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    // linearly interpolate between both textures (80% container, 20% "
      "awesomeface)\n"
      "    FragColor = mix(texture(texture1, TexCoord), texture(texture2, "
      "TexCoord), 0.2);\n"
      "}\n";

  void createShaders();
  void setBuffers();
  void setTexture();
};