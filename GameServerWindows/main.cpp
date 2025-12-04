#include "Core/Engine.h"
#include <iostream>
#include <thread>
#include <atomic>
#include "ReflectionRegistry.h"

// Simple atomic flag to control the server loop
std::atomic<bool> g_Running{ true };

// Console input handler to stop the server
void ConsoleInputThread() {
    std::cout << "Press 'q' and Enter to stop the server." << std::endl;
    while (g_Running) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "q") {
            g_Running = false;
        }
    }
}

int main(int argc, char** argv) {
    std::cout << "Starting Game Server..." << std::endl;
    
    // Register all reflection classes
    Reflection::registerAllClasses();

    Engine engine;
    engine.Initialize();
    
    // Start console input thread
    std::thread inputThread(ConsoleInputThread);

    std::cout << "Server Initialized. Running..." << std::endl;

    while (g_Running) {
        engine.Update();
        
        // Yield to prevent 100% CPU usage on an empty loop
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "Stopping Server..." << std::endl;
    engine.Shutdown();
    
    // Detach input thread as we can't interrupt getline
    inputThread.detach();

    return 0;
}
