#include <vulkan_renderer.hpp>

namespace VulkanStuff {

VulkanRenderer::VulkanRenderer(SDL_Window *sdlWindow) : window{sdlWindow} {}
VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(vulkanDevice.logicalDevice);
}

void VulkanRenderer::beginRenderPass(VkCommandBuffer commandBuffer) {
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vulkanPipeline.vulkanRenderPass->renderPass;
  renderPassInfo.framebuffer =
      vulkanPipeline.swapChainFramebuffers[currentImageIndex];

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

void VulkanRenderer::endDrawingCommandBuffer(VkCommandBuffer commandBuffer) {
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record drawing command buffer!");
  }
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {vulkanSwapChain.imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {vulkanSwapChain.renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(vulkanDevice.graphicsQueue, 1, &submitInfo,
                    vulkanSwapChain.inFlightFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
  /* vkQueueWaitIdle(vulkanDevice.graphicsQueue);

  vkFreeCommandBuffers(vulkanDevice.logicalDevice,
                       vulkanPipeline.vulkanCommand->commandPool, 1,
                       &commandBuffer);
                       */
}

void VulkanRenderer::drawFrame() {
  vkWaitForFences(vulkanDevice.logicalDevice, 1, &vulkanSwapChain.inFlightFence,
                  VK_TRUE, UINT64_MAX);
  vkResetFences(vulkanDevice.logicalDevice, 1, &vulkanSwapChain.inFlightFence);

  vkAcquireNextImageKHR(vulkanDevice.logicalDevice, vulkanSwapChain.swapChain,
                        UINT64_MAX, vulkanSwapChain.imageAvailableSemaphore,
                        VK_NULL_HANDLE, &currentImageIndex);

  // vkResetCommandBuffer(vulkanPipeline.vulkanCommand->commandBuffer, 0);

  VkCommandBuffer commandBuf =
      vulkanPipeline.vulkanCommand->beginSingleTimeCommands();

  /*
    vkCmdBindPipeline(commandBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vulkanPipeline.graphicsPipeline);
                      */

  beginRenderPass(commandBuf);
  drawObjects(commandBuf);
  endRenderPass(commandBuf);
  endDrawingCommandBuffer(commandBuf);

  // Now present the image

  VkSemaphore signalSemaphores[] = {vulkanSwapChain.renderFinishedSemaphore};
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vulkanSwapChain.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &currentImageIndex;

  presentInfo.pResults = nullptr; // Optional
  vkQueuePresentKHR(vulkanDevice.presentQueue, &presentInfo);
}
} // namespace VulkanStuff