// Enhanced NLM Main CLI
// Replaced the monolithic main.cpp with a modular command-line interface
// Provides user-friendly access to NLM functionality

#include "cli/NLMCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Initialize NLM logging and systems
    nlm::initializeNLM();
    
    try {
        // Create and run CLI
        nlm::NLMCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}