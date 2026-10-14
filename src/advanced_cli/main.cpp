// Advanced CLI Main Program
// Provides an enhanced command-line interface for advanced users

#include "advanced_cli/AdvancedCLI.hpp"
#include <iostream>

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "NLM Advanced Command Line Interface v2.0" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This is an enhanced CLI interface for the NLM (Neural Learning Machine) framework." << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  • Command history and undo/redo support" << std::endl;
    std::cout << "  • Advanced configuration management" << std::endl;
    std::cout << "  • Performance profiling and benchmarking" << std::endl;
    std::cout << "  • Experiment control and automation" << std::endl;
    std::cout << "  • Command aliases and custom commands" << std::endl;
    std::cout << std::endl;
    std::cout << "Getting Started:" << std::endl;
    std::cout << "  • Type 'help' to see all available commands" << std::endl;
    std::cout << "  • Type 'create' to start with default configuration" << std::endl;
    std::cout << "  • Type 'step 10' to run 10 simulation steps" << std::endl;
    std::cout << std::endl;
    
    try {
        nlm::AdvancedCLI cli;
        cli.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}
