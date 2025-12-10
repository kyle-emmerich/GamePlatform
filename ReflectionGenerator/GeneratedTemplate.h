#pragma once
#include <typeinfo>
#include <memory>
#include <stdexcept>
#include "Instance/Reflection.h"

class System;
class Instance;
class Engine;


namespace Reflection {
	inline Instance* instantiate_{{classSanitizedName}}(Engine* engine);
	inline ::std::shared_ptr<Instance> instantiateShared_{{classSanitizedName}}(Engine* engine);


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
		::std::function<Instance * (Engine*)>(&instantiate_{{classSanitizedName}}),
		::std::function<::std::shared_ptr<Instance>(Engine*)>(&instantiateShared_{{classSanitizedName}}),
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
		 return Reflection::reflected_{{classSanitizedName}}; \
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
	inline void register_{{classSanitizedName}}() { \
		Reflection::GetRegistry().classes.insert({ "{{className}}", &Reflection::reflected_{{classSanitizedName}} }); \
		Reflection::GetRegistry().classesById.insert({ {{classId}}, &Reflection::reflected_{{classSanitizedName}} }); \
{{propResolvers}} \
	} \
{{instantiateFunctions}} \
} \

