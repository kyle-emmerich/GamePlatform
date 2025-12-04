

template<typename T>
struct NumberSequenceKeypoint {
    T Time;
    T Value;
    T Envelope;

    NumberSequenceKeypoint() = default;
    NumberSequenceKeypoint(T time, T value, T envelope = 0) : Time(time), Value(value), Envelope(envelope) {}
};

template<typename T>
struct NumberSequence {
    std::vector<NumberSequenceKeypoint<T>> Keypoints;

    NumberSequence() = default;
    NumberSequence(const std::vector<NumberSequenceKeypoint<T>>& keypoints) : Keypoints(keypoints) {}

    T GetValueAt(T time) const {
        if (Keypoints.empty()) return T(0);
        if (time <= Keypoints.front().Time) return Keypoints.front().Value;
        if (time >= Keypoints.back().Time) return Keypoints.back().Value;

        for (size_t i = 0; i < Keypoints.size() - 1; ++i) {
            const auto& kp1 = Keypoints[i];
            const auto& kp2 = Keypoints[i + 1];
            if (time >= kp1.Time && time <= kp2.Time) {
                T t = (time - kp1.Time) / (kp2.Time - kp1.Time);
                return kp1.Value + t * (kp2.Value - kp1.Value);
            }
        }
        return T(0); // Should not reach here
    }
};