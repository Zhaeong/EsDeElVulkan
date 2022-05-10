
#pragma once
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

namespace VulkanStuff {
class VulkanSyncObject {
public:
  // From Vulkan Device ======
  VkDevice device;
  //  =========
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  VulkanSyncObject(VkDevice inputDevice, uint32_t number);
  ~VulkanSyncObject();

  void createSyncObjects(uint32_t number);
};
} // namespace VulkanStuff