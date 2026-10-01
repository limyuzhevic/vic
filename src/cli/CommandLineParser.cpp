#include "CommandLineParser.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace nlm {

void CommandLineParser::addOption(const std::string& longName, char shortName, 
                               const std::string& description, const std::string& defaultValue,
                               bool required, bool hasValue) {
    Option option;
    option.longName = longName;
    option.shortName = shortName;
    option.description = description;
    option.defaultValue = defaultValue;
    option.required = required;
    option.hasValue = hasValue;
    
    options_.push_back(option);
    if (hasValue) {
        defaultValues_[longName] = defaultValue;
    }
}

bool CommandLineParser::parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

bool CommandLineParser::parse(const std::vector<std::string>& args) {
    values_.clear();
    
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;  // Exit after showing help
        }
        
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;  // Exit after showing version
        }
        
        // Check for --key=value format
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t equalsPos = arg.find("=");
            std::string key = arg.substr(2, equalsPos - 2);
            std::string value = arg.substr(equalsPos + 1);
            values_[key] = value;
        }
        // Check for -k value format
        else if (arg.find("-") == 0 && arg.size() > 1 && !std::isalpha(arg[1])) {
            // This is likely a standalone option, treat as flag
            std::string key = arg.substr(1);
            values_[key] = "true";
        }
        // Check for --key value format (for options without short form)
        else if (arg.find("--") == 0 && (i + 1 < args.size()) && 
                 args[i + 1].find("-") != 0) {
            // Next argument doesn't start with -, so it's the value
            std::string key = arg.substr(2);
            std::string value = args[++i];
            values_[key] = value;
        }
        // Check for -k value format (for short options with values)
        else if (arg.find("-") == 0 && arg.size() > 1) {
            std::string key = arg.substr(1);
            if (i + 1 < args.size() && args[i + 1].find("-") != 0) {
                // Next argument doesn't start with -, so it's the value
                std::string value = args[++i];
                values_[key] = value;
            }
        }
    }
    
    // Apply default values for options not provided
    for (const auto& option : options_) {
        if (option.hasValue && values_.find(option.longName) == values_.end()) {
            if (!option.defaultValue.empty()) {
                values_[option.longName] = option.defaultValue;
            }
        }
    }
    
    return validate();
}

std::string CommandLineParser::getString(const std::string& option) const {
    auto it = values_.find(option);
    if (it != values_.end()) {
        return it->second;
    }
    return defaultValues_.find(option) != defaultValues_.end() ? 
           defaultValues_.at(option) : "";
}

int CommandLineParser::getInt(const std::string& option) const {
    std::string value = getString(option);
    try {
        return std::stoi(value);
    } catch (...) {
        return 0;
    }
}

double CommandLineParser::getDouble(const std::string& option) const {
    std::string value = getString(option);
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool CommandLineParser::getBool(const std::string& option) const {
    std::string value = getString(option);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

bool CommandLineParser::hasOption(const std::string& option) const {
    return values_.find(option) != values_.end();
}

bool CommandLineParser::validate() const {
    std::string errors = getValidationErrors();
    if (!errors.empty()) {
        std::cerr << "Validation errors:\n" << errors << std::endl;
        return false;
    }
    return true;
}

std::string CommandLineParser::getValidationErrors() const {
    std::stringstream ss;
    
    for (const auto& option : options_) {
        if (option.required && values_.find(option.longName) == values_.end()) {
            ss << "  Error: Required option --" << option.longName << " is missing.\n";
        }
        
        if (values_.find(option.longName) != values_.end()) {
            std::string value = values_.at(option.longName);
            
            // Try to convert to appropriate type for basic validation
            if (option.hasValue) {
                if (option.longName == "steps" || option.longName == "seed") {
                    try {
                        std::stoi(value);
                    } catch (...) {
                        ss << "  Error: Invalid integer value for option --" << option.longName << ": " << value << "\n";
                    }
                } else if (option.longName == "timestep") {
                    try {
                        std::stod(value);
                    } catch (...) {
                        ss << "  Error: Invalid double value for option --" << option.longName << ": " << value << "\n";
                    }
                } else if (option.longName == "config") {
                    // Check if config file exists
                    std::ifstream file(value);
                    if (!file.good()) {
                        ss << "  Error: Configuration file not found: " << value << "\n";
                    }
                }
            }
        }
    }
    
    return ss.str();
}

void CommandLineParser::printHelp() const {
    printUsage();
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help                    Show help information and exit" << std::endl;
    std::cout << "  -v, --version                 Show version information and exit" << std::endl;
    std::cout << "  -c, --config <file>           Configuration file to load" << std::endl;
    std::cout << "  -o, --output <dir>            Output directory" << std::endl;
    std::cout << "  -V, --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  -q, --quiet                   Suppress output messages" << std::endl;
    std::cout << "  -s, --steps <steps>            Number of simulation steps" << std::endl;
    std::cout << "  -t, --timestep <time>         Simulation time step size" << std::endl;
    std::cout << "  -S, --seed <seed>             Random seed for reproducibility" << std::endl;
    std::cout << "  -w, --save                    Save final brain state to file" << std::endl;
    std::cout << "  -r, --load                    Load brain state from file" << std::endl;
    
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  nlm --steps 5000 --timestep 0.001 --seed 123" << std::endl;
    std::cout << "  nlm --config my_config.cfg --save --output results/" << std::endl;
    std::cout << "  nlm --verbose --seed 42" << std::endl;
}

void CommandLineParser::printUsage() const {
    std::cout << "Usage: " << programName_ << " [options]" << std::endl;
    if (!description_.empty()) {
        std::cout << description_ << std::endl;
    }
}

void CommandLineParser::printVersion() const {
    std::cout << programName_ << " Version 1.0.0" << std::endl;
    std::cout << "Phase 6: Final Integration of Artificial Brain System" << std::endl;
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
}

bool CommandLineParser::loadConfigFile(const std::string& filepath) {
    return parseConfigFile(filepath, values_);
}

bool CommandLineParser::saveConfigFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by " << programName_ << std::endl;
    file << std::endl;
    
    for (const auto& option : options_) {
        file << "# " << option.description << std::endl;
        if (values_.find(option.longName) != values_.end()) {
            file << option.longName << " = " << values_.at(option.longName) << std::endl;
        } else if (!option.defaultValue.empty()) {
            file << option.longName << " = " << option.defaultValue << std::endl;
        }
        file << std::endl;
    }
    
    return true;
}

bool CommandLineParser::parseConfigFile(const std::string& filepath, 
                                       std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config[key] = value;
        }
    }
    
    return true;
}

std::string CommandLineParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string CommandLineParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string CommandLineParser::getOptionName(const Option& option) const {
    if (!option.shortName) {
        return "--" + option.longName;
    }
    return "-" + std::string(1, option.shortName) + ", --" + option.longName;
}

} // namespace nlm
