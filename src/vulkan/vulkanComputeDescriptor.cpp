#include <window.hpp>

void WindowHandler::createComputeDescriptorSetLayout() {

  VkDescriptorSetLayoutBinding storageImageBinding{};
  storageImageBinding.binding = 0;
  storageImageBinding.descriptorCount = 1;
  storageImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  storageImageBinding.pImmutableSamplers = nullptr;
  storageImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  VkDescriptorSetLayoutBinding triangleBufferBinding{};
  triangleBufferBinding.binding = 1;
  triangleBufferBinding.descriptorCount = 1;
  triangleBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  triangleBufferBinding.pImmutableSamplers = nullptr;
  triangleBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  VkDescriptorSetLayoutBinding bvhBufferBinding{};
  bvhBufferBinding.binding = 2;
  bvhBufferBinding.descriptorCount = 1;
  bvhBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  bvhBufferBinding.pImmutableSamplers = nullptr;
  bvhBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  VkDescriptorSetLayoutBinding cameraUBOBinding{};
  cameraUBOBinding.binding = 3;
  cameraUBOBinding.descriptorCount = 1;
  cameraUBOBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  cameraUBOBinding.pImmutableSamplers = nullptr;
  cameraUBOBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
      storageImageBinding, triangleBufferBinding, bvhBufferBinding,
      cameraUBOBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &computeDescriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create compute descriptor set layout!");
  }
}

void WindowHandler::createComputeDescriptorPool() {
  std::array<VkDescriptorPoolSize, 4> poolSizes{};

  // Storage image
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  poolSizes[0].descriptorCount = 1;

  // Triangle buffer
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[1].descriptorCount = 1;

  // BVH buffer
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[2].descriptorCount = 1;

  // Camera/light UBO
  poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[3].descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = 1;

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr,
                             &computeDescriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create compute descriptor pool!");
  }
}

void WindowHandler::createComputeDescriptorSet() {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = computeDescriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &computeDescriptorSetLayout;

  if (vkAllocateDescriptorSets(device, &allocInfo, &computeDescriptorSet) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate compute descriptor set!");
  }

  //----------------------------------------
  // Storage Image
  //----------------------------------------
  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageInfo.imageView = resultImageView;
  imageInfo.sampler = VK_NULL_HANDLE;

  //----------------------------------------
  // Triangle Buffer
  //----------------------------------------
  VkDescriptorBufferInfo triangleBufferInfo{};
  triangleBufferInfo.buffer = triangleBuffer;
  triangleBufferInfo.offset = 0;
  triangleBufferInfo.range = VK_WHOLE_SIZE;

  //----------------------------------------
  // BVH Buffer
  //----------------------------------------
  VkDescriptorBufferInfo bvhBufferInfo{};
  bvhBufferInfo.buffer = bvhBuffer;
  bvhBufferInfo.offset = 0;
  bvhBufferInfo.range = VK_WHOLE_SIZE;

  //----------------------------------------
  // Camera UBO
  //----------------------------------------
  VkDescriptorBufferInfo cameraBufferInfo{};
  cameraBufferInfo.buffer = computeUniformBuffer;
  cameraBufferInfo.offset = 0;
  cameraBufferInfo.range = sizeof(ComputeUBO);

  std::array<VkWriteDescriptorSet, 4> descriptorWrites{};

  // Binding 0 — Storage Image
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = computeDescriptorSet;
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pImageInfo = &imageInfo;

  // Binding 1 — Triangle Buffer
  descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[1].dstSet = computeDescriptorSet;
  descriptorWrites[1].dstBinding = 1;
  descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  descriptorWrites[1].descriptorCount = 1;
  descriptorWrites[1].pBufferInfo = &triangleBufferInfo;

  // Binding 2 — BVH Buffer
  descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[2].dstSet = computeDescriptorSet;
  descriptorWrites[2].dstBinding = 2;
  descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  descriptorWrites[2].descriptorCount = 1;
  descriptorWrites[2].pBufferInfo = &bvhBufferInfo;

  // Binding 3 — Camera UBO
  descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[3].dstSet = computeDescriptorSet;
  descriptorWrites[3].dstBinding = 3;
  descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[3].descriptorCount = 1;
  descriptorWrites[3].pBufferInfo = &cameraBufferInfo;

  vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void WindowHandler::createComputeBuffers() {

  triangleBufferSize = sizeof(TriangleGPU) * 4;
  createBuffer(triangleBufferSize,
               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, triangleBuffer,
               triangleBufferMemory);

  bvhBufferSize = sizeof(BVHNodeGPU) * 4;
  createBuffer(bvhBufferSize,
               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bvhBuffer, bvhBufferMemory);

  createBuffer(sizeof(ComputeUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               computeUniformBuffer, computeUniformBufferMemory);
}

void WindowHandler::updateComputeDescriptorBuffers() {
  VkDescriptorBufferInfo triangleBufferInfo{};
  triangleBufferInfo.buffer = triangleBuffer;
  triangleBufferInfo.offset = 0;
  triangleBufferInfo.range = VK_WHOLE_SIZE;

  VkDescriptorBufferInfo bvhBufferInfo{};
  bvhBufferInfo.buffer = bvhBuffer;
  bvhBufferInfo.offset = 0;
  bvhBufferInfo.range = VK_WHOLE_SIZE;

  std::array<VkWriteDescriptorSet, 2> writes{};

  // Triangle buffer
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].dstSet = computeDescriptorSet;
  writes[0].dstBinding = 1;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  writes[0].descriptorCount = 1;
  writes[0].pBufferInfo = &triangleBufferInfo;

  // BVH buffer
  writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[1].dstSet = computeDescriptorSet;
  writes[1].dstBinding = 2;
  writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  writes[1].descriptorCount = 1;
  writes[1].pBufferInfo = &bvhBufferInfo;

  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()),
                         writes.data(), 0, nullptr);
}