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

  // This is set first as VK_NULL_HANDLE, since we initially want to destroy
  // buffer, and VK_NULL_HANDLE is valid for vkDestroyBuffer when buffer in
  // unitialized
  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

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