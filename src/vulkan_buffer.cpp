
#include <vulkan_buffer.hpp>

namespace VulkanStuff {
VulkanBuffer::VulkanBuffer(VkPhysicalDevice inputPhysicalDevice,
                           VkDevice inputDevice, VkQueue inputGraphicsQueue,
                           VkCommandPool inputCommandPool)
    : device{inputDevice}, physicalDevice{inputPhysicalDevice},
      graphicsQueue{inputGraphicsQueue}, commandPool{inputCommandPool} {}

VulkanBuffer::~VulkanBuffer() {
  vkDestroyBuffer(device, vertexBuffer, nullptr);
  vkFreeMemory(device, vertexBufferMemory, nullptr);

  vkDestroyBuffer(device, indexBuffer, nullptr);
  vkFreeMemory(device, indexBufferMemory, nullptr);

  for (size_t i = 0; i < uniformBuffers.size(); i++) {
    vkDestroyBuffer(device, uniformBuffers[i], nullptr);
    vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
  }
  vkDestroyDescriptorPool(device, descriptorPool, nullptr);

  // Destroy second stuff
  vkDestroyBuffer(device, secondUniformBuffers, nullptr);
  vkFreeMemory(device, secondUniformBuffersMemory, nullptr);
  vkDestroyDescriptorPool(device, secondDescriptorPool, nullptr);
}

void VulkanBuffer::createVertexBuffer(std::vector<Utils::Vertex> vertices) {

  // For recreation of vertex buffer, need to first kill the old vertex buffer,
  // and also need to wait until queue is clear since current buffer might be
  // used in the command buffer

  // vkQueueWaitIdle(graphicsQueue);
  // vkDestroyBuffer(device, vertexBuffer, nullptr);
  // vkFreeMemory(device, vertexBufferMemory, nullptr);

  //===================================================

  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  // Create a staging buffer as source for cpu accessible then copy over to
  // actual bufffer
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  Utils::createBuffer(physicalDevice, device, bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  Utils::createBuffer(
      physicalDevice, device, bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

  Utils::copyBuffer(device, commandPool, graphicsQueue, stagingBuffer,
                    vertexBuffer, bufferSize);
  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}
void VulkanBuffer::createIndexBuffer(std::vector<uint16_t> indices) {
  VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  Utils::createBuffer(physicalDevice, device, bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  Utils::createBuffer(
      physicalDevice, device, bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

  Utils::copyBuffer(device, commandPool, graphicsQueue, stagingBuffer,
                    indexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}
void VulkanBuffer::createUniformBuffers(int number) {
  VkDeviceSize bufferSize = sizeof(Utils::UniformBufferObject);

  uniformBuffers.resize(number);
  uniformBuffersMemory.resize(number);

  for (size_t i = 0; i < number; i++) {
    Utils::createBuffer(physicalDevice, device, bufferSize,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        uniformBuffers[i], uniformBuffersMemory[i]);
  }

  // Second uniform buffer
  Utils::createBuffer(physicalDevice, device, bufferSize,
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      secondUniformBuffers, secondUniformBuffersMemory);
}

void VulkanBuffer::createDescriptorPool(int number) {

  std::vector<VkDescriptorPoolSize> poolSizes{};

  VkDescriptorPoolSize poolSizeUBO{};
  poolSizeUBO.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizeUBO.descriptorCount = static_cast<uint32_t>(number);
  poolSizes.push_back(poolSizeUBO);

  VkDescriptorPoolSize poolSizeIMGSampler{};
  poolSizeIMGSampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizeIMGSampler.descriptorCount = static_cast<uint32_t>(number);
  poolSizes.push_back(poolSizeIMGSampler);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();

  poolInfo.maxSets = static_cast<uint32_t>(number);
  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }

  // Create second descriptor pool
  if (vkCreateDescriptorPool(device, &poolInfo, nullptr,
                             &secondDescriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

VkDescriptorSet
VulkanBuffer::createDescriptorSet(VkDevice device,
                                  VkDescriptorSetLayout descriptorSetLayout,
                                  VkDescriptorPool pool) {

  VkDescriptorSet descriptorSet;
  std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
  VkDescriptorSetAllocateInfo secondAllocInfo{};
  secondAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  secondAllocInfo.descriptorPool = pool;
  secondAllocInfo.descriptorSetCount = static_cast<uint32_t>(1);
  secondAllocInfo.pSetLayouts = layouts.data();

  if (vkAllocateDescriptorSets(device, &secondAllocInfo, &descriptorSet) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
  return descriptorSet;
}

void VulkanBuffer::writeDescritorSets(VkDevice device,
                                      VkDescriptorSet descritorSet,
                                      VkBuffer uniformBuffer,
                                      VkImageView imageView,
                                      VkSampler sampler) {

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(Utils::UniformBufferObject);

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = imageView;
  imageInfo.sampler = sampler;

  std::vector<VkWriteDescriptorSet> descriptorWrites{};

  VkWriteDescriptorSet descriptorWriteUBO{};
  descriptorWriteUBO.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWriteUBO.dstSet = descritorSet;
  descriptorWriteUBO.dstBinding = 0;
  descriptorWriteUBO.dstArrayElement = 0;

  descriptorWriteUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWriteUBO.descriptorCount = 1;

  descriptorWriteUBO.pBufferInfo = &bufferInfo;
  descriptorWriteUBO.pImageInfo = nullptr;       // Optional
  descriptorWriteUBO.pTexelBufferView = nullptr; // Optional
  descriptorWrites.push_back(descriptorWriteUBO);

  VkWriteDescriptorSet descriptorWriteImgSampler{};
  descriptorWriteImgSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWriteImgSampler.dstSet = descritorSet;
  descriptorWriteImgSampler.dstBinding = 1;
  descriptorWriteImgSampler.dstArrayElement = 0;
  descriptorWriteImgSampler.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWriteImgSampler.descriptorCount = 1;
  descriptorWriteImgSampler.pBufferInfo = nullptr;
  descriptorWriteImgSampler.pImageInfo = &imageInfo;
  descriptorWriteImgSampler.pTexelBufferView = nullptr; // Optional
  descriptorWrites.push_back(descriptorWriteImgSampler);

  vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void VulkanBuffer::createDescriptorSets(
    int number, VkDescriptorSetLayout descriptorSetLayout,
    VkImageView textureImageView, VkSampler textureSampler,
    VkImageView secondTextureImageView) {

  // Theyve been allocated, now they need to be configured
  // Bind uniform buffers to descriptorsi

  descriptorSets.resize(number);
  for (size_t i = 0; i < number; i++) {

    descriptorSets[i] =
        createDescriptorSet(device, descriptorSetLayout, descriptorPool);

    writeDescritorSets(device, descriptorSets[i], uniformBuffers[i],
                       textureImageView, textureSampler);
  }

  // Create second descriptor set
  secondDescriptorSet =
      createDescriptorSet(device, descriptorSetLayout, secondDescriptorPool);

  writeDescritorSets(device, secondDescriptorSet, uniformBuffers[0],
                     secondTextureImageView, textureSampler);
}

} // namespace VulkanStuff