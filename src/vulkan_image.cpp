
#include <vulkan_image.hpp>

namespace VulkanStuff {

VulkanImage::VulkanImage(VkPhysicalDevice inputPhysicalDevice,

                         VkDevice inputDevice, VkQueue inputGraphicsQueue,
                         VkCommandPool inputCommandPool)
    : device{inputDevice}, physicalDevice{inputPhysicalDevice},
      graphicsQueue{inputGraphicsQueue}, commandPool{inputCommandPool} {
  createTextureImage("textures/texture.jpg", textureImage, textureImageMemory,
                     false);
  textureImageView =
      Utils::createImageView(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB);

  createTextureImage("textures/amdtexture.jpg", second_textureImage,
                     second_textureImageMemory, false);

  second_textureImageView = Utils::createImageView(device, second_textureImage,
                                                   VK_FORMAT_R8G8B8A8_SRGB);

  createTextureImage("textures/amdtexture2.jpg", third_textureImage,
                     third_textureImageMemory, true);

  third_textureImageView = Utils::createImageView(device, third_textureImage,
                                                  VK_FORMAT_R8G8B8A8_SRGB);

  createTextureSampler();
}

VulkanImage::~VulkanImage() {
  vkDestroyImage(device, textureImage, nullptr);
  vkFreeMemory(device, textureImageMemory, nullptr);
  vkDestroyImageView(device, textureImageView, nullptr);

  vkDestroyImage(device, second_textureImage, nullptr);
  vkFreeMemory(device, second_textureImageMemory, nullptr);
  vkDestroyImageView(device, second_textureImageView, nullptr);

  vkDestroyImage(device, third_textureImage, nullptr);
  vkFreeMemory(device, third_textureImageMemory, nullptr);
  vkDestroyImageView(device, third_textureImageView, nullptr);

  vkDestroySampler(device, textureSampler, nullptr);
}

void VulkanImage::createImage(uint32_t width, uint32_t height, VkFormat format,
                              VkImageTiling tiling, VkImageUsageFlags usage,
                              VkMemoryPropertyFlags properties, VkImage &image,
                              VkDeviceMemory &imageMemory, bool isExplicit) {
  // Now create the VKImage
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = static_cast<uint32_t>(width);
  imageInfo.extent.height = static_cast<uint32_t>(height);
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;

  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  imageInfo.usage = usage;

  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

  // VkImageFormatListCreateInfo formatList{};
  // formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
  // formatList.viewFormatCount = 2;
  // std::vector formats = {VK_FORMAT_A8B8G8R8_UNORM_PACK32,
  //                        VK_FORMAT_R8G8B8A8_SRGB};
  // formatList.pViewFormats = formats.data();

  // imageInfo.pNext = &formatList;

  // Enabling this will disable implicit gmsharing for this image
  if (isExplicit) {
    VkExternalMemoryImageCreateInfo memCreate{};
    memCreate.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
    memCreate.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
    imageInfo.pNext = &memCreate;
  }

  if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = Utils::findMemoryType(
      physicalDevice, memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(device, image, imageMemory, 0);
}

void VulkanImage::transitionImageLayout(VkImage image, VkFormat format,
                                        VkImageLayout oldLayout,
                                        VkImageLayout newLayout) {

  VkCommandBuffer commandBuffer =
      Utils::beginSingleTimeCommands(device, commandPool);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;

  // must be set to VK_QUEUE_FAMILY_IGNORED if you don't want to transfer queue
  // family ownership
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  barrier.srcAccessMask = 0; // TODO
  barrier.dstAccessMask = 0; // TODO

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  }

  else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
  Utils::endSingleTimeCommands(device, commandPool, commandBuffer,
                               graphicsQueue);
}

void VulkanImage::copyBufferToImage(VkBuffer buffer, VkImage image,
                                    uint32_t width, uint32_t height) {
  VkCommandBuffer commandBuffer =
      Utils::beginSingleTimeCommands(device, commandPool);

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  Utils::endSingleTimeCommands(device, commandPool, commandBuffer,
                               graphicsQueue);
}

void VulkanImage::createTextureImage(const char *texPath, VkImage &image,
                                     VkDeviceMemory &imageMemory,
                                     bool isExplicit) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels =
      stbi_load(texPath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  Utils::createBuffer(physicalDevice, device, imageSize,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);

  stbi_image_free(pixels);

  // createImage(
  //     texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
  //     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
  //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

  createImage(texWidth, texHeight, VK_FORMAT_A8B8G8R8_UNORM_PACK32,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, false);

  transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth),
                    static_cast<uint32_t>(texHeight));

  transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void VulkanImage::createTextureSampler() {
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  // https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  // Can also just disable if feature not supported
  // samplerInfo.anisotropyEnable = VK_FALSE;
  // samplerInfo.maxAnisotropy = 1.0f;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

  samplerInfo.unnormalizedCoordinates = VK_FALSE;

  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

} // namespace VulkanStuff