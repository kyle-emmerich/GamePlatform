#pragma once
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <cstdio>
#include <chrono>
#include <functional>

struct EngineUUID {
	static const int SIZE = 16; // UUID size in bytes

	uint8_t bytes[SIZE];
	EngineUUID() {
		using namespace std::chrono;

		const std::uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		std::random_device device;
		std::mt19937_64 generator((static_cast<std::uint64_t>(device()) << 32) ^ device());
		std::uniform_int_distribution<std::uint64_t> distribution;
		std::uint64_t randomA = distribution(generator);
		std::uint64_t randomB = distribution(generator);

		bytes[0] = static_cast<uint8_t>((ms >> 40) & 0xFF);
		bytes[1] = static_cast<uint8_t>((ms >> 32) & 0xFF);
		bytes[2] = static_cast<uint8_t>((ms >> 24) & 0xFF);
		bytes[3] = static_cast<uint8_t>((ms >> 16) & 0xFF);
		bytes[4] = static_cast<uint8_t>((ms >> 8) & 0xFF);
		bytes[5] = static_cast<uint8_t>((ms >> 0) & 0xFF);

		bytes[6] = static_cast<uint8_t>(0x70 | (randomA & 0x0F)); // Version 7 UUID
		randomA >>= 4;

		bytes[7] = static_cast<uint8_t>(randomA & 0xFF);
		randomA >>= 8;

		bytes[8] = static_cast<uint8_t>(0x80 | (randomB & 0x3F)); // Variant 1 UUID
		randomA >>= 6;

		for (int i = 9; i < SIZE; ++i) {
			if (randomA) {
				bytes[i] = static_cast<std::uint8_t>(randomA & 0xFF);
				randomA >>= 8;
			} else {
				bytes[i] = static_cast<std::uint8_t>(randomB & 0xFF);
				randomB >>= 8;
			}
		}
	}
	EngineUUID(const uint8_t* data) {
		for (int i = 0; i < SIZE; ++i) {
			bytes[i] = data[i];
		}
	}
	EngineUUID(const EngineUUID& other) {
		for (int i = 0; i < SIZE; ++i) {
			bytes[i] = other.bytes[i];
		}
	}

	bool operator==(const EngineUUID& other) const {
		for (int i = 0; i < SIZE; ++i) {
			if (bytes[i] != other.bytes[i]) return false;
		}
		return true;
	}

	bool operator!=(const EngineUUID& other) const {
		return !(*this == other);
	}

	//Convert UUID to UTF-8 string
	std::string ToString() const {
		char buffer[37];
		std::snprintf(buffer, sizeof(buffer),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5],
			bytes[6], bytes[7],
			bytes[8], bytes[9],
			bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
		return std::string(buffer);
	}
};

using uuid_bytes = std::array<std::uint8_t, 16>;

namespace std {
	template<> struct hash<EngineUUID> {
		size_t operator()(const EngineUUID& uuid) const {
			size_t hash = 0;
			// FNV-1a hash
			hash = 14695981039346656037ULL;
			for (int i = 0; i < EngineUUID::SIZE; ++i) {
				hash ^= uuid.bytes[i];
				hash *= 1099511628211ULL;
			}
			return hash;
		}
	};
}