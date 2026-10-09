// Command demo application
#include "CommandSystem.hpp"
#include "NLMUI.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM Advanced Command System Demo ===\n\n";
    
    // Create NLM UI
    auto ui = std::make_shared<nlm::NLMUI>();
    
    // Start UI
    ui->start();
    
    std::cout << "NLM Command System started.\n";
    std::cout << "Available commands:\n";
    
    // Show available commands
    auto commandSystem = ui->getCommandSystem();
    if (commandSystem) {
        auto commands = commandSystem->getAllCommands();
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd << "\n";
        }
    }
    
    std::cout << "\nDemo commands:\n";
    
    // Execute demo commands
    std::vector<std::string> demoCommands = {
        "status",
        "metrics --detailed",
        "help",
        "help monitoring"
    };
    
    for (const auto& cmd : demoCommands) {
        std::cout << "\nExecuting: " << cmd << "\n";
        std::cout << ui->executeCommand(cmd) << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout << "\n=== Demo Complete ===\n";
    
    // Stop UI
    ui->stop();
    
    return 0;
}
