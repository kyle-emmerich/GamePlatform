#pragma once
#include <typeinfo>
#include <memory>
#include "Instance/Reflection.h"

class System;
namespace Reflection {
	inline ::Instance* instantiate_{{className}}(Engine* engine);
	inline ::std::shared_ptr<::Instance> instantiateShared_{{className}}(Engine* engine);


	//Class runtime reflection data
	inline static Class reflected_{{className}} = {
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
		::std::function<::Instance * (Engine*)>(&instantiate_{{className}}),
		::std::function<::std::shared_ptr<::Instance>(Engine*)>(&instantiateShared_{{className}}),
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
	static Reflection::Class& StaticClass() { \
		 return Reflection::reflected_{{className}}; \
	} \
	struct ClassRegistrar { \
		template<typename T> \
		ClassRegistrar(T* inst) { \
			Reflection::TrySetClass(inst, &StaticClass()); \
		} \
	} _classRegistrar{this}; \
public: \
{{memberIds}} \
{{raisePropChangedMethod}} \
private: \

#undef REFLECTION_END

//Macro that goes after the class definition, at the end of any header with reflected objects.
#define REFLECTION_END() \
namespace Reflection { \
{{wrapperFunctions}} \
	inline void register_{{className}}() { \
		Reflection::registry.classes["{{className}}"] = &Reflection::reflected_{{className}}; \
		Reflection::registry.classesById[{{classId}}] = &Reflection::reflected_{{className}}; \
{{propResolvers}} \
	} \
{{instantiateFunctions}} \
} \

