// Test for Command System
#include "CommandSystem.hpp"
#include <cassert>
#include <iostream>

void testCommandSystem() {
    std::cout << "Testing CommandSystem...\n";
    
    // Create command system
    auto commandSystem = std::make_shared<nlm::CommandSystem>();
    
    // Test basic commands
    assert(commandSystem->getAllCommands().size() > 0);
    
    // Test command execution
    auto result = commandSystem->executeCommand("status");
    assert(std::holds_alternative<std::string>(result));
    
    std::cout << "CommandSystem tests passed.\n";
}

void testCommandRegistry() {
    std::cout << "Testing CommandRegistry...\n";
    
    auto factory = std::make_shared<nlm::AdvancedCommandFactory>();
    auto registry = std::make_shared<nlm::DefaultCommandRegistry>(factory);
    
    // Test registration
    auto command = std::make_shared<nlm::Command>();
    command->setName("test_command");
    registry->registerCommand(command);
    
    assert(registry->getCommand("test_command") != nullptr);
    
    std::cout << "CommandRegistry tests passed.\n";
}

int main() {
    try {
        testCommandSystem();
        testCommandRegistry();
        std::cout << "All tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
