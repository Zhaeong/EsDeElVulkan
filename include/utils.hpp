
#pragma once

// Main utils lib, can't include any game/application specific headers here
#include <SDL2/SDL.h>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct VkSharedPoolInfoAMD {
  VkStructureType sType;
  const void *pNext;
  uint64_t poolSize;
  uint64_t poolUsage;
  uint32_t poolIndex;
};

struct VkSharedPoolAllocationInfoAMD {
  VkStructureType sType;
  const void *pNext;
  uint64_t hashcode;
  uint64_t offset;
  uint64_t size;
  uint64_t alignment;
  VkFormat vkFormat;
};

namespace Utils {

static const int WIDTH = 800;
static const int HEIGHT = 600;
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);

// Image functions
VkImageView createImageView(VkDevice device, VkImage image, VkFormat format,
                            VkImageAspectFlags aspectFlags);

std::vector<char> readFile(std::string filePath);

void showWindowFlags(int flags);

//===========================
// Input Structs

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::vector<VkVertexInputAttributeDescription>
  getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
  }
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct Query {
  uint64_t value{};
  uint64_t availability{};
};
//===========================

VkCommandBuffer beginSingleTimeCommands(VkDevice device,
                                        VkCommandPool commandPool);

void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool,
                           VkCommandBuffer commandBuffer, VkQueue submitQueue);

// Buffer functions
uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                  VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory);

void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue submitQueue,
                VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

std::vector<VkFramebuffer>
createFramebuffers(VkDevice device,
                   std::vector<VkImageView> swapChainImageViews,
                   VkImageView depthImageView, VkRenderPass renderPass,
                   VkExtent2D swapChainExtent);
bool hasStencilComponent(VkFormat format);

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);
} // namespace Utils