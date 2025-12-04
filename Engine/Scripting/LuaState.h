#pragma once
#include <cinttypes>
#include <string>
#include <functional>
#include "lua.h"
#include "lualib.h"
#include "Core/Export.h"
#include "Scripting/StateContext.h"

struct lua_State;

namespace Lua {
	class State;
	struct DataType;

	template<typename T>
	void registerType(State* state) {}

	class GP_EXPORT State {
	protected:
		lua_State* L;
		StateContext context = StateContext::Client;
	public:
		State(StateContext _context);
		State(lua_State* existing);
		~State();

/*		void registerType(const Lua::DataType& dataType) {
			luaL_newmetatable(L, dataType.name.c_str());

			if (dataType.__new) {
				lua_pushstring(L, "new");
				lua_pushcfunction(L, [](lua_State* L) -> int {
					State state(L);
					return state->dataType.__new(*state);
				});
				lua_settable(L, -3);
			}

			lua_pushstring(L, "__index");
			lua_pushvalue(L, -2); // push the metatable
			lua_settable(L, -3);  // metatable.__index = metatable
			lua_pop(L, 1); // pop the metatable
		}*/

		template<typename T>
		T* newUserData() {
			T* obj = new T();
			*(T**)lua_newuserdata(L, sizeof(T*)) = obj;
			luaL_getmetatable(L, typeid(T).name());
			lua_setmetatable(L, -2);
			return obj;
		}

		int top();
		void pop(int count = 1);

		void pushFunction(std::function<int(State&)> func);

		bool Execute(const std::string& code);

		template<typename... Args>
		int error(std::string format, Args... args) {
			int size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // +1 for null terminator
			if (size <= 0) {
				luaL_error(L, "String formatting error");
				return 0;
			}
			char* buffer = new char[size];
			snprintf(buffer, size, format.c_str(), args...);
			luaL_error(L, "%s", buffer);
			// delete[] buffer; // Unreachable
			return 0;
		}

		template<typename... Args>
		int warn(std::string format, Args... args) {
			int size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // +1 for null terminator
			if (size <= 0) {
				// luaL_error(L, "String formatting error");
				return 0;
			}
			char* buffer = new char[size];
			snprintf(buffer, size, format.c_str(), args...);
			printf("LUA WARN: %s\n", buffer);
			delete[] buffer;
			return 0;
		}

		StateContext getContext() const;

		operator lua_State*() {
			return L;
		}

	};
}