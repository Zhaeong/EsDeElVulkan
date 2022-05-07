#include <vulkan_renderer.hpp>

namespace VulkanStuff {

VulkanRenderer::VulkanRenderer(SDL_Window *sdlWindow) : window{sdlWindow} {
  vulkanCommand =
      new VulkanCommand(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice,
                        vulkanDevice.surface, MAX_FRAMES_IN_FLIGHT);

  vulkanSyncObject =
      new VulkanSyncObject(vulkanDevice.logicalDevice, MAX_FRAMES_IN_FLIGHT);
}
VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(vulkanDevice.logicalDevice);
  delete vulkanCommand;
  delete vulkanSyncObject;
}

void VulkanRenderer::beginRenderPass(VkCommandBuffer commandBuffer,
                                     uint32_t imageIndex) {
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vulkanPipeline.vulkanRenderPass->renderPass;
  renderPassInfo.framebuffer = vulkanPipeline.swapChainFramebuffers[imageIndex];

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vulkanPipeline.swapChainExtent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}
void VulkanRenderer::endRenderPass(VkCommandBuffer commandBuffer) {
  vkCmdEndRenderPass(commandBuffer);
}

void VulkanRenderer::drawObjects(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkanPipeline.graphicsPipeline);
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void VulkanRenderer::beginDrawingCommandBuffer(VkCommandBuffer commandBuffer) {

  if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS) {
    throw std::runtime_error("failed to reset command buffer!");
  }
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  // telling driver about our onetime usage
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void VulkanRenderer::endDrawingCommandBuffer(
    VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore,
    VkSemaphore renderFinishedSemaphore, VkFence inFlightFence) {
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record drawing command buffer!");
  }
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(vulkanDevice.graphicsQueue, 1, &submitInfo,
                    inFlightFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
  /*
  vkQueueWaitIdle(vulkanDevice.graphicsQueue);

  vkFreeCommandBuffers(vulkanDevice.logicalDevice, vulkanCommand->commandPool,
                       1, &commandBuffer);
                       */
}

void VulkanRenderer::drawFrame() {
  std::cout << "Drawing frame: " << currentFrame << "\n";
  vkWaitForFences(vulkanDevice.logicalDevice, 1,
                  &vulkanSyncObject->inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  vkResetFences(vulkanDevice.logicalDevice, 1,
                &vulkanSyncObject->inFlightFences[currentFrame]);

  vkAcquireNextImageKHR(
      vulkanDevice.logicalDevice, vulkanSwapChain.swapChain, UINT64_MAX,
      vulkanSyncObject->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
      &currentImage);

  beginDrawingCommandBuffer(vulkanCommand->commandBuffers[currentFrame]);

  /*
    vkCmdBindPipeline(commandBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vulkanPipeline.graphicsPipeline);
                      */

  beginRenderPass(vulkanCommand->commandBuffers[currentFrame], currentImage);
  drawObjects(vulkanCommand->commandBuffers[currentFrame]);
  endRenderPass(vulkanCommand->commandBuffers[currentFrame]);
  endDrawingCommandBuffer(
      vulkanCommand->commandBuffers[currentFrame],
      vulkanSyncObject->imageAvailableSemaphores[currentFrame],
      vulkanSyncObject->renderFinishedSemaphores[currentFrame],
      vulkanSyncObject->inFlightFences[currentFrame]);

  // Now present the image
  VkSemaphore signalSemaphores[] = {
      vulkanSyncObject->renderFinishedSemaphores[currentFrame]};

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vulkanSwapChain.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &currentImage;

  presentInfo.pResults = nullptr; // Optional
  vkQueuePresentKHR(vulkanDevice.presentQueue, &presentInfo);

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
} // namespace VulkanStuff