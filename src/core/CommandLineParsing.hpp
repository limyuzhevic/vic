#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

// Command line parsing result
struct CommandLineResult {
    std::vector<std::string> positionalArgs;
    std::map<std::string, std::string> options;
    std::map<std::string, std::vector<std::string>> multiOptions;
    std::vector<std::string> flags;
    bool helpRequested;
    bool versionRequested;
    std::string errorMessage;
    
    CommandLineResult() : helpRequested(false), versionRequested(false) {}
    
    bool isValid() const {
        return errorMessage.empty();
    }
};

// Enhanced command line parser
class CommandLineParser {
public:
    CommandLineParser() : helpTextShown(false), verbose(false), debug(false) {}
    
    // Parse command line arguments
    CommandLineResult parse(int argc, char** argv) {
        CommandLineResult result;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --help
            if (arg == "--help" || arg == "-h") {
                result.helpRequested = true;
                helpTextShown = true;
                continue;
            }
            
            // Handle --version
            if (arg == "--version" || arg == "-v") {
                result.versionRequested = true;
                helpTextShown = true;
                continue;
            }
            
            // Handle --key=value or --key value
            if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Handle flags
                if (isFlag(key)) {
                    result.flags.push_back(key);
                } else {
                    result.options[key] = value;
                }
            }
            // Handle -key value or --key (boolean)
            else if (arg[0] == '-' && arg.size() > 1) {
                std::string key = arg.substr(1);
                
                if (i + 1 < argc) {
                    std::string nextArg = argv[i + 1];
                    // Next argument is not a flag (doesn't start with -)
                    if (nextArg.substr(0, 2) != "--" && nextArg[0] != '-') {
                        result.options[key] = nextArg;
                        i++; // Skip the value
                        continue;
                    }
                }
                
                // Boolean flag
                if (isFlag(key)) {
                    result.flags.push_back(key);
                } else {
                    result.options[key] = "true";
                }
            }
            // Positional argument
            else {
                result.positionalArgs.push_back(arg);
            }
        }
        
        // Validate required options
        validateRequired(result);
        
        return result;
    }
    
    // Add command line option definition
    void addOption(const std::string& longOpt, const std::string& shortOpt = "",
                   const std::string& description = "",
                   const std::string& defaultValue = "",
                   bool required = false,
                   const std::string& type = "string",
                   const std::string& help = "") {
        OptionDef def;
        def.longOpt = longOpt;
        def.shortOpt = shortOpt;
        def.description = description;
        def.defaultValue = defaultValue;
        def.required = required;
        def.type = type;
        def.help = help;
        
        options.push_back(def);
        
        if (!shortOpt.empty()) {
            optionMap[shortOpt] = def.longOpt;
        }
        optionMap[longOpt] = longOpt;
    }
    
    // Add flag (boolean option)
    void addFlag(const std::string& longOpt, const std::string& shortOpt = "",
                 const std::string& description = "") {
        OptionDef def;
        def.longOpt = longOpt;
        def.shortOpt = shortOpt;
        def.description = description;
        def.type = "flag";
        
        options.push_back(def);
        
        if (!shortOpt.empty()) {
            optionMap[shortOpt] = def.longOpt;
        }
        optionMap[longOpt] = longOpt;
    }
    
    // Get help text
    std::string getHelpText() const {
        if (helpTextShown) {
            return "";
        }
        
        std::stringstream ss;
        ss << "Usage: nlm [options] [arguments]" << std::endl;
        ss << std::endl;
        ss << "Options:" << std::endl;
        
        for (const auto& opt : options) {
            ss << "  ";
            if (!opt.shortOpt.empty()) {
                ss << "-" << opt.shortOpt << ", ";
            }
            ss << "--" << opt.longOpt;
            
            if (!opt.defaultValue.empty()) {
                ss << "=" << opt.defaultValue;
            }
            
            ss << std::endl;
            ss << "    " << opt.description << std::endl;
            
            if (!opt.help.empty()) {
                ss << "    " << opt.help << std::endl;
            }
        }
        
        ss << std::endl;
        ss << "Flags:" << std::endl;
        for (const auto& opt : options) {
            if (opt.type == "flag") {
                ss << "  ";
                if (!opt.shortOpt.empty()) {
                    ss << "-" << opt.shortOpt << ", ";
                }
                ss << "--" << opt.longOpt << std::endl;
                ss << "    " << opt.description << std::endl;
            }
        }
        
        ss << std::endl;
        ss << "Arguments:" << std::endl;
        ss << "  [arguments]...   Positional arguments (e.g., file paths, IDs)" << std::endl;
        
        ss << std::endl;
        ss << "Examples:" << std::endl;
        ss << "  nlm --config=file.cfg --neuron_count=1000" << std::endl;
        ss << "  nlm -c file.cfg -n 1000" << std::endl;
        ss << "  nlm --enable_reward_modulation --enable_curiosity" << std::endl;
        ss << "  nlm my_simulation.cfg my_checkpoint.bin" << std::endl;
        
        return ss.str();
    }
    
    // Set verbose mode
    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }
    
    // Set debug mode
    void setDebug(bool debug) {
        this->debug = debug;
    }
    
    // Get parsed values
    std::string getOption(const CommandLineResult& result, const std::string& key,
                         const std::string& defaultValue = "") const {
        auto it = result.options.find(key);
        if (it != result.options.end()) {
            return it->second;
        }
        
        // Check for short option mapping
        auto shortIt = optionMap.find(key);
        if (shortIt != optionMap.end()) {
            it = result.options.find(shortIt->second);
            if (it != result.options.end()) {
                return it->second;
            }
        }
        
        return defaultValue;
    }
    
    // Get flag value
    bool getFlag(const CommandLineResult& result, const std::string& key) const {
        return std::find(result.flags.begin(), result.flags.end(), key) != result.flags.end();
    }
    
    // Get positional argument
    std::string getPositionalArg(const CommandLineResult& result, size_t index) const {
        if (index < result.positionalArgs.size()) {
            return result.positionalArgs[index];
        }
        return "";
    }
    
    // Get all positional arguments
    std::vector<std::string> getPositionalArgs(const CommandLineResult& result) const {
        return result.positionalArgs;
    }
    
    // Get multi-value option
    std::vector<std::string> getMultiOption(const CommandLineResult& result, const std::string& key) const {
        auto it = result.multiOptions.find(key);
        if (it != result.multiOptions.end()) {
            return it->second;
        }
        return std::vector<std::string>();
    }
    
    // Add multi-value option (like --include multiple times)
    void addMultiOption(const std::string& longOpt, const std::string& shortOpt = "",
                        const std::string& description = "") {
        OptionDef def;
        def.longOpt = longOpt;
        def.shortOpt = shortOpt;
        def.description = description;
        def.type = "multi";
        
        options.push_back(def);
        
        if (!shortOpt.empty()) {
            optionMap[shortOpt] = def.longOpt;
        }
        optionMap[longOpt] = longOpt;
    }
    
    // Validate required options
    void validateRequired(CommandLineResult& result) const {
        for (const auto& opt : options) {
            if (opt.required && opt.type != "flag") {
                bool found = false;
                
                // Check long option
                if (result.options.find(opt.longOpt) != result.options.end()) {
                    found = true;
                }
                
                // Check short option
                if (!opt.shortOpt.empty()) {
                    auto shortIt = optionMap.find(opt.shortOpt);
                    if (shortIt != optionMap.end() && result.options.find(shortIt->second) != result.options.end()) {
                        found = true;
                    }
                }
                
                if (!found) {
                    result.errorMessage = "Required option --" + opt.longOpt + " is missing";
                    if (!opt.shortOpt.empty()) {
                        result.errorMessage += " (-" + opt.shortOpt + ")";
                    }
                    return;
                }
            }
        }
    }
    
    // Check if a key is a flag
    bool isFlag(const std::string& key) const {
        auto it = std::find_if(options.begin(), options.end(), 
            [&key](const OptionDef& opt) {
                return opt.longOpt == key || opt.shortOpt == key;
            });
        
        return (it != options.end() && it->type == "flag");
    }
    
    // Print error message
    void printError(const std::string& error) const {
        if (verbose || debug) {
            std::cerr << "Error: " << error << std::endl;
        } else {
            std::cerr << error << std::endl;
        }
    }
    
    // Print help text
    void printHelp() const {
        std::cout << getHelpText();
    }
    
    // Print version
    void printVersion() const {
        std::cout << "NLM (Neural Learning Machine) v1.0.0" << std::endl;
        std::cout << "Enhanced Command Line Interface" << std::endl;
        std::cout << "Build with enhanced configuration and logging support" << std::endl;
    }

private:
    struct OptionDef {
        std::string longOpt;
        std::string shortOpt;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string type;
        std::string help;
    };
    
    std::vector<OptionDef> options;
    std::map<std::string, std::string> optionMap;
    bool helpTextShown;
    bool verbose;
    bool debug;
};

// Convenient helper functions for common command line options
namespace CommandLine {
    
    // Add common NLM options
    inline void addCommonOptions(CommandLineParser& parser) {
        parser.addOption("config", "c", "Load configuration from file", "configs/default.cfg");
        parser.addOption("neuron_count", "n", "Number of neurons in the brain", "1000");
        parser.addOption("region_count", "r", "Number of brain regions", "2");
        parser.addOption("simulation_timestep", "t", "Simulation timestep in seconds", "0.001");
        parser.addOption("random_seed", "s", "Random seed for reproducible experiments", "42");
        parser.addOption("log_level", "l", "Log level (trace, debug, info, warning, error, critical)", "info");
        parser.addOption("log_file", "", "Output log file", "");
        parser.addOption("max_steps", "", "Maximum simulation steps", "10000");
        
        parser.addFlag("enable_reward_modulation", "", "Enable reward-based learning");
        parser.addFlag("enable_structural_plasticity", "", "Enable structural plasticity (synaptogenesis/pruning)");
        parser.addFlag("enable_development", "", "Enable brain development");
        parser.addFlag("enable_curiosity", "", "Enable curiosity-driven exploration");
        parser.addFlag("enable_stdp", "", "Enable Spike-Timing-Dependent Plasticity");
        parser.addFlag("enable_hebbian", "", "Enable Hebbian learning");
        parser.addFlag("verbose", "v", "Enable verbose output");
        parser.addFlag("debug", "", "Enable debug output (includes trace information)");
        parser.addFlag("quiet", "q", "Suppress informational messages");
        
        parser.addMultiOption("include", "i", "Include additional configuration files");
        parser.addMultiOption("plugin", "", "Load plugin module");
    }
    
    // Parse and validate arguments
    inline CommandLineResult parse(int argc, char** argv) {
        CommandLineParser parser;
        addCommonOptions(parser);
        return parser.parse(argc, argv);
    }
    
    // Print help text
    inline void printHelp() {
        CommandLineParser parser;
        addCommonOptions(parser);
        parser.printHelp();
    }
    
    // Print version
    inline void printVersion() {
        CommandLineParser parser;
        addCommonOptions(parser);
        parser.printVersion();
    }
    
    // Parse and validate, exit on error
    inline CommandLineResult parseAndValidate(int argc, char** argv) {
        CommandLineParser parser;
        addCommonOptions(parser);
        
        CommandLineResult result = parser.parse(argc, argv);
        
        if (!result.isValid()) {
            parser.printError(result.errorMessage);
            std::exit(1);
        }
        
        // Set verbose/debug flags
        if (result.flags.empty() && (parser.verbose || parser.debug)) {
            // Set from parser internal state (for backwards compatibility)
        }
        
        return result;
    }
    
    // Convert string to boolean
    inline bool toBool(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        }
        
        throw std::invalid_argument("Invalid boolean string: " + str);
    }
    
    // Convert string to integer
    inline int toInt(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid integer string: " + str);
        }
    }
    
    // Convert string to double
    inline double toDouble(const std::string& str) {
        try {
            return std::stod(str);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid double string: " + str);
        }
    }
    
    // Convert string to LogLevel
    inline LogLevel toLogLevel(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "trace") return LogLevel::Trace;
        if (lower == "debug") return LogLevel::Debug;
        if (lower == "info") return LogLevel::Info;
        if (lower == "notice") return LogLevel::Notice;
        if (lower == "warning") return LogLevel::Warning;
        if (lower == "error") return LogLevel::Error;
        if (lower == "critical") return LogLevel::Critical;
        if (lower == "alert") return LogLevel::Alert;
        if (lower == "emergency") return LogLevel::Emergency;
        
        throw std::invalid_argument("Invalid log level: " + str);
    }
}

// Convenience functions for backward compatibility
inline std::string getOptionValue(const CommandLineResult& result, const std::string& key,
                                 const std::string& defaultValue = "") {
    static CommandLineParser parser;
    return parser.getOption(result, key, defaultValue);
}

inline bool getFlagValue(const CommandLineResult& result, const std::string& key) {
    static CommandLineParser parser;
    return parser.getFlag(result, key);
}

inline std::string getPositionalArg(const CommandLineResult& result, size_t index) {
    static CommandLineParser parser;
    return parser.getPositionalArg(result, index);
}

inline std::vector<std::string> getPositionalArgs(const CommandLineResult& result) {
    static CommandLineParser parser;
    return parser.getPositionalArgs(result);
}

} // namespace nlm

// Version functions for backwards compatibility
namespace nlm {
    std::string getCommandLineVersion() {
        return "Enhanced Command Line Parser v1.0.0";
    }
}

// Macro for common option parsing
#define PARSE_COMMON_OPTIONS(result) \
    do { \
        nlm::CommandLineResult parsed = nlm::CommandLine::parse(argc, argv); \
        result = parsed; \
        if (!parsed.isValid()) { \
            std::cerr << "Error: " << parsed.errorMessage << std::endl; \
            nlm::CommandLine::printHelp(); \
            std::exit(1); \
        } \
    } while(0)

// Macro for parsing with validation
#define PARSE_AND_VALIDATE(argc, argv) nlm::CommandLine::parseAndValidate(argc, argv)