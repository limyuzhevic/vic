#include <iostream>
#include <string>

// Include the CommandLineInterface
#include "core/cli/CommandLineInterface.hpp"

int main() {
    std::cout << "NLM CommandLineInterface Demo" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Create CommandLineInterface instance
    nlm::CommandLineInterface cli;
    
    // Initialize with minimal arguments
    int argc = 2;
    char* argv[] = {
        (char*)"nlm",
        (char*)"--help"
    };
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Failed to initialize CommandLineInterface" << std::endl;
        return 1;
    }
    
    std::cout << "CommandLineInterface initialized successfully" << std::endl;
    
    // Show help
    std::cout << "\nHelp:" << std::endl;
    std::cout << cli.getHelp() << std::endl;
    
    // Show status
    std::cout << "\nStatus:" << std::endl;
    std::cout << cli.getStatus() << std::endl;
    
    // Shutdown
    cli.shutdown();
    
    std::cout << "\nDemo completed successfully" << std::endl;
    
    return 0;
}
