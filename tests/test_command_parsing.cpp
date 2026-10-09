// Test for Command Parsing
#include "CommandParsing.hpp"
#include <cassert>
#include <iostream>

void testCommandParser() {
    std::cout << "Testing CommandParser...\n";
    
    // Test simple command
    auto parsed = nlm::CommandParser::parse("status");
    assert(parsed.commandName == "status");
    
    // Test command with arguments
    parsed = nlm::CommandParser::parse("metrics --detailed --json");
    assert(parsed.commandName == "metrics");
    assert(parsed.arguments.find("detailed") != parsed.arguments.end());
    assert(std::get<bool>(parsed.arguments.at("detailed")) == true);
    
    // Test command with key=value arguments
    parsed = nlm::CommandParser::parse("connectivity source=1 target=2 threshold=0.5");
    assert(parsed.commandName == "connectivity");
    assert(parsed.arguments.find("source") != parsed.arguments.end());
    assert(std::get<int>(parsed.arguments.at("source")) == 1);
    
    std::cout << "CommandParser tests passed.\n";
}

void testCommandValidator() {
    std::cout << "Testing CommandValidator...\n";
    
    // Create a simple command for testing
    auto command = std::make_shared<nlm::Command>();
    command->setName("test_command");
    
    // Test valid arguments
    std::map<std::string, nlm::CommandArgument> validArgs;
    validArgs["arg1"] = std::string("value1");
    validArgs["arg2"] = 42;
    validArgs["arg3"] = 3.14;
    validArgs["arg4"] = true;
    
    // Validation should pass for unknown command (no strict validation)
    assert(nlm::CommandValidator::validate(command, validArgs));
    
    std::cout << "CommandValidator tests passed.\n";
}

int main() {
    try {
        testCommandParser();
        testCommandValidator();
        std::cout << "All command parsing tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
