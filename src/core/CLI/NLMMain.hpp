#ifndef NLM_MAIN_HPP
#define NLM_MAIN_HPP

#include "core/CLI/CLIFramework.hpp"
#include "core/CLI/CLISubcommands.hpp"

namespace nlm {

class UnifiedCLI : public CLIApplication {
public:
    UnifiedCLI() : CLIApplication("nlm", "Neural Learning Machine - Unified Command Line Interface") {
        setVersion("0.1.0");
        
        // Register global options
        addOption('h', "help", "Show help message", false);
        addOption('v', "version", "Show version information", false);
        addOption('l', "log-level", "Set logging level", std::string("info"));
        addOption('c', "config", "Load configuration file", std::string(""));
        
        // Register commands
        addCommand("help", "Show help message", helpCommand);
        addCommand("version", "Show version information", versionCommand);
        addCommand("phase2", "Run Phase 2 neural computation", phase2Command);
        addCommand("phase3", "Run Phase 3 experiments", phase3Command);
        addCommand("phase4", "Run Phase 4 experiments", phase4Command);
        addCommand("phase5", "Run Phase 5 experiments", phase5Command);
        addCommand("phase6", "Run Phase 6 integration experiments", phase6Command);
        addCommand("benchmark", "Run performance benchmarks", benchmarkCommand);
        addCommand("config", "Configuration management", configCommand);
        
        // Register configuration subcommands
        addCommand("config-load", "Load configuration", configLoadCommand);
        addCommand("config-save", "Save configuration", configSaveCommand);
        addCommand("config-validate", "Validate configuration", configValidateCommand);
        addCommand("config-show", "Show configuration", configShowCommand);
    }
    
    // Phase 2 command implementation
    static int phase2Command(int argc, char** argv) {
        printPhaseInfo(2);
        std::cout << "Running Phase 2 neural computation..." << std::endl;
        
        // For now, run the original main.cpp functionality
        // In a complete implementation, this would include all Phase 2 tests
        std::cout << "Phase 2: LIF neuron dynamics, event-driven spike propagation, STDP, Hebbian plasticity" << std::endl;
        
        printPhaseInfo(2);
        return 0;
    }
    
private:
    // Placeholder for individual phase command implementations
    static int phase3Command(int argc, char** argv) { return phase3DemoCommand(argc, argv); }
    static int phase4Command(int argc, char** argv) { return phase4DemoCommand(argc, argv); }
    static int phase5Command(int argc, char** argv) { return phase5DemoCommand(argc, argv); }
    static int phase6Command(int argc, char** argv) { return phase6DemoCommand(argc, argv); }
};

} // namespace nlm

#endif // NLM_MAIN_HPP
