
import tree_sitter_cpp as tscpp
from tree_sitter import Language, Parser

CPP = Language(tscpp.language())
parser = Parser(CPP)

code = b"""
class GP_EXPORT Engine : public Instance {
};
"""

code = code.replace(b"GP_EXPORT", b"")

tree = parser.parse(code)
root = tree.root_node
class_node = root.children[0]
print("Type:", class_node.type)
# print("Name field:", class_node.child_by_field_name("name").text.decode('utf-8'))

for child in class_node.children:
    print(f"Child type: {child.type}, text: {child.text.decode('utf-8')}")
    if child.type == "class_specifier":
        print("  Inside class_specifier:")
        print("  Name field:", child.child_by_field_name("name").text.decode('utf-8'))
        for subchild in child.children:
             print(f"  Subchild type: {subchild.type}, text: {subchild.text.decode('utf-8')}")
