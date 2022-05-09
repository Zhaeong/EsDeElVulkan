
#include <vulkan_command.hpp>

namespace VulkanStuff {
VulkanCommand::VulkanCommand(VkPhysicalDevice inputPhysicalDevice,
                             VkDevice inputDevice, VkSurfaceKHR inputSurface,
                             uint32_t number)
    : physicalDevice{inputPhysicalDevice}, surface{inputSurface},
      device{inputDevice} {
  createCommandPool();
  createCommandBuffers(number);
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

void VulkanCommand::createCommandBuffers(uint32_t number) {

  commandBuffers.resize(number);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

} // namespace VulkanStuff