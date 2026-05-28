#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace EG {

    class Log {
    public:
        static void init();
        static std::shared_ptr<spdlog::logger>& core();
        static std::shared_ptr<spdlog::logger>& gfx();   // Vulkan/Render 专用也行

    private:
        static std::shared_ptr<spdlog::logger> s_core;
        static std::shared_ptr<spdlog::logger> s_gfx;
    };

} // namespace EG

// ---- Macros ----
#define EG_TRACE(...) ::EG::Log::core()->trace(__VA_ARGS__)
#define EG_INFO(...)  ::EG::Log::core()->info(__VA_ARGS__)
#define EG_WARN(...)  ::EG::Log::core()->warn(__VA_ARGS__)
#define EG_ERROR(...) ::EG::Log::core()->error(__VA_ARGS__)
#define EG_CRITICAL(...) ::EG::Log::core()->critical(__VA_ARGS__)

// 渲染/GFX 子系统（可选）
#define EG_GFX_TRACE(...) ::EG::Log::gfx()->trace(__VA_ARGS__)
#define EG_GFX_INFO(...)  ::EG::Log::gfx()->info(__VA_ARGS__)
#define EG_GFX_WARN(...)  ::EG::Log::gfx()->warn(__VA_ARGS__)
#define EG_GFX_ERROR(...) ::EG::Log::gfx()->error(__VA_ARGS__)
#define EG_GFX_CRITICAL(...) ::EG::Log::gfx()->critical(__VA_ARGS__)