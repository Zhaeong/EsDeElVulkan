#pragma once
#include <utils.hpp>
#include <vulkan_renderpass.hpp>

namespace VulkanStuff {
class VulkanPipeline {
public:
  // From VulkanDevice ====================================
  VkDevice device;
  //======================================

  // From VulkanSwapChain ===============================
  VkExtent2D swapChainExtent;
  VkFormat swapChainImageFormat;
  //    ============================================

  VkPipelineLayout pipelineLayout;
  VulkanRenderPass *vulkanRenderPass;

  VkPipeline graphicsPipeline;

  VulkanPipeline(VkDevice inputDevice, VkExtent2D inputSwapChainExtent,
                 VkFormat inputSwapChainImageFormat);
  ~VulkanPipeline();

  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};
} // namespace VulkanStuff