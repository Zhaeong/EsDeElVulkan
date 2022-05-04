#pragma once
#include <utils.hpp>

namespace VulkanStuff {
class VulkanPipeline {
public:
  // From VulkanDevice ====================================
  VkDevice device;
  //======================================

  // From VulkanSwapChain ===============================
  VkExtent2D swapChainExtent;
  //    ============================================

  VkPipelineLayout pipelineLayout;

  VulkanPipeline(VkDevice inputDevice, VkExtent2D inputSwapChainExtent);
  ~VulkanPipeline();

  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};
} // namespace VulkanStuff