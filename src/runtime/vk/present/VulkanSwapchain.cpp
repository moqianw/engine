#include "runtime/vk/present/VulkanSwapchain.hpp"
#include "core/Log.hpp"
namespace RT {
	namespace {
		vk::SurfaceFormatKHR chooseSurfaceFormat(
			const std::vector<vk::SurfaceFormatKHR>& surfaceFormats)
		{
			if (surfaceFormats.size() == 1
				&& surfaceFormats.front().format == vk::Format::eUndefined) {
				return vk::SurfaceFormatKHR{
					vk::Format::eB8G8R8A8Srgb,
					vk::ColorSpaceKHR::eSrgbNonlinear
				};
			}

			const auto prefer = [&](vk::Format format) {
				return std::find_if(
					surfaceFormats.begin(),
					surfaceFormats.end(),
					[format](const vk::SurfaceFormatKHR& candidate) {
						return candidate.format == format
							&& candidate.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
					});
				};

			if (auto it = prefer(vk::Format::eB8G8R8A8Srgb); it != surfaceFormats.end()) {
				return *it;
			}
			if (auto it = prefer(vk::Format::eR8G8B8A8Srgb); it != surfaceFormats.end()) {
				return *it;
			}

			return surfaceFormats.front();
		}

		vk::PresentModeKHR choosePresentMode(
			const std::vector<vk::PresentModeKHR>& presentModes)
		{
			if (std::find(
				presentModes.begin(),
				presentModes.end(),
				vk::PresentModeKHR::eMailbox) != presentModes.end()) {
				return vk::PresentModeKHR::eMailbox;
			}
			if (std::find(
				presentModes.begin(),
				presentModes.end(),
				vk::PresentModeKHR::eFifo) != presentModes.end()) {
				return vk::PresentModeKHR::eFifo;
			}
			return presentModes.front();
		}

		uint32_t chooseImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
		{
			uint32_t imageCount = std::max(2u, capabilities.minImageCount + 1);
			if (capabilities.maxImageCount > 0) {
				imageCount = std::min(imageCount, capabilities.maxImageCount);
			}
			return imageCount;
		}

		vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilities.currentExtent;
			}

			return vk::Extent2D{
				std::clamp(
					capabilities.minImageExtent.width,
					capabilities.minImageExtent.width,
					capabilities.maxImageExtent.width),
				std::clamp(
					capabilities.minImageExtent.height,
					capabilities.minImageExtent.height,
					capabilities.maxImageExtent.height)
			};
		}
	}
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
		if (!isValid() || !device || !surface || !physicaldevice) {
			EG_ERROR("Swapchain::create failed: invalid Vulkan device, physical device or surface");
			return;
		}

		const vk::SurfaceCapabilitiesKHR surfacecaps =
			physicaldevice.getSurfaceCapabilitiesKHR(surface);
		const std::vector<vk::SurfaceFormatKHR> surfaceformats =
			physicaldevice.getSurfaceFormatsKHR(surface);
		const std::vector<vk::PresentModeKHR> surfacepresentmodes =
			physicaldevice.getSurfacePresentModesKHR(surface);

		if (surfaceformats.empty()) {
			EG_ERROR("Swapchain::create failed: surface does not expose any formats");
			return;
		}
		if (surfacepresentmodes.empty()) {
			EG_ERROR("Swapchain::create failed: surface does not expose any present modes");
			return;
		}

		const vk::SurfaceFormatKHR surfaceformat = chooseSurfaceFormat(surfaceformats);
		presentMode_ = choosePresentMode(surfacepresentmodes);

		imageFormat_ = surfaceformat.format;
		extent_ = chooseExtent(surfacecaps);
		if (extent_.width == 0 || extent_.height == 0) {
			EG_WARN("Swapchain::create skipped because surface extent is zero");
			return;
		}

		vk::SwapchainCreateInfoKHR createinfo;
		createinfo.setSurface(surface)
			.setClipped(true)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setImageArrayLayers(1)
			.setImageFormat(surfaceformat.format)
			.setMinImageCount(chooseImageCount(surfacecaps))
			.setImageColorSpace(surfaceformat.colorSpace)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setPresentMode(presentMode_)
			.setImageExtent(extent_);

		uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };
		if (graphicsFamily != presentFamily) {
			createinfo.setImageSharingMode(vk::SharingMode::eConcurrent)
				.setQueueFamilyIndexCount(2)
				.setPQueueFamilyIndices(queueFamilyIndices);
		}
		else {
			createinfo.setImageSharingMode(vk::SharingMode::eExclusive);
		}

		swapchain_ = device.createSwapchainKHR(createinfo);
		if (!swapchain_) {
			EG_ERROR("Create SwapChain ERROR");
			return;
		}

		images_ = device.getSwapchainImagesKHR(swapchain_);

		imageviews_.resize(images_.size());
		vk::ImageViewCreateInfo createinfo_;
		vk::ImageSubresourceRange subrange;
		subrange.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1);
		vk::ComponentMapping componentmapping;
		componentmapping.setA(vk::ComponentSwizzle::eA)
			.setB(vk::ComponentSwizzle::eB)
			.setG(vk::ComponentSwizzle::eG)
			.setR(vk::ComponentSwizzle::eR);
		createinfo_.setFormat(imageFormat_)
			.setSubresourceRange(subrange)
			.setComponents(componentmapping)
			.setViewType(vk::ImageViewType::e2D);

		for (uint32_t i = 0; i < imageviews_.size(); i++) {
			createinfo_.setImage(images_[i]);
			imageviews_[i] = device.createImageView(createinfo_);
		}
		isvalid_ = true;
		EG_INFO("SwapChain created");
	}
	void VulkanSwapchain::destroy(const vk::Device& device) {
		isvalid_ = false;
		if (device) {
			for (auto& imageview : imageviews_) {
				if (imageview) {
					device.destroyImageView(imageview);
				}
			}
			if (swapchain_) {
				device.destroySwapchainKHR(swapchain_);
			}
		}

		imageviews_.clear();
		images_.clear();
		swapchain_ = nullptr;
		extent_ = vk::Extent2D{ 0u, 0u };
		imageFormat_ = vk::Format::eUndefined;
		EG_INFO("SwapChain destroyed");
	}
	void VulkanSwapchain::recreate(const vk::Device& device,
		const vk::SurfaceKHR& surface,
		const vk::PhysicalDevice& physicaldevice,
		const uint32_t& graphicsFamily,
		const uint32_t& presentFamily) {
		destroy(device);
		create(device, surface, physicaldevice, graphicsFamily, presentFamily);
		EG_TRACE("SwapChain recreated");
	}

	bool VulkanSwapchain::isValid() const {
		return isvalid_ && static_cast<bool>(swapchain_);
	}
	VulkanSwapchain::operator bool() const {
		return isValid();
	}
}