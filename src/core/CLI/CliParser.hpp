#pragma once

/**
 * Enhanced Command Line Interface Parser for NLM
 * 
 * Provides comprehensive CLI parsing capabilities for the NLM project,
 * including argument validation, help generation, and configuration management.
 */

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>

namespace nlm {

/**
 * CLI option definition
 */
struct CliOption {
    std::string name;
    std::string description;
    std::string defaultValue;
    std::string type;  // "string", "int", "double", "bool", "file"
    bool required;
    bool hasValue;
    std::function<bool(const std::string&)> validator;
    
    CliOption() : required(false), hasValue(false) {}
    
    CliOption(const std::string& n, const std::string& desc, const std::string& def = "", 
              const std::string& t = "string", bool req = false)
        : name(n), description(desc), defaultValue(def), type(t), required(req), hasValue(false) {
        // Set up default validators based on type
        if (type == "int") {
            validator = [](const std::string& v) {
                try {
                    std::stoi(v);
                    return true;
                } catch (...) {
                    return false;
                }
            };
        } else if (type == "double") {
            validator = [](const std::string& v) {
                try {
                    std::stod(v);
                    return true;
                } catch (...) {
                    return false;
                }
            };
        } else if (type == "bool") {
            validator = [](const std::string& v) {
                return v == "true" || v == "false" || v == "1" || v == "0";
            };
        } else if (type == "file") {
            validator = [](const std::string& v) {
                // File validator - can be enhanced to check file existence
                return !v.empty();
            };
        }
    }
};

/**
 * Parsed command line arguments
 */
struct ParsedArgs {
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    bool helpRequested;
    bool versionRequested;
    
    ParsedArgs() : helpRequested(false), versionRequested(false) {}
};

/**
 * Enhanced CLI parser
 */
class CliParser {
public:
    CliParser() = default;
    ~CliParser() = default;
    
    /**
     * Add a command line option
     */
    void addOption(const CliOption& option) {
        options_[option.name] = option;
    }
    
    /**
     * Add a standard option with name, description, and default value
     */
    void addOption(const std::string& name, const std::string& description, 
                   const std::string& defaultValue = "", 
                   const std::string& type = "string", bool required = false) {
        options_[name] = CliOption(name, description, defaultValue, type, required);
    }
    
    /**
     * Parse command line arguments
     */
    ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        
        // Process arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Help and version flags
            if (arg == "--help" || arg == "-h") {
                args.helpRequested = true;
                continue;
            }
            if (arg == "--version" || arg == "-v") {
                args.versionRequested = true;
                continue;
            }
            
            // Options with values (--option value)
            if (arg.find("--") == 0 && arg.length() > 2) {
                std::string optionName = arg.substr(2);
                
                // Check if next argument exists and is not another option
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string value = argv[++i];
                    
                    if (options_.find(optionName) != options_.end()) {
                        // Validate option
                        if (validateOption(optionName, value)) {
                            args.options[optionName] = value;
                            options_[optionName].hasValue = true;
                        } else {
                            std::cerr << "Error: Invalid value for option '--" << optionName << ": " 
                                      << value << std::endl;
                            args.helpRequested = true;  // Show help on error
                        }
                    } else {
                        std::cerr << "Error: Unknown option '--" << optionName << "'" << std::endl;
                        args.helpRequested = true;  // Show help on error
                    }
                } else {
                    std::cerr << "Error: Option '--" << optionName << "' requires a value" << std::endl;
                    args.helpRequested = true;  // Show help on error
                }
            } else if (arg.find('-') == 0 && arg.length() > 1) {
                // Short options (-o value)
                std::string optionName = arg.substr(1);
                
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string value = argv[++i];
                    
                    if (options_.find(optionName) != options_.end()) {
                        if (validateOption(optionName, value)) {
                            args.options[optionName] = value;
                            options_[optionName].hasValue = true;
                        } else {
                            std::cerr << "Error: Invalid value for option '-" << optionName << ": " 
                                      << value << std::endl;
                            args.helpRequested = true;
                        }
                    } else {
                        std::cerr << "Error: Unknown option '-" << optionName << "'" << std::endl;
                        args.helpRequested = true;
                    }
                } else {
                    std::cerr << "Error: Option '-" << optionName << "' requires a value" << std::endl;
                    args.helpRequested = true;
                }
            } else {
                // Positional argument
                args.positionalArgs.push_back(arg);
            }
        }
        
        return args;
    }
    
    /**
     * Get option value as string
     */
    std::string getOption(const std::string& name, const std::string& defaultValue = "") const {
        auto it = options_.find(name);
        if (it != options_.end()) {
            if (it->second.hasValue) {
                return it->second.defaultValue;  // Actually store value in defaultValue field
            } else if (!it->second.defaultValue.empty()) {
                return it->second.defaultValue;
            }
        }
        return defaultValue;
    }
    
    /**
     * Get option value as integer
     */
    int getOptionInt(const std::string& name, int defaultValue = 0) const {
        std::string value = getOption(name);
        if (value.empty()) return defaultValue;
        try {
            return std::stoi(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    /**
     * Get option value as double
     */
    double getOptionDouble(const std::string& name, double defaultValue = 0.0) const {
        std::string value = getOption(name);
        if (value.empty()) return defaultValue;
        try {
            return std::stod(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    /**
     * Get option value as boolean
     */
    bool getOptionBool(const std::string& name, bool defaultValue = false) const {
        std::string value = getOption(name);
        if (value.empty()) return defaultValue;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes";
    }
    
    /**
     * Check if option was provided
     */
    bool hasOption(const std::string& name) const {
        auto it = options_.find(name);
        return it != options_.end() && it->second.hasValue;
    }
    
    /**
     * Get all option names
     */
    std::vector<std::string> getOptionNames() const {
        std::vector<std::string> names;
        for (const auto& pair : options_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * Print help message
     */
    void printHelp(const std::string& programName) const {
        std::cout << "Usage: " << programName << " [options] [arguments]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        
        for (const auto& pair : options_) {
            const auto& option = pair.second;
            std::cout << "  --" << option.name;
            if (option.type != "bool") {
                std::cout << " " << option.type;
            }
            if (option.required) {
                std::cout << " (required)";
            }
            if (!option.defaultValue.empty()) {
                std::cout << " (default: " << option.defaultValue << ")";
            }
            std::cout << std::endl;
            std::cout << "    " << option.description << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Short options:" << std::endl;
        std::cout << "  -h, --help          Show this help message" << std::endl;
        std::cout << "  -v, --version       Show version information" << std::endl;
        
        // Group options by type for better organization
        std::cout << std::endl;
        std::cout << "Configuration options:" << std::endl;
        std::cout << "  --config <file>     Configuration file" << std::endl;
        std::cout << "  --neuron-count <N>  Number of neurons (default: 500)" << std::endl;
        std::cout << "  --timestep <t>      Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --seed <seed>       Random seed (default: 42)" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Experiment options:" << std::endl;
        std::cout << "  --experiment <name>  Run specific experiment" << std::endl;
        std::cout << "  --max-steps <N>      Maximum simulation steps" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Performance options:" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --benchmark         Run benchmark mode" << std::endl;
        std::cout << "  --profile           Enable performance profiling" << std::endl;
    }
    
    /**
     * Validate option value
     */
    bool validateOption(const std::string& optionName, const std::string& value) const {
        auto it = options_.find(optionName);
        if (it == options_.end()) {
            return false;  // Option doesn't exist
        }
        
        const auto& option = it->second;
        
        // Check custom validator first
        if (option.validator && !option.validator(value)) {
            return false;
        }
        
        // Basic type validation
        if (option.type == "int") {
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (option.type == "double") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (option.type == "bool") {
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return value == "true" || value == "false" || value == "1" || value == "0" || value == "yes" || value == "no";
        }
        
        return true;  // String and file types always pass basic validation
    }

private:
    std::map<std::string, CliOption> options_;
};

/**
 * Global CLI parser instance
 */
inline CliParser& getCliParser() {
    static CliParser parser;
    return parser;
}

/**
 * Convenience functions for CLI setup
 */
inline void setupStandardOptions(CliParser& parser) {
    // Configuration file option
    parser.addOption("config", "Configuration file path", "configs/default.cfg", "file");
    
    // Basic simulation options
    parser.addOption("neuron_count", "Number of neurons in the brain", "500", "int");
    parser.addOption("region_count", "Number of brain regions", "1", "int");
    parser.addOption("timestep", "Simulation timestep in seconds", "0.001", "double");
    parser.addOption("seed", "Random seed for reproducible experiments", "42", "int");
    
    // Connectivity options
    parser.addOption("connection_probability", "Probability of connection between neurons", "0.15", "double");
    
    // Experiment options
    parser.addOption("experiment", "Specific experiment to run", "all", "string");
    parser.addOption("max_steps", "Maximum simulation steps", "1000", "int");
    
    // Performance options
    parser.addOption("verbose", "Enable verbose output", "false", "bool");
    parser.addOption("benchmark", "Run benchmark mode", "false", "bool");
    parser.addOption("profile", "Enable performance profiling", "false", "bool");
}

/**
 * Parse command line arguments with standard options
 */
inline ParsedArgs parseCommandLine(int argc, char** argv) {
    CliParser& parser = getCliParser();
    setupStandardOptions(parser);
    return parser.parse(argc, argv);
}

/**
 * Get standard option value
 */
inline std::string getCliOption(const std::string& name, const std::string& defaultValue = "") {
    return getCliParser().getOption(name, defaultValue);
}

/**
 * Get standard option value as integer
 */
inline int getCliOptionInt(const std::string& name, int defaultValue = 0) {
    return getCliParser().getOptionInt(name, defaultValue);
}

/**
 * Get standard option value as double
 */
inline double getCliOptionDouble(const std::string& name, double defaultValue = 0.0) {
    return getCliParser().getOptionDouble(name, defaultValue);
}

/**
 * Get standard option value as boolean
 */
inline bool getCliOptionBool(const std::string& name, bool defaultValue = false) {
    return getCliParser().getOptionBool(name, defaultValue);
}

/**
 * Check if standard option was provided
 */
inline bool hasCliOption(const std::string& name) {
    return getCliParser().hasOption(name);
}

/**
 * Print CLI help
 */
inline void printCliHelp(const std::string& programName) {
    getCliParser().printHelp(programName);
}

} // namespace nlm