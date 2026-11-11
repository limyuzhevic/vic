// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// Updated main entry point that uses the modular application structure.
// This provides a cleaner, more maintainable application structure with:
// - Command-line argument parsing and help system
// - Configurable experiment running
// - Enhanced benchmark support
// - Better separation of concerns

#include "main_application.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        // Create and run the application
        auto app = std::make_unique<NLMApplication>(argc, argv);
        app->run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}