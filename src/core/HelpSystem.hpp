// nlm_help.h - Comprehensive help system for NLM executables
//
// This header provides a unified help system for all NLM executables,
// including consistent formatting, command-line parsing, and user guidance.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

class HelpSystem {
public:
    struct CommandOption {
        std::string flag;
        std::string description;
        std::string longDescription;
        std::string defaultValue;
        std::function<void(const std::string&)> action;
        bool requiresArgument = false;
        bool advanced = false;
    };
    
    struct CommandInfo {
        std::string name;
        std::string description;
        std::vector<CommandOption> options;
        std::string examples;
        std::string version;
    };
    
    HelpSystem() {
        initializeBuiltins();
    }
    
    void initializeBuiltins() {
        // NLM core command options
        builtinCommands["nlm"] = {
            "nlm",
            "NLM Phase 2: Real Neural Computation",
            {
                {"--help", "Show help information", "Display help for all available commands", "", nullptr, false},
                {"--version", "Show version information", "Display NLM version and build information", "", nullptr, false},
                {"--verbose", "Enable verbose logging", "Increase verbosity of output logs", "", nullptr, false, true},
                {"--log-level", "Set log level", "Set minimum log level (trace, debug, info, warn, error, fatal)", "info", nullptr, true},
                {"--config", "Path to configuration file", "Load configuration from file", "configs/default.cfg", nullptr, true},
                {"--output", "Output file path", "Save results to specified file", "", nullptr, true},
                {"--quiet", "Suppress output", "Minimize output to stdout", "", nullptr, false},
                {"--profile", "Enable profiling", "Collect performance metrics", "", nullptr, false, true},
                {"--benchmark", "Run benchmark", "Run performance benchmark instead of demo", "", nullptr, false, true},
            },
            "  nlm --help                    # Display help\n"
            "  nlm --version                 # Show version\n"
            "  nlm --config myconfig.cfg     # Use custom config\n"
            "  nlm --benchmark --profile     # Run benchmark with profiling",
            "NLM 0.1.0 - Neural Learning Machine Phase 2"
        };
        
        builtinCommands["nlm_phase3_demo"] = {
            "nlm_phase3_demo",
            "NLM Phase 3: World Interaction and Development",
            {
                {"--help", "Show help information", "Display help for Phase 3 demo", "", nullptr, false},
                {"--world-size", "Set world dimensions", "Set world width and height (e.g., 16x16)", "16", nullptr, true},
                {"--steps", "Number of simulation steps", "Total number of simulation steps to run", "1000", nullptr, true},
                {"--agent", "Agent type", "Select agent behavior (reactive, planning, learning)", "reactive", nullptr, true},
                {"--enable-development", "Enable developmental processes", "Enable neural development and maturation", "", nullptr, false},
                {"--enable-curiosity", "Enable curiosity-driven learning", "Enable intrinsic motivation system", "", nullptr, false},
                {"--enable-reward", "Enable reward modulation", "Enable reward-based learning", "", nullptr, false},
                {"--checkpoint", "Checkpoint path", "Path for saving/loading checkpoints", "", nullptr, true},
                {"--verbose", "Enable verbose output", "Show detailed simulation information", "", nullptr, false},
            },
            "  nlm_phase3_demo --world-size 32 --enable-development\n"
            "  nlm_phase3_demo --steps 5000 --agent planning\n"
            "  nlm_phase3_demo --checkpoint ./checkpoint.bin",
            "Phase 3 - World Interaction and Development"
        };
        
        builtinCommands["nlm_phase4_demo"] = {
            "nlm_phase4_demo",
            "NLM Phase 4: Advanced Cognitive Mechanisms",
            {
                {"--help", "Show help information", "Display help for Phase 4 demo", "", nullptr, false},
                {"--neurons", "Number of neurons", "Total neurons in the brain network", "500", nullptr, true},
                {"--regions", "Number of neural regions", "Number of brain regions", "1", nullptr, true},
                {"--plasticity", "Enable plasticity rules", "Enable synaptic plasticity", "", nullptr, false},
                {"--memory", "Memory system type", "Type of memory system (working, episodic, semantic)", "working", nullptr, true},
                {"--prediction", "Enable prediction system", "Enable predictive coding and forward models", "", nullptr, false},
                {"--attention", "Enable attention mechanism", "Enable attentional selection and focus", "", nullptr, false},
                {"--concept", "Enable concept formation", "Enable abstract concept learning", "", nullptr, false},
                {"--planner", "Enable planning system", "Enable action planning and sequencing", "", nullptr, false},
                {"--duration", "Simulation duration", "Total simulation time in seconds", "60", nullptr, true},
                {"--mode", "Operation mode", "Select simulation mode (test, learning, exploration)", "test", nullptr, true},
            },
            "  nlm_phase4_demo --neurons 1000 --regions 2 --memory episodic\n"
            "  nlm_phase4_demo --prediction --attention --planner\n"
            "  nlm_phase4_demo --duration 120 --mode exploration",
            "Phase 4 - Advanced Cognitive Mechanisms"
        };
        
        builtinCommands["nlm_phase6_demo"] = {
            "nlm_phase6_demo",
            "NLM Phase 6: Final Integration Test",
            {
                {"--help", "Show help information", "Display help for Phase 6 integration demo", "", nullptr, false},
                {"--neurons", "Number of neurons", "Total neurons in the integrated brain", "500", nullptr, true},
                {"--steps", "Maximum simulation steps", "Maximum number of simulation steps", "2000", nullptr, true},
                {"--memory-systems", "Memory systems to enable", "Comma-separated list: working, episodic, semantic, procedural", "working,episodic", nullptr, true},
                {"--neuromodulation", "Neuromodulators to enable", "Comma-separated list: dopamine, curiosity, novelty, reward", "dopamine,curiosity", nullptr, true},
                {""--prediction-system", "Enable prediction system", "Enable predictive modeling and error signals", "", nullptr, false},
                {"--development", "Enable development", "Enable developmental processes and maturation", "", nullptr, false},
                {"--checkpointing", "Enable checkpointing", "Enable save/load functionality", "", nullptr, false},
                {"--replay", "Enable replay system", "Enable episodic memory replay and consolidation", "", nullptr, false},
                {"--output-format", "Output format", "Format for results (json, csv, text)", "json", nullptr, true},
                {"--output-file", "Output file", "Path to save integration results", "", nullptr, true},
                {"--validation-only", "Validation mode only", "Only run integration verification, skip full simulation", "", nullptr, false},
                {"--quiet", "Reduce output", "Minimize output for automated testing", "", nullptr, false},
                {"--benchmark", "Benchmark mode", "Run performance benchmark instead of integration test", "", nullptr, false},
                {"--debug", "Debug mode", "Enable debug output and assertions", "", nullptr, false, true},
            },
            "  nlm_phase6_demo --neurons 1000 --steps 5000\n"
            "  nlm_phase6_demo --memory-systems working,episodic,semantic\n"
            "  nlm_phase6_demo --prediction-system --replay --checkpointing\n"
            "  nlm_phase6_demo --validation-only\n"
            "  nlm_phase6_demo --benchmark --debug",
            "Phase 6 - Final Integration Test"
        };
    }
    
    void displayCommandHelp(const std::string& command) {
        auto it = builtinCommands.find(command);
        if (it == builtinCommands.end()) {
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            return;
        }
        
        const CommandInfo& cmd = it->second;
        
        std::cout << "=== " << cmd.name << " ===" << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << std::endl;
        
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help                     " << std::left << std::setw(40) << "Show this help message" << std::endl;
        
        for (const auto& option : cmd.options) {
            if (option.advanced) continue;  // Skip advanced options in main help
            
            std::string flag = option.flag;
            std::string desc = option.description;
            
            // Format option display
            if (option.requiresArgument) {
                flag += " <arg>";
            }
            
            std::cout << "  " << std::left << std::setw(25) << flag << " " << desc << std::endl;
        }
        
        std::cout << std::endl;
        
        if (!cmd.examples.empty()) {
            std::cout << "Examples:" << std::endl;
            std::cout << cmd.examples << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "For more information about advanced options, use:" << std::endl;
        std::cout << "  " << cmd.name << " --help --verbose" << std::endl;
        
        std::cout << std::endl;
    }
    
    void displayAdvancedHelp(const std::string& command) {
        auto it = builtinCommands.find(command);
        if (it == builtinCommands.end()) {
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            return;
        }
        
        const CommandInfo& cmd = it->second;
        
        std::cout << "=== " << cmd.name << " - Advanced Options ===" << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << "Version: " << cmd.version << std::endl;
        std::cout << std::endl;
        
        for (const auto& option : cmd.options) {
            if (!option.advanced) continue;
            
            std::cout << "  " << option.flag;
            if (option.requiresArgument) {
                std::cout << " <arg>";
            }
            std::cout << std::endl;
            std::cout << "    " << option.longDescription;
            if (!option.defaultValue.empty()) {
                std::cout << " (default: " << option.defaultValue << ")";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
    
    std::vector<std::string> parseArguments(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        return args;
    }
    
    bool parseOptions(const std::vector<std::string>& args, std::map<std::string, std::string>& parsedOptions) {
        bool showHelp = false;
        bool showAdvanced = false;
        
        for (size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg == "--verbose" || arg == "-v") {
                parsedOptions["verbose"] = "true";
            } else if (arg == "--quiet" || arg == "-q") {
                parsedOptions["quiet"] = "true";
            } else if (arg == "--benchmark" || arg == "-b") {
                parsedOptions["benchmark"] = "true";
            } else if (arg == "--debug" || arg == "-d") {
                parsedOptions["debug"] = "true";
            } else if (arg.find("--") == 0) {
                // Long option with potential argument
                size_t equalsPos = arg.find('=');
                std::string flag, value;
                
                if (equalsPos != std::string::npos) {
                    flag = arg.substr(0, equalsPos);
                    value = arg.substr(equalsPos + 1);
                } else {
                    flag = arg;
                    // Check if next argument is a value (not another option)
                    if (i + 1 < args.size() && args[i + 1].find("--") != 0) {
                        value = args[++i];
                    } else {
                        value = "true"; // Flag option
                    }
                }
                
                // Remove leading -- if present
                if (flag.size() > 2 && flag.substr(0, 2) == "--") {
                    flag = flag.substr(2);
                }
                
                parsedOptions[flag] = value;
            } else {
                // Positional argument or unrecognized
                parsedOptions["positional"] = arg;
            }
        }
        
        return showHelp;
    }
    
    void printVersion() {
        std::cout << "NLM 0.1.0 - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << std::endl;
        std::cout << "Built with C++20, CMake 3.16+" << std::endl;
        std::cout << "License: MIT" << std::endl;
        std::cout << "Homepage: https://github.com/nlm-project/nlm" << std::endl;
        std::cout << std::endl;
    }
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics           ║
    ║     - Event-driven spike propagation with delays                ║
    ║     - STDP and Hebbian plasticity rules                         ║
    ║     - Structural plasticity (synaptogenesis/pruning)           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
private:
    std::map<std::string, CommandInfo> builtinCommands;
};