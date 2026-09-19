#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * Simple CLI argument parser
 * Provides help/version flags and argument validation
 */
class SimpleCLI {
public:
    struct Option {
        std::string name;
        std::string shortName;
        std::string description;
        bool hasValue;
        bool required;
    };
    
    SimpleCLI(const std::string& programName, const std::string& version) 
        : programName(programName), version(version) {}
    
    // Add a command line option
    void addOption(const std::string& name, const std::string& shortName, 
                   const std::string& description, bool hasValue = false, 
                   bool required = false) {
        options[name] = {name, shortName, description, hasValue, required};
    }
    
    // Get option value with error checking
    std::string getOptionWithValidation(const std::string& name, const std::string& defaultValue = "") {
        std::string value = getOption(name, defaultValue);
        
        // Basic validation - check for empty values where not allowed
        if (value.empty() && hasOption(name) && !defaultValue.empty()) {
            return defaultValue;
        }
        
        return value;
    }
    
    // Get option value with error checking
    std::string getOptionWithValidation(const std::string& name, const std::string& defaultValue = "") {
        std::string value = getOption(name, defaultValue);
        
        // Basic validation - check for empty values where not allowed
        if (value.empty() && hasOption(name) && !defaultValue.empty()) {
            return defaultValue;
        }
        
        return value;
    }
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false; // Exit after help
            } else if (arg == "--version" || arg == "-v") {
                showVersion();
                return false; // Exit after version
            } else if (arg.substr(0, 2) == "--") {
                std::string optName = arg.substr(2);
                if (options.find(optName) != options.end()) {
                    Option& opt = options[optName];
                    if (!opt.hasValue) {
                        parsedOptions[optName] = "true";
                    } else {
                        if (i + 1 >= argc) {
                            std::cerr << "Error: Option --" << optName << " requires a value!" << std::endl;
                            showHelp();
                            return false;
                        }
                        parsedOptions[optName] = argv[++i];
                    }
                } else {
                    std::cerr << "Error: Unknown option --" << optName << "!" << std::endl;
                    showHelp();
                    return false;
                }
            } else if (arg.substr(0, 1) == "-") {
                std::string shortName = arg.substr(1);
                bool found = false;
                for (auto& pair : options) {
                    if (pair.second.shortName == shortName) {
                        Option& opt = pair.second;
                        if (!opt.hasValue) {
                            parsedOptions[pair.first] = "true";
                        } else {
                            if (i + 1 >= argc) {
                                std::cerr << "Error: Option -" << shortName << " requires a value!" << std::endl;
                                showHelp();
                                return false;
                            }
                            parsedOptions[pair.first] = argv[++i];
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cerr << "Error: Unknown option -" << shortName << "!" << std::endl;
                    showHelp();
                    return false;
                }
            } else {
                // Positional argument (for now, just store as config)
                positionalArgs.push_back(arg);
            }
        }
        
        // Validate required options
        for (const auto& pair : options) {
            if (pair.second.required && parsedOptions.find(pair.first) == parsedOptions.end()) {
                std::cerr << "Error: Required option --" << pair.first << " is missing!" << std::endl;
                showHelp();
                return false;
            }
        }
        
        return true;
    }
    
    // Check if an option was provided
    bool hasOption(const std::string& name) const {
        return parsedOptions.find(name) != parsedOptions.end();
    }
    
    // Get option value as string
    std::string getOption(const std::string& name, const std::string& defaultValue = "") const {
        auto it = parsedOptions.find(name);
        if (it != parsedOptions.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const {
        return positionalArgs;
    }
    
    // Show help message
    void showHelp() const {
        std::cout << "Usage: " << programName << " [OPTIONS] [ARGS]..." << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        
        for (const auto& pair : options) {
            const Option& opt = pair.second;
            std::cout << "  ";
            if (!opt.shortName.empty()) {
                std::cout << "-" << opt.shortName << ", ";
            }
            std::cout << "--" << opt.name;
            if (opt.hasValue) {
                std::cout << "=<value>";
            }
            std::cout << std::endl;
            std::cout << "      " << opt.description << std::endl;
            if (opt.required) {
                std::cout << "      (Required)" << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << "General Options:" << std::endl;
        std::cout << "  -h, --help      Show this help message" << std::endl;
        std::cout << "  -v, --version   Show version information" << std::endl;
        std::cout << std::endl;
    }
    
    // Show version information
    void showVersion() const {
        std::cout << programName << " version " << version << std::endl;
    }
    
private:
    std::string programName;
    std::string version;
    std::map<std::string, Option> options;
    std::map<std::string, std::string> parsedOptions;
    std::vector<std::string> positionalArgs;
};

// Helper function to print user-friendly error messages
void printUserFriendlyError(const std::string& error) {
    std::cerr << "Error: " << error << std::endl;
    std::cerr << std::endl;
    std::cerr << "Use '--help' for usage information." << std::endl;
}

// Helper function to parse integer with error checking
bool parseInt(const std::string& str, int64_t& result, int64_t min = INT64_MIN, int64_t max = INT64_MAX) {
    try {
        size_t pos;
        result = std::stoll(str, &pos);
        if (pos != str.length()) {
            return false; // Contains non-numeric characters
        }
        if (result < min || result > max) {
            return false; // Out of range
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Helper function to parse float with error checking
bool parseFloat(const std::string& str, float& result, float min = -std::numeric_limits<float>::max(), 
                float max = std::numeric_limits<float>::max()) {
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        if (pos != str.length()) {
            return false; // Contains non-numeric characters
        }
        if (val < min || val > max) {
            return false; // Out of range
        }
        result = val;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}