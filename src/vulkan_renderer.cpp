#include <vulkan_renderer.hpp>

namespace VulkanStuff {

VulkanRenderer::VulkanRenderer(SDL_Window *sdlWindow) : window{sdlWindow} {
  vulkanCommand =
      new VulkanCommand(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice,
                        vulkanDevice.surface, MAX_FRAMES_IN_FLIGHT);

  vulkanSyncObject =
      new VulkanSyncObject(vulkanDevice.logicalDevice, MAX_FRAMES_IN_FLIGHT);

  vulkanBuffer =
      new VulkanBuffer(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice,
                       vulkanDevice.graphicsQueue, vulkanCommand->commandPool);

  vulkanImage =
      new VulkanImage(vulkanDevice.physicalDevice, vulkanDevice.logicalDevice,
                      vulkanDevice.graphicsQueue, vulkanCommand->commandPool);

  // vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //             {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //             {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
              {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
              {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
              {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
              {{0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
              {{0.9f, -0.9f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
              {{0.9f, 0.9f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

  vulkanBuffer->createVertexBuffer(vertices);

  indices = {0, 1, 2, 2, 3, 0, 4, 5, 6};
  vulkanBuffer->createIndexBuffer(indices);

  vulkanBuffer->createUniformBuffers(vulkanSwapChain.imageCount);
  vulkanBuffer->createDescriptorPool(vulkanSwapChain.imageCount);
  vulkanBuffer->createDescriptorSets(
      vulkanSwapChain.imageCount, vulkanPipeline.descriptorSetLayout,
      vulkanImage->textureImageView, vulkanImage->textureSampler);
}
VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(vulkanDevice.logicalDevice);
  delete vulkanCommand;
  delete vulkanSyncObject;
  delete vulkanBuffer;
  delete vulkanImage;
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

void VulkanRenderer::drawFromVertices(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkanPipeline.graphicsPipeline);

  VkBuffer vertexBuffers[] = {vulkanBuffer->vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}

void VulkanRenderer::drawFromIndices(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkanPipeline.graphicsPipeline);

  VkBuffer vertexBuffers[] = {vulkanBuffer->vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, vulkanBuffer->indexBuffer, 0,
                       VK_INDEX_TYPE_UINT16);

  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
                   0, 0);
}

void VulkanRenderer::drawFromDescriptors(VkCommandBuffer commandBuffer,
                                         int imageIndex) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vulkanPipeline.graphicsPipeline);

  VkBuffer vertexBuffers[] = {vulkanBuffer->vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, vulkanBuffer->indexBuffer, 0,
                       VK_INDEX_TYPE_UINT16);

  // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                         vulkanPipeline.pipelineLayout, 0, 1,
  //                         &vulkanBuffer->descriptorSets[imageIndex], 0,
  //                         nullptr);

  // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1,
  // 0,
  //                  0, 0);

  // first object
  // vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

  // Second object
  // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                         vulkanPipeline.pipelineLayout, 0, 1,
  //                         &vulkanBuffer->descriptorSets[0], 0, nullptr);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          vulkanPipeline.pipelineLayout, 0, 1,
                          &vulkanBuffer->secondDescriptorSet, 0, nullptr);

  vkCmdDrawIndexed(commandBuffer, 3, 1, 6, 0, 0);
}

void VulkanRenderer::clearColorImage() {

  vulkanImage->transitionImageLayout(vulkanImage->textureImage,
                                     VK_FORMAT_R8G8B8A8_SRGB,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(
      vulkanDevice.logicalDevice, vulkanBuffer->commandPool);
  VkImageSubresourceRange ImageSubresourceRange;
  ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ImageSubresourceRange.baseMipLevel = 0;
  ImageSubresourceRange.levelCount = 1;
  ImageSubresourceRange.baseArrayLayer = 0;
  ImageSubresourceRange.layerCount = 1;

  VkClearColorValue ClearColorValue = {0, 0.111111, 0.222222, 0.333333};
  vkCmdClearColorImage(commandBuffer, vulkanImage->textureImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue,
                       1, &ImageSubresourceRange);

  Utils::endSingleTimeCommands(vulkanDevice.logicalDevice,
                               vulkanBuffer->commandPool, commandBuffer,
                               vulkanDevice.graphicsQueue);

  vulkanImage->transitionImageLayout(vulkanImage->textureImage,
                                     VK_FORMAT_R8G8B8A8_SRGB,
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

void VulkanRenderer::cleanupSwapChain() {
  vkDestroyPipeline(vulkanDevice.logicalDevice, vulkanPipeline.graphicsPipeline,
                    nullptr);
  vkDestroyPipelineLayout(vulkanDevice.logicalDevice,
                          vulkanPipeline.pipelineLayout, nullptr);

  for (auto framebuffer : vulkanPipeline.swapChainFramebuffers) {
    vkDestroyFramebuffer(vulkanDevice.logicalDevice, framebuffer, nullptr);
  }

  delete vulkanPipeline.vulkanRenderPass;

  for (size_t i = 0; i < vulkanSwapChain.swapChainImageViews.size(); i++) {
    vkDestroyImageView(vulkanDevice.logicalDevice,
                       vulkanSwapChain.swapChainImageViews[i], nullptr);
  }

  vkDestroySwapchainKHR(vulkanDevice.logicalDevice, vulkanSwapChain.swapChain,
                        nullptr);
}
void VulkanRenderer::recreateSwapChain() {
  std::cout << "Recreating Swapchain\n";

  Uint32 flags = SDL_GetWindowFlags(window);
  Utils::showWindowFlags(flags);

  // don't recreate swapchain if minimized
  while (flags & SDL_WINDOW_MINIMIZED) {
    flags = SDL_GetWindowFlags(window);
    SDL_Event event;
    SDL_WaitEvent(&event);
  }

  vkDeviceWaitIdle(vulkanDevice.logicalDevice);

  cleanupSwapChain();

  // recreateswapchain
  vulkanSwapChain.createSwapChain();
  vulkanSwapChain.createSwapChainImageViews();

  // recreate renderpass
  vulkanPipeline.vulkanRenderPass = new VulkanRenderPass(
      vulkanDevice.logicalDevice, vulkanSwapChain.swapChainImageFormat);

  // reassign swapchain vars for framebuffers recreation
  vulkanPipeline.swapChainImageFormat = vulkanSwapChain.swapChainImageFormat;
  vulkanPipeline.swapChainImageViews = vulkanSwapChain.swapChainImageViews;
  vulkanPipeline.swapChainExtent = vulkanSwapChain.swapChainExtent;

  vulkanPipeline.createGraphicsPipeline();
  vulkanPipeline.createFramebuffers();
}

void VulkanRenderer::recreateVertexBuffer(
    std::vector<Utils::Vertex> inputVertices) {

  vertices = inputVertices;
  vulkanBuffer->createVertexBuffer(inputVertices);
}

void VulkanRenderer::updateUniformBuffer(uint32_t currentImage) {
  /*
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   currentTime - startTime)
                   .count();
*/

  Utils::UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), rotation * glm::radians(90.0f),
                          glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  ubo.proj = glm::perspective(glm::radians(45.0f),
                              vulkanSwapChain.swapChainExtent.width /
                                  (float)vulkanSwapChain.swapChainExtent.height,
                              0.1f, 10.0f);

  ubo.proj[1][1] *= -1;

  void *data;
  vkMapMemory(vulkanDevice.logicalDevice,
              vulkanBuffer->uniformBuffersMemory[currentImage], 0, sizeof(ubo),
              0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(vulkanDevice.logicalDevice,
                vulkanBuffer->uniformBuffersMemory[currentImage]);
}

void VulkanRenderer::drawFrame() {
  // std::cout << "Drawing frame: " << currentFrame << "\n";
  vkWaitForFences(vulkanDevice.logicalDevice, 1,
                  &vulkanSyncObject->inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(
      vulkanDevice.logicalDevice, vulkanSwapChain.swapChain, UINT64_MAX,
      vulkanSyncObject->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
      &currentImage);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  updateUniformBuffer(currentImage);

  vkResetFences(vulkanDevice.logicalDevice, 1,
                &vulkanSyncObject->inFlightFences[currentFrame]);

  beginDrawingCommandBuffer(vulkanCommand->commandBuffers[currentFrame]);

  beginRenderPass(vulkanCommand->commandBuffers[currentFrame], currentImage);

  //  drawObjects(vulkanCommand->commandBuffers[currentFrame]);
  // drawFromVertices(vulkanCommand->commandBuffers[currentFrame]);
  // drawFromIndices(vulkanCommand->commandBuffers[currentFrame]);
  drawFromDescriptors(vulkanCommand->commandBuffers[currentFrame],
                      currentImage);

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
  result = vkQueuePresentKHR(vulkanDevice.presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace VulkanStuff