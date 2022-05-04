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

  for (int i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      std::cout << "Graphics Queue Index: " << i
                << " Can create queuecount: " << queueFamilies[i].queueCount
                << "\n";
    }
    // Find if the device supports window system and present images to the
    // surface we created
    VkBool32 presentSupport = false;
    // To determine whether a queue family of a physical device supports
    // presentation to a given surface
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
      std::cout << "Present Queue family Index: " << i
                << " Queuecount: " << queueFamilies[i].queueCount << "\n";
    }
  }
  return indices;
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format) {
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
  // viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  // viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  // viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  // viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // subresourceRange field describes what the image's purpose is and which
  // part of the image should be accessed. Our images will be used as color
  // targets without any mipmapping levels or multiple layers.
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

} // namespace Utils