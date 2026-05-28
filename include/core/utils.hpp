#pragma once
#include <fstream>
#include <optional>

namespace UT {
	inline void hashCombine(std::size_t& seed, std::size_t value) {
		seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	}

    // 1. traits：默认不开启，只有特化后才能作为 Flags 使用
    template <typename BitType>
    struct FlagTraits
    {
        static constexpr bool isBitmask = false;
    };

    // 2. Flags 模板
    template <typename BitType>
    class Flags
    {
        static_assert(std::is_enum_v<BitType>, "BitType must be an enum type");
        static_assert(FlagTraits<BitType>::isBitmask, "BitType is not enabled for bitmask operations");

    public:
        using MaskType = std::underlying_type_t<BitType>;

        constexpr Flags() noexcept = default;

        constexpr Flags(BitType bit) noexcept
            : m_mask(static_cast<MaskType>(bit))
        {}

        constexpr explicit Flags(MaskType flags) noexcept
            : m_mask(flags)
        {}

        constexpr Flags(const Flags&) noexcept = default;
        constexpr Flags& operator=(const Flags&) noexcept = default;

        constexpr explicit operator bool() const noexcept
        {
            return m_mask != 0;
        }

        constexpr explicit operator MaskType() const noexcept
        {
            return m_mask;
        }

        constexpr MaskType mask() const noexcept
        {
            return m_mask;
        }

        constexpr Flags operator~() const noexcept
        {
            return Flags(static_cast<MaskType>(~m_mask));
        }

        constexpr Flags& operator|=(Flags rhs) noexcept
        {
            m_mask |= rhs.m_mask;
            return *this;
        }

        constexpr Flags& operator&=(Flags rhs) noexcept
        {
            m_mask &= rhs.m_mask;
            return *this;
        }

        constexpr Flags& operator^=(Flags rhs) noexcept
        {
            m_mask ^= rhs.m_mask;
            return *this;
        }

        constexpr Flags& operator|=(BitType rhs) noexcept
        {
            m_mask |= static_cast<MaskType>(rhs);
            return *this;
        }

        constexpr Flags& operator&=(BitType rhs) noexcept
        {
            m_mask &= static_cast<MaskType>(rhs);
            return *this;
        }

        constexpr Flags& operator^=(BitType rhs) noexcept
        {
            m_mask ^= static_cast<MaskType>(rhs);
            return *this;
        }

        friend constexpr Flags operator|(Flags lhs, Flags rhs) noexcept
        {
            return Flags(lhs.m_mask | rhs.m_mask);
        }

        friend constexpr Flags operator&(Flags lhs, Flags rhs) noexcept
        {
            return Flags(lhs.m_mask & rhs.m_mask);
        }

        friend constexpr Flags operator^(Flags lhs, Flags rhs) noexcept
        {
            return Flags(lhs.m_mask ^ rhs.m_mask);
        }

        friend constexpr bool operator==(Flags lhs, Flags rhs) noexcept
        {
            return lhs.m_mask == rhs.m_mask;
        }

        friend constexpr bool operator!=(Flags lhs, Flags rhs) noexcept
        {
            return lhs.m_mask != rhs.m_mask;
        }

    private:
        MaskType m_mask = 0;
    };

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator|(BitType lhs, BitType rhs) noexcept
    {
        using MaskType = std::underlying_type_t<BitType>;
        return Flags<BitType>(static_cast<MaskType>(lhs) | static_cast<MaskType>(rhs));
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator&(BitType lhs, BitType rhs) noexcept
    {
        using MaskType = std::underlying_type_t<BitType>;
        return Flags<BitType>(static_cast<MaskType>(lhs) & static_cast<MaskType>(rhs));
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator^(BitType lhs, BitType rhs) noexcept
    {
        using MaskType = std::underlying_type_t<BitType>;
        return Flags<BitType>(static_cast<MaskType>(lhs) ^ static_cast<MaskType>(rhs));
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator~(BitType bit) noexcept
    {
        using MaskType = std::underlying_type_t<BitType>;
        return Flags<BitType>(static_cast<MaskType>(~static_cast<MaskType>(bit)));
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator|(Flags<BitType> lhs, BitType rhs) noexcept
    {
        return lhs | Flags<BitType>(rhs);
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator|(BitType lhs, Flags<BitType> rhs) noexcept
    {
        return Flags<BitType>(lhs) | rhs;
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator&(Flags<BitType> lhs, BitType rhs) noexcept
    {
        return lhs & Flags<BitType>(rhs);
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator&(BitType lhs, Flags<BitType> rhs) noexcept
    {
        return Flags<BitType>(lhs) & rhs;
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator^(Flags<BitType> lhs, BitType rhs) noexcept
    {
        return lhs ^ Flags<BitType>(rhs);
    }

    template <typename BitType,
        typename = std::enable_if_t<FlagTraits<BitType>::isBitmask>>
        constexpr Flags<BitType> operator^(BitType lhs, Flags<BitType> rhs) noexcept
    {
        return Flags<BitType>(lhs) ^ rhs;
    }
}

