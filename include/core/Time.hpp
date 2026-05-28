#pragma once

#include <chrono>
#include <cstdint>

namespace CO {

    class TimeSystem {
    public:
        using Clock = std::chrono::high_resolution_clock;

        // 每帧开始调用
        void beginFrame();

        // 基本时间
        float deltaTime() const { return deltaTime_; }        // 秒
        float unscaledDeltaTime() const { return rawDelta_; } // 未乘 timeScale

        float totalTime() const { return totalTime_; }        // 从启动到现在
        uint64_t frameIndex() const { return frameIndex_; }

        // 控制
        void setTimeScale(float scale);
        float timeScale() const { return timeScale_; }

        void setMaxDeltaTime(float maxDt); // 防止卡顿爆炸
        float maxDeltaTime() const { return maxDeltaTime_; }

        // 可选：平滑 dt（减少抖动）
        void enableSmoothing(bool enable);
        float smoothedDeltaTime() const { return smoothedDelta_; }

    private:
        void updateDelta(float dt);

    private:
        Clock::time_point lastTime_{};
        bool initialized_ = false;

        float rawDelta_ = 0.0f;      // 原始 dt
        float deltaTime_ = 0.0f;     // scale 后 dt
        float totalTime_ = 0.0f;

        float timeScale_ = 1.0f;
        float maxDeltaTime_ = 0.05f; // 默认 50ms（≈20 FPS 下限保护）

        bool smoothingEnabled_ = false;
        float smoothedDelta_ = 0.0f;

        uint64_t frameIndex_ = 0;
    };

} // namespace EG