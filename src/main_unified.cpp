// NLM Unified Command Line Interface
// This is the main entry point for the new unified CLI framework
// It replaces the original main.cpp with a comprehensive subcommand-based interface

#include "core/CLI/NLMMain.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        // Create and run the unified CLI
        nlm::UnifiedCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
