// NLM CLI Application Entry Point
// Main entry point for the NLM command-line interface

#include "NLMCLIApp.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // Create and initialize the CLI application
    NLMCLIApp app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize NLM CLI application" << std::endl;
        return 1;
    }
    
    try {
        // Check for batch mode flags
        bool batchMode = false;
        std::string batchFile;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--batch" || arg == "-b") {
                batchMode = true;
            } else if (arg == "--file" || arg == "-f") {
                if (i + 1 < argc) {
                    batchFile = argv[++i];
                }
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
                std::cout << "Usage: nlm_cli [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --batch, -b           Run in batch mode (read commands from stdin)" << std::endl;
                std::cout << "  --file <filename>, -f <filename> Run commands from file" << std::endl;
                std::cout << "  --help, -h            Show this help message" << std::endl;
                std::cout << "  --version             Show version information" << std::endl;
                std::cout << "  --interactive         Force interactive mode (default)" << std::endl;
                return 0;
            } else if (arg == "--version") {
                std::cout << "NLM CLI Version: 0.1.0" << std::endl;
                std::cout << "Build Date: " << __DATE__ << std::endl;
                std::cout << "C++ Standard: C++17" << std::endl;
                std::cout << "Phase: 6 - Final Integration" << std::endl;
                return 0;
            }
        }
        
        // Set interactive mode based on flags
        app.setInteractive(!batchMode);
        
        if (batchMode && !batchFile.empty()) {
            // Run from file
            std::ifstream inputFile(batchFile);
            if (!inputFile.is_open()) {
                std::cerr << "Error: Cannot open batch file: " << batchFile << std::endl;
                return 1;
            }
            
            // Replace stdin with file input
            std::cin.rdbuf(inputFile.rdbuf());
        }
        
        // Run the application
        app.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}