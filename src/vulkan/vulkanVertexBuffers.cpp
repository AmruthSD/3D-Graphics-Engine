#include <window.hpp>

void WindowHandler::createVertexBuffer(Chunk chunk) {

  terrain->generateChunk(chunk.x, chunk.z);
  const auto &verts = terrain->ChunkMeshes[chunk].vertices;
  if (verts.empty())
    return;

  VkDeviceSize bufferSize = sizeof(Vertex) * verts.size();

  // --- Destroy old buffer if chunk already exists ---
  if (vertexBuffers.contains(chunk)) {
    vkDestroyBuffer(device, vertexBuffers[chunk], nullptr);
    vkFreeMemory(device, vertexBufferMemories[chunk], nullptr);
  }

  // --- Staging buffer ---
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, verts.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  // --- GPU local vertex buffer ---
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

  copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  // --- Cleanup staging ---
  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);

  // --- Store per-chunk ---
  vertexBuffers[chunk] = vertexBuffer;
  vertexBufferMemories[chunk] = vertexBufferMemory;
}