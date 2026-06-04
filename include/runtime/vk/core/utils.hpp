#pragma once

#include <optional>
#include <cstdint>
namespace RT {
    struct VulkanQueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily;
        std::optional<uint32_t> computeFamily;
        VulkanQueueFamilyIndices() = default;
        VulkanQueueFamilyIndices& operator=(const VulkanQueueFamilyIndices& other) = default;
        VulkanQueueFamilyIndices(const VulkanQueueFamilyIndices& other) = default;
        bool hasGraphics() const {
            return graphicsFamily.has_value();
        }
        bool hasPresent() const {
            return presentFamily.has_value();
        }
        bool hasCompute() const {
            return computeFamily.has_value();
        }
        bool hasTransfer() const {
            return transferFamily.has_value();
        }
    };

    struct VulkanAdapterRequirements {
        bool requireGraphics = true;
        bool requireCompute = false;
        bool requireTransfer = false;
        bool requirePresent = false;
    };

    struct VulkanDeviceRequirements {
        bool requireGraphics = true;
        bool requirePresent = false;
        bool requireCompute = false;
        bool requireTransfer = false;
        bool enableSwapchain = false;
    };
}

