
#pragma once

#include <vulkan/vulkan.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <set>
#include <string>
#include <utils.hpp>
#include <vector>

namespace VulkanStuff {

class VulkanDevice {

public:
  SDL_Window *window;
  VkInstance instance;

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const bool enableValidationLayers = true;

  VkDebugUtilsMessengerEXT debugMessenger;

  // Device variables

  // actual physical device
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  // logical device
  VkDevice logicalDevice;

  std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
      VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

  VkSurfaceKHR surface;

  // Queues
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  //=========
  // Functions
  //=========

  VulkanDevice(SDL_Window *sdlWindow);
  ~VulkanDevice();

  // deleting copy constructors
  VulkanDevice(const VulkanDevice &) = delete;
  void operator=(const VulkanDevice &) = delete;

  void createInstance();

  bool checkValidationLayerSupport();

  std::vector<const char *> getRequiredVkExtensions();

  //==============================================
  // Debug functions
  //==============================================
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  // Vulkan macros for different compilers in this form
  // VKAPI_ATTR <return_type> VKAPI_CALL <command_name>(<command_parameters>);
  // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#_platform_specific_calling_conventions
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);
  void setupDebugMessenger();

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  static void
  DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);

  void createSurface();
  //
  //==============================================u===
  // Device functions
  //
  void pickPhysicalDevice();
  void createLogicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};

} // namespace VulkanStuff