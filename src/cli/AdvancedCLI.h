#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <regex>

/**
 * Advanced CLI argument parser with validation and completion
 * Provides comprehensive command-line interface for NLM
 */
class AdvancedCLI {
public:
    struct Option {
        std::string name;
        std::string shortName;
        std::string description;
        bool hasValue;
        bool required;
        std::string valueType; // "int", "float", "string", "bool"
        std::string validationPattern; // Regex pattern for string validation
        std::string helpText; // Extended help text
        
        Option() : hasValue(false), required(false), valueType("string") {}
        
        Option(const std::string& n, const std::string& sn, 
               const std::string& desc, bool hv = false, bool req = false,
               const std::string& vt = "string", const std::string& vp = "",
               const std::string& ht = "")
            : name(n), shortName(sn), description(desc), hasValue(hv), 
              required(req), valueType(vt), validationPattern(vp), helpText(ht) {}
    };
    
    AdvancedCLI(const std::string& programName, const std::string& version) 
        : programName(programName), version(version), exitOnHelp(true) {
        // Add built-in options
        addBuiltInOptions();
    }
    
    // Add a command line option with type validation
    void addOption(const std::string& name, const std::string& shortName, 
                   const std::string& description, bool hasValue = false, 
                   bool required = false, const std::string& valueType = "string",
                   const std::string& validationPattern = "",
                   const std::string& helpText = "") {
        options[name] = Option(name, shortName, description, hasValue, required,
                               valueType, validationPattern, helpText);
    }
    
    // Parse command line arguments with validation
    bool parse(int argc, char* argv[]) {
        parsedOptions.clear();
        positionalArgs.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false; // Exit after help
            } else if (arg == "--version" || arg == "-v") {
                showVersion();
                return false; // Exit after version
            } else if (arg == "--verbose" || arg == "-V") {
                parsedOptions["verbose"] = "true";
            } else if (arg == "--quiet" || arg == "-q") {
                parsedOptions["quiet"] = "true";
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
                        std::string value = argv[++i];
                        
                        // Validate value based on option type
                        if (!validateOptionValue(optName, value)) {
                            std::cerr << "Error: Invalid value for option --" << optName << ": " << value << std::endl;
                            showHelp();
                            return false;
                        }
                        
                        parsedOptions[optName] = value;
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
                            std::string value = argv[++i];
                            
                            if (!validateOptionValue(pair.first, value)) {
                                std::cerr << "Error: Invalid value for option -" << shortName << ": " << value << std::endl;
                                showHelp();
                                return false;
                            }
                            
                            parsedOptions[pair.first] = value;
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
                // Positional argument
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
    
    // Get option value with type conversion
    template<typename T>
    T getOption(const std::string& name, const T& defaultValue = T()) const {
        auto it = parsedOptions.find(name);
        if (it == parsedOptions.end()) {
            return defaultValue;
        }
        
        const std::string& strValue = it->second;
        
        // Convert based on type
        if constexpr (std::is_same_v<T, std::string>) {
            return strValue;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            try {
                return std::stoll(strValue);
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid integer value for option --" + name + ": " + strValue);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                return std::stod(strValue);
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid float value for option --" + name + ": " + strValue);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Parse boolean values
            std::string lowerValue = strValue;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on");
        } else {
            return defaultValue;
        }
    }
    
    // Check if option was provided
    bool hasOption(const std::string& name) const {
        return parsedOptions.find(name) != parsedOptions.end();
    }
    
    // Get raw string value
    std::string getRawOption(const std::string& name, const std::string& defaultValue = "") const {
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
    
    // Show comprehensive help
    void showHelp() const {
        std::cout << "Usage: " << programName << " [OPTIONS] [ARGS]..." << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        
        // Group options by category
        std::vector<std::pair<std::string, std::vector<Option*>>> categories;
        
        // General options
        std::vector<Option*> generalOpts;
        if (options.find("verbose") != options.end()) generalOpts.push_back(&options.at("verbose"));
        if (options.find("quiet") != options.end()) generalOpts.push_back(&options.at("quiet"));
        if (options.find("help") != options.end()) generalOpts.push_back(&options.at("help"));
        if (options.find("version") != options.end()) generalOpts.push_back(&options.at("version"));
        if (!generalOpts.empty()) {
            categories.push_back({"General Options", generalOpts});
        }
        
        // Other options
        std::vector<Option*> otherOpts;
        for (const auto& pair : options) {
            if (pair.first != "verbose" && pair.first != "quiet" && 
                pair.first != "help" && pair.first != "version") {
                otherOpts.push_back(&pair.second);
            }
        }
        if (!otherOpts.empty()) {
            categories.push_back({"Application Options", otherOpts});
        }
        
        // Display options by category
        for (const auto& category : categories) {
            std::cout << category.first << ":" << std::endl;
            for (const Option* opt : category.second) {
                std::cout << "  ";
                if (!opt->shortName.empty()) {
                    std::cout << "-" << opt->shortName << ", ";
                }
                std::cout << "--" << opt->name;
                if (opt->hasValue) {
                    std::cout << "=<" << opt->valueType << ">";
                }
                std::cout << std::endl;
                std::cout << "      " << opt->description << std::endl;
                if (!opt->helpText.empty()) {
                    std::cout << "      " << opt->helpText << std::endl;
                }
                if (opt->required) {
                    std::cout << "      (Required)" << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName << " --config demo.cfg --verbose" << std::endl;
        std::cout << "  " << programName << " -c myconfig.cfg -v -h" << std::endl;
        std::cout << std::endl;
    }
    
    // Show version information
    void showVersion() const {
        std::cout << programName << " version " << version << std::endl;
        std::cout << "Built for NLM Phase 2 Neural Learning Machine" << std::endl;
    }
    
    // Enable/disable exit on help
    void setExitOnHelp(bool enabled) {
        exitOnHelp = enabled;
    }
    
private:
    std::string programName;
    std::string version;
    bool exitOnHelp;
    std::map<std::string, Option> options;
    std::map<std::string, std::string> parsedOptions;
    std::vector<std::string> positionalArgs;
    
    // Add built-in options
    void addBuiltInOptions() {
        addOption("help", "h", "Show this help message", false, false, "string", "",
                 "Display comprehensive help information including examples and all available options");
        addOption("version", "v", "Show version information", false, false, "string", "",
                 "Display program version and build information");
        addOption("verbose", "V", "Enable verbose output", false, false, "bool", "",
                 "Increase output verbosity for debugging and monitoring");
        addOption("quiet", "q", "Suppress output", false, false, "bool", "",
                 "Reduce output to minimal information");
    }
    
    // Validate option value based on type and pattern
    bool validateOptionValue(const std::string& optName, const std::string& value) const {
        auto it = options.find(optName);
        if (it == options.end()) {
            return true; // Unknown option, let other validation handle it
        }
        
        const Option& opt = it->second;
        
        // Validate based on value type
        if (opt.valueType == "int" || opt.valueType == "int64_t") {
            try {
                std::stoll(value);
            } catch (const std::exception&) {
                return false;
            }
        } else if (opt.valueType == "float" || opt.valueType == "double") {
            try {
                std::stod(value);
            } catch (const std::exception&) {
                return false;
            }
        } else if (opt.valueType == "bool") {
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue != "true" && lowerValue != "false" && 
                lowerValue != "1" && lowerValue != "0" &&
                lowerValue != "yes" && lowerValue != "no" &&
                lowerValue != "on" && lowerValue != "off") {
                return false;
            }
        }
        
        // Validate regex pattern if provided
        if (!opt.validationPattern.empty()) {
            try {
                std::regex pattern(opt.validationPattern);
                if (!std::regex_match(value, pattern)) {
                    return false;
                }
            } catch (const std::exception&) {
                // Invalid regex pattern, skip validation
            }
        }
        
        return true;
    }
};

// Advanced helper functions
namespace AdvancedHelpers {
    
    // Validate and parse integer with comprehensive error handling
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
    
    // Validate and parse float with comprehensive error handling
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
    
    // Validate string format (email, IP, etc.)
    bool validateStringFormat(const std::string& str, const std::string& format) {
        try {
            std::regex pattern(format);
            return std::regex_match(str, pattern);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Parse boolean with multiple formats
    bool parseBool(const std::string& str) {
        std::string lowerValue = str;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on");
    }
    
    // Generate user-friendly error message
    void printUserFriendlyError(const std::string& error, bool showHelp = true) {
        std::cerr << "Error: " << error << std::endl;
        std::cerr << std::endl;
        if (showHelp) {
            std::cerr << "Use '--help' for usage information." << std::endl;
        }
    }
    
    // Format a value for display
    template<typename T>
    std::string formatValue(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    
    // Parse comma-separated list
    std::vector<std::string> parseCSV(const std::string& str) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            // Trim whitespace
            size_t start = item.find_first_not_of(" \t\r\n");
            size_t end = item.find_last_not_of(" \t\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                item = item.substr(start, end - start + 1);
                if (!item.empty()) {
                    result.push_back(item);
                }
            }
        }
        
        return result;
    }
    
} // namespace AdvancedHelpers