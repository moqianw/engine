#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include "runtime/core/Handle.hpp"

namespace RT {

    template<typename H, typename D>
    class HandleTable {
    private:
        std::vector<D> data_;
        SlotMap<H> slots_{};

    public:
        HandleTable() = default;

        HandleTable(const HandleTable&) = delete;
        HandleTable& operator=(const HandleTable&) = delete;

        HandleTable(HandleTable&&) noexcept = default;
        HandleTable& operator=(HandleTable&&) noexcept = default;

        template<typename... Args>
        H create(Args&&... args)
        {
            static_assert(
                std::is_default_constructible_v<D>,
                "HandleTable with std::vector<D> requires D to be default constructible"
                );

            static_assert(
                std::is_move_assignable_v<D>,
                "HandleTable with std::vector<D> requires D to be move assignable"
                );

            H handle = slots_.allocate();

            const std::uint32_t slot = slotIndex(handle);
            ensureRecordStorage(slot);

            data_[slot] = D(std::forward<Args>(args)...);

            return handle;
        }

        bool markPendingDestroy(const H& handle)
        {
            if (!slots_.isAlive(handle)) {
                return false;
            }

            return slots_.pendingRelease(handle);
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

        bool releaseNow(const H& handle)
        {
            if (!slots_.isAlive(handle)) {
                return false;
            }

            if (!slots_.pendingRelease(handle)) {
                return false;
            }

            return release(handle);
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
            if (!slots_.isValidGeneration(handle)) {
                return nullptr;
            }

            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        const D* getAliveOrPending(const H& handle) const
        {
            if (!slots_.isValidGeneration(handle)) {
                return nullptr;
            }

            const SlotState state = slots_.stateOf(handle);

            if (state != SlotState::eAlive &&
                state != SlotState::ePendingDestroy) {
                return nullptr;
            }

            return recordAt(handle);
        }

        bool contains(const H& handle) const
        {
            return getAliveOrPending(handle) != nullptr;
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

        void clear()
        {
            data_.clear();
            slots_.reset();
        }

        std::size_t slotCount() const
        {
            return slots_.slotCount();
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
    };

} // namespace RT