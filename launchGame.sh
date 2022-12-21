#!/bin/bash
#GMSharing Portion
export VK_ICD_FILENAMES=/etc/vulkan/icd.d/amd_icd64.json
export VK_LAYER_PATH="/usr/share/vulkan/explicit_layer.d:/opt/GfxCloudService/settings"

export ENABLE_GMSHARING_LAYER=1
export GM_ANALYSIS_MODE=2
export GM_ENABLE_VRS=0 
export GM_DISABLE_SHARED=0 

export GM_DISABLE_FILE_OUTPUT=0
export GM_LOGGER_NAME=/home/mvtest/Documents/GMUser.txt

export VK_INSTANCE_LAYERS="VK_LAYER_cloud_service"
export GM_SUPPORT_ALL_PLATFORMS=1

exec ./bin/vkGame 2>&1
#exec vkcube 2>&1
#/home/mvtest/Desktop/aco_reqs/oxenfreebins/OXENFREE/1011build
#exec /home/mvtest/Desktop/aco_reqs/oxenfreebins/OXENFREE/1011build/OXENFREE.x86_64 -vsync 1 -postprocessingmode 1 -useantialaise true -usebloom true


