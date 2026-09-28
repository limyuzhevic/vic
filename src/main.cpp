#include "cli/CLIManager.hpp"
#include <iostream>

// Forward declaration for legacy support
void legacyMain(int argc, char** argv);

int main(int argc, char* argv[]) {
    // Check if we're running the legacy Phase 2 implementation
    // This can be enabled via a command line argument for backward compatibility
    bool useLegacy = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--legacy" || arg == "--phase2") {
            useLegacy = true;
            break;
        }
    }
    
    if (useLegacy) {
        std::cout << "=== NLM Phase 2 Legacy Implementation ===" << std::endl;
        std::cout << "Note: This is the old Phase 2 implementation." << std::endl;
        std::cout << "For the new Phase 6 implementation, run without --legacy flag." << std::endl;
        std::cout << std::endl;
        
        // Run legacy main (Phase 2)
        legacyMain(argc, argv);
        return 0;
    }
    
    // Run new CLI manager (Phase 6)
    nlm::CLIManager cli;
    
    // Register subcommands
    cli.registerSubcommand("run", "Run NLM neural simulation", 
                          [](const std::vector<std::string>& args) {
                              return cli.handleRun(args);
                          });
    
    cli.registerSubcommand("config", "Configuration management",
                          [](const std::vector<std::string>& args) {
                              return cli.handleConfig(args);
                          });
    
    cli.registerSubcommand("experiment", "Experiment management",
                          [](const std::vector<std::string>& args) {
                              return cli.handleExperiment(args);
                          });
    
    cli.registerSubcommand("benchmark", "Performance benchmarking",
                          [](const std::vector<std::string>& args) {
                              return cli.handleBenchmark(args);
                          });
    
    cli.registerSubcommand("debug", "Debug tools",
                          [](const std::vector<std::string>& args) {
                              return cli.handleDebug(args);
                          });
    
    cli.registerSubcommand("help", "Show help information",
                          [](const std::vector<std::string>& args) {
                              return cli.handleHelp(args);
                          });
    
    return cli.run(argc, argv);
}

// Legacy Phase 2 main function (for backward compatibility)
void legacyMain(int argc, char** argv) {
    // This is the old Phase 2 main implementation
    // It's kept here for backward compatibility
    
    // Include the original Phase 2 main code
    // In a real implementation, this would be the actual Phase 2 code
    
    std::cout << "Phase 2 legacy simulation started." << std::endl;
    std::cout << "This is the old implementation." << std::endl;
    
    // For now, just show a message
    std::cout << "To run the new Phase 6 implementation, remove the --legacy flag." << std::endl;
}
