#pragma once

#include "runtime/vk/core/VulkanCore.hpp"
#include "runtime/vk/present/VulkanSurface.hpp"
namespace PL {
	class QtWindow;
}
namespace RT {
	class VulkanBackend {
	private:
		VulkanCore core_{};


		bool isvalid_ = false;

	public:
		VulkanBackend() = default;
		~VulkanBackend() = default;
		void create();
		void destroy();


		bool isValid() const;
		explicit operator bool() const;

	};
}