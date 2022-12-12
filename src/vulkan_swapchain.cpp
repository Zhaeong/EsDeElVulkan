#include <vulkan_swapchain.hpp>

namespace VulkanStuff {

VulkanSwapChain::VulkanSwapChain(SDL_Window *sdlWindow,
                                 VkPhysicalDevice inputPhysicalDevice,
                                 VkDevice inputDevice,
                                 VkSurfaceKHR inputSurface)
    : window{sdlWindow}, physicalDevice{inputPhysicalDevice},
      device{inputDevice}, surface{inputSurface} {
  createSwapChain();
  createSwapChainImageViews();
}
VulkanSwapChain::~VulkanSwapChain() {
  for (auto imageView : swapChainImageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(device, swapChain, nullptr);
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {

  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  // Else just settle on the first format
  return availableFormats[0];
}
VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

// Extent is resolution of swap chain images which is based on surface

// capabilities currentExtent is the current width and height of the surface, or
// the special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the surface size
// will be determined by the extent of a swapchain targeting the surface.

VkExtent2D VulkanSwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    SDL_Vulkan_GetDrawableSize(window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void VulkanSwapChain::createSwapChain() {
  Utils::SwapChainSupportDetails swapChainSupport =
      Utils::querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  // minimum + 1, and don't go over maximum
  imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  std::cout << "SwapChain Image count: " << imageCount << "\n";
  // VK_FORMAT_B8G8R8A8_SRGB = 50,
  std::cout << "SwapChain Image format: " << surfaceFormat.format << "\n";

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.flags = VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR;

  VkImageFormatListCreateInfo formatList{};
  formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
  formatList.viewFormatCount = 2;
  std::vector formats = {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB};
  formatList.pViewFormats = formats.data();

  createInfo.pNext = &formatList;

  // Image format
  createInfo.imageFormat = surfaceFormat.format;
  // createInfo.imageFormat = VK_FORMAT_A8B8G8R8_UNORM_PACK32;

  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  Utils::QueueFamilyIndices indices =
      Utils::findQueueFamilies(physicalDevice, surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());
  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;
}

void VulkanSwapChain::createSwapChainImageViews() {
  swapChainImageViews.resize(swapChainImages.size());

  VkFormat testFormat = VK_FORMAT_B8G8R8A8_SRGB;
  std::cout << "swapchain Image View Format: " << swapChainImageFormat << "\n";

  for (size_t i = 0; i < swapChainImages.size(); i++) {
    swapChainImageViews[i] =
        Utils::createImageView(device, swapChainImages[i], swapChainImageFormat,
                               VK_IMAGE_ASPECT_COLOR_BIT);
    // swapChainImageViews[i] =
    //     Utils::createImageView(device, swapChainImages[i], testFormat);
  }
}

} // namespace VulkanStuff
