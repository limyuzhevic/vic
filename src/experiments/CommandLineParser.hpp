// Common Command Line Argument Parser for NLM Experiments
// Provides consistent command-line parsing across all NLM experiment demos

#ifndef NLM_COMMAND_LINE_PARSER_HPP
#define NLM_COMMAND_LINE_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Command line argument parser with support for flags, options, and help
class CommandLineParser {
public:
    struct Argument {
        std::string name;
        std::string description;
        std::string type;
        std::string defaultValue;
        bool required;
        bool hasValue;
    };
    
    CommandLineParser(const std::string& programName, const std::string& description)
        : programName_(programName), description_(description) {}
    
    void addArgument(const std::string& name, const std::string& description, 
                     const std::string& type = "int", 
                     const std::string& defaultValue = "", 
                     bool required = false, bool hasValue = true) {
        Argument arg;
        arg.name = name;
        arg.description = description;
        arg.type = type;
        arg.defaultValue = defaultValue;
        arg.required = required;
        arg.hasValue = hasValue;
        
        arguments_[name] = arg;
        argumentOrder_.push_back(name);
    }
    
    bool parse(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        return parseArguments(args);
    }
    
    bool parse(const std::vector<std::string>& args) {
        return parseArguments(args);
    }
    
    bool hasArgument(const std::string& name) const {
        return arguments_.find(name) != arguments_.end();
    }
    
    std::string getArgument(const std::string& name) const {
        auto it = parsedArguments_.find(name);
        if (it != parsedArguments_.end()) {
            return it->second;
        }
        
        auto argIt = arguments_.find(name);
        if (argIt != arguments_.end() && !argIt->second.defaultValue.empty()) {
            return argIt->second.defaultValue;
        }
        
        return "";
    }
    
    int getArgumentAsInt(const std::string& name, int defaultValue = 0) const {
        std::string value = getArgument(name);
        if (value.empty()) {
            return defaultValue;
        }
        
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    double getArgumentAsDouble(const std::string& name, double defaultValue = 0.0) const {
        std::string value = getArgument(name);
        if (value.empty()) {
            return defaultValue;
        }
        
        try {
            return std::stod(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    float getArgumentAsFloat(const std::string& name, float defaultValue = 0.0f) const {
        std::string value = getArgument(name);
        if (value.empty()) {
            return defaultValue;
        }
        
        try {
            return std::stof(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    std::string getArgumentAsString(const std::string& name, const std::string& defaultValue = "") const {
        return getArgument(name).empty() ? defaultValue : getArgument(name);
    }
    
    bool hasFlag(const std::string& name) const {
        if (!hasArgument(name)) {
            return false;
        }
        
        std::string value = getArgument(name);
        return value == "true" || value == "1" || value == "yes" || 
               value == "on" || value == "enable";
    }
    
    void displayHelp() const {
        std::cout << "Usage: " << programName_ << " [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << description_ << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        
        for (const auto& name : argumentOrder_) {
            auto& arg = arguments_.at(name);
            
            std::cout << "  --" << name;
            if (arg.hasValue) {
                std::cout << "=<value>";
            }
            std::cout << std::endl;
            
            std::cout << "    " << arg.description << std::endl;
            
            if (!arg.defaultValue.empty()) {
                std::cout << "    Default: " << arg.defaultValue << std::endl;
            }
            
            if (arg.required) {
                std::cout << "    Required: yes" << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName_ << " --trials 100" << std::endl;
        std::cout << "  " << programName_ << " --epochs 50 --batch-size 32" << std::endl;
        std::cout << "  " << programName_ << " --help" << std::endl;
    }
    
    std::string getUsage() const {
        std::ostringstream oss;
        oss << programName_ << " [OPTIONS]";
        return oss.str();
    }
    
private:
    bool parseArguments(const std::vector<std::string>& args) {
        std::vector<std::string> remainingArgs;
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                displayHelp();
                return false;
            }
            
            if (arg.substr(0, 2) == "--") {
                // Check if this is an option with value (--name=value)
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string name = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (hasArgument(name)) {
                        parsedArguments_[name] = value;
                    } else {
                        std::cerr << "Unknown option: --" << name << std::endl;
                        return false;
                    }
                } else {
                    // Check if this is a flag (--name true/false)
                    std::string name = arg.substr(2);
                    if (hasArgument(name) && arguments_[name].hasValue) {
                        // If no value provided, use "true" for boolean flags
                        if (i + 1 < args.size() && args[i + 1][0] != '-') {
                            parsedArguments_[name] = args[++i];
                        } else {
                            parsedArguments_[name] = "true";
                        }
                    } else {
                        std::cerr << "Unknown option: --" << name << std::endl;
                        return false;
                    }
                }
            } else if (arg[0] == '-') {
                // Short option (-n value)
                std::string name = arg.substr(1);
                if (hasArgument(name)) {
                    if (i + 1 < args.size() && args[i + 1][0] != '-') {
                        parsedArguments_[name] = args[++i];
                    }
                } else {
                    std::cerr << "Unknown option: -" << name << std::endl;
                    return false;
                }
            } else {
                remainingArgs.push_back(arg);
            }
        }
        
        // Check for required arguments
        for (const auto& pair : arguments_) {
            const auto& arg = pair.second;
            if (arg.required && parsedArguments_.find(arg.name) == parsedArguments_.end()) {
                std::cerr << "Error: Required option --" << arg.name << " is missing" << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    std::string programName_;
    std::string description_;
    std::map<std::string, Argument> arguments_;
    std::vector<std::string> argumentOrder_;
    std::map<std::string, std::string> parsedArguments_;
};

// Template for common experiment demo setup
inline bool parseExperimentArguments(int argc, char* argv[],
                                     size_t& numTrials, size_t& stepsPerEpisode,
                                     size_t& neuronCount, float& timestep) {
    CommandLineParser parser("nlm_experiment", "NLM Experiment Demo");
    
    parser.addArgument("trials", "Number of experimental trials", "int", "50");
    parser.addArgument("steps", "Number of steps per episode", "int", "200");
    parser.addArgument("neurons", "Number of neurons in the brain", "int", "1000");
    parser.addArgument("timestep", "Simulation timestep (seconds)", "float", "0.001");
    parser.addArgument("seed", "Random seed for reproducibility", "int", "42");
    parser.addArgument("help", "Display help information", "bool", "false");
    
    if (!parser.parse(argc, argv)) {
        return false;
    }
    
    numTrials = parser.getArgumentAsInt("trials");
    stepsPerEpisode = parser.getArgumentAsInt("steps");
    neuronCount = static_cast<size_t>(parser.getArgumentAsInt("neurons"));
    timestep = parser.getArgumentAsFloat("timestep");
    
    int seed = parser.getArgumentAsInt("seed");
    if (seed != 0) {
        // Set random seed if provided
    }
    
    return true;
}

} // namespace nlm

#endif // NLM_COMMAND_LINE_PARSER_HPP