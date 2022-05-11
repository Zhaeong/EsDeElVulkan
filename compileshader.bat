C:\VulkanSDK\1.3.211.0\Bin\glslc.exe shaders\simple_shader.vert -o shaders\simple_shader.vert.spv
C:\VulkanSDK\1.3.211.0\Bin\glslc.exe shaders\simple_shader.frag -o shaders\simple_shader.frag.spv

::C:\VulkanSDK\1.3.211.0\Bin\glslangvalidator --target-env vulkan1.2 -x -e main -o shaders\simple_shader.frag.spv shaders\simple_shader.frag
pause