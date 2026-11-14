// Command-line interface utilities for NLM
// Provides consistent argument parsing, help system, and configuration management

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>
#include <sstream>
#include <algorithm>
#include <variant>

namespace nlm {

// CLI argument types
enum class ArgumentType {
    Flag,
    Value,
    String,
    Integer,
    Float,
    Choice
};

// Command-line option definition
struct CLIOption {
    std::string name;
    std::string shortName;
    std::string description;
    ArgumentType type;
    std::string defaultValue;
    std::string configKey;
    std::vector<std::string> validChoices;
    bool required;
    std::function<void(const std::string&)> callback;
    
    CLIOption(const std::string& n, const std::string& desc, ArgumentType t, const std::string& def = "",
              const std::string& config = "", const std::vector<std::string>& choices = {},
              bool req = false)
        : name(n), shortName(""), description(desc), type(t), defaultValue(def), 
          configKey(config), validChoices(choices), required(req) {}
    
    CLIOption(const std::string& n, char shortN, const std::string& desc, ArgumentType t,
              const std::string& def = "", const std::string& config = "", bool req = false)
        : name(n), shortName(1, shortN), description(desc), type(t), defaultValue(def),
          configKey(config), required(req) {}
};

// CLI argument result
struct CLIResult {
    std::unordered_map<std::string, std::variant<bool, std::string, int, float>> values;
    std::vector<std::string> positionalArgs;
    std::string phase;
    bool verbose;
    bool debug;
    bool help;
    std::string configFile;
    std::string logLevel;
    bool version;
    
    CLIResult() : verbose(false), debug(false), help(false), version(false) {}
};

// Command-line interface parser
class CLIParser {
public:
    CLIParser() {}
    
    // Add an option
    void addOption(const CLIOption& option) {
        options.push_back(option);
        optionMap[option.name] = &options.back();
        if (!option.shortName.empty()) {
            shortOptionMap[option.shortName] = &options.back();
        }
    }
    
    // Parse command line arguments
    CLIResult parse(int argc, char* argv[]) {
        CLIResult result;
        
        // Process positional arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                result.help = true;
                result.phase = "help";
            } else if (arg == "--version" || arg == "-v") {
                result.version = true;
            } else if (arg == "--verbose" || arg == "-V") {
                result.verbose = true;
            } else if (arg == "--debug" || arg == "-d") {
                result.debug = true;
            } else if (arg == "--phase" || arg == "-p") {
                if (i + 1 < argc) {
                    result.phase = argv[++i];
                }
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    result.configFile = argv[++i];
                }
            } else if (arg == "--log-level" || arg == "-l") {
                if (i + 1 < argc) {
                    result.logLevel = argv[++i];
                }
            } else if (arg.substr(0, 2) == "--") {
                processOption(arg.substr(2), result, i, argc, argv);
            } else if (arg.substr(0, 1) == "-") {
                processShortOption(arg.substr(1), result, i, argc, argv);
            } else {
                result.positionalArgs.push_back(arg);
            }
        }
        
        return result;
    }
    
    // Display help
    void showHelp(const std::string& programName) {
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << "===============================" << std::endl << std::endl;
        
        std::cout << "USAGE:" << std::endl;
        std::cout << "  " << programName << " [OPTIONS] [ARGUMENTS]" << std::endl << std::endl;
        
        std::cout << "OPTIONS:" << std::endl;
        for (const auto& option : options) {
            std::string optName = "--" + option.name;
            if (!option.shortName.empty()) {
                optName += ", -" + option.shortName;
            }
            
            std::cout << "  " << std::left << std::setw(20) << optName;
            
            switch (option.type) {
                case ArgumentType::Flag:
                    std::cout << "[flag]";
                    break;
                case ArgumentType::Value:
                    std::cout << "<value>";
                    break;
                case ArgumentType/rub/:
                    std::cout << "<string>";
                    break;
                case ArgumentType::Integer:
                    std::cout << "<int>";
                    break;
                case ArgumentType::Float:
                    std::cout << "<float>";
                    break;
                case ArgumentType::Choice:
                    std::cout << "<choice>";
                    break;
            }
            
            std::cout << " " << option.description;
            
            if (!option.defaultValue.empty()) {
                std::cout << " (default: " << option.defaultValue << ")";
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "GLOBAL OPTIONS:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -v, --version           Show version information" << std::endl;
        std::cout << "  -V, --verbose           Enable verbose output" << std::endl;
        std::cout << "  -d, --debug             Enable debug mode" << std::endl;
        std::cout << "  -p, --phase <phase>     Select phase (1, 2, 3, 4, 5, 6)" << std::endl;
        std::cout << "  -c, --config <file>     Load configuration from file" << std::endl;
        std::cout << "  -l, --log-level <level> Set log level (debug, info, warn, error)" << std::endl;
        
        std::cout << std::endl;
        std::cout << "ARGUMENTS:" << std::endl;
        std::cout << "  <episodes>             Number of episodes (for Phase 3)" << std::endl;
        std::cout << "  <steps>                Number of steps per episode (for Phase 3)" << std::endl;
        std::cout << "  <trials>                Number of trials (for Phase 4)" << std::endl;
    }
    
    // Get option by name
    const CLIOption* getOption(const std::string& name) const {
        auto it = optionMap.find(name);
        return it != optionMap.end() ? it->second : nullptr;
    }
    
private:
    void processOption(const std::string& arg, CLIResult& result, int& i, int argc, char* argv[]) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string name = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            auto it = optionMap.find(name);
            if (it != optionMap.end()) {
                CLIOption* option = it->second;
                setOptionValue(*option, value, result.values);
            }
        } else {
            auto it = optionMap.find(arg);
            if (it != optionMap.end()) {
                CLIOption* option = it->second;
                if (option->type == ArgumentType::Flag) {
                    setOptionValue(*option, "true", result.values);
                } else {
                    if (i + 1 < argc) {
                        setOptionValue(*option, argv[++i], result.values);
                    }
                }
            }
        }
    }
    
    void processShortOption(const std::string& arg, CLIResult& result, int& i, int argc, char* argv[]) {
        if (arg.size() == 1) {
            auto it = shortOptionMap.find(arg);
            if (it != shortOptionMap.end()) {
                CLIOption* option = it->second;
                if (option->type == ArgumentType::Flag) {
                    setOptionValue(*option, "true", result.values);
                } else {
                    if (i + 1 < argc) {
                        setOptionValue(*option, argv[++i], result.values);
                    }
                }
            }
        } else {
            std::string argWithEquals = "--" + arg;
            processOption(argWithEquals, result, i, argc, argv);
        }
    }
    
    void setOptionValue(const CLIOption& option, const std::string& value,
                       std::unordered_map<std::string, std::variant<bool, std::string, int, float>>& values) {
        if (option.type == ArgumentType::Flag) {
            values[option.name] = true;
        } else if (option.type == ArgumentType::String || option.type == ArgumentType::Choice) {
            values[option.name] = value;
        } else if (option.type == ArgumentType::Integer) {
            values[option.name] = std::stoi(value);
        } else if (option.type == ArgumentType::Float) {
            values[option.name] = std::stof(value);
        }
    }
    
    std::vector<CLIOption> options;
    std::unordered_map<std::string, CLIOption*> optionMap;
    std::unordered_map<std::string, CLIOption*> shortOptionMap;
};

// Phase selector for unified interface
class PhaseSelector {
public:
    enum Phase {
        Phase1,
        Phase2,
        Phase3,
        Phase4,
        Phase5,
        Phase6,
        Benchmark,
        All
    };
    
    PhaseSelector() : currentPhase(Phase2) {}
    
    Phase parsePhase(const std::string& phaseStr) {
        std::string phase = phaseStr;
        std::transform(phase.begin(), phase.end(), phase.begin(), ::tolower);
        
        if (phase == "1" || phase == "phase1" || phase == "basic") {
            return Phase1;
        } else if (phase == "2" || phase == "phase2" || phase == "core") {
            return Phase2;
        } else if (phase == "3" || phase == "phase3" || phase == "world") {
            return Phase3;
        } else if (phase == "4" || phase == "phase4" || phase == "cognition") {
            return Phase4;
        } else if (phase == "5" || phase == "phase5" || phase == "personality") {
            return Phase5;
        } else if (phase == "6" || phase == "phase6" || phase == "integration") {
            return Phase6;
        } else if (phase == "benchmark" || phase == "bench") {
            return Benchmark;
        } else if (phase == "all") {
            return All;
        } else {
            return Phase2; // Default
        }
    }
    
    Phase getCurrentPhase() const { return currentPhase; }
    void setCurrentPhase(Phase phase) { currentPhase = phase; }
    
    std::string phaseToString(Phase phase) {
        switch (phase) {
            case Phase1: return "Phase 1 - Basic";
            case Phase2: return "Phase 2 - Core Neural Computation";
            case Phase3: return "Phase 3 - World Interaction";
            case Phase4: return "Phase 4 - Emerging Cognition";
            case Phase5: return "Phase 5 - Brain-Personality";
            case Phase6: return "Phase 6 - Final Integration";
            case Benchmark: return "Benchmark";
            case All: return "All Phases";
            default: return "Unknown";
        }
    }
    
    void executePhase(Phase phase, const CLIResult& cliResult) {
        currentPhase = phase;
        std::cout << "Executing " << phaseToString(phase) << std::endl;
        
        // Implementation would depend on the specific phase
        // This is a framework that allows easy extension
        switch (phase) {
            case Phase2:
                std::cout << "Running Phase 2: Real Neural Computation" << std::endl;
                break;
            case Phase3:
                std::cout << "Running Phase 3: World Interaction Demo" << std::endl;
                // Parse arguments for Phase 3
                int episodes = 5;
                int stepsPerEpisode = 200;
                
                if (!cliResult.positionalArgs.empty()) {
                    try { episodes = std::stoi(cliResult.positionalArgs[0]); }
                    catch (...) {}
                }
                if (cliResult.positionalArgs.size() > 1) {
                    try { stepsPerEpisode = std::stoi(cliResult.positionalArgs[1]); }
                    catch (...) {}
                }
                
                std::cout << "  Episodes: " << episodes << ", Steps per episode: " << stepsPerEpisode << std::endl;
                break;
            case Phase4:
                std::cout << "Running Phase 4: Emerging Cognition" << std::endl;
                // Parse trials argument
                int trials = 50;
                if (!cliResult.positionalArgs.empty()) {
                    try { trials = std::stoi(cliResult.positionalArgs[0]); }
                    catch (...) {}
                }
                std::cout << "  Trials: " << trials << std::endl;
                break;
            case Phase6:
                std::cout << "Running Phase 6: Integration Demo" << std::endl;
                break;
            default:
                std::cout << "Phase not yet implemented" << std::endl;
                break;
        }
    }
    
private:
    Phase currentPhase;
};

} // namespace nlm
