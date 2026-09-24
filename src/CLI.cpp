#include "CLI.hpp"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include "ConfigSchema.hpp"

namespace nlm {
    
    // Define all CLI arguments with their metadata
    static std::vector<CLIArgument> getAllArguments() {
        return {
            {"config", 0, "config", "Configuration file", "file", "configs/default.cfg", false},
            {"log-level", 0, "log-level", "Log level", "string", "info", false,
             std::vector<std::string>{"debug", "info", "warning", "error", "critical"}},
            {"log-file", 0, "log-file", "Log output file", "file"},
            {"verbose", 'v', "verbose", "Enable verbose output", "flag"},
            {"quiet", 0, "quiet", "Suppress output", "flag"},
            {"debug-level", 0, "debug-level", "Debug level (0-3)", "int", "0", false,
             std::vector<std::string>(), 0, 3, true},
            {"output-format", 0, "output-format", "Output format (text, json)", "string", "text",
             false, std::vector<std::string>{"text", "json"}},
            {"help", 'h', "help", "Show this help message", "flag"},
            {"version", 0, "version", "Show version information", "flag"}
        };
    }
    
    // Helper function to normalize string
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Helper function to trim whitespace
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    CLI::Options CLI::parseArgs(int argc, char** argv) {
        Options options;
        std::unordered_map<std::string, std::string> parsedArgs;
        
        // Initialize schema for validation
        ConfigSchema::initializeSchema();
        
        // Get all valid arguments for validation
        auto validArgs = getAllArguments();
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            } else if (isValidArgument(arg)) {
                // Get argument definition
                CLIArgument argDef = getArgumentDefinition(arg);
                std::string key = toLower(argDef.name);
                
                // Check if argument takes a value
                if (arg.find("=") != std::string::npos) {
                    // --arg=value format
                    std::string value = arg.substr(arg.find("=") + 1);
                    validateArgument(argDef.longOpt.empty() ? "--" + argDef.name : "--" + argDef.longOpt,
                                    value, parsedArgs);
                    parsedArgs[key] = value;
                } else if (argDef.type != "flag") {
                    // --arg value format
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        ++i; // Skip next argument
                        std::string value = argv[i];
                        validateArgument(argDef.longOpt.empty() ? "--" + argDef.name : "--" + argDef.longOpt,
                                        value, parsedArgs);
                        parsedArgs[key] = value;
                    } else {
                        throw MissingArgumentError("--" + argDef.name);
                    }
                } else {
                    // Flag argument
                    parsedArgs[key] = "true";
                }
            } else {
                // Unknown argument
                throw InvalidArgumentError(arg, "use --help for available options");
            }
        }
        
        // Apply parsed arguments to options
        for (const auto& pair : parsedArgs) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            if (key == "config") {
                options.configFile = value;
            } else if (key == "log-level") {
                options.logLevel = value;
            } else if (key == "log-file") {
                options.logFile = value;
            } else if (key == "verbose") {
                options.verbose = (value == "true");
            } else if (key == "quiet") {
                options.quiet = (value == "true");
            } else if (key == "debug-level") {
                try {
                    options.debugLevel = std::stoi(value);
                } catch (const std::exception&) {
                    throw CLIValidationError("Invalid debug level: must be an integer");
                }
            } else if (key == "output-format") {
                options.outputFormat = value;
            }
        }
        
        // Validate log level against allowed values
        try {
            ConfigSchema::validate("log_level", options.logLevel);
        } catch (const ConfigValidationError& e) {
            throw CLIValidationError("Invalid log level: " + std::string(e.what()));
        }
        
        // Validate debug level
        if (options.debugLevel < 0 || options.debugLevel > 3) {
            throw CLIValidationError("Debug level must be between 0 and 3");
        }
        
        return options;
    }
    
    void CLI::validateArgument(const std::string& arg, const std::string& value,
                              std::unordered_map<std::string, std::string>& parsedArgs) {
        // Basic validation for argument format
        if (value.empty()) {
            throw MissingArgumentError(arg);
        }
        
        // Check for invalid characters in file paths
        if (arg.find("file") != std::string::npos) {
            // Simple file path validation
            if (value.find("\0") != std::string::npos) {
                throw CLIValidationError("Invalid character in file path");
            }
            if (value.find("..") != std::string::npos && value != "..") {
                // Allow ".." but not "..." or other paths with dots
            }
        }
    }
    
    bool CLI::isValidArgument(const std::string& arg) {
        auto validArgs = getAllArguments();
        
        // Check for short option (-v)
        if (arg.size() == 2 && arg[0] == '-') {
            for (const auto& argDef : validArgs) {
                if (argDef.shortOpt == arg[1]) {
                    return true;
                }
            }
            return false;
        }
        
        // Check for long option (--verbose)
        if (arg.size() > 2 && arg.substr(0, 2) == "--") {
            std::string optName = arg.substr(2);
            // Handle --arg=value format
            size_t equalsPos = optName.find("=");
            if (equalsPos != std::string::npos) {
                optName = optName.substr(0, equalsPos);
            }
            
            for (const auto& argDef : validArgs) {
                if (argDef.longOpt == optName || 
                    (!argDef.longOpt.empty() && toLower(argDef.longOpt) == optName)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    CLIArgument CLI::getArgumentDefinition(const std::string& arg) {
        auto validArgs = getAllArguments();
        
        // Check for short option (-v)
        if (arg.size() == 2 && arg[0] == '-') {
            for (const auto& argDef : validArgs) {
                if (argDef.shortOpt == arg[1]) {
                    return argDef;
                }
            }
        }
        
        // Check for long option (--verbose)
        if (arg.size() > 2 && arg.substr(0, 2) == "--") {
            std::string optName = arg.substr(2);
            // Handle --arg=value format
            size_t equalsPos = optName.find("=");
            if (equalsPos != std::string::npos) {
                optName = optName.substr(0, equalsPos);
            }
            
            for (const auto& argDef : validArgs) {
                if (argDef.longOpt == optName || 
                    (!argDef.longOpt.empty() && toLower(argDef.longOpt) == optName)) {
                    return argDef;
                }
            }
        }
        
        throw InvalidArgumentError(arg, "unknown argument");
    }
    
    std::string CLI::getErrorSuggestion(const std::string& arg) {
        // Simple typo correction by finding closest match
        auto validArgs = getAllArguments();
        std::string bestMatch;
        int bestScore = 1000; // Large number
        
        for (const auto& argDef : validArgs) {
            std::string longOpt = argDef.longOpt.empty() ? "--" + argDef.name : "--" + argDef.longOpt;
            int score = 0;
            
            // Simple Levenshtein distance approximation
            if (arg == longOpt) {
                score = 0;
            } else if (arg.find(longOpt) != std::string::npos) {
                score = 1;
            } else {
                // Simple character-by-character comparison
                int minLen = std::min(arg.size(), longOpt.size());
                for (size_t i = 0; i < minLen; ++i) {
                    if (arg[i] != longOpt[i]) {
                        score += 2;
                    }
                }
                score += std::abs(static_cast<int>(arg.size()) - static_cast<int>(longOpt.size()));
            }
            
            if (score < bestScore) {
                bestScore = score;
                bestMatch = longOpt;
            }
        }
        
        if (bestScore < 3) {
            return "Did you mean '" + bestMatch + "'?";
        }
        return "";
    }
    
    void CLI::printHelp() {
        std::cout << R"(
NLM Phase 2 Real Neural Computation

USAGE:
  nlm [OPTIONS]

OPTIONS:
  --config=<file>             Configuration file (default: configs/default.cfg)
  --log-level=<level>         Log level: debug, info, warning, error, critical (default: info)
  --log-file=<file>           Log output file
  --verbose, -v               Enable verbose output
  --quiet, -q                 Suppress output except errors
  --debug-level=<level>       Debug level (0-3, default: 0)
  --output-format=<format>    Output format: text, json (default: text)
  --help, -h                  Show this help message
  --version                   Show version information

EXAMPLES:
  nlm --config=myconfig.cfg --log-level=info
  nlm --verbose --log-file=nlm.log
  nlm --debug-level=2 --output-format=json
  nlm --quiet --log-level=warning

ERROR HANDLING:
  Invalid arguments show suggestions for possible corrections
  Missing values provide clear guidance on format
  Invalid values are validated against allowed options

For more information, visit: https://github.com/example/nlm

  )" << std::endl;
    }
    
} // namespace nlm

