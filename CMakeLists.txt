cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VKGame VERSION 1.0)

# if(NOT DEFINED ENV{VULKAN_SDK})
#     message("Environment variable of VULKAN_SDK is not defined: $ENV{VULKAN_SDK}")
# else()
#     set(VULKAN_SDK  $ENV{VULKAN_SDK})
#     message("Environment variable of VULKAN_SDK is defined: $ENV{VULKAN_SDK}")
# endif()

# if (VULKAN_SDK)
#     include_directories ("${VULKAN_SDK}/include")
# endif()

# find_library(Vulkan_LIBRARY
#              NAMES vulkan vulkan-1
#              PATHS ${VULKAN_SDK}/lib )
# message("Vulkan_LIBRARY:${Vulkan_LIBRARY}")

# devenv  Path: D:\visualstudio2022\Common7\IDE
# MSBuild Path: D:\visualstudio2022\MSBuild\Current\Bin
# To Build Command line windows
# devenv VKGame.sln /Rebuild "Release|x64"
# MSBuild VKGame.vcxproj -t:Rebuild -p:Configuration=Release

# Help
# devenv /?

# Debug
# devenv /DebugExe Debug/VKGame.exe
#
# To Regenerate the compile_commands.json
# Open Visual Studio->Project->Clang Power Tools->Export Compilation Database

find_package(Vulkan REQUIRED)

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/VulkanSDK/SDL2-2.30.7/x86_64-w64-mingw32/lib/cmake/SDL2")
    message("CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")
ENDIF(WIN32)

include_directories(${Vulkan_INCLUDE_DIR})

message("Vulkan_INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

message("SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")

#/Users/bora/VulkanSDK/1.3.283.0/iOS/include

message("Project Build Dir: ${PROJECT_BINARY_DIR}")
file(COPY "${PROJECT_SOURCE_DIR}/textures" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${PROJECT_SOURCE_DIR}/shaders" DESTINATION "${PROJECT_BINARY_DIR}")

if(WIN32)
    file(COPY "${PROJECT_SOURCE_DIR}/bin/SDL2.dll" DESTINATION "${PROJECT_BINARY_DIR}")
    set_property(DIRECTORY ${PROJECT_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
ENDIF(WIN32)


include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/external")
include_directories ("${PROJECT_SOURCE_DIR}/include")

if(WIN32)
    #This sets Project Properties->Linker->System->Subsystem to Windows
    #see https://cmake.org/cmake/help/v3.0/prop_tgt/WIN32_EXECUTABLE.html#prop_tgt:WIN32_EXECUTABLE
    #add_executable (VKGame WIN32
    #else it will create as default console app
    add_executable (VKGame
        "src/game.cpp"
        "src/vulkan_renderer.cpp"
        "src/utils.cpp"
        "src/vulkan_buffer.cpp"
	"src/vulkan_command.cpp"
	"src/vulkan_device.cpp"
	"src/vulkan_image.cpp"
	"src/vulkan_pipeline.cpp"
	"src/vulkan_renderpass.cpp"
	"src/vulkan_swapchain.cpp"
	"src/vulkan_syncobject.cpp"
        "src/main.cpp")
ELSEIF(UNIX)
    include_directories("/Users/bora/VulkanSDK/1.3.283.0/iOS/include")
    add_executable (VKGame
        "src/game.cpp"
        "src/vulkan_renderer.cpp"
        "src/text_overlay.cpp"
        "src/gltf_loader.cpp"
        "src/main.cpp")
ENDIF(WIN32)

target_link_libraries(VKGame PUBLIC "${SDL2_LIBRARIES}")
target_link_libraries(VKGame PUBLIC "${Vulkan_LIBRARY}")
