#!/bin/bash
#GMSharing Portion
export VK_ICD_FILENAMES=/etc/vulkan/icd.d/amd_icd64.json
export VK_LAYER_PATH="/home/mvtest/Documents/VulkanTools/builddebug/layersvt:/opt/GfxCloudService/settings"

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/mvtest/Documents/VulkanTools/builddebug/layersvt

export ENABLE_GMSHARING_LAYER=1
export GM_ANALYSIS_MODE=2
export GM_ENABLE_VRS=0 
export GM_DISABLE_SHARED=0 

export GM_DISABLE_FILE_OUTPUT=0
export GM_LOGGER_NAME=/home/mvtest/Documents/GMUser.txt

#export VK_INSTANCE_LAYERS="VK_LAYER_LUNARG_api_dump:VK_LAYER_cloud_service"
#export VK_INSTANCE_LAYERS="VK_LAYER_cloud_service:VK_LAYER_LUNARG_api_dump"
#export VK_INSTANCE_LAYERS="VK_LAYER_LUNARG_api_dump"
export VK_INSTANCE_LAYERS="VK_LAYER_cloud_service"
export VK_APIDUMP_LOG_FILENAME="apilog.txt"
export VK_APIDUMP_OUTPUT_FORMAT="text"
export GM_SUPPORT_ALL_PLATFORMS=1




#exec ./bin/vkGame 2>&1
exec vkcube 2>&1
#/home/mvtest/Desktop/aco_reqs/oxenfreebins/OXENFREE/1011build
#exec /home/mvtest/Documents/OxenfreeGame/02_23/oxenfree_gms/OXENFREE.x86_64 -screen-fullscreen 0 -screen-height 480 -screen-width 720 -vsync 1 -postprocessingmode 1 -useantialaise true -usebloom true


