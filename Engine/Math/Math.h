#pragma once

#include <vector>

namespace Math {
	template<typename T>
	static int MaxOf(const std::vector<T>& vals) {
		int idx = -1;
		T best = -std::numeric_limits<T>::infinity();
		for (int i = 0; i < static_cast<int>(vals.size()); ++i) {
			const T a = std::abs(vals[i]);
			if (a > best) {
				best = a;
				idx = i;
			}
		}
		return idx;
	}

	template<typename T>
	static int MinOf(const std::vector<T>& vals) {
		int idx = -1;
		T best = std::numeric_limits<T>::infinity();
		for (int i = 0; i < static_cast<int>(vals.size()); ++i) {
			const T a = std::abs(vals[i]);
			if (a < best) {
				best = a;
				idx = i;
			}
		}
		return idx;
	}
}