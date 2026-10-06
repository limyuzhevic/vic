// CLI Implementation
#include "CLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLI::CLI(int argc, char* argv[]) : helpRequested(false), versionRequested(false), verbose(false) {
    programName = argv[0];
    if (programName.find('/') != std::string::npos) {
        programName = programName.substr(programName.find_last_of('/') + 1);
    }
    
    setupDefaultCommands();
    parseArguments();
}

void CLI::addCommand(const CLICommand& cmd) {
    commands.push_back(cmd);
}

void CLI::showHelp() const {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "An experimental artificial developmental brain simulation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Global Options:" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --config <file>     Load configuration from file" << std::endl;
    std::cout << "  --output <file>     Save output to file" << std::endl;
    std::cout << "  --verbose, -v       Enable verbose output" << std::endl;
    std::cout << std::endl;
}

void CLI::showVersion() const {
    std::cout << "NLM Phase 2 (熙然)" << std::endl;
    std::cout << "Neural Learning Machine - Phase 2: Real Neural Computation" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  Compiled: " __DATE__ " " __TIME__ << std::endl;
    std::cout << "  Version: 0.1.0 (Phase 2)" << std::endl;
    std::cout << "  Author: NLM Research Project" << std::endl;
}

bool CLI::parseArguments() {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version") {
            versionRequested = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--config" && i + 1 < args.size()) {
            configFile = args[++i];
        } else if (arg == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg.substr(0, 2) == "--") {
            // Unknown flag, treat as extra argument
            extraArgs.push_back(arg);
        } else {
            // Not a flag, likely a command or value
            extraArgs.push_back(arg);
        }
    }
    
    return true;
}

void CLI::run() const {
    if (helpRequested) {
        showHelp();
        return;
    }
    
    if (versionRequested) {
        showVersion();
        return;
    }
    
    if (extraArgs.empty()) {
        std::cerr << "Error: No command specified. Use --help for available commands." << std::endl;
        return;
    }
    
    std::string command = extraArgs[0];
    std::vector<std::string> commandArgs;
    for (size_t i = 1; i < extraArgs.size(); ++i) {
        commandArgs.push_back(extraArgs[i]);
    }
    
    // Find and run the command
    for (const auto& cmd : commands) {
        if (cmd.name == command) {
            cmd.handler(commandArgs);
            return;
        }
    }
    
    std::cerr << "Error: Unknown command '" << command << "'. Use --help for available commands." << std::endl;
}

void CLI::setupDefaultCommands() {
    addCommand({
        "run", 
        "Run NLM simulation with specified configuration",
        "run [options]",
        [](const std::vector<std::string>& args) {
            // Default run handler
            std::cout << "Running NLM simulation..." << std::endl;
        }
    });
    
    addCommand({
        "test",
        "Run NLM test suite",
        "test [options]",
        [](const std::vector<std::string>& args) {
            // Test handler
            std::cout << "Running NLM test suite..." << std::endl;
        }
    });
    
    addCommand({
        "experiment",
        "Run NLM experiment",
        "experiment <name> [options]",
        [](const std::vector<std::string>& args) {
            // Experiment handler
            if (args.empty()) {
                std::cerr << "Error: Experiment name required." << std::endl;
                return;
            }
            std::cout << "Running experiment: " << args[0] << std::endl;
        }
    });
    
    addCommand({
        "visualize",
        "Launch visualization interface",
        "visualize [options]",
        [](const std::vector<std::string>& args) {
            // Visualization handler
            std::cout << "Launching visualization interface..." << std::endl;
        }
    });
}

void CLI::setupPhase2Commands() {
    addCommand({
        "phase2-run",
        "Run Phase 2 simulation (basic neural computation)",
        "phase2-run [options]",
        [](const std::vector<std::string>& args) {
            std::cout << "Running Phase 2 simulation..." << std::endl;
            // Would call actual phase 2 simulation code
        }
    });
    
    addCommand({
        "phase2-test",
        "Run Phase 2 test suite (connectivity, plasticity, STDP)",
        "phase2-test [options]",
        [](const std::vector<std::string>& args) {
            std::cout << "Running Phase 2 test suite..." << std::endl;
            // Would call actual phase 2 test code
        }
    });
}

void CLI::setupExperimentCommands() {
    addCommand({
        "run-experiment",
        "Run specific experiment (phase3, phase4, phase5, phase6)",
        "run-experiment <phase> [options]",
        [](const std::vector<std::string>& args) {
            if (args.empty()) {
                std::cerr << "Error: Experiment phase required (phase3, phase4, phase5, phase6)" << std::endl;
                return;
            }
            std::cout << "Running " << args[0] << " experiment..." << std::endl;
        }
    });
}

void CLI::setupDebugCommands() {
    addCommand({
        "debug",
        "Enable debug mode with extended logging",
        "debug [options]",
        [](const std::vector<std::string>& args) {
            std::cout << "Debug mode enabled..." << std::endl;
            // Would enable debug logging
        }
    });
    
    addCommand({
        "validate",
        "Validate configuration and brain state",
        "validate [options]",
        [](const std::vector<std::string>& args) {
            std::cout << "Validating configuration and brain state..." << std::endl;
            // Would validate brain state
        }
    });
}

void CLI::setupOutputCommands() {
    addCommand({
        "save-state",
        "Save current brain state to checkpoint",
        "save-state <filename>",
        [](const std::vector<std::string>& args) {
            if (args.empty()) {
                std::cerr << "Error: Output filename required." << std::endl;
                return;
            }
            std::cout << "Saving brain state to: " << args[0] << std::endl;
        }
    });
    
    addCommand({
        "export-metrics",
        "Export performance metrics to file",
        "export-metrics <filename>",
        [](const std::vector<std::string>& args) {
            if (args.empty()) {
                std::cerr << "Error: Output filename required." << std::endl;
                return;
            }
            std::cout << "Exporting metrics to: " << args[0] << std::endl;
        }
    });
}

} // namespace nlm