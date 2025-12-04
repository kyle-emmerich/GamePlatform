#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include <vector>

template<typename VectorType, typename T>
struct SplinePoint {
	VectorType position;
	VectorType tangentIn;
	VectorType tangentOut;
	SplinePoint()
		: position(VectorType()), tangentIn(VectorType()), tangentOut(VectorType()) {}
	SplinePoint(const VectorType& pos, const VectorType& tanIn, const VectorType& tanOut)
		: position(pos), tangentIn(tanIn), tangentOut(tanOut) {}
};

template<typename VectorType, typename T>
class Spline {
public:
	std::vector<SplinePoint<VectorType, T>> points;

	VectorType<T> Sample(T t) const {
		if (points.empty()) return VectorType<T>();
		// Clamp t to [0, 1]
		if (t <= 0.0f) return points.front().position;
		if (t >= 1.0f) return points.back().position;
		// Scale t to the number of segments
		T scaledT = t * (points.size() - 1);
		int segment = static_cast<int>(scaledT);
		T localT = scaledT - segment;
		const SplinePoint<VectorType<T>>& p0 = points[segment];
		const SplinePoint<VectorType<T>>& p1 = points[segment + 1];
		// Hermite spline interpolation
		VectorType<T> h00 = (2 * localT * localT * localT) - (3 * localT * localT) + VectorType<T>(1, 0, 0);
		VectorType<T> h10 = (localT * localT * localT) - (2 * localT * localT) + localT;
		VectorType<T> h01 = (-2 * localT * localT * localT) + (3 * localT * localT);
		VectorType<T> h11 = (localT * localT * localT) - (localT * localT);
		return (h00 * p0.position) + (h10 * p0.tangentOut) + (h01 * p1.position) + (h11 * p1.tangentIn);
	}
	VectorType<T> operator[](T t) const {
		return Sample(t);
	}

	T EstimateArcLength(int samplesPerSegment = 10) {
		if (points.size() < 2) return 0;
		T length = 0;
		for (size_t i = 0; i < points.size() - 1; ++i) {
			const SplinePoint<VectorType<T>>& p0 = points[i];
			const SplinePoint<VectorType<T>>& p1 = points[i + 1];
			VectorType<T> prevPoint = p0.position;
			for (int j = 1; j <= samplesPerSegment; ++j) {
				float t = static_cast<float>(j) / samplesPerSegment;
				VectorType<T> currPoint = GetPointAt((i + t) / (points.size() - 1));
				length += (currPoint - prevPoint).Length();
				prevPoint = currPoint;
			}
		}
		return length;
	}
	T ExactArcLength() {
		if (points.size() < 2) return 0;
		T length = 0;
		for (size_t i = 0; i < points.size() - 1; ++i) {
			length += exactArcLengthBetweenPoints(points[i], points[i + 1]);
		}
		return length;
	}

private:
    T exactArcLengthBetweenPoints(SplinePoint<VectorType, T> a, SplinePoint<VectorType, T> b) {
        const VectorType<T> c0 = a.tangentOut;
        const VectorType<T> c1 = (b.position - a.position) * T(6) - a.tangentOut * T(4) - b.tangentIn * T(2);
        const VectorType<T> c2 = (a.position - b.position) * T(6) + (a.tangentOut + b.tangentIn) * T(3);

        constexpr auto evaluateDerivative = [c0, c1, c2](T t) -> VectorType<T> {
            return c0 + (c1 + (c2 * t)) * t;
        };

        struct GaussLegendreCoefficient
        {
            float abscissa; // xi in [-1, 1]
            float weight;   // wi
        };

        static const GaussLegendreCoefficient coeffs[] =
        {
            {  0.0f,        0.5688889f  },
            { -0.5384693f,  0.47862867f },
            {  0.5384693f,  0.47862867f },
            { -0.90617985f, 0.23692688f },
            {  0.90617985f, 0.23692688f }
        };

        T length = T(0);
        for (const auto& c : coeffs)
        {
            const T t = T(0.5) * (T(1) + T(c.abscissa)); // map [-1,1] -> [0,1]
            length += evaluateDerivative(t).Length() * T(c.weight);
        }
        return T(0.5) * length; // account for the interval scaling
    }
};