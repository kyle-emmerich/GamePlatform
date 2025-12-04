#pragma once

namespace Lua {
	enum class StateContext : uint8_t {
		Client = 0,
		Server = 1,
		Developer = 2,
		Editor = 3,

		Engine = 255
	};
}