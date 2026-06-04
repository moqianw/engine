#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "core/Log.hpp"
namespace RT {
	enum class SlotState : uint8_t {
		eFree = 0,
		eAlive,
		ePendingDestroy
	};
	struct SlotMeta {
		uint32_t generation = 1;
		SlotState state = SlotState::eFree;
		uint64_t lastusedvalue = 0;
	};
	template<typename Tag>
	class Handle {
	public:
		constexpr Handle() = default;
		constexpr explicit Handle(uint64_t raw) { u_.raw_ = raw; }
		constexpr bool isValid() const { return u_.raw_ != 0; }
		constexpr explicit operator bool() const { return isValid(); }
		constexpr uint32_t index() const { return u_.parts.index; }
		constexpr uint32_t generation() const { return u_.parts.generation; }
		constexpr uint64_t raw() const { return u_.raw_; }
		constexpr bool operator==(const Handle& other) const { return u_.raw_ == other.u_.raw_; }
		constexpr bool operator!=(const Handle& other) const { return u_.raw_ != other.u_.raw_; }
	private:
		union{
			uint64_t raw_ = 0;
			struct {
				uint32_t index;//1-base
				uint32_t generation;//1-base
			}parts;
		}u_;
		template<typename H> friend class SlotMap;
	};


	template<typename H>
	class SlotMap {
	public:
		H allocate() {
			uint32_t index = 0;
			if (freehead_ != 0) {
				index = freehead_;
				freehead_ = nextfree_[freehead_ - 1];
			}
			else {
				meta_.emplace_back();
				index = meta_.size();
				nextfree_.push_back(0);
			}
			auto& m = meta_[index - 1];
			m.state = SlotState::eAlive;
			return makeHandle(index, m.generation);
		}
		bool isAlive(const H& h) const {
			const uint32_t index = h.index();
			if (index == 0 || index > meta_.size()) {
				return false;
			}
			const auto& m = meta_[index - 1];
			return (m.state == SlotState::eAlive) && (m.generation == h.generation());
		}

		bool isValidGeneration(const H& h) const {//存活或待销毁

			const uint32_t index = h.index();
			if (index == 0 || index > meta_.size()) {
				return false;
			}
			const auto& m = meta_[index - 1];
			return (m.state != SlotState::eFree) && (m.generation == h.generation());
		}
		void pendingRelease(const H& h) {//逻辑删除
			const auto index = validate(h);
			auto& m = meta_[index - 1];
			if (m.state == SlotState::eFree) {
				EG_TRACE("SlotMap::pendingRelease: handle {} is not alive, cannot pending release", h.raw());
			}
			m.state = SlotState::ePendingDestroy;
		}

		void release(const H& h) {
			const auto index = validate(h);
			auto& m = meta_[index - 1];
			switch (m.state) {
			case SlotState::eFree:
			{
				EG_TRACE("SlotMap::release: handle {} is already free, cannot release again", h.raw());
				return;
			}
			case SlotState::eAlive:
			{
				EG_ERROR("SlotMap::release: handle {} is alive but not pending destroy, releasing directly", h.raw());
				return;
			}
			case SlotState::ePendingDestroy:
				m.state = SlotState::eFree;
				m.generation++;
				if (m.generation == 0) m.generation = 1;
				m.lastusedvalue = 0;
				nextfree_[index - 1] = freehead_;
				freehead_ = index;
			}

		}
		void reset() {
			meta_.clear();
			nextfree_.clear();
			freehead_ = 0;
		}
		size_t count() const {
			return meta_.size();
		}
		void touch(const H& h, uint64_t usedvalue) {
			const auto index = validate(h);
			auto& m = meta_[index - 1];
			if (m.state != SlotState::eAlive) return;
			m.lastusedvalue = usedvalue;
		}
		uint64_t getLastUsedValue(const H& h) const {
			const auto index = validate(h);
			return meta_[index - 1].lastusedvalue;
		}
	private:
		uint32_t validate(const H& h) const {
			const uint32_t idx = h.index();
			if (idx == 0 || idx > meta_.size()) {
				EG_ERROR("SlotMap::validate: handle {} index {} out of range", h.raw(), idx);
				return 0;
			}
			const SlotMeta& m = meta_[idx - 1];
			if (m.generation != h.generation()) {
				EG_ERROR("SlotMap::validate: handle {} generation mismatch, expected {}, actual {}", h.raw(), m.generation, h.generation());
				return 0;
			}
			return idx;
		}
		H makeHandle(uint32_t index, uint32_t generation) {
			uint64_t h = (static_cast<uint64_t>(generation) << 32) | static_cast<uint64_t>(index);
			return H{ h };
		}

		std::vector<SlotMeta> meta_;
		//freehandle的链表
		std::vector<uint32_t> nextfree_;//下一个freehandle
		uint32_t freehead_ = 0;//第一个freehandle
	};

	
}