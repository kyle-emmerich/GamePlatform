#include <iostream>

template<typename T>
class BaseOfBases {
public:

    template<typename Self>
    void Initialize(this Self&& self) {
		std::cout << "BaseOfBases Initialize" << std::endl;
        self.Initialize();
    }

};

class System {
public:
    void InitializeImpl() {}

    template<typename Self>
    void Initialize(this Self&& self) {
        std::cout << "System Initialize" << std::endl;
        self.InitializeImpl();
    }
};

class TestSystem : public System {
public:
    void InitializeImpl() override {
        std::cout << "TestSystem Initialize" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::cout <<"test"<<std::endl;

	TestSystem* testSystem = new TestSystem();

    System* systemTest = static_cast<System*>(testSystem);
    systemTest->Initialize();

    return 0;
}