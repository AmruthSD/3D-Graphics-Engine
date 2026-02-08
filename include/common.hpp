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

static const int CHUNK_SIZE = 16;
static const int MAX_HEIGHT = 128;
static const int SEA_LEVEL = 0;
const int RADIUS = 8;

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

struct Vertex {
  glm::vec3 position;
  uint32_t material;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static std::array<VkVertexInputAttributeDescription, 2>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attrs{};

    // position
    attrs[0].binding = 0;
    attrs[0].location = 0;
    attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[0].offset = offsetof(Vertex, position);

    // color
    attrs[1].binding = 0;
    attrs[1].location = 1;
    attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[1].offset = offsetof(Vertex, material);

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