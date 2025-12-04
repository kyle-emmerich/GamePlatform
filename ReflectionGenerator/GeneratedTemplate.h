#pragma once
#include <typeinfo>
#include <memory>
#include <stdexcept>
#include "Instance/Reflection.h"

namespace Engine {
	class System;
	class Instance;
	class Engine;
}

namespace Engine::Reflection {
	inline Engine::Instance* instantiate_{{classSanitizedName}}(Engine::Engine* engine);
	inline ::std::shared_ptr<Engine::Instance> instantiateShared_{{classSanitizedName}}(Engine::Engine* engine);


	//Class runtime reflection data
	inline static Class reflected_{{classSanitizedName}} = {
		//Class name
		"{{className}}",
		//Class hash id
		{{classId}},
		{
			//	Name, Type, Id, Flags, TypeFlags, Setter, Getter
{{propDefs}}
		},
		{
			//events
{{eventDefs}}
		},
		{
			//methods
{{methodDefs}}
		},
		::std::function<Engine::Instance * (Engine::Engine*)>(&instantiate_{{classSanitizedName}}),
		::std::function<::std::shared_ptr<Engine::Instance>(Engine::Engine*)>(&instantiateShared_{{classSanitizedName}}),
		{
			//public base classes
{{publicBases}}
		},
		{
			//protected base classes
{{protectedBases}}
		},
		{
			//private base classes
{{privateBases}}
		},
		{
			//derived classes
{{derivedClasses}}
		}
	};
}

#undef REFLECTION

//Class body macro (contains property hash ids)
#define REFLECTION() \
	using Base = {{mainBaseClassName}}; \
public:\
	const static std::string& ClassName() { \
		static std::string _className = "{{className}}"; \
		return _className; \
	} \
	static Engine::Reflection::Class& StaticClass() { \
		 return Engine::Reflection::reflected_{{classSanitizedName}}; \
	} \
	struct ClassRegistrar { \
		template<typename T> \
		ClassRegistrar(T* inst) { \
			Engine::Reflection::TrySetClass(inst, &StaticClass()); \
		} \
	} _classRegistrar{this}; \
public: \
{{memberIds}} \
{{raisePropChangedMethod}} \
private: \

#undef REFLECTION_END

//Macro that goes after the class definition, at the end of any header with reflected objects.
#define REFLECTION_END() \
namespace Engine::Reflection { \
{{wrapperFunctions}} \
	inline void register_{{classSanitizedName}}() { \
		Engine::Reflection::registry.classes["{{className}}"] = &Engine::Reflection::reflected_{{classSanitizedName}}; \
		Engine::Reflection::registry.classesById[{{classId}}] = &Engine::Reflection::reflected_{{classSanitizedName}}; \
{{propResolvers}} \
	} \
{{instantiateFunctions}} \
} \

