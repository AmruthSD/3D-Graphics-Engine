#pragma once
#include <bits/stdc++.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <stb_image.h>

using namespace std;

const std::string TEXTURE_PATH = "../assets/viking_room.png";

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

static const int CHUNK_SIZE = 12;
static const int MAX_HEIGHT = 128;
static const int SEA_LEVEL = 0;
const int RADIUS = 10;
const float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = RADIUS * CHUNK_SIZE;

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

struct Vertex {
  glm::vec3 position;
  uint32_t inBiome;
  glm::vec2 localUV;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attrs{};

    // position
    attrs[0].binding = 0;
    attrs[0].location = 0;
    attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[0].offset = offsetof(Vertex, position);

    // color
    attrs[1].binding = 0;
    attrs[1].location = 1;
    attrs[1].format = VK_FORMAT_R32_UINT;
    attrs[1].offset = offsetof(Vertex, inBiome);

    // coordinates
    attrs[2].binding = 0;
    attrs[2].location = 2;
    attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
    attrs[2].offset = offsetof(Vertex, localUV);

    return attrs;
  }
};

struct ChunkMesh {
  std::vector<Vertex> vertices;
};

enum class Biome { Ocean, Plain, Mountain, Snow };

struct Chunk {
  int x;
  int z;

  bool operator==(const Chunk &other) const {
    return x == other.x && z == other.z;
  }
};

struct ChunkHash {
  std::size_t operator()(const Chunk &c) const {
    return std::hash<int>()(c.x) ^ (std::hash<int>()(c.z) << 1);
  }
};

struct FrustumPlane {
  glm::vec3 normal;
  float d;
};

struct TriangleGPU {
  glm::vec3 v0;
  float pad0;
  glm::vec3 v1;
  float pad1;
  glm::vec3 v2;
  float pad2;
};

struct BVHNodeGPU {
  glm::vec3 minBounds;
  float pad0;
  glm::vec3 maxBounds;
  float pad1;
  int leftFirst;
  int count;
  int pad2;
  int pad3;
};

struct alignas(16) ComputeUBO {
  glm::mat4 invViewProj;

  glm::vec4 cameraPos; // w unused
  glm::vec4 lightDir;  // w unused

  glm::vec2 resolution;
  glm::vec2 padding;
};