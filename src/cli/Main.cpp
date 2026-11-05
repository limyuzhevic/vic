// Main.cpp - Entry point for the new nlm command
#include "CLI.hpp"
#include <iostream>
#include <cstring>

using namespace nlm;

int main(int argc, char** argv) {
    // Check if we're running as the legacy demo
    if (argc >= 2 && strcmp(argv[1], "--legacy") == 0) {
        // Launch legacy demo executable
        // For demonstration, we'll just pass arguments to existing main.cpp
        // In real implementation, this would launch the legacy demo
        return 0;
    }
    
    // Create CLI instance
    CLI cli;
    
    // Run CLI
    if (!cli.run(argc, argv)) {
        std::cerr << "Error: Failed to execute command" << std::endl;
        return 1;
    }
    
    return 0;
}