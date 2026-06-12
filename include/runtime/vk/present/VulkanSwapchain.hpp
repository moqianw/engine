#pragma once
#include <vulkan/vulkan.hpp>
namespace RT {
	class VulkanSwapchain {
	private:
		vk::SwapchainKHR swapchain_ = nullptr;
		vk::Extent2D extent_{};
		vk::Format imageFormat_ = vk::Format::eUndefined;
		vk::ColorSpaceKHR colorSpace_ = vk::ColorSpaceKHR::eSrgbNonlinear;
		vk::PresentModeKHR presentMode_ = vk::PresentModeKHR::eFifo;
		uint32_t imageCount_ = 0;
		std::vector<vk::Image> images_;
		std::vector<vk::ImageView> imageviews_;
		bool isvalid_ = false;
	public:
		vk::SwapchainKHR handle() const;

		vk::Format imageFormat() const;
		vk::ColorSpaceKHR colorSpace() const;
		vk::Extent2D extent() const;
		vk::PresentModeKHR presentMode() const;

		uint32_t width() const;
		uint32_t height() const;

		std::size_t imageCount() const;

		vk::Image image(std::size_t index) const;
		vk::ImageView imageView(std::size_t index) const;

		const std::vector<vk::Image>& images() const;
		const std::vector<vk::ImageView>& imageViews() const;

		void create(const vk::Device& device,
			const vk::SurfaceKHR& surface,
			const vk::PhysicalDevice& physicaldevice,
			const uint32_t& graphicsFamily,
			const uint32_t& presentFamily);
		void destroy(const vk::Device& device);
		void recreate(const vk::Device& device,
			const vk::SurfaceKHR& surface,
			const vk::PhysicalDevice& physicaldevice,
			const uint32_t& graphicsFamily,
			const uint32_t& presentFamily);

		bool isValid() const;
		explicit operator bool() const;

	};
}