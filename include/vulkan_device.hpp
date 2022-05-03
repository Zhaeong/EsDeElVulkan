
#pragma once

#include <vulkan/vulkan.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <iostream>
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

  VulkanDevice(SDL_Window *sdlWindow);
  ~VulkanDevice();

  // deleting copy constructors
  VulkanDevice(const VulkanDevice &) = delete;
  void operator=(const VulkanDevice &) = delete;

  void createInstance();

  bool checkValidationLayerSupport();

  std::vector<const char *> getRequiredVkExtensions();

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
};

} // namespace VulkanStuff