#pragma once
#include <utils.hpp>
#include <vulkan_renderpass.hpp>

namespace VulkanStuff {
class VulkanPipeline {
public:
  // From VulkanDevice ====================================
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSurfaceKHR surface;
  VkQueue graphicsQueue;
  //======================================

  // From VulkanSwapChain ===============================
  VkExtent2D swapChainExtent;
  VkFormat swapChainImageFormat;
  std::vector<VkImageView> swapChainImageViews;
  //    ============================================

  VkPipelineLayout pipelineLayout;
  VulkanRenderPass *vulkanRenderPass;

  VkPipeline graphicsPipeline;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  // Functions ============================

  VulkanPipeline(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
                 VkSurfaceKHR inputSurface, VkQueue inputGraphicsQueue,
                 VkExtent2D inputSwapChainExtent,
                 VkFormat inputSwapChainImageFormat,
                 std::vector<VkImageView> inputSwapChainImageViews);
  ~VulkanPipeline();

  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char> &code);

  void createFramebuffers();
};
} // namespace VulkanStuff