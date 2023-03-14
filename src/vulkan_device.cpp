
#include <vulkan_device.hpp>
namespace VulkanStuff {
VulkanDevice::VulkanDevice(SDL_Window *sdlWindow) : window{sdlWindow} {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();

  m_vkLoader = dlopen("/home/mvtest/Documents/Vulkan-Loader/builddbg/loader/libvulkan.so", RTLD_NOW | RTLD_LOCAL);

  //pfn_vkQuerySharedPoolProperties = (PFN_vkQuerySharedPoolProperties)dlsym(m_vkLoader, "vkQuerySharedPoolProperties");
  
  //pfn_vkCreateInstance = (PFN_vkCreateInstance)dlsym(m_vkLoader, "vkCreateInstance");

  //pfn_vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)dlsym(m_vkLoader, "vkGetDeviceProcAddr");

  //vkGet
  //pfn_vkQuerySharedPoolProperties = (PFN_vkQuerySharedPoolProperties)vkGetInstanceProcAddr(instance, "vkQuerySharedPoolProperties");

  //pfn_vkQuerySharedPoolPropertiesAMD = (pfn_vkQuerySharedPoolPropertiesAMD)vkGetDeviceProcAddr(logicalDevice, "pfn_vkQuerySharedPoolPropertiesAMD");

  pfn_vkQuerySharedPoolPropertiesAMD = reinterpret_cast<PFN_vkQuerySharedPoolPropertiesAMD>(vkGetDeviceProcAddr(logicalDevice, "vkQuerySharedPoolPropertiesAMD"));

    if (pfn_vkQuerySharedPoolPropertiesAMD != nullptr)
    {
        VkSharedPoolInfoAMD poolInfo = {};
        uint64_t* pNumOfAllocations = new uint64_t(0);

        pfn_vkQuerySharedPoolPropertiesAMD(logicalDevice, &poolInfo, pNumOfAllocations, nullptr);

        printf("pool size:%lu, pool usage: %lu, sub allocation count:%lu",
            poolInfo.poolSize, poolInfo.poolUsage, *pNumOfAllocations);

        uint64_t originalNumOfAllocations = *pNumOfAllocations;

        VkSharedPoolAllocationInfoAMD *pSubAllocationInfo = new VkSharedPoolAllocationInfoAMD[*pNumOfAllocations];

        pfn_vkQuerySharedPoolPropertiesAMD(logicalDevice, &poolInfo, pNumOfAllocations, pSubAllocationInfo);

        for (uint32_t idx = 0; idx < originalNumOfAllocations; idx++)
        {
            printf("hashcode:%lu, size:%lu",
            pSubAllocationInfo[idx].hashcode,
            pSubAllocationInfo[idx].size);
        }
        delete[] pSubAllocationInfo;
        delete pNumOfAllocations;
    }



  // int i = 30;

  pfn_vkCreateInstance = (PFN_vkCreateInstance)dlsym(m_vkLoader, "vkCreateInstance");
}

VulkanDevice::~VulkanDevice() {

  std::cout << "Cleaning up VulkanDevice Init\n";
  if (enableValidationLayers) {
    // Can remove this line to trigger validation layer error
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
  vkDestroyDevice(logicalDevice, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
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
  // appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  //createInfo.pApplicationInfo = VK_NULL_HANDLE;

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

      // std::cout << "Avail Layer :" << availableLayers[j].layerName << "\n";

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

  // for (int i = 0; i < extensions.size(); i++) {
  //   std::cout << "ext: " << extensions[i] << "\n";
  // }

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

void VulkanDevice::createSurface() {
  if (SDL_Vulkan_CreateSurface(window, instance, &surface) != SDL_TRUE) {
    throw std::runtime_error("failed to create window surface!");
  }
}
void VulkanDevice::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  } else {
    std::cout << "Num Physical Devices: " << deviceCount << "\n";
  }

  std::vector<VkPhysicalDevice> vPhysicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, vPhysicalDevices.data());

  for (int i = 1; i < vPhysicalDevices.size(); i++) {
    if (isDeviceSuitable(vPhysicalDevices[i])) {
      physicalDevice = vPhysicalDevices[i];
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error(
        "Failed to find Physical Device with all required features");
  } else {

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    std::cout << "Picked " << deviceProperties.deviceName
              << " Vendor: " << deviceProperties.vendorID << "\n";
  }
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) {

  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;

  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  std::cout << "Device Name: " << deviceProperties.deviceName
            << " Device ID: " << deviceProperties.deviceID << "\n";

  Utils::QueueFamilyIndices indices = Utils::findQueueFamilies(device, surface);

  bool swapChainAdequate = false;
  Utils::SwapChainSupportDetails swapChainSupport =
      Utils::querySwapChainSupport(device, surface);

  swapChainAdequate = !swapChainSupport.formats.empty() &&
                      !swapChainSupport.presentModes.empty();

  return indices.graphicsFamily.has_value() &&
         indices.presentFamily.has_value() &&
         checkDeviceExtensionSupport(device) && swapChainAdequate &&
         deviceFeatures.samplerAnisotropy;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (int i = 0; i < availableExtensions.size(); i++) {
    // std::cout << "Avail: " << availableExtensions[i].extensionName
    //           << " Version: " << availableExtensions[i].specVersion << "\n";
    requiredExtensions.erase(availableExtensions[i].extensionName);
  }
  if (requiredExtensions.empty()) {
    std::cout << "Physical device contains all required extensions\n";
  }

  return requiredExtensions.empty();
}

void VulkanDevice::createLogicalDevice() {

  // Specifying queues to be created
  Utils::QueueFamilyIndices indices =
      Utils::findQueueFamilies(physicalDevice, surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  // set will only contain unique values
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  // create device queue
  // Assigns priorty to queues to influence scheduling of comand buffer
  // execution
  float queuePriority = 1.0f;
  for (int i = 0; i < uniqueQueueFamilies.size(); i++) {

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = i;
    queueCreateInfo.queueCount = 1;

    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  // Specifying used device features
  VkPhysicalDeviceFeatures deviceFeatures{};

  // enable anisotropy
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());

  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  // Query vk12 features
  VkPhysicalDeviceVulkan12Features vk12Features{};
  vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  createInfo.pNext = &vk12Features;

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0,
                   &graphicsQueue);

  // Now create the present queue
  vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0,
                   &presentQueue);
}

} // namespace VulkanStuff