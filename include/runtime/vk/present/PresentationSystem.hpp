#pragma once
#include "runtime/vk/present/PresentTarget.hpp"
#include "runtime/core/HandleTable.hpp"

namespace RT {
	using PresentTargetTable = HandleTable<PresentTargetHandle, PresentTarget>;
	class PresentationSystem {
	private:
		PresentTargetTable table_{};
	public:

	};
}