#pragma once

#include <optional>
#include <runtime/vk/core/VertexFormat.hpp>
namespace UT {
    struct VulkanQueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        VulkanQueueFamilyIndices() = default;
        VulkanQueueFamilyIndices& operator=(const VulkanQueueFamilyIndices& other) = default;
        VulkanQueueFamilyIndices(const VulkanQueueFamilyIndices& other) = default;
        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
        operator bool() { return isComplete(); }
    };

}

