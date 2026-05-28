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

		//test surface
		VulkanSurface surface_{};
	public:
		VulkanBackend() = default;
		void create();
		void destroy();

		~VulkanBackend();

		bool isValid() const;
		explicit operator bool() const;

		//test
		void pushtestsurface(PL::QtWindow* window);

	};
}