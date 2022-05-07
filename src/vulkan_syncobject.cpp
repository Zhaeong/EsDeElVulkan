
#include <vulkan_syncobject.hpp>
namespace VulkanStuff {
VulkanSyncObject::VulkanSyncObject(VkDevice inputDevice, uint32_t number)
    : device{inputDevice} {
  createSyncObjects(number);
}

VulkanSyncObject::~VulkanSyncObject() {
  for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
  }
}

void VulkanSyncObject::createSyncObjects(uint32_t number) {
  imageAvailableSemaphores.resize(number);
  renderFinishedSemaphores.resize(number);
  inFlightFences.resize(number);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < number; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) !=
            VK_SUCCESS) {

      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
}
} // namespace VulkanStuff