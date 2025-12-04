#include <iostream>
#include <iomanip>
using namespace std;

#include "Instance/Instance.h"

//This must be the LAST thing included.
#include "ReflectionRegistry.h"

int main() {
	Reflection::registerAllClasses();

	for (auto& classEntry : Reflection::registry.classes) {
		cout << "Found class \"" << classEntry.first << "\":" << endl;
		for (auto& propEntry : classEntry.second->properties) {
			cout << "\t" << propEntry.type 
				<< " " << propEntry.name 
				<< "; setter (" << (propEntry.setter != nullptr ? "yes" : "no")
				<< "), getter (" << (propEntry.getter != nullptr ? "yes" : "no")
				<< ")" << endl;
		}
	}

	cin.get();
	return 0;
}