#pragma once

#include <vector>
#include "Math/Color.h"

struct ColorSequenceKeypoint {
    float Time;
    Color Value;
    
    ColorSequenceKeypoint() = default;
    ColorSequenceKeypoint(float time, const Color& value) : Time(time), Value(value) {}
};

struct ColorSequence {
    std::vector<ColorSequenceKeypoint> Keypoints;

    ColorSequence() = default;
    ColorSequence(const std::vector<ColorSequenceKeypoint>& keypoints) : Keypoints(keypoints) {}

    Color GetValueAt(float time) const {
        if (Keypoints.empty()) return Color(0, 0, 0, 1);
        if (time <= Keypoints.front().Time) return Keypoints.front().Value;
        if (time >= Keypoints.back().Time) return Keypoints.back().Value;

        for (size_t i = 0; i < Keypoints.size() - 1; ++i) {
            const auto& kp1 = Keypoints[i];
            const auto& kp2 = Keypoints[i + 1];
            if (time >= kp1.Time && time <= kp2.Time) {
                float t = (time - kp1.Time) / (kp2.Time - kp1.Time);
                return Color(
                    kp1.Value.R + t * (kp2.Value.R - kp1.Value.R),
                    kp1.Value.G + t * (kp2.Value.G - kp1.Value.G),
                    kp1.Value.B + t * (kp2.Value.B - kp1.Value.B),
                    kp1.Value.A + t * (kp2.Value.A - kp1.Value.A)
                );
            }
        }
        return Color(0, 0, 0, 1); // Should not reach here
    }
};