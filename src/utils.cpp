#include <utils.hpp>

namespace Utils {

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  } else {
    std::cout << "Swapchain Support, has no formats";
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  } else {
    std::cout << "Swapchain Support, has no present modes";
  }
  return details;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  // Queues are what you submit command buffers to, and a queue family describes
  // a set of queues that do a certain thing e.g. graphics for draw calls
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  bool foundPresentSupport = false;

  for (int i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    // Find if the device supports window system and present images to the
    // surface we created
    VkBool32 presentSupport = false;

    // To determine whether a queue family of a physical device supports
    // presentation to a given surface
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport && !foundPresentSupport) {
      indices.presentFamily = i;
      foundPresentSupport = true;
    }
  }

  std::cout << "graphicsFamily: " << indices.graphicsFamily.value()
            << " presentFamily: " << indices.presentFamily.value() << "\n";

  return indices;
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format,
                            VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;

  // The viewType and format fields specify how the image data should be
  // interpreted. The viewType parameter allows you to treat images as 1D
  // textures, 2D textures, 3D textures and cube maps.
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;

  // allows you to swizzle the color channels around. For example, you can map
  // all of the channels to the red channel for a monochrome texture. You can
  // also map constant values of 0 and 1 to a channel.
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // subresourceRange field describes what the image's purpose is and which
  // part of the image should be accessed. Our images will be used as color
  // targets without any mipmapping levels or multiple layers.
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return imageView;
}

std::vector<char> readFile(std::string filePath) {

  // std::ios::ate means seek the end immediatly
  // std::ios::binary read it in as a binary
  std::ifstream file{filePath, std::ios::ate | std::ios::binary};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  // tellg gets last position which is the filesize
  size_t fileSize = static_cast<size_t>(file.tellg());

  std::vector<char> buffer(fileSize);

  // Go to beginning
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

void showWindowFlags(int flags) {

  printf("\nFLAGS ENABLED: ( %d )\n", flags);
  printf("=======================\n");
  if (flags & SDL_WINDOW_FULLSCREEN)
    printf("SDL_WINDOW_FULLSCREEN\n");
  if (flags & SDL_WINDOW_OPENGL)
    printf("SDL_WINDOW_OPENGL\n");
  if (flags & SDL_WINDOW_SHOWN)
    printf("SDL_WINDOW_SHOWN\n");
  if (flags & SDL_WINDOW_HIDDEN)
    printf("SDL_WINDOW_HIDDEN\n");
  if (flags & SDL_WINDOW_BORDERLESS)
    printf("SDL_WINDOW_BORDERLESS\n");
  if (flags & SDL_WINDOW_RESIZABLE)
    printf("SDL_WINDOW_RESIZABLE\n");
  if (flags & SDL_WINDOW_MINIMIZED)
    printf("SDL_WINDOW_MINIMIZED\n");
  if (flags & SDL_WINDOW_MAXIMIZED)
    printf("SDL_WINDOW_MAXIMIZED\n");
  if (flags & SDL_WINDOW_INPUT_GRABBED)
    printf("SDL_WINDOW_INPUT_GRABBED\n");
  if (flags & SDL_WINDOW_INPUT_FOCUS)
    printf("SDL_WINDOW_INPUT_FOCUS\n");
  if (flags & SDL_WINDOW_MOUSE_FOCUS)
    printf("SDL_WINDOW_MOUSE_FOCUS\n");
  if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    printf("SDL_WINDOW_FULLSCREEN_DESKTOP\n");
  if (flags & SDL_WINDOW_FOREIGN)
    printf("SDL_WINDOW_FOREIGN\n");
  printf("=======================\n");
}

VkCommandBuffer beginSingleTimeCommands(VkDevice device,
                                        VkCommandPool commandPool) {
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

void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool,
                           VkCommandBuffer commandBuffer, VkQueue submitQueue) {

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

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;

  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    // typeFiler specifies the bit field of memory types that are suitable
    // can find index of suitable memory by iterating over all memoryTypes and
    // checking if the bit is set to 1

    // need to also look at special features of the memory, like being able to
    // map so we can write to it from CPU so look for a bitwise match

    if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                  properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("Failed to find memory type!");
}

void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                  VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  // After this buffer has been created, but doesn't have memory inside
  // First step of allocating memory to buffer requires querying its memory
  // requirements
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      physicalDevice, memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }
  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue submitQueue,
                VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

  VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0; // Optional
  copyRegion.dstOffset = 0; // Optional
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommands(device, commandPool, commandBuffer, submitQueue);
}

std::vector<VkFramebuffer>
createFramebuffers(VkDevice device,
                   std::vector<VkImageView> swapChainImageViews,
                   VkImageView depthImageView, VkRenderPass renderPass,
                   VkExtent2D swapChainExtent, VkImageView colorImageView) {

  std::vector<VkFramebuffer> swapChainFramebuffers{};
  swapChainFramebuffers.resize(swapChainImageViews.size());
  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    std::vector<VkImageView> attachments = { colorImageView, depthImageView, swapChainImageViews[i]
                                            };

    VkFramebufferCreateInfo framebufferInfo{};
    // framebufferInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }

  return swapChainFramebuffers;
}

bool hasStencilComponent(VkFormat format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features) {

  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

} // namespace Utils