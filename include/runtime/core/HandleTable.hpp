#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "runtime/core/Handle.hpp"

namespace RT {

    template<typename H, typename D>
    class HandleTable {
    private:
        std::vector<D> data_;
        std::vector<H> aliveHandles_;
        SlotMap<H> slots_{};

    public:
        HandleTable() = default;

        H allocate()
        {
            H handle = slots_.allocate();

            const std::uint32_t slot = slotIndex(handle);
            ensureRecordStorage(slot);

            data_[slot] = D{};

            return handle;
        }

        bool commit(const H& handle)
        {
            if (!slots_.isReserved(handle)) {
                return false;
            }

            if (!slots_.markAlive(handle)) {
                return false;
            }

            aliveHandles_.push_back(handle);
            return true;
        }

        bool cancel(const H& handle)
        {
            if (!slots_.isReserved(handle)) {
                return false;
            }

            const std::uint32_t slot = slotIndex(handle);

            if (slot < data_.size()) {
                data_[slot] = D{};
            }

            return slots_.cancelAllocate(handle);
        }

        bool pendingDestroy(const H& handle)
        {
            if (!slots_.isAlive(handle)) {
                return false;
            }

            if (!slots_.pendingRelease(handle)) {
                return false;
            }

            eraseAliveHandle(handle);
            return true;
        }

        bool release(const H& handle)
        {
            if (!slots_.isPendingDestroy(handle)) {
                return false;
            }

            const std::uint32_t slot = slotIndex(handle);

            if (slot < data_.size()) {
                data_[slot] = D{};
            }

            return slots_.release(handle);
        }

        bool releaseForce(const H& handle)
        {
            if (!slots_.isAlive(handle)) {
                return false;
            }

            if (!slots_.pendingRelease(handle)) {
                return false;
            }

            eraseAliveHandle(handle);

            return release(handle);
        }

        D* getReserved(const H& handle)
        {
            if (!slots_.isReserved(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* getReserved(const H& handle) const
        {
            if (!slots_.isReserved(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        D* get(const H& handle)
        {
            if (!slots_.isAlive(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* get(const H& handle) const
        {
            if (!slots_.isAlive(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        D* getPending(const H& handle)
        {
            if (!slots_.isPendingDestroy(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* getPending(const H& handle) const
        {
            if (!slots_.isPendingDestroy(handle)) {
                return nullptr;
            }

            return recordAt(handle);
        }

        D* getAliveOrPending(const H& handle)
        {
            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* getAliveOrPending(const H& handle) const
        {
            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        D* getAnyValid(const H& handle)
        {
            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eReserved &&
                state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* getAnyValid(const H& handle) const
        {
            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eReserved &&
                state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        bool contains(const H& handle) const
        {
            return getAliveOrPending(handle) != nullptr;
        }

        bool isReserved(const H& handle) const
        {
            return slots_.isReserved(handle);
        }

        bool isAlive(const H& handle) const
        {
            return slots_.isAlive(handle);
        }

        bool isPendingDestroy(const H& handle) const
        {
            return slots_.isPendingDestroy(handle);
        }

        bool isValidGeneration(const H& handle) const
        {
            return slots_.isValidGeneration(handle);
        }

        SlotState stateOf(const H& handle) const
        {
            return slots_.stateOf(handle);
        }

        bool touch(const H& handle, std::uint64_t timelineValue)
        {
            return slots_.touch(handle, timelineValue);
        }

        std::uint64_t lastUsedValue(const H& handle) const
        {
            return slots_.getLastUsedValue(handle);
        }

        template<typename Fn>
        void forEachAlive(Fn&& fn)
        {
            for (const H& handle : aliveHandles_) {
                if (D* record = get(handle)) {
                    fn(handle, *record);
                }
            }
        }

        template<typename Fn>
        void forEachAlive(Fn&& fn) const
        {
            for (const H& handle : aliveHandles_) {
                if (const D* record = get(handle)) {
                    fn(handle, *record);
                }
            }
        }

        const std::vector<H>& aliveHandles() const
        {
            return aliveHandles_;
        }

        void clear()
        {
            data_.clear();
            aliveHandles_.clear();
            slots_.reset();
        }

        std::size_t slotCount() const
        {
            return slots_.slotCount();
        }

        std::size_t reservedCount() const
        {
            return slots_.reservedCount();
        }

        std::size_t aliveCount() const
        {
            return slots_.aliveCount();
        }

        std::size_t pendingDestroyCount() const
        {
            return slots_.pendingDestroyCount();
        }

        std::size_t freeCount() const
        {
            return slots_.freeCount();
        }

        bool empty() const
        {
            return slots_.empty();
        }

    private:
        static std::uint32_t slotIndex(const H& handle)
        {
            return handle.index() - 1;
        }

        void ensureRecordStorage(std::uint32_t slot)
        {
            if (slot >= data_.size()) {
                data_.resize(static_cast<std::size_t>(slot) + 1);
            }
        }

        D* recordAt(const H& handle)
        {
            const std::uint32_t slot = slotIndex(handle);

            if (slot >= data_.size()) {
                return nullptr;
            }

            return &data_[slot];
        }

        const D* recordAt(const H& handle) const
        {
            const std::uint32_t slot = slotIndex(handle);

            if (slot >= data_.size()) {
                return nullptr;
            }

            return &data_[slot];
        }

        void eraseAliveHandle(const H& handle)
        {
            auto it = std::find(aliveHandles_.begin(), aliveHandles_.end(), handle);

            if (it == aliveHandles_.end()) {
                return;
            }

            *it = aliveHandles_.back();
            aliveHandles_.pop_back();
        }
    };

} // namespace RT