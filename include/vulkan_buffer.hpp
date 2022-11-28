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

  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;

  // Descriptor Stuff
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  // Stuff for second texture
  VkBuffer secondUniformBuffers;
  VkDeviceMemory secondUniformBuffersMemory;
  VkDescriptorPool secondDescriptorPool;
  VkDescriptorSet secondDescriptorSet;

  // End of stuff for second texture

  // Functions

  VulkanBuffer(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
               VkQueue inputGraphicsQueue, VkCommandPool inputCommandPool);
  ~VulkanBuffer();

  void createVertexBuffer(std::vector<Utils::Vertex> vertices);
  void createIndexBuffer(std::vector<uint16_t> indices);
  void createUniformBuffers(int number);
  void createDescriptorPool(int number);
  void createDescriptorSets(int number,
                            VkDescriptorSetLayout descriptorSetLayout,
                            VkImageView textureImageView,
                            VkSampler textureSampler);
};
} // namespace VulkanStuff