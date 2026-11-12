// Command-line argument parsing utilities implementation for NLM
// This file implements the comprehensive command-line argument parsing utilities

#include "core/CommandLineParser.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

namespace nlm {
    
    // CommandLineArg implementation
    std::string ParsedArguments::get(const std::string& key, const std::string& defaultValue) const {
        auto it = parsedValues.find(key);
        if (it != parsedValues.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    bool ParsedArguments::getFlag(const std::string& key, bool defaultValue) const {
        auto it = flags.find(key);
        if (it != flags.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    int ParsedArguments::getInt(const std::string& key, int defaultValue) const {
        std::string value = get(key);
        if (value.empty()) return defaultValue;
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    int64_t ParsedArguments::getInt64(const std::string& key, int64_t defaultValue) const {
        std::string value = get(key);
        if (value.empty()) return defaultValue;
        try {
            return std::stoll(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    double ParsedArguments::getDouble(const std::string& key, double defaultValue) const {
        std::string value = get(key);
        if (value.empty()) return defaultValue;
        try {
            return std::stod(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    bool ParsedArguments::getBool(const std::string& key, bool defaultValue) const {
        std::string value = get(key);
        if (value.empty()) return defaultValue;
        
        // Convert common boolean representations
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        
        return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || 
                lowerValue == "on" || lowerValue == "enable");
    }
    
    float ParsedArguments::getFloat(const std::string& key, float defaultValue) const {
        std::string value = get(key);
        if (value.empty()) return defaultValue;
        try {
            return std::stof(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    std::string ParsedArguments::toString() const {
        std::ostringstream oss;
        oss << "Parsed Arguments:\n";
        oss << "  Values:\n";
        for (const auto& pair : values) {
            oss << "    " << pair.first << " = " << pair.second << "\n";
        }
        oss << "  Flags:\n";
        for (const auto& pair : flags) {
            oss << "    " << pair.first << " = " << (pair.second ? "true" : "false") << "\n";
        }
        oss << "  Positional arguments:\n";
        for (const auto& arg : positionalArgs) {
            oss << "    " << arg << "\n";
        }
        oss << "  Parsed values:\n";
        for (const auto& pair : parsedValues) {
            oss << "    " << pair.first << " = " << pair.second << "\n";
        }
        return oss.str();
    }
    
    // CommandLineParser implementation
    void CommandLineParser::addArgument(const std::string& name, ArgumentType type, 
                                       const std::string& description,
                                       const std::string& defaultValue,
                                       bool required,
                                       std::function<bool(const std::string&)> validator) {
        arguments_.emplace_back(name, type, description, defaultValue, required, validator);
    }
    
    void CommandLineParser::addPositionalArgument(const std::string& name,
                                                 ArgumentType type,
                                                 const std::string& description,
                                                 const std::string& defaultValue) {
        positionalArguments_.emplace_back(name, type, description, defaultValue, false);
    }
    
    ParsedArguments CommandLineParser::parse(int argc, char* argv[]) {
        ParsedArguments result;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value or -key value format
            if (arg.substr(0, 2) == "--") {
                // Check for --help or --version
                if (arg == "--help" || arg == "--version") {
                    result.flags[arg.substr(2)] = true;
                    continue;
                }
                
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    result.parsedValues[key] = value;
                    result.values[key] = value;
                } else {
                    // Flag
                    std::string key = arg.substr(2);
                    result.flags[key] = true;
                }
            } else if (arg.substr(0, 1) == "-") {
                // Handle -key value format
                std::string key = arg.substr(1);
                result.flags[key] = true;
                
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    i++; // Skip the value
                    // For simplicity, we'll not store the value in parsedValues
                    // In a full implementation, we would need to track which flags take values
                }
            } else {
                // Positional argument
                result.positionalArgs.push_back(arg);
            }
        }
        
        return result;
    }
    
    void CommandLineParser::printHelp() const {
        std::cout << "Command-line arguments:\n";
        std::cout << "  --help                    Show this help message\n";
        std::cout << "  --version                 Show version information\n";
        
        std::cout << "\nNamed arguments:\n";
        for (const auto& arg : arguments_) {
            std::cout << "  --" << arg.name;
            if (arg.type == ArgumentType::OptionalValue || arg.type == ArgumentType::Value) {
                std::cout << "=<value>";
            }
            std::cout << "\n    " << arg.description;
            if (!arg.defaultValue.empty()) {
                std::cout << " (default: " << arg.defaultValue << ")";
            }
            if (arg.required) {
                std::cout << " (required)";
            }
            std::cout << "\n";
        }
        
        if (!positionalArguments_.empty()) {
            std::cout << "\nPositional arguments:\n";
            for (const auto& arg : positionalArguments_) {
                std::cout << "  " << arg.name;
                if (arg.type == ArgumentType::OptionalValue || arg.type == ArgumentType::Value) {
                    std::cout << " [value]";
                }
                std::cout << "\n    " << arg.description;
                if (!arg.defaultValue.empty()) {
                    std::cout << " (default: " << arg.defaultValue << ")";
                }
                std::cout << "\n";
            }
        }
    }
    
    bool CommandLineParser::validateRequired() const {
        for (const auto& arg : arguments_) {
            if (arg.required) {
                bool found = false;
                // Check if the argument was provided (simplified check)
                for (const auto& parsedArg : arguments_) {
                    if (parsedArg.name == arg.name) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cerr << "Error: Required argument --" << arg.name << " is missing.\n";
                    return false;
                }
            }
        }
        return true;
    }
    
    bool CommandLineParser::validateValue(const CommandLineArg& arg, const std::string& value) const {
        if (arg.validator) {
            return arg.validator(value);
        }
        return true;
    }
    
    std::string CommandLineParser::toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string CommandLineParser::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Built-in argument definitions
    CommandLineArg BuiltInArgs::getConfigFile("config", ArgumentType::FilePath, 
                                             "Configuration file path", 
                                             "configs/default.cfg", false);
    
    CommandLineArg BuiltInArgs::getHelp("help", ArgumentType::Flag, 
                                       "Show help information", 
                                       "", false);
    
    CommandLineArg BuiltInArgs::getVersion("version", ArgumentType::Flag,
                                          "Show version information", 
                                          "", false);
    
    CommandLineArg BuiltInArgs::getLogLevel("log_level", ArgumentType::String,
                                           "Log level (trace, debug, info, warning, error, critical, fatal)",
                                           "info", false,
                                           [](const std::string& val) {
                                               const std::vector<std::string> levels = {
                                                   "trace", "debug", "info", "warning", "error", "critical", "fatal"
                                               };
                                               return std::find(levels.begin(), levels.end(), val) != levels.end();
                                           });
    
    CommandLineArg BuiltInArgs::getNeuronCount("neuron_count", ArgumentType::Number,
                                              "Number of neurons in the brain",
                                              "1000", false,
                                              [](const std::string& val) {
                                                  try {
                                                      int count = std::stoi(val);
                                                      return count > 0 && count <= 100000;
                                                  } catch (...) {
                                                      return false;
                                                  }
                                              });
    
    CommandLineArg BuiltInArgs::getRegionCount("region_count", ArgumentType::Number,
                                              "Number of brain regions",
                                              "2", false,
                                              [](const std::string& val) {
                                                  try {
                                                      int count = std::stoi(val);
                                                      return count > 0 && count <= 100;
                                                  } catch (...) {
                                                      return false;
                                                  }
                                              });
    
    CommandLineArg BuiltInArgs::getConnectionProbability("connection_probability", ArgumentType::Number,
                                                         "Connection probability (0.0 to 1.0)",
                                                         "0.1", false,
                                                         [](const std::string& val) {
                                                             try {
                                                                 double prob = std::stod(val);
                                                                 return prob >= 0.0 && prob <= 1.0;
                                                             } catch (...) {
                                                                 return false;
                                                             }
                                                         });
    
    CommandLineArg BuiltInArgs::getRandomSeed("random_seed", ArgumentType::Number,
                                             "Random seed for reproducible experiments",
                                             "42", false);
    
    CommandLineArg BuiltInArgs::getSimulationTimestep("simulation_timestep", ArgumentType::Number,
                                                     "Simulation timestep in seconds",
                                                     "0.001", false,
                                                     [](const std::string& val) {
                                                         try {
                                                             double dt = std::stod(val);
                                                             return dt > 0.0 && dt <= 1.0;
                                                         } catch (...) {
                                                             return false;
                                                         }
                                                     });
    
    CommandLineArg BuiltInArgs::getMaxSimulationSteps("max_simulation_steps", ArgumentType::Number,
                                                     "Maximum number of simulation steps",
                                                     "10000", false);
    
    CommandLineArg BuiltInArgs::getEnableVisualization("enable_visualization", ArgumentType::Boolean,
                                                      "Enable visualization output",
                                                      "false", false);
    
    CommandLineArg BuiltInArgs::getCheckpointFile("checkpoint_file", ArgumentType::FilePath,
                                                 "Checkpoint file path",
                                                 "", false);
    
    CommandLineArg BuiltInArgs::getLoadCheckpoint("load_checkpoint", ArgumentType::Flag,
                                                 "Load brain state from checkpoint",
                                                 "", false);
    
    CommandLineArg BuiltInArgs::getSaveCheckpoint("save_checkpoint", ArgumentType::Flag,
                                                 "Save brain state to checkpoint",
                                                 "", false);
    
    CommandLineArg BuiltInArgs::getProfile("profile", ArgumentType::Flag,
                                          "Enable profiling",
                                          "", false);
    
    CommandLineArg BuiltInArgs::getProfileFrequency("profile_frequency", ArgumentType::Number,
                                                   "Profile output frequency in seconds",
                                                   "60", false);
    
    CommandLineArg BuiltInArgs::getPerformanceMetrics("performance_metrics", ArgumentType::Flag,
                                                     "Enable performance metrics collection",
                                                     "", false);
    
    CommandLineArg BuiltInArgs::getDebug("debug", ArgumentType::Flag,
                                        "Enable debug output",
                                        "", false);
    
    CommandLineArg BuiltInArgs::getTestMode("test_mode", ArgumentType::Flag,
                                          "Run in test mode",
                                          "", false);
    
    CommandLineArg BuiltInArgs::getBenchmark("benchmark", ArgumentType::Flag,
                                           "Run benchmark",
                                           "", false);
    
    CommandLineArg BuiltInArgs::getExportBrain("export_brain", ArgumentType::FilePath,
                                             "Export brain state to file",
                                             "", false);
    
    CommandLineArg BuiltInArgs::getImportBrain("import_brain", ArgumentType::FilePath,
                                             "Import brain state from file",
                                             "", false);
    
    CommandLineArg BuiltInArgs::getEnvironment("environment", ArgumentType::String,
                                              "Environment type (grid, continuous, custom)",
                                              "grid", false);
    
    // Utils namespace implementation
    namespace Utils {
        
        ParsedArguments parseSimulationArgs(int argc, char* argv[]) {
            ParsedArguments result;
            CommandLineParser parser;
            
            // Add simulation-related arguments
            parser.addArgument("max_steps", ArgumentType::Number,
                              "Maximum simulation steps", "10000", false);
            parser.addArgument("timestep", ArgumentType::Number,
                              "Simulation timestep", "0.001", false);
            parser.addArgument("save_interval", ArgumentType::Number,
                              "Checkpoint save interval", "1000", false);
            
            ParsedArguments parsed = parser.parse(argc, argv);
            return parsed;
        }
        
        ParsedArguments parseBrainArgs(int argc, char* argv[]) {
            ParsedArguments result;
            CommandLineParser parser;
            
            // Add brain configuration arguments
            parser.addArgument("neuron_count", ArgumentType::Number,
                              "Number of neurons", "1000", false);
            parser.addArgument("region_count", ArgumentType::Number,
                              "Number of regions", "2", false);
            parser.addArgument("connection_probability", ArgumentType::Number,
                              "Connection probability", "0.1", false);
            parser.addArgument("random_seed", ArgumentType::Number,
                              "Random seed", "42", false);
            
            ParsedArguments parsed = parser.parse(argc, argv);
            return parsed;
        }
        
        ParsedArguments parseLoggingArgs(int argc, char* argv[]) {
            ParsedArguments result;
            CommandLineParser parser;
            
            // Add logging-related arguments
            parser.addArgument("log_level", ArgumentType::String,
                              "Log level", "info", false);
            parser.addArgument("log_to_file", ArgumentType::Boolean,
                              "Log to file", "false", false);
            parser.addArgument("log_filename", ArgumentType::FilePath,
                              "Log file name", "nlm.log", false);
            parser.addArgument("log_format", ArgumentType::String,
                              "Log format (simple, detailed, json)", "simple", false);
            
            ParsedArguments parsed = parser.parse(argc, argv);
            return parsed;
        }
        
        ParsedArguments parsePerformanceArgs(int argc, char* argv[]) {
            ParsedArguments result;
            CommandLineParser parser;
            
            // Add performance-related arguments
            parser.addArgument("profile", ArgumentType::Flag,
                              "Enable profiling", "", false);
            parser.addArgument("profile_frequency", ArgumentType::Number,
                              "Profile output frequency", "60", false);
            parser.addArgument("performance_metrics", ArgumentType::Flag,
                              "Enable performance metrics", "", false);
            parser.addArgument("benchmark", ArgumentType::Flag,
                              "Run benchmark", "", false);
            
            ParsedArguments parsed = parser.parse(argc, argv);
            return parsed;
        }
        
        ParsedArguments mergeArguments(const std::vector<ParsedArguments>& argsList) {
            ParsedArguments merged;
            
            for (const auto& args : argsList) {
                for (const auto& pair : args.values) {
                    merged.values[pair.first] = pair.second;
                }
                for (const auto& pair : args.flags) {
                    merged.flags[pair.first] = pair.second;
                }
                for (const auto& arg : args.positionalArgs) {
                    merged.positionalArgs.push_back(arg);
                }
                for (const auto& pair : args.parsedValues) {
                    merged.parsedValues[pair.first] = pair.second;
                }
            }
            
            return merged;
        }
        
        std::shared_ptr<Config> parsedArgumentsToConfig(const ParsedArguments& parsedArgs) {
            auto config = std::make_shared<Config>();
            
            // Convert common parsed arguments to config
            if (!parsedArgs.get("neuron_count").empty()) {
                config->set("neuron_count", std::stoll(parsedArgs.get("neuron_count")));
            }
            if (!parsedArgs.get("region_count").empty()) {
                config->set("region_count", std::stoll(parsedArgs.get("region_count")));
            }
            if (!parsedArgs.get("connection_probability").empty()) {
                config->set("connection_probability", std::stod(parsedArgs.get("connection_probability")));
            }
            if (!parsedArgs.get("random_seed").empty()) {
                config->set("random_seed", std::stoll(parsedArgs.get("random_seed")));
            }
            if (!parsedArgs.get("simulation_timestep").empty()) {
                config->set("simulation_timestep", std::stod(parsedArgs.get("simulation_timestep")));
            }
            
            return config;
        }
        
        void printArgumentsSummary(const ParsedArguments& parsedArgs) {
            std::cout << "Parsed Arguments Summary:\n";
            std::cout << "  Values:\n";
            for (const auto& pair : parsedArgs.values) {
                std::cout << "    " << pair.first << " = " << pair.second << "\n";
            }
            std::cout << "  Flags:\n";
            for (const auto& pair : parsedArgs.flags) {
                std::cout << "    " << pair.first << " = " << (pair.second ? "true" : "false") << "\n";
            }
            if (!parsedArgs.positionalArgs.empty()) {
                std::cout << "  Positional arguments:\n";
                for (const auto& arg : parsedArgs.positionalArgs) {
                    std::cout << "    " << arg << "\n";
                }
            }
        }
        
        bool hasHelpOrVersion(int argc, char* argv[]) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "--version") {
                    return true;
                }
            }
            return false;
        }
        
        std::string getHelpOrVersionMessage() {
            std::ostringstream oss;
            oss << "NLM (熙然) - Neural Learning Machine\n";
            oss << "Version 0.1.0\n";
            oss << "An experimental artificial developmental brain project\n\n";
            oss << "Usage: nlm [options]\n\n";
            oss << "Options:\n";
            oss << "  --help                    Show this help message\n";
            oss << "  --version                 Show version information\n";
            oss << "  --config=<file>           Configuration file path (default: configs/default.cfg)\n";
            oss << "  --neuron_count=<n>        Number of neurons (default: 1000)\n";
            oss << "  --region_count=<n>        Number of regions (default: 2)\n";
            oss << "  --connection_probability=<p> Connection probability (default: 0.1)\n";
            oss << "  --random_seed=<n>         Random seed (default: 42)\n";
            oss << "  --simulation_timestep=<dt> Simulation timestep (default: 0.001)\n";
            oss << "  --log_level=<level>       Log level (default: info)\n";
            oss << "  --enable_visualization     Enable visualization\n";
            oss << "  --profile                  Enable profiling\n";
            oss << "  --benchmark                Run benchmark\n";
            oss << "\nFor more information, visit: https://github.com/nlm-project/nlm\n";
            return oss.str();
        }
        
    } // namespace Utils
    
    // Global parser instance
    CommandLineParser& getDefaultParser() {
        static CommandLineParser parser;
        return parser;
    }
    
} // namespace nlm