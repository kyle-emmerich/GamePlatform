from typing import Generator
import tree_sitter_cpp as tscpp
from tree_sitter import Language, Parser, Tree, Node
import os
from pathlib import Path
import json
from json import JSONEncoder
import sys
from datetime import datetime

CPP = Language(tscpp.language())
parser = Parser(CPP)

def fnv1a_64(data):
    # FNV-1a 64-bit parameters
    FNV_prime_64 = 0x100000001b3  # 1099511628211
    FNV_offset_basis_64 = 0xcbf29ce484222325  # 14695981039346656037

    hash_value = FNV_offset_basis_64

    for byte in data.encode('utf-8'):
        hash_value ^= byte
        hash_value *= FNV_prime_64
        # Ensure the hash value remains within 64 bits (unsigned)
        hash_value &= 0xFFFFFFFFFFFFFFFF 

    return hash_value

ignored_attributes = [ "argNames", "reflect" ]

class ReflectedProperty:
	def __init__(self, parent, prop_type, name, flags, attributes):
		self.type = "property"
		self.parent = parent
		self.prop_type = prop_type
		self.name = name
		self.flags = flags
		self.hash = fnv1a_64(f"{self.type}:{self.parent.name}.{self.name}")

		self.setter = "nullptr"
		self.getter = "nullptr"

		self.attributes = dict()
		for key in attributes:
			if len(attributes[key]) == 1 and not key in ignored_attributes:
				self.attributes[key] = attributes[key][0]

	def has_flag(self, flag):
		return flag in self.flags
	def generate_default_getter_setter(self):
		#let's check if we HAVE a getter and setter
		setter = None
		setter_wrapper = None
		getter = None
		getter_wrapper = None

		for method in self.parent.methods:
			if method.get_associated_prop() == self:
				if method.is_setter:
					setter = method
				elif method.is_getter:
					getter = method
				if getter and setter:
					break

		if not setter:
			if "ReadOnly" in self.flags:
				setter_wrapper = f"/*ReadOnly setter*/ inline void wrap_{self.parent.name}_Set{self.name}(void* obj, void* value) {{ throw std::exception(\"Cannot set {self.parent.name}.{self.name}\"); }}"
			else:
				setter_wrapper = f"inline void wrap_{self.parent.name}_Set{self.name}(void* obj, void* value) {{ reinterpret_cast<{self.parent.name}*>(obj)->{self.name} = *reinterpret_cast<{self.prop_type}*>(value); }}"
		if not getter:
			getter_wrapper = f"inline void* wrap_{self.parent.name}_Get{self.name}(void* obj) {{ {self.prop_type}* value = &(reinterpret_cast<{self.parent.name}*>(obj)->{self.name}); return reinterpret_cast<void*>(value); }}"
		return setter_wrapper, getter_wrapper

	def __repr__(self):
		return f"ReflectedProperty({self.prop_type}, {self.name}, {self.flags})"

class ReflectedEvent:
	def __init__(self, parent, return_type, name, args, flags, attributes):
		self.type = "event"
		self.parent = parent
		self.return_type = return_type
		self.name = name
		self.args = args
		self.flags = flags
		self.hash = fnv1a_64(f"{self.type}:{self.parent.name}.{self.name}")

		self.attributes = dict()
		for key in attributes:
			if len(attributes[key]) == 1 and not key in ignored_attributes:
				self.attributes[key] = attributes[key][0]
	def has_flag(self, flag):
		return flag in self.flags

class ReflectedArgument:
	def __init__(self, arg_type, arg_name):
		self.type = arg_type
		self.name = arg_name

class ReflectedMethod:
	def __init__(self, parent, return_type, name, args, flags, attributes):
		self.type = "method"
		self.parent = parent
		self.return_type = return_type
		self.name = name
		self.args = args
		self.flags = flags
		self.hash = fnv1a_64(f"{self.type}:{self.parent.name}.{self.name}")

		self.attributes = dict()
		for key in attributes:
			if len(attributes[key]) == 1 and not key in ignored_attributes:
				self.attributes[key] = attributes[key][0]

		self.is_setter = False
		self.is_getter = False
		self.is_changed_handler = False

		self.associated_prop_name = None
		if self.name[0:3] == "Get":
			self.is_getter = True
			self.associated_prop_name = self.name[3:]
		elif self.name[0:3] == "Set":
			self.is_setter = True
			self.associated_prop_name = self.name[3:]
		elif self.name[0:2] == "On" and self.name[-7:] == "Changed":
			self.is_changed_handler = True
			self.associated_prop_name = self.name[2:-7]

	def has_flag(self, flag):
		for f in self.flags:
			if f == flag:
				return True
		return False
	def get_associated_prop(self):
		if self.is_setter or self.is_getter or self.is_changed_handler:
			return self.parent.get_prop(self.associated_prop_name)
		return None
	def get_wrapped_func_name(self):
		return f"wrap_{self.parent.name}_{self.name}"
	def generate_wrapper(self):
		prop = self.get_associated_prop()
		wrapped_func_name = self.get_wrapped_func_name()
		if prop:
			#todo: change these to take a lua_State* and just do that shit directly?
			if self.is_setter:
				return f"inline void {wrapped_func_name}(void* obj, void* value) {{ reinterpret_cast<{self.parent.name}*>(obj)->{self.name}(*reinterpret_cast<{prop.prop_type}*>(value)); }}"
			elif self.is_getter:
				#we need to handle the case of the return type being a pointer or not...
				return f"inline void* {wrapped_func_name}(void* obj) {{ {prop.prop_type} value = reinterpret_cast<{self.parent.name}*>(obj)->{self.name}(); return reinterpret_cast<void*>(&value); }}"
		else:
			#todo: how should this work, exactly? hmm... probably just gonna do straight lua_State* shit.
			pass
		return None

	def generate_prop_changed_handler_call(self):
		if not self.is_changed_handler:
			return None
		prop = self.get_associated_prop()
		if not prop:
			return None
		#this goes into a raisePropChanged() override function in the generated body macro
		#so each line needs to end with a \, and each line should start with \t\t
		return f"\t\tif (propId == prop_{prop.name}) this->{self.name}(); \\\n"

class ReflectedClass:
	def __init__(self, header, name, reflected, flags, public_base_classes, protected_base_classes, private_base_classes, derived_classes):
		self.header = header
		self.type = "class"
		self.name = name
		self.reflected = reflected
		self.flags = flags
		self.props = []
		self.methods = []
		self.events = []
		self.hash = fnv1a_64(f"{self.type}:{self.name}")

		self.public_base_classes = public_base_classes
		self.protected_base_classes = protected_base_classes
		self.private_base_classes = private_base_classes
		self.derived_classes = derived_classes

	def __repr__(self):
		return f"ReflectedClass({self.name}, flags={self.flags}, props={self.props})"
	def get_prop(self, name):
		for prop in self.props:
			if prop.name == name:
				return prop
		return None
	def resolve(self):
		#assign getters and setters if found
		for method in self.methods:
			if method.name[0:3] == "Set":
				#maybe a setter...
				prop_name = method.name[3:]
				prop = self.get_prop(prop_name)
				if prop:
					prop.setter = f"&{method.get_wrapped_func_name()}"
			elif method.name[0:3] == "Get":
				prop_name = method.name[3:]
				prop = self.get_prop(prop_name)
				if prop:
					prop.getter = f"&{method.get_wrapped_func_name()}"
	def get_primary_base(self, all_classes):
		if len(self.public_base_classes) == 0:
			return None
		first_public_base = self.public_base_classes[0]
		for other in all_classes:
			if other.name == first_public_base:
				return other
		return None


class ReflectionEncoder(JSONEncoder):
	def default(self, o):
		d = o.__dict__
		if 'parent' in d:
			del d['parent']
		return d
		

def extract_text(source_code, node):
	return source_code[node.start_byte : node.end_byte].decode("utf8")

def get_reflection_flags(node, source_code):
	for child in node.children:
		if child.type == "attribute_specifier":
			for attr in child.named_children:
				if attr.type == "attribute":
					name_node = attr.child_by_field_name("name")
					if extract_text(source_code, name_node) == "reflect":
						arg_node = attr.child_by_field_name("argument_list")
						if arg_node:
							return extract_text(source_code, arg_node)[1:-1].strip()
						else:
							return "Default"
	return None

def get_attribute(node, looking_for = "reflect"):
	if not node:
		return None, []

	attributes = get_attributes(node)
	if looking_for in attributes:
		return looking_for, attributes[looking_for]
	else:
		return None, []

def get_first_child_of_type(node, seeking_type):
	for child in node.children:
		if child.type == seeking_type:
			return child
	return None

def print_node(node, level=0):
	tabs = "\t" * level
	print(tabs, node.type, node.text.decode('utf-8'))
	for child in node.children:
		print_node(child, level + 1)


def get_attributes(node):
	attributes = dict()
	for child in node.children:
		if child.type == "attribute_declaration":
			attribute_node = get_first_child_of_type(child, "attribute")
			if attribute_node:
				identifier_node = attribute_node.child_by_field_name("name")
				attribute_name = identifier_node.text.decode('utf-8')
				argument_list = identifier_node.next_sibling
				attribute_flags = []
				if argument_list and argument_list.type == "argument_list":
					for arg in argument_list.children:
						if arg.type == "identifier":
							attribute_flags.append(arg.text.decode('utf-8'))
						elif arg.type == "string_literal":
							string_content = get_first_child_of_type(arg, "string_content")
							if string_content:
								attribute_flags.append(string_content.text.decode('utf-8'))
						#elif arg.type == "number_literal": todo: make this work with floats and such
						#	number = float(arg.text.decode('utf-8'))
						#	attribute_flags.append(number)
				attributes[attribute_name] = attribute_flags
	return attributes

def get_pointer_indirection(top_node):
	#we can technically have infinitely nested pointer_declarators... which would be dumb, but why not support it I guess
	node = top_node
	num_pointer_symbols = 0
	while node and node.type == "pointer_declarator":
		#first child is going to be * node, second will be another pointer_declarator or a field_identifier.
		next_node = node.children[1]
		num_pointer_symbols += 1
		node = next_node
		if num_pointer_symbols > 99: #who's gonna have more than 99 pointer symbols?
			raise "too many pointer symbols dude"
	return num_pointer_symbols, node

def get_type(node):
	if node.type == "type_identifier" or node.type == "primitive_type" or node.type == "template_type":
		return node.text.decode('utf-8')
	elif node.type == "qualified_identifier":
		return node.text.decode('utf-8')
	return None

def contains_function_declarator(node):
	for child in node.children:
		if child.type == "function_declarator":
			return True
		else:
			if contains_function_declarator(child):
				return True
	return False

def get_function_name(node):
	identifier_node = node.children[0]
	if identifier_node and identifier_node.type == "field_identifier":
		return identifier_node.text.decode('utf-8')

	for child in node.children:
		print("\t\t", child.type, child.text.decode('utf-8'))
	return None

def handle_function(current_class, node):
	#we might get passed a field_declaration or we might get passed a function_definition. we should handle both.
	return_type = None
	function_name = None
	if node.type == "field_declaration" or node.type == "function_definition":
		#Find the function declarator and the return type
		func_decl_node = None
		type_node = None

		for child in node.children:
			if child.type == "function_declarator":
				func_decl_node = child
				break
			elif child.type not in ["attribute_declaration", "storage_class_specifier", "virtual", "friend", "explicit", "inline", "static", "comment"]:
				if not type_node:
					type_node = child
		
		if type_node:
			return_type = get_type(type_node)
		
		if not return_type or not func_decl_node:
			print("Malformed function definition: ", node.text.decode('utf-8'))
			return

		return_type_pointer_symbols, function_name_node = get_pointer_indirection(func_decl_node)
		return_type += ("*") * return_type_pointer_symbols
		if function_name_node:
			function_name = get_function_name(function_name_node)
		else:
			print("Malformed function definition: ", node.text.decode('utf-8'))
			return
	
	print("got function node: ", return_type, function_name)
	if function_name:
		attributes = get_attributes(node)
		reflect_flags = []
		if "reflect" in attributes:
			reflect_flags = attributes["reflect"]

		method = ReflectedMethod(current_class, return_type, function_name, [], reflect_flags, attributes)
		current_class.methods.append(method)

		if "Derived" in reflect_flags and method.is_getter:
			prop_name = method.associated_prop_name
			if not current_class.get_prop(prop_name):
				prop_flags = reflect_flags.copy()
				prop_flags.remove("Derived")
				if "ReadOnly" not in prop_flags:
					prop_flags.append("ReadOnly")
				current_class.props.append(ReflectedProperty(current_class, return_type, prop_name, prop_flags, attributes))

def is_event_type(type_identifier_str):
	return type_identifier_str[0:14] == "MulticastEvent" or type_identifier_str[0:5] == "Event"

def get_template_args(type_identifier_node, attributes):
	arg_types = []
	arg_names = []
	for child in type_identifier_node.children:
		if child.type == "template_argument_list":
			for subchild in child.children:
				if subchild.type == "type_descriptor":
					idx = len(arg_types)
					arg_types.append(subchild.text.decode('utf-8'))
	if 'argNames' in attributes:
		arg_names = attributes['argNames']

	args = []
	if len(arg_types) == len(arg_names):
		for i in range(0, len(arg_types)):
			args.append(ReflectedArgument(arg_types[i], arg_names[i]))
	else:
		for i in range(0, len(arg_types)):
			args.append(ReflectedArgument(arg_types[i], "???"))
	return args
		

def handle_field(node, current_class):
	attributes = get_attributes(node)
	type_identifier, field_identifier = None, None
	type_identifier_node = None
	attribute_node = None

	for child in node.children:
		if child.type == "pointer_declarator" and type_identifier:
			num_pointer_symbols, field_identifier_node = get_pointer_indirection(child)
			type_identifier += ("*") * num_pointer_symbols
			field_identifier = field_identifier_node.text.decode('utf-8')
		elif child.type == "field_identifier":
			field_identifier = child.text.decode('utf-8')
		else:
			if not type_identifier:
				type_identifier_node = child
				type_identifier = get_type(child)
	if "reflect" in attributes and type_identifier:
		new_item = None
		if not is_event_type(type_identifier):
			current_class.props.append(ReflectedProperty(current_class, type_identifier, field_identifier, attributes["reflect"], attributes))
		else:
			event_args = get_template_args(type_identifier_node, attributes)
			current_class.events.append(ReflectedEvent(current_class, "void", field_identifier, event_args, attributes["reflect"], attributes)) 

def get_base_classes(class_specifier_node, base_class_node):
	access_spec = None
	public_base_classes = []
	protected_base_classes = []
	private_base_classes = []
	for child in base_class_node.children:
		if child.type == "access_specifier":
			access_spec = child.text.decode('utf-8')
		elif child.type == "type_identifier" or child.type == 'template_type':
			base_class_type = child.text.decode('utf-8')
			if access_spec == "public":
				public_base_classes.append(base_class_type)
			elif access_spec == "protected":
				protected_base_classes.append(base_class_type)
			elif access_spec == "private" or access_spec is None:
				private_base_classes.append(base_class_type)
			access_spec = None
	return public_base_classes, protected_base_classes, private_base_classes

def walk_tree(header_filename, tree, source_code):
	cursor = tree.walk()
	classes = []
	stack = []

	current_class = None

	attribute_name = None
	attribute_flags = []

	def enter(node):
		nonlocal current_class, attribute_name, attribute_flags
		if node.type == "class_specifier" or node.type == "struct_specifier":
			public_base_classes, protected_base_classes, private_base_classes = [], [], []
			derived_classes = []
			class_attributes = get_attributes(node)
			print("class attributes:", class_attributes)

			for child in node.children:
				if child.type == "attribute_declaration":
					attribute_name, attribute_flags = get_attribute(child)
				elif child.type == "base_class_clause":
					public_base_classes, protected_base_classes, private_base_classes = get_base_classes(node, child)

			class_name_node = node.child_by_field_name("name")
			
			if class_name_node:
				print("class name node:", class_name_node.text.decode('utf-8'))
				class_name = class_name_node.text.decode('utf-8')
				class_flags = []
				class_reflected = False

				if 'reflect' in class_attributes:
					print('class', class_name, "is reflected")
					class_reflected = True
					class_flags = class_attributes
				current_class = ReflectedClass(header_filename, class_name, class_reflected, class_flags, public_base_classes, protected_base_classes, private_base_classes, derived_classes)
				if class_reflected:
					classes.append(current_class)
		elif node.type == "field_declaration":
			if not current_class:
				return
			if not contains_function_declarator(node):
				handle_field(node, current_class)
				attribute_name = None
				attribute_flags = []
			else:
				handle_function(current_class, node)
		elif node.type == "function_definition":
			handle_function(current_class, node)
	def leave(node):
		pass

	visited_children = False
	while True:
		node = cursor.node
		if not visited_children:
			enter(node)
			if cursor.goto_first_child():
				visited_children = False
				continue
			leave(node)
		if cursor.goto_next_sibling():
			visited_children = False
			continue
		if not cursor.goto_parent():
			break
		visited_children = True

	return classes


def iterate_headers(root_dir, ignore_paths = None):
	root = Path(root_dir).resolve()
	ignore_paths = [root / Path(p) for p in (ignore_paths or [])]
	for dirpath, dirnames, filenames in os.walk(root):
		dirnames[:] = [
			d for d in dirnames
			if not any((Path(dirpath) / d).resolve().is_relative_to(ip) for ip in ignore_paths)
		]
		for filename in filenames:
			if filename.endswith((".h", ".hpp")):
				filepath = Path(dirpath) / filename
				if any(filepath.resolve().is_relative_to(ip) for ip in ignore_paths):
					continue
				if any(filename == ip for ip in ignore_paths):
					continue
				yield filepath

def generate_prop_def_text(prop_info):
	flags = ""
	if len(prop_info.flags) == 0:
		flags = "DefaultPropFlags"
	else:
		num_flags = len(prop_info.flags)
		for i in range(0, num_flags):
			if i < num_flags - 1:
				flags += f"PropFlags::{prop_info.flags[i]} | "
			else:
				flags += f"PropFlags::{prop_info.flags[i]}"

	access_level = "Public"
	minimum_context = "Client"
	return f'\t\t\t{{ "{prop_info.name}", "{prop_info.prop_type}", {prop_info.hash}, {flags}, PropTypeFlags::None, AccessLevel::{access_level}, Lua::StateContext::{minimum_context}, nullptr, nullptr }}'

def generate_prop_defs_text(class_info):
	result = ""
	num_props = len(class_info.props)
	for i in range(0, num_props):
		prop_info = class_info.props[i]
		if i < num_props - 1:
			result += generate_prop_def_text(prop_info) + ",\n"
		else:
			result += generate_prop_def_text(prop_info)
	return result
def generate_member_ids_text(class_info):
	result = ""
	for prop_info in class_info.props:
			result += f"\tstatic constexpr uint64_t prop_{prop_info.name} = {prop_info.hash}Ui64; \\\n"
	for event_info in class_info.events:
			result += f"\tstatic constexpr uint64_t event_{event_info.name} = {event_info.hash}Ui64; \\\n"
	for method_info in class_info.methods:
			result += f"\tstatic constexpr uint64_t method_{method_info.name} = {method_info.hash}Ui64; \\\n"
	return result
def generate_prop_changed_handlers_text(class_info):
	result = ""
	for method_info in class_info.methods:
		call = method_info.generate_prop_changed_handler_call()
		if call:
			result += call
	return result
def generate_event_def_text(event_info):
	event_args = ""
	num_args = len(event_info.args)
	for i in range(0, num_args):
		arg = event_info.args[i]
		if i < num_args - 1:
			event_args += f"{{ \"{arg.type}\", \"{arg.name}\" }}, "
		else:
			event_args += f"{{ \"{arg.type}\", \"{arg.name}\" }}"
	access_level = "Public"
	minimum_context = "Client"
	return f'\t\t\t{{ "{event_info.name}", {{{event_args}}}, {event_info.hash}, AccessLevel::{access_level}, Lua::StateContext::{minimum_context} }}'
def generate_event_defs_text(class_info):
	result = ""
	num_events = len(class_info.events)
	for i in range(0, num_events):
		event_info = class_info.events[i]
		if i < num_events - 1:
			result += generate_event_def_text(event_info) + ",\n"
		else:
			result += generate_event_def_text(event_info)
	return result
def generate_method_defs_text(class_info):
	return ""
def generate_class_refs_text(class_info, class_refs):
	if len(class_refs) == 0:
		return ""
	result = ""
	for i in range(0, len(class_refs)):
		class_hash = fnv1a_64(f"class:{class_refs[i]}")
		if i < len(class_refs) - 1:
			result += f"{class_hash}Ui64, "
		else:
			result += f"{class_hash}Ui64"
	return result

def generate_wrapper_functions_text(class_info):
	method_wrappers = ""
	for method in class_info.methods:
		wrapper = method.generate_wrapper()
		if wrapper:
			method_wrappers += "\t" + wrapper + " \\\n"
	
	for prop_info in class_info.props:
		setter_wrapper, getter_wrapper = prop_info.generate_default_getter_setter()
		if setter_wrapper:
			method_wrappers += "\t" + setter_wrapper + " \\\n"
		if getter_wrapper:
			method_wrappers += "\t" + getter_wrapper + " \\\n"
	return method_wrappers

def generate_prop_resolvers_text(class_info):
	prop_setter_getter_resolution = ""
	for method in class_info.methods:
		wrapper = method.generate_wrapper()
		if wrapper:
			prop_name = method.associated_prop_name
			if method.is_setter:
				prop_setter_getter_resolution += f"\t\treflected_{class_info.name}.ResolvePropSetter({class_info.name}::prop_{prop_name}, &wrap_{class_info.name}_Set{prop_name}); \\\n"
			if method.is_getter:
				prop_setter_getter_resolution += f"\t\treflected_{class_info.name}.ResolvePropGetter({class_info.name}::prop_{prop_name}, &wrap_{class_info.name}_Get{prop_name}); \\\n"
	
	for prop_info in class_info.props:
		setter_wrapper, getter_wrapper = prop_info.generate_default_getter_setter()
		if setter_wrapper:
			prop_setter_getter_resolution += f"\t\treflected_{class_info.name}.ResolvePropSetter({class_info.name}::prop_{prop_info.name}, &wrap_{class_info.name}_Set{prop_info.name}); \\\n"
		if getter_wrapper:
			prop_setter_getter_resolution += f"\t\treflected_{class_info.name}.ResolvePropGetter({class_info.name}::prop_{prop_info.name}, &wrap_{class_info.name}_Get{prop_info.name}); \\\n"
	return prop_setter_getter_resolution

def generate_instantiate_functions_text(class_info):
	if "reflect" in class_info.flags and "Interface" in class_info.flags["reflect"]:
		return f"""	inline ::Instance* instantiate_{class_info.name}(Engine* engine) {{ \\
		return nullptr; \\
	}} \\
	inline ::std::shared_ptr<::Instance> instantiateShared_{class_info.name}(Engine* engine) {{ \\
		return nullptr; \\
	}}"""
	elif "reflect" in class_info.flags and "Engine" in class_info.flags["reflect"]:
		return f"""	inline ::Instance* instantiate_{class_info.name}(Engine* engine) {{ \\
		return (::Instance*)new {class_info.name}(nullptr); \\
	}} \\
	inline ::std::shared_ptr<::Instance> instantiateShared_{class_info.name}(Engine* engine) {{ \\
		return ::std::static_pointer_cast<::Instance>(::std::make_shared<{class_info.name}>(nullptr)); \\
	}}"""
	else:
		return f"""	inline ::Instance* instantiate_{class_info.name}(Engine* engine) {{ \\
		return (::Instance*)new {class_info.name}(engine); \\
	}} \\
	inline ::std::shared_ptr<::Instance> instantiateShared_{class_info.name}(Engine* engine) {{ \\
		return ::std::static_pointer_cast<::Instance>(::std::make_shared<{class_info.name}>(engine)); \\
	}}"""

def generate_raise_prop_changed_method_text(class_info):
	if "reflect" in class_info.flags and "Interface" in class_info.flags["reflect"]:
		return "\tvoid raisePropChanged(uint64_t propId) {}"
	
	handlers = generate_prop_changed_handlers_text(class_info)
	return f"""\tvoid raisePropChanged(uint64_t propId) {{ \\
{handlers}\
		PropertyChanged.Fire(propId); \\
		if (LuaPropertyChanged.HasAnyListeners()) {{ \\
			const std::string& propName = StaticClass().GetPropName(propId); \\
			LuaPropertyChanged.Fire(propName); \\
		}} \\
	}}"""

def generate_header(template_path, target_dir, header_filename, class_info):
	output = ""
	with open(template_path, "r") as template_file:
		output = template_file.read()

		replacements = dict()
		replacements["className"] = class_info.name
		replacements["classId"] = str(class_info.hash)
		
		main_base_class = "void"
		if len(class_info.public_base_classes) > 0:
			main_base_class = class_info.public_base_classes[0]
		elif len(class_info.protected_base_classes) > 0:
			main_base_class = class_info.protected_base_classes[0]
		elif len(class_info.private_base_classes) > 0:
			main_base_class = class_info.private_base_classes[0]
		replacements["mainBaseClassName"] = main_base_class

		replacements["propDefs"] = generate_prop_defs_text(class_info)
		replacements["memberIds"] = generate_member_ids_text(class_info)
		replacements["raisePropChangedMethod"] = generate_raise_prop_changed_method_text(class_info)
		replacements["eventDefs"] = generate_event_defs_text(class_info)
		replacements["methodDefs"] = generate_method_defs_text(class_info)
		replacements["publicBases"] = generate_class_refs_text(class_info, class_info.public_base_classes)
		replacements["protectedBases"] = generate_class_refs_text(class_info, class_info.protected_base_classes)
		replacements["privateBases"] = generate_class_refs_text(class_info, class_info.private_base_classes)
		replacements["derivedClasses"] = generate_class_refs_text(class_info, class_info.derived_classes)
		replacements["wrapperFunctions"] = generate_wrapper_functions_text(class_info)
		replacements["propResolvers"] = generate_prop_resolvers_text(class_info)
		replacements["instantiateFunctions"] = generate_instantiate_functions_text(class_info)
			
		for key in replacements:
			print("replacing", key, "with", replacements[key])
			output = output.replace(f"{{{{{key}}}}}", replacements[key])
	
	base_dir, base_filename = os.path.split(header_filename)
	base_filename, base_extension = os.path.splitext(base_filename)
	generated_filename = f"{base_filename}.generated{base_extension}"
	generated_path = os.path.join(target_dir, generated_filename)
	with open(generated_path, "w") as output_file:
		output_file.write(f"//Autogenerated at {datetime.now()}\n")
		output_file.write(output)
		print(f"Wrote {generated_path}.")



if __name__ == "__main__":
	# Get the target directory from the command line args
	if len(sys.argv) < 2:
		print("Usage: python parse.py <target_directory> [<filename_filter>?]")
		sys.exit(1)
	target_directory = os.path.abspath(sys.argv[1])

	filenames = None
	if len(sys.argv) >= 3:
		filenames = sys.argv[2:]

	script_dir = os.path.dirname(os.path.abspath(__file__))
	template_path = os.path.join(script_dir, "GeneratedTemplate.h")

	print("Processing files in directory:", target_directory)
	os.chdir(target_directory)

	# Iterate over files in directory
	# Load each line of .reflectionignore into ignore_paths
	ignore_paths = []

	if os.path.exists(".reflectionignore"):
		with open(".reflectionignore", "r") as f:
			ignore_paths = [line.strip() for line in f if line.strip() and not line.startswith("#")]
	
	generated_dir = "Generated"



	all_classes = []
	with open("parsed_headers.log", "w") as header_log:
		for header in iterate_headers(".", ignore_paths):
			header_filename = os.path.relpath(header, target_directory).replace('\\', '/')

			if filenames and not any(fn in header_filename for fn in filenames):
				continue

			print(f"Parsing {header}")
			header_log.write(f"{header}\n")
			with open(header, "r") as f:
				source_code = f.read()
				# Hack to fix tree-sitter parsing of GP_EXPORT
				source_code = source_code.replace("GP_EXPORT", "")
				print(f"Source code length: {len(source_code)}")
				tree = parser.parse(bytes(source_code, "utf8"))
				classes = walk_tree(header_filename, tree, bytes(source_code, "utf8"))
				print(f"Found {len(classes)} class(es) in {header}!")
				all_classes += classes

				main_class = None
				base_path, base_filename = os.path.split(header)
				base_filename, base_extension = os.path.splitext(base_filename)
				for class_info in classes:
					class_info.resolve()
					if class_info.name == base_filename:
						main_class = class_info

				if main_class:
					generate_header(template_path, generated_dir, header, main_class)
				else:
					print(f"No main class found in {header}!")

	reflection_header_filename = os.path.join(generated_dir, "ReflectionRegistry.h")
	with open(reflection_header_filename, "w") as file:
		class_registration_str = ""
		class_includes_str = ""
		for class_info in all_classes:
			class_header_dir, class_header_file = os.path.split(class_info.header)
			class_header_base, class_header_ext = os.path.splitext(class_header_file)

			class_includes_str += f"#include \"{class_info.header}\"\n"
			class_includes_str += f"#include \"{class_header_base}.generated.h\"\n"
			class_registration_str += f"\t\tReflection::register_{class_info.name}();\n"

		file.write(f"""
{class_includes_str}

namespace Reflection {{
	static void registerAllClasses() {{
{class_registration_str}
	}}
}}
""")

	print(f"Found a total of {len(all_classes)} classes!")
	print(f"Generating reflection data json pack...")
	json_data = json.dumps(all_classes, cls=ReflectionEncoder, indent=4)

	json_pack_filename = "Generated/reflection_data.json"
	with open(json_pack_filename, "w") as file:
		file.write(json_data)
		print(f"Wrote {len(json_data)} bytes to {json_pack_filename}.")

#todo:
# generate setter/getters for props that don't have them so raisePropChanged still gets called
# enforce that props are private