#include "core/Time.hpp"

#include <algorithm>

namespace CO {

    void TimeSystem::beginFrame() {
        const auto now = Clock::now();

        if (!initialized_) {
            lastTime_ = now;
            initialized_ = true;
            rawDelta_ = 0.0f;
            deltaTime_ = 0.0f;
            smoothedDelta_ = 0.0f;
            frameIndex_ = 0;
            return;
        }

        float dt = std::chrono::duration<float>(now - lastTime_).count();
        lastTime_ = now;

        updateDelta(dt);

        totalTime_ += deltaTime_;
        frameIndex_++;
    }

    void TimeSystem::updateDelta(float dt) {
        // 防止极端卡顿（比如断点回来）
        dt = std::min(dt, maxDeltaTime_);

        rawDelta_ = dt;

        // 应用 time scale
        deltaTime_ = rawDelta_ * timeScale_;

        // 平滑处理（指数平滑）
        if (smoothingEnabled_) {
            const float alpha = 0.1f; // 越小越平滑
            smoothedDelta_ = smoothedDelta_ * (1.0f - alpha) + deltaTime_ * alpha;
        }
        else {
            smoothedDelta_ = deltaTime_;
        }
    }

    void TimeSystem::setTimeScale(float scale) {
        timeScale_ = std::max(0.0f, scale);
    }

    void TimeSystem::setMaxDeltaTime(float maxDt) {
        maxDeltaTime_ = std::max(0.001f, maxDt);
    }

    void TimeSystem::enableSmoothing(bool enable) {
        smoothingEnabled_ = enable;
    }

} // namespace EG