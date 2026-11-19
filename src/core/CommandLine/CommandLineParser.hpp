// Advanced command-line parser for NLM applications
// Supports complex command-line arguments for debugging, visualization, and configuration

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cstdint>

namespace nlm {

// Command-line argument types
enum class ArgumentType {
    String,
    Int,
    Double,
    Bool,
    File,
    Directory
};

// Command-line option definition
struct CommandLineOption {
    std::string flag;
    ArgumentType type;
    std::string description;
    std::string defaultValue;
    std::function<void(const std::string&)> action;
    bool isAdvanced = false;
};

// Command-line argument parser
class CommandLineParser {
public:
    CommandLineParser() : debugLevel(0), verbose(false) {}
    
    // Add a command-line option
    void addOption(const std::string& flag, ArgumentType type, 
                   const std::string& description, 
                   const std::string& defaultValue = "",
                   const std::function<void(const std::string&)>& action = nullptr,
                   bool isAdvanced = false) {
        CommandLineOption option;
        option.flag = flag;
        option.type = type;
        option.description = description;
        option.defaultValue = defaultValue;
        option.action = action;
        option.isAdvanced = isAdvanced;
        options[flag] = option;
    }
    
    // Parse command-line arguments
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    processArgument(key, value);
                } else {
                    std::cerr << "Warning: Missing value for argument --" << key << std::endl;
                }
            }
            // Handle -k value format
            else if (arg[0] == '-' && arg.size() > 1) {
                std::string key = arg.substr(1);
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    processArgument(key, value);
                } else {
                    std::cerr << "Warning: Missing value for argument -" << key << std::endl;
                }
            }
            // Handle boolean flags
            else if (arg == "--debug" || arg == "--verbose" || 
                     arg == "--help" || arg == "--version" ||
                     arg == "--quiet" || arg == "--force") {
                processFlag(arg);
            }
        }
        return true;
    }
    
    // Get value for a specific option
    template<typename T>
    std::optional<T> getValue(const std::string& flag) const {
        auto it = parsedArgs.find(flag);
        if (it == parsedArgs.end()) {
            return std::nullopt;
        }
        // This is a simplified implementation
        return std::nullopt;
    }
    
    // Get boolean flag value
    bool getFlag(const std::string& flag) const {
        return flags.find(flag) != flags.end();
    }
    
    // Get debug level
    int getDebugLevel() const { return debugLevel; }
    
    // Get verbose flag
    bool isVerbose() const { return verbose; }
    
    // Show help for all options
    void showHelp(bool showAdvanced = false) const {
        std::cout << "NLM Advanced Command Line Options:\n";
        std::cout << "=============================\n\n";
        
        std::cout << "Basic Options:\n";
        for (const auto& pair : options) {
            const auto& option = pair.second;
            if (!option.isAdvanced) {
                std::cout << "  --" << option.flag;
                if (option.type != ArgumentType::Bool) {
                    std::cout << "=<value>";
                }
                std::cout << "\n";
                std::cout << "    " << option.description << "\n";
                if (!option.defaultValue.empty()) {
                    std::cout << "    Default: " << option.defaultValue << "\n";
                }
                std::cout << "\n";
            }
        }
        
        if (showAdvanced) {
            std::cout << "\nAdvanced Options:\n";
            for (const auto& pair : options) {
                const auto& option = pair.second;
                if (option.isAdvanced) {
                    std::cout << "  --" << option.flag;
                    if (option.type != ArgumentType::Bool) {
                        std::cout << "=<value>";
                    }
                    std::cout << "\n";
                    std::cout << "    " << option.description << "\n";
                    if (!option.defaultValue.empty()) {
                        std::cout << "    Default: " << option.defaultValue << "\n";
                    }
                    std::cout << "\n";
                }
            }
        }
        
        std::cout << "Boolean Flags:\n";
        std::cout << "  --debug        Enable debug output\n";
        std::cout << "  --verbose      Enable verbose logging\n";
        std::cout << "  --quiet        Quiet mode (minimal output)\n";
        std::cout << "  --force        Force operation (override safety checks)\n";
        std::cout << "  --help         Show this help message\n";
        std::cout << "  --version      Show version information\n";
    }
    
    // Process an argument
    void processArgument(const std::string& key, const std::string& value) {
        auto it = options.find(key);
        if (it != options.end()) {
            const auto& option = it->second;
            if (option.action) {
                option.action(value);
            }
            parsedArgs[key] = value;
            
            // Handle special arguments
            if (key == "debug") {
                debugLevel = std::stoi(value);
            }
        } else {
            std::cerr << "Warning: Unknown argument --" << key << std::endl;
        }
    }
    
    // Process a flag
    void processFlag(const std::string& flag) {
        if (flag == "--debug") {
            debugLevel = 1;
        } else if (flag == "--verbose") {
            verbose = true;
        } else if (flag == "--quiet") {
            verbose = false;
        } else if (flag == "--force") {
            flags.insert("force");
        }
        
        flags.insert(flag.substr(2)); // Remove --
        parsedArgs[flag.substr(2)] = "true";
    }
    
    // Get parsed arguments
    const std::map<std::string, std::string>& getParsedArgs() const { return parsedArgs; }
    
    // Get flags
    const std::set<std::string>& getFlags() const { return flags; }
    
private:
    std::map<std::string, CommandLineOption> options;
    std::map<std::string, std::string> parsedArgs;
    std::set<std::string> flags;
    int debugLevel;
    bool verbose;
};

// Common option definitions for NLM applications
namespace common_options {
    void setupOptions(CommandLineParser& parser);
}

// Configuration file loading
class ConfigFileLoader {
public:
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    static bool saveConfig(const std::string& filepath, const std::map<std::string, std::string>& config);
    static bool validateConfig(const std::map<std::string, std::string>& config);
};

} // namespace nlm
