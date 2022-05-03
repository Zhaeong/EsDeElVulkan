
#include <vulkan_device.hpp>
namespace VulkanStuff {
VulkanDevice::VulkanDevice(SDL_Window *sdlWindow) : window{sdlWindow} {
  createInstance();
  setupDebugMessenger();
}

VulkanDevice::~VulkanDevice() {

  std::cout << "Cleaning up VulkanDevice Init\n";
  if (enableValidationLayers) {
    // Can remove this line to trigger validation layer error
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  // have to destroy logical device first it seems
  vkDestroyInstance(instance, nullptr);
}

void VulkanDevice::createInstance() {

  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Validation Layer requested but not available\n");
  } else if (enableValidationLayers) {
    std::cout << "Enabling Validation Layers\n";
  }
  std::vector<const char *> requiredVkExtenstionsForSDL =
      getRequiredVkExtensions();

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "SDLVulkan";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(requiredVkExtenstionsForSDL.size());
  createInfo.ppEnabledExtensionNames = requiredVkExtenstionsForSDL.data();

  // The debugCreateInfo variable is placed outside the if statement
  // to ensure that it is not destroyed before the vkCreateInstance call
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool VulkanDevice::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (int i = 0; i < validationLayers.size(); i++) {

    bool layerFound = false;

    // std::cout << "Layer :" << validationLayers[i] << "\n";

    for (int j = 0; j < availableLayers.size(); j++) {

      std::cout << "Avail Layer :" << availableLayers[j].layerName << "\n";

      //== compairs pointers
      // strcmp compairs actual string content
      if (strcmp(validationLayers[i], availableLayers[j].layerName)) {
        layerFound = true;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}
std::vector<const char *> VulkanDevice::getRequiredVkExtensions() {

  unsigned int extensionCount;

  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr)) {
    std::cout << "Can't get required extensions\n";
  }
  // Need to allocate memory according to required extensions
  // This is different than glfw which returns a pointer with extension memory
  // already allocated
  std::vector<const char *> extensions;
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  size_t additional_extension_count = extensions.size();
  extensions.resize(additional_extension_count + extensionCount);

  // Array needs to be prefilled with available extension names
  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
                                        extensions.data() +
                                            additional_extension_count)) {
    std::cout << "Extensions Didn't get it right\n";
  }

  for (int i = 0; i < extensions.size(); i++) {
    std::cout << "ext: " << extensions[i] << "\n";
  }

  return extensions;
}

void VulkanDevice::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  // char *UserData = "Heyya\n";
  std::string UserData = "Heyya\n";
  void *userDataVoidPtr = static_cast<void *>(new std::string("ohh man\n"));
  // createInfo.pUserData = nullptr; // Optional
  createInfo.pUserData = userDataVoidPtr;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {

  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  std::string *userData = static_cast<std::string *>(pUserData);
  std::cerr << "UserData: " << *userData << "\n";
  // std::cerr << "UserData: " << *userData << "\n";
  return VK_FALSE;
}

void VulkanDevice::setupDebugMessenger() {
  if (!enableValidationLayers) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

VkResult VulkanDevice::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void VulkanDevice::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

} // namespace VulkanStuff