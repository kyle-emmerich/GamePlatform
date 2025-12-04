#include "Scripting/LuaState.h"
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
#include <iostream>

// Custom print function to redirect Lua print to C++ std::cout
static int lua_print(lua_State* L) {
	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++) {
		size_t len;
		const char* s = luaL_tolstring(L, i, &len);
		if (s) {
			std::cout.write(s, len);
			if (i < n) {
				std::cout << "\t";
			}
		}
	}
	std::cout << std::endl;
	return 0;
}

Lua::State::State(StateContext _context)
	: context(_context) {
	L = luaL_newstate();



	lua_pushstring(L, "Context");
	lua_pushinteger(L, static_cast<int>(context));
	lua_settable(L, LUA_REGISTRYINDEX);

	luaL_openlibs(L);

	lua_pushcfunction(L, lua_print, "print");
	lua_setglobal(L, "print");
}

Lua::State::State(lua_State* existing) {
	lua_pushstring(existing, "Context");
	lua_gettable(existing, LUA_REGISTRYINDEX);
	int contextValue = lua_tointeger(existing, -1);
	
	context = (StateContext)contextValue;
	L = existing;
}

Lua::State::~State() {
	lua_close(L);
}

int Lua::State::top() {
	return lua_gettop(L);
}

void Lua::State::pop(int count) {
	lua_pop(L, count);
}

Lua::StateContext Lua::State::getContext() const {
	return context;
}

bool Lua::State::Execute(const std::string& code) {
	size_t bytecodeSize = 0;
	lua_CompileOptions options = {};
	//todo: set options
	char* bytecode = luau_compile(code.c_str(), code.size(), &options, &bytecodeSize);
	int result = luau_load(L, "chunk", bytecode, bytecodeSize, 0);
	free(bytecode);

	if (result == 0) {
		result = lua_pcall(L, 0, LUA_MULTRET, 0);
		return true;
	} else {
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
		return false;
	}
}