# SDL Vulkan Sample
## Requirements
Install Vulkan SDK https://sdk.lunarg.com/sdk/download/1.3.290.0/windows/VulkanSDK-1.3.290.0-Installer.exe

Download [https://github.com/libsdl-org/SDL/releases/tag/release-2.30.7](https://github.com/libsdl-org/SDL/releases/download/release-2.30.7/SDL2-devel-2.30.7-mingw.zip)

Unzip SDL2-2.30.7 to C:\VulkanSDK
## To configure
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## To Run

```
cd build
.\Debug\VKGame.exe
```
