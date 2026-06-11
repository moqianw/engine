#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace RT {

    enum class SlotState : std::uint8_t {
        eFree = 0,
        eReserved,
        eAlive,
        ePendingDestroy
    };

    struct SlotMeta {
        std::uint32_t generation_ = 1;
        SlotState state_ = SlotState::eFree;
        std::uint64_t lastUsedValue_ = 0;
    };

    template<typename Tag>
    class Handle {
    private:
        std::uint64_t raw_ = 0;

        template<typename H>
        friend class SlotMap;

    public:
        constexpr Handle() = default;

        constexpr explicit Handle(std::uint64_t raw)
            : raw_(raw)
        {
        }

        static constexpr Handle invalid()
        {
            return Handle{};
        }

        constexpr bool isValid() const
        {
            return raw_ != 0 && index() != 0 && generation() != 0;
        }

        constexpr explicit operator bool() const
        {
            return isValid();
        }

        // 1-based index.
        // 0 means invalid.
        constexpr std::uint32_t index() const
        {
            return static_cast<std::uint32_t>(raw_ & 0xffffffffull);
        }

        // 1-based generation.
        // 0 means invalid.
        constexpr std::uint32_t generation() const
        {
            return static_cast<std::uint32_t>(raw_ >> 32);
        }

        constexpr std::uint64_t raw() const
        {
            return raw_;
        }

        constexpr bool operator==(const Handle& other) const
        {
            return raw_ == other.raw_;
        }

        constexpr bool operator!=(const Handle& other) const
        {
            return raw_ != other.raw_;
        }
    };

    template<typename H>
    class SlotMap {
    private:
        std::vector<SlotMeta> meta_;
        std::vector<std::uint32_t> nextFree_;

        // 1-based index of first free slot.
        // 0 means no free slot.
        std::uint32_t freeHead_ = 0;

    public:
        H allocate()
        {
            std::uint32_t index = 0;

            if (freeHead_ != 0) {
                index = freeHead_;
                freeHead_ = nextFree_[index - 1];
            }
            else {
                meta_.emplace_back();
                nextFree_.push_back(0);
                index = static_cast<std::uint32_t>(meta_.size());
            }

            SlotMeta& meta = meta_[index - 1];

            meta.state_ = SlotState::eReserved;
            meta.lastUsedValue_ = 0;

            return makeHandle(index, meta.generation_);
        }

        bool markAlive(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            if (meta.state_ != SlotState::eReserved) {
                return false;
            }
            meta.state_ = SlotState::eAlive;
            return true;
        }

        bool cancelAllocate(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            if (meta.state_ != SlotState::eReserved) {
                return false;
            }

            releaseToFreeList(index, meta);
            return true;
        }

        bool pendingRelease(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            switch (meta.state_) {
            case SlotState::eFree:
                return false;

            case SlotState::eReserved:
                return false;

            case SlotState::ePendingDestroy:
                return true;

            case SlotState::eAlive:
                meta.state_ = SlotState::ePendingDestroy;
                return true;
            }

            return false;
        }

        bool release(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            switch (meta.state_) {
            case SlotState::eFree:
                return false;

            case SlotState::eReserved:
                return false;

            case SlotState::eAlive:
                return false;

            case SlotState::ePendingDestroy:
                releaseToFreeList(index, meta);
                return true;
            }

            return false;
        }

        bool releaseNow(const H& handle)
        {
            if (!pendingRelease(handle)) {
                return false;
            }

            return release(handle);
        }

        bool isReserved(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            return meta_[index - 1].state_ == SlotState::eReserved;
        }

        bool isAlive(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            return meta_[index - 1].state_ == SlotState::eAlive;
        }

        bool isPendingDestroy(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            return meta_[index - 1].state_ == SlotState::ePendingDestroy;
        }

        bool isFree(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            return meta_[index - 1].state_ == SlotState::eFree;
        }

        bool isValidGeneration(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            return meta_[index - 1].state_ != SlotState::eFree;
        }

        SlotState stateOf(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return SlotState::eFree;
            }

            return meta_[index - 1].state_;
        }

        bool touch(const H& handle, std::uint64_t usedValue)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            if (meta.state_ != SlotState::eAlive) {
                return false;
            }

            meta.lastUsedValue_ = usedValue;
            return true;
        }

        std::uint64_t getLastUsedValue(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index)) {
                return 0;
            }

            return meta_[index - 1].lastUsedValue_;
        }

        void reset()
        {
            meta_.clear();
            nextFree_.clear();
            freeHead_ = 0;
        }

        std::size_t slotCount() const
        {
            return meta_.size();
        }

        std::size_t reservedCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state_ == SlotState::eReserved) {
                    ++count;
                }
            }

            return count;
        }

        std::size_t aliveCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state_ == SlotState::eAlive) {
                    ++count;
                }
            }

            return count;
        }

        std::size_t pendingDestroyCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state_ == SlotState::ePendingDestroy) {
                    ++count;
                }
            }

            return count;
        }

        std::size_t freeCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state_ == SlotState::eFree) {
                    ++count;
                }
            }

            return count;
        }

        bool empty() const
        {
            return reservedCount() == 0 &&
                aliveCount() == 0 &&
                pendingDestroyCount() == 0;
        }

    private:
        bool tryGetIndex(
            const H& handle,
            std::uint32_t& outIndex
        ) const
        {
            if (!handle.isValid()) return false;


            const std::uint32_t index = handle.index();

            if (index == 0 || index > meta_.size()) return false;

            const SlotMeta& meta = meta_[index - 1];

            if (meta.generation_ != handle.generation()) return false;
           
            outIndex = index;
            return true;
        }

        void releaseToFreeList(std::uint32_t index, SlotMeta& meta)
        {
            meta.state_ = SlotState::eFree;

            ++meta.generation_;
            if (meta.generation_ == 0) {
                meta.generation_ = 1;
            }

            meta.lastUsedValue_ = 0;

            nextFree_[index - 1] = freeHead_;
            freeHead_ = index;
        }

        static H makeHandle(std::uint32_t index, std::uint32_t generation)
        {
            const std::uint64_t raw =
                (static_cast<std::uint64_t>(generation) << 32) |
                static_cast<std::uint64_t>(index);

            return H{ raw };
        }
    };

} // namespace RT

namespace std {

    template<typename Tag>
    struct hash<RT::Handle<Tag>> {
        std::size_t operator()(const RT::Handle<Tag>& handle) const noexcept
        {
            return std::hash<std::uint64_t>{}(handle.raw());
        }
    };

} // namespace std