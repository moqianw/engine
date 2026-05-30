#pragma once
#include <array>
#include <vector>
#include <cstddef>
#include <cstdint>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
namespace RT{
    enum class VertexFormat : uint8_t {
        // P3: position 3 floats
        // C4: color 4 uint8_t
        // N3: normal 3 floats
        // T2: texcoord0 (uv0) 2 floats
        // UV2: second texcoord (uv1) 2 floats
        // W4: weights 4 floats
        // I4: indices 4 uint32_t
        eNone = 0,

        eP3,
        eP3C4,
        eP3C4T2,

        eP3N3,
        eP3N3C4,
        eP3N3C4T2,

        eP3T2,
        eP3T2C4,
        eP3T2C4UV2,

        eP3N3T2,
        eP3N3T2C4,
        eP3N3T2C4UV2,

        eSkinnedP3N3T4UV2W4I4,
        eOverlayP3N3I4C4,

        eCount
    };

    // ----------------- Vertex structs -----------------

    struct VertexP3 {
        glm::vec3 position;
    };

    struct VertexP3C4 {
        glm::vec3 position;
        glm::vec4 color;
    };

    struct VertexP3C4T2 {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 uv0;
    };

    struct VertexP3N3 {
        float position[3];
        float normal[3];
    };

	struct VertexP3N3C4 {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
	};

    struct VertexP3N3C4T2 {
        glm::vec3   position;
        glm::vec3   normal;
        glm::vec4 color;
        glm::vec2   uv0;
    };

    struct VertexP3T2 {
        glm::vec3 position;
        glm::vec2 uv0;
    };

    struct VertexP3T2C4 {
        glm::vec3   position;
        glm::vec2   uv0;
        glm::vec4 color;
    };

    struct VertexP3T2C4UV2 {
        glm::vec3   position;
        glm::vec2   uv0;
        glm::vec4 color;
        glm::vec2   uv1;
    };

    struct VertexP3N3T2 {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv0;
    };

    struct VertexP3N3T2C4 {
        glm::vec3   position;
        glm::vec3   normal;
        glm::vec2   uv0;
        glm::vec4 color;
    };

    struct VertexP3N3T2C4UV2 {
        glm::vec3   position;
        glm::vec3   normal;
        glm::vec2   uv0;
        glm::vec4 color;
        glm::vec2   uv1;
    };

    struct VertexSkinnedP3N3T4UV2W4I4 {
        glm::vec3    position;
        glm::vec3    normal;
        glm::vec4    tangent;
        glm::vec2    uv0;
        glm::vec4    weights;
        uint32_t indices[4];
    };

    struct VertexOverlayP3N3 {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct VertexOverlayInstanceM4C4 {
        glm::mat4 model;
        glm::vec4 color;
    };

    static_assert(std::is_standard_layout_v<VertexP3>);
    static_assert(std::is_standard_layout_v<VertexSkinnedP3N3T4UV2W4I4>);

    struct VertexLayoutDesc {
        std::vector<vk::VertexInputBindingDescription>   bindingdescs_;
        std::vector<vk::VertexInputAttributeDescription> attributedescs_;
    };

    class VertexLayoutRegistry {
    private:
        std::array<VertexLayoutDesc, static_cast<size_t>(VertexFormat::eCount)> layouts_;

    public:
        VertexLayoutRegistry() {
            // eNone intentionally left empty

            // ---------- eP3 ----------
            layouts_[size_t(VertexFormat::eP3)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3, position)}
            };

            // ---------- eP3C4 ----------
            layouts_[size_t(VertexFormat::eP3C4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3C4), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3C4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3C4, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3C4, color)}
            };

            // ---------- eP3C4T2 ----------
            layouts_[size_t(VertexFormat::eP3C4T2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3C4T2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3C4T2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3C4T2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3C4T2, color)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3C4T2, uv0)}
            };

            // ---------- eP3N3 ----------
            layouts_[size_t(VertexFormat::eP3N3)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3, normal)}
            };

            // ---------- eP3N3C4 ----------
            layouts_[size_t(VertexFormat::eP3N3C4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3C4), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3C4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3C4, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3C4, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3N3C4, color)}
            };

            // ---------- eP3N3C4T2 ----------
            layouts_[size_t(VertexFormat::eP3N3C4T2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3C4T2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3C4T2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3C4T2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3C4T2, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3N3C4T2, color)},
                vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3N3C4T2, uv0)}
            };

            // ---------- eP3T2 ----------
            layouts_[size_t(VertexFormat::eP3T2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3T2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3T2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3T2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3T2, uv0)}
            };

            // ---------- eP3T2C4 ----------
            layouts_[size_t(VertexFormat::eP3T2C4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3T2C4), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3T2C4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3T2C4, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3T2C4, uv0)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3T2C4, color)}
            };

            // ---------- eP3T2C4UV2 ----------
            layouts_[size_t(VertexFormat::eP3T2C4UV2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3T2C4UV2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3T2C4UV2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3T2C4UV2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3T2C4UV2, uv0)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3T2C4UV2, color)},
                vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3T2C4UV2, uv1)}
            };

            // ---------- eP3N3T2 ----------
            layouts_[size_t(VertexFormat::eP3N3T2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3T2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3T2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3N3T2, uv0)}
            };

            // ---------- eP3N3T2C4 ----------
            layouts_[size_t(VertexFormat::eP3N3T2C4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3T2C4), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3T2C4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2C4, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2C4, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3N3T2C4, uv0)},
                vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3N3T2C4, color)}
            };

            // ---------- eP3N3T2C4UV2 ----------
            layouts_[size_t(VertexFormat::eP3N3T2C4UV2)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexP3N3T2C4UV2), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eP3N3T2C4UV2)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2C4UV2, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexP3N3T2C4UV2, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3N3T2C4UV2, uv0)},
                vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32B32A32Sfloat,   offsetof(VertexP3N3T2C4UV2, color)},
                vk::VertexInputAttributeDescription{4, 0, vk::Format::eR32G32Sfloat,    offsetof(VertexP3N3T2C4UV2, uv1)}
            };

            // ---------- eSkinnedP3N3T4UV2W4I4 ----------
            // Naming note: enum says UV2 (two floats) but semantically it's the primary UV set (uv0).
            layouts_[size_t(VertexFormat::eSkinnedP3N3T4UV2W4I4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexSkinnedP3N3T4UV2W4I4), vk::VertexInputRate::eVertex}
            };
            layouts_[size_t(VertexFormat::eSkinnedP3N3T4UV2W4I4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat,    offsetof(VertexSkinnedP3N3T4UV2W4I4, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat,    offsetof(VertexSkinnedP3N3T4UV2W4I4, normal)},
                vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexSkinnedP3N3T4UV2W4I4, tangent)},
                vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32Sfloat,       offsetof(VertexSkinnedP3N3T4UV2W4I4, uv0)},
                vk::VertexInputAttributeDescription{4, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexSkinnedP3N3T4UV2W4I4, weights)},
                vk::VertexInputAttributeDescription{5, 0, vk::Format::eR32G32B32A32Uint,   offsetof(VertexSkinnedP3N3T4UV2W4I4, indices)}
            };

            // ---------- eOverlayP3N3I4C4 ----------
            layouts_[size_t(VertexFormat::eOverlayP3N3I4C4)].bindingdescs_ = {
                vk::VertexInputBindingDescription{0, sizeof(VertexOverlayP3N3), vk::VertexInputRate::eVertex},
                vk::VertexInputBindingDescription{1, sizeof(VertexOverlayInstanceM4C4), vk::VertexInputRate::eInstance}
            };
            layouts_[size_t(VertexFormat::eOverlayP3N3I4C4)].attributedescs_ = {
                vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexOverlayP3N3, position)},
                vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(VertexOverlayP3N3, normal)},
                vk::VertexInputAttributeDescription{2, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexOverlayInstanceM4C4, model) + sizeof(glm::vec4) * 0},
                vk::VertexInputAttributeDescription{3, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexOverlayInstanceM4C4, model) + sizeof(glm::vec4) * 1},
                vk::VertexInputAttributeDescription{4, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexOverlayInstanceM4C4, model) + sizeof(glm::vec4) * 2},
                vk::VertexInputAttributeDescription{5, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexOverlayInstanceM4C4, model) + sizeof(glm::vec4) * 3},
                vk::VertexInputAttributeDescription{6, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexOverlayInstanceM4C4, color)}
            };
        }

        static const VertexLayoutRegistry& instance() {
            static VertexLayoutRegistry inst;
            return inst;
        }

        static const VertexLayoutDesc& getLayoutDesc(VertexFormat fmt) {
            return instance().layouts_.at(static_cast<size_t>(fmt));
        }
    };
}
