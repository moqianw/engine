#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "core/Log.hpp"

namespace RT {

    enum class SlotState : std::uint8_t {
        eFree = 0,
        eAlive,
        ePendingDestroy
    };

    struct SlotMeta {
        std::uint32_t generation = 1;
        SlotState state = SlotState::eFree;

        std::uint64_t lastUsedValue = 0;
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
            return raw_ != 0;
        }

        constexpr explicit operator bool() const
        {
            return isValid();
        }

        // 1-based index.
        // 0 means invalid handle.
        constexpr std::uint32_t index() const
        {
            return static_cast<std::uint32_t>(raw_ & 0xffffffffull);
        }

        // 1-based generation.
        // 0 is never used for valid handles.
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

            meta.state = SlotState::eAlive;
            meta.lastUsedValue = 0;

            return makeHandle(index, meta.generation);
        }

        bool isAlive(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, false)) {
                return false;
            }

            const SlotMeta& meta = meta_[index - 1];
            return meta.state == SlotState::eAlive;
        }

        bool isPendingDestroy(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, false)) {
                return false;
            }

            const SlotMeta& meta = meta_[index - 1];
            return meta.state == SlotState::ePendingDestroy;
        }

        bool isValidGeneration(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, false)) {
                return false;
            }

            const SlotMeta& meta = meta_[index - 1];
            return meta.state != SlotState::eFree;
        }

        SlotState stateOf(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, false)) {
                return SlotState::eFree;
            }

            return meta_[index - 1].state;
        }

        bool pendingRelease(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, true)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            switch (meta.state) {
            case SlotState::eFree:
                EG_TRACE(
                    "SlotMap::pendingRelease: handle {} is already free",
                    handle.raw()
                );
                return false;

            case SlotState::ePendingDestroy:
                EG_TRACE(
                    "SlotMap::pendingRelease: handle {} is already pending destroy",
                    handle.raw()
                );
                return true;

            case SlotState::eAlive:
                meta.state = SlotState::ePendingDestroy;
                return true;
            }

            return false;
        }

        bool release(const H& handle)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, true)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            switch (meta.state) {
            case SlotState::eFree:
                EG_TRACE(
                    "SlotMap::release: handle {} is already free",
                    handle.raw()
                );
                return false;

            case SlotState::eAlive:
                EG_ERROR(
                    "SlotMap::release: handle {} is alive, call pendingRelease first",
                    handle.raw()
                );
                return false;

            case SlotState::ePendingDestroy:
                meta.state = SlotState::eFree;
                ++meta.generation;

                if (meta.generation == 0) {
                    meta.generation = 1;
                }

                meta.lastUsedValue = 0;

                nextFree_[index - 1] = freeHead_;
                freeHead_ = index;

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

        bool touch(const H& handle, std::uint64_t usedValue)
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, true)) {
                return false;
            }

            SlotMeta& meta = meta_[index - 1];

            if (meta.state != SlotState::eAlive) {
                return false;
            }

            meta.lastUsedValue = usedValue;
            return true;
        }

        std::uint64_t getLastUsedValue(const H& handle) const
        {
            std::uint32_t index = 0;

            if (!tryGetIndex(handle, index, true)) {
                return 0;
            }

            return meta_[index - 1].lastUsedValue;
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

        std::size_t aliveCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state == SlotState::eAlive) {
                    ++count;
                }
            }

            return count;
        }

        std::size_t pendingDestroyCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state == SlotState::ePendingDestroy) {
                    ++count;
                }
            }

            return count;
        }

        std::size_t freeCount() const
        {
            std::size_t count = 0;

            for (const SlotMeta& meta : meta_) {
                if (meta.state == SlotState::eFree) {
                    ++count;
                }
            }

            return count;
        }

        bool empty() const
        {
            return aliveCount() == 0 && pendingDestroyCount() == 0;
        }

    private:
        bool tryGetIndex(
            const H& handle,
            std::uint32_t& outIndex,
            bool logError
        ) const
        {
            if (!handle.isValid()) {
                if (logError) {
                    EG_ERROR("SlotMap::tryGetIndex: invalid handle 0");
                }

                return false;
            }

            const std::uint32_t index = handle.index();

            if (index == 0 || index > meta_.size()) {
                if (logError) {
                    EG_ERROR(
                        "SlotMap::tryGetIndex: handle {} index {} out of range",
                        handle.raw(),
                        index
                    );
                }

                return false;
            }

            const SlotMeta& meta = meta_[index - 1];

            if (meta.generation != handle.generation()) {
                if (logError) {
                    EG_ERROR(
                        "SlotMap::tryGetIndex: handle {} generation mismatch, expected {}, actual {}",
                        handle.raw(),
                        meta.generation,
                        handle.generation()
                    );
                }

                return false;
            }

            outIndex = index;
            return true;
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