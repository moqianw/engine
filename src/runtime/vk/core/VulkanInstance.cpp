#include "runtime/vk/core/VulkanInstance.hpp"

#include <vector>
#include "core/Log.hpp"
namespace RT {
	VulkanInstance::~VulkanInstance()
	{
		destroy();
	}
	void VulkanInstance::create()
	{
		if (isValid()) return;
		vk::InstanceCreateInfo createinfo{};
		vk::ApplicationInfo appinfo;
		appinfo.setApiVersion(vk::ApiVersion14)
			.setApplicationVersion(1)
			.setEngineVersion(1)
			.setPApplicationName("gameengine")
			.setPEngineName("gameengine");

		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
		};
		createinfo.setPApplicationInfo(&appinfo)
			.setPpEnabledExtensionNames(extensions.data())
			.setEnabledExtensionCount(extensions.size());
		instance_ = vk::createInstance(createinfo);
		if (!instance_) {
			EG_ERROR("VulkanInstance::create: error");
		}
		EG_INFO("VulkanInstance created");
	}
	void VulkanInstance::destroy()
	{
		if (!isValid()) return;
		instance_.destroy();
		EG_INFO("VulkanInstance destroyed");
		instance_ = nullptr;

	}
	vk::Instance VulkanInstance::handle() const
	{
		return instance_;
	}
	vk::Instance VulkanInstance::handle()
	{
		return instance_;
	}
	bool VulkanInstance::isValid() const
	{
		return static_cast<bool>(instance_);
	}
	VulkanInstance::operator bool() const {
		return isValid();
	}
}