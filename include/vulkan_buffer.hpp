#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include <utils.hpp>

namespace VulkanStuff {
class VulkanBuffer {
public:
  // From VulkanDevice ========
  VkDevice device;
  VkPhysicalDevice physicalDevice;
  VkQueue graphicsQueue;
  //===========================

  // From VulkanCommand =========
  VkCommandPool commandPool;
  //============================

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  VulkanBuffer(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
               VkQueue inputGraphicsQueue, VkCommandPool inputCommandPool);
  ~VulkanBuffer();

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void createVertexBuffer(std::vector<Utils::Vertex> vertices);
};
} // namespace VulkanStuff