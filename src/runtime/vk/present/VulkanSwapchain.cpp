#include "runtime/vk/present/VulkanSwapchain.hpp"
#include "core/Log.hpp"
namespace RT {
	vk::SwapchainKHR VulkanSwapchain::handle() const {
		return swapchain_;
	}

	vk::Format VulkanSwapchain::imageFormat() const {
		return imageFormat_;
	}
	vk::ColorSpaceKHR VulkanSwapchain::colorSpace() const {
		return colorSpace_;
	}
	vk::Extent2D VulkanSwapchain::extent() const {
		return extent_;
	}
	vk::PresentModeKHR VulkanSwapchain::presentMode() const {
		return presentMode_;
	}

	uint32_t VulkanSwapchain::width() const {
		return extent_.width;
	}
	uint32_t VulkanSwapchain::height() const {
		return extent_.height;
	}

	std::size_t VulkanSwapchain::imageCount() const {
		return imageCount_;
	}

	vk::Image VulkanSwapchain::image(std::size_t index) const {
		return images_[index];
	}
	vk::ImageView VulkanSwapchain::imageView(std::size_t index) const {
		return imageviews_[index];
	}

	const std::vector<vk::Image>& VulkanSwapchain::images() const {
		return images_;
	}
	const std::vector<vk::ImageView>& VulkanSwapchain::imageViews() const {
		return imageviews_;
	}

	void VulkanSwapchain::create(const vk::Device& device,
		const vk::SurfaceKHR& surface,
		const vk::PhysicalDevice& physicaldevice,
		const uint32_t& graphicsFamily,
		const uint32_t& presentFamily) {

	}
	void VulkanSwapchain::destroy(const vk::Device& device) {

	}
	void VulkanSwapchain::recreate(const vk::Device& device,
		const vk::SurfaceKHR& surface,
		const vk::PhysicalDevice& physicaldevice,
		const uint32_t& graphicsFamily,
		const uint32_t& presentFamily) {

	}

	bool VulkanSwapchain::isValid() const {
		return isvalid_ && static_cast<bool>(swapchain_);
	}
	VulkanSwapchain::operator bool() const {
		return isValid();
	}
}