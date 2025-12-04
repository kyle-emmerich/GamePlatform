#pragma once

#include "Scripting/LuaState.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace Lua {
	typedef double NumberType;

	struct DataType {
		std::string name;
		std::function<int(Lua::State& State)> __new;
		std::function<int(Lua::State& State)> __gc;
		std::function<int(Lua::State& State)> __index;
		std::function<int(Lua::State& State)> __newindex;
		std::function<int(Lua::State& State)> __tostring;
		std::function<int(Lua::State& State)> __add;
		std::function<int(Lua::State& State)> __sub;
		std::function<int(Lua::State& State)> __mul;
		std::function<int(Lua::State& State)> __div;
		std::function<int(Lua::State& State)> __unm;
		std::function<int(Lua::State& State)> __eq;
		std::function<int(Lua::State& State)> __lt;
		std::function<int(Lua::State& State)> __le;
		std::function<int(Lua::State& State)> __mod;
		std::function<int(Lua::State& State)> __pow;
		std::function<int(Lua::State& State)> __concat;
		std::function<int(Lua::State& State)> __call;
		std::function<int(Lua::State& State)> __metatable;

		std::unordered_map<std::string, std::function<int(Lua::State& State)>> methods;
	};


}