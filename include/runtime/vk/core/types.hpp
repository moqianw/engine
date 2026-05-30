#pragma once

namespace RT {
	enum class VulkanQueueType {
		UnKnown,
		Graphics,
		Present,
		Compute,
		Transfer
	};
}