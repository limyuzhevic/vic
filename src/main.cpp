// Refactored main.cpp with improved structure and separation of concerns
// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation

#include "NLMAdvancedCommandProcessor.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

// Main function with improved argument parsing and error handling
int main(int argc, char** argv) {
    try {
        // Initialize advanced command processor
        AdvancedCommandProcessor processor;
        
        // Parse and execute commands
        AdvancedCommandProcessor::AdvancedCommandOptions options = processor.parse(argc, argv);
        
        if (processor.execute(options)) {
            return 0;
        } else {
            std::cerr << "[ERROR] Command execution failed" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Unexpected error: " << e.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "[ERROR] Unknown error occurred" << std::endl;
        return 3;
    }
}