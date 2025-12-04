#pragma once

#include "Math/pcg/pcg_random.hpp"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include <cinttypes>

class Random {
private:
	pcg64 generator;
public:
	Random() = default;
	Random(uint64_t seed) : generator(seed) {}
	Random(uint64_t seed, uint64_t seq) : generator(seed, seq) {}

	uint64_t NextUInt64() {
		return generator();
	}
	int64_t NextInt64() {
		return static_cast<int64_t>(generator());
	}

	float NextFloat() {
		return static_cast<float>(generator()) / static_cast<float>(std::numeric_limits<uint32_t>::max());
	}
	double NextDouble() {
		return static_cast<double>(generator()) / static_cast<double>(std::numeric_limits<uint32_t>::max());
	}

	template<typename T>
	T Next() {
		//specialize me!
		return T(0.0);
	}
	template<>
	uint8_t Next<uint8_t>() {
		return static_cast<uint8_t>(NextUInt64() & 0xFF);
	}
	template<>
	uint16_t Next<uint16_t>() {
		return static_cast<uint16_t>(NextUInt64() & 0xFFFF);
	}
	template<>
	uint32_t Next<uint32_t>() {
		return static_cast<uint32_t>(NextUInt64() & 0xFFFFFFFF);
	}
	template<>
	uint64_t Next<uint64_t>() {
		return NextUInt64();
	}
	template<>
	int8_t Next<int8_t>() {
		return static_cast<int8_t>(NextUInt64() & 0xFF);
	}
	template<>
	int16_t Next<int16_t>() {
		return static_cast<int16_t>(NextUInt64() & 0xFFFF);
	}
	template<>
	int32_t Next<int32_t>() {
		return static_cast<int32_t>(NextUInt64() & 0xFFFFFFFF);
	}
	template<>
	int64_t Next<int64_t>() {
		return NextInt64();
	}

	template<>
	float Next<float>() {
		return NextFloat();
	}
	template<>
	double Next<double>() {
		return NextDouble();
	}

	template<typename T>
	T Next(T upperBound) {
		return Next() % upperBound;
	}
	template<typename T>
	T Next(T lowerBound, T upperBound) {
		return lowerBound + (Next() % (upperBound - lowerBound));
	}

	template<typename T>
	T NextUnitVector2() {
		float angle = Next<T>() * 2.0 * 3.14159265358979323846;
		return Vector2(std::cos(angle), std::sin(angle));
	}
	template<typename T>
	T NextUnitVector3() {
		float z = Next<T>(-1.0, 1.0);
		float r = std::sqrt(1.0 - z * z);
		float angle = Next<T>() * 2.0 * 3.14159265358979323846;
		float x = r * std::cos(angle);
		float y = r * std::sin(angle);
		return Vector3(x, y, z);
	}

};