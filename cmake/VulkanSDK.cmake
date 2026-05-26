find_package(Vulkan REQUIRED)

message(STATUS "Vulkan found")
message(STATUS "Vulkan include dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")