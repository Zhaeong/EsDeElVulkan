
#include <vulkan_command.hpp>

namespace VulkanStuff {
VulkanCommand::VulkanCommand(VkPhysicalDevice inputPhysicalDevice,
                             VkDevice inputDevice, VkSurfaceKHR inputSurface)
    : physicalDevice{inputPhysicalDevice}, surface{inputSurface},
      device{inputDevice} {
  createCommandPool();
  createCommandBuffer();
}
VulkanCommand::~VulkanCommand() {
  vkDestroyCommandPool(device, commandPool, nullptr);
}

void VulkanCommand::createCommandPool() {
  Utils::QueueFamilyIndices queueFamilyIndices =
      Utils::findQueueFamilies(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VulkanCommand::createCommandBuffer() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

VkCommandBuffer VulkanCommand::beginSingleTimeCommands() {
  // First need to allocate a temporary command buffer
  //  Can create seperate command pool, but maybe at another time
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  // Begin recording to command buffer
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  // telling driver about our onetime usage
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void VulkanCommand::endSingleTimeCommands(VkCommandBuffer commandBuffer,
                                          VkQueue submitQueue) {

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(submitQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(submitQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

} // namespace VulkanStuff