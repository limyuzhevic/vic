// Enhanced NLM Main CLI
// Entry point for the improved command-line interface

#include "nlm_cli.cpp"
#include <iostream>

int main(int argc, char** argv) {
    // Initialize NLM
    nlm::initializeNLM();
    
    try {
        // Create and run CLI
        nlm::SimpleCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}