#include "CLI/CLI.hpp"
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace nlm {

struct CLIArgumentParser::Impl {
    std::map<std::string, ArgumentInfo> arguments;
    std::map<std::string, bool> flags;
    std::map<std::string, std::string> parsedValues;
    std::map<std::string, std::string> parsedFlags;
    bool helpRequested = false;
    bool versionRequested = false;
};

CLIArgumentParser::CLIArgumentParser(const std::string& programName, 
                                     const std::string& description)
    : programName(programName), description(description) {
    pImpl = std::make_unique<Impl>();
}

void CLIArgumentParser::addArgument(const std::string& name, const std::string& shortName,
                                    const std::string& description, 
                                    const std::string& defaultValue,
                                    const std::function<bool(std::string)>& validator,
                                    const std::function<void(std::string)>& callback,
                                    bool required,
                                    const std::string& valueType) {
    ArgumentInfo info;
    info.name = name;
    info.shortName = shortName;
    info.description = description;
    info.defaultValue = defaultValue;
    info.validator = validator;
    info.callback = callback;
    info.required = required;
    info.valueType = valueType;
    
    pImpl->arguments[name] = info;
}

void CLIArgumentParser::addFlag(const std::string& name, const std::string& description,
                                const std::function<void(bool)>& callback) {
    ArgumentInfo info;
    info.name = name;
    info.shortName = "";
    info.description = description;
    info.defaultValue = "";
    info.validator = nullptr;
    info.callback = callback;
    info.required = false;
    info.valueType = "flag";
    
    pImpl->arguments[name] = info;
    pImpl->flags[name] = false;
}

bool CLIArgumentParser::parse(int argc, char** argv) {
    std::string currentKey;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help and --version shortcuts
        if (arg == "--help" || arg == "-h") {
            pImpl->helpRequested = true;
            continue;
        }
        if (arg == "--version" || arg == "-v") {
            pImpl->versionRequested = true;
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                currentKey = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!validateAndSet(currentKey, value)) {
                    return false;
                }
            } else {
                currentKey = arg.substr(2);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (currentKey.empty()) {
                // Check if it's a flag
                bool isFlag = false;
                for (const auto& pair : pImpl->arguments) {
                    if ((pair.first == arg.substr(1) || 
                         (pair.second.shortName == arg.substr(1))) && 
                        pair.second.valueType == "flag") {
                        isFlag = true;
                        break;
                    }
                }
                
                if (isFlag) {
                    bool value = (arg != "--quiet" && arg != "-q");
                    std::string key = arg.substr(1);
                    pImpl->parsedFlags[key] = std::to_string(value);
                    if (pImpl->arguments.find(key) != pImpl->arguments.end()) {
                        auto& info = pImpl->arguments[key];
                        if (info.callback) {
                            info.callback(std::to_string(value));
                        }
                    }
                    currentKey.clear();
                } else {
                    currentKey = arg.substr(1);
                }
            } else {
                // Previous argument was a flag without value
                return validateAndSet(currentKey, arg);
            }
        } else {
            // Regular value argument
            if (!currentKey.empty()) {
                return validateAndSet(currentKey, arg);
            }
        }
    }
    
    return true;
}

bool CLIArgumentParser::validateAndSet(const std::string& key, const std::string& value) {
    // Check for help/version first
    if (key == "help" || key == "h") {
        pImpl->helpRequested = true;
        return true;
    }
    if (key == "version" || key == "v") {
        pImpl->versionRequested = true;
        return true;
    }
    
    // Look up the argument
    std::string keyLower = key;
    std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
    
    // Try direct match first
    auto it = pImpl->arguments.find(key);
    if (it == pImpl->arguments.end()) {
        // Try case-insensitive match
        for (auto& pair : pImpl->arguments) {
            if (toLower(pair.first) == keyLower) {
                it = pair;
                break;
            }
        }
    }
    
    if (it == pImpl->arguments.end()) {
        std::cerr << "Error: Unknown argument --" << key << std::endl;
        return false;
    }
    
    const auto& info = it->second;
    
    // Validate if validator provided
    if (info.validator && !info.validator(value)) {
        std::cerr << "Error: Invalid value for argument --" << key 
                  << ". Value must be " << info.description << std::endl;
        return false;
    }
    
    // Store the value
    pImpl->parsedValues[key] = value;
    
    // Execute callback if provided
    if (info.callback) {
        info.callback(value);
    }
    
    return true;
}

template<typename T>
std::optional<T> CLIArgumentParser::getArgument(const std::string& name) const {
    auto it = pImpl->parsedValues.find(name);
    if (it == pImpl->parsedValues.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(it->second);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(it->second);
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string val = it->second;
            std::transform(val.begin(), val.end(), val.begin(), ::tolower);
            return val == "true" || val == "1" || val == "yes";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return it->second;
        }
    } catch (...) {
        return std::nullopt;
    }
    return std::nullopt;
}

bool CLIArgumentParser::getFlag(const std::string& name) const {
    auto it = pImpl->parsedFlags.find(name);
    if (it != pImpl->parsedFlags.end()) {
        return it->second == "true";
    }
    return false;
}

bool CLIArgumentParser::hasArgument(const std::string& name) const {
    return pImpl->parsedValues.find(name) != pImpl->parsedValues.end();
}

bool CLIArgumentParser::hasFlag(const std::string& name) const {
    return pImpl->parsedFlags.find(name) != pImpl->parsedFlags.end();
}

void CLIArgumentParser::printHelp(std::ostream& out) const {
    out << "Usage: " << programName << " [OPTIONS]" << std::endl;
    out << std::endl;
    if (!description.empty()) {
        out << description << std::endl;
        out << std::endl;
    }
    
    // Group arguments by type
    out << "Arguments:" << std::endl;
    for (const auto& pair : pImpl->arguments) {
        const auto& info = pair.second;
        if (info.name == "help" || info.name == "version") continue; // Already handled
        
        out << "  --" << info.name;
        if (!info.shortName.empty()) {
            out << ", -" << info.shortName;
        }
        out << std::string(20 - info.name.length() - (info.shortName.empty() ? 2 : info.shortName.length()), ' ');
        
        if (!info.defaultValue.empty() && info.name != "set") {
            out << "[default: " << info.defaultValue << "] ";
        }
        
        out << info.description << std::endl;
    }
}

void CLIArgumentParser::printVersion(std::ostream& out) const {
    out << programName << " version " << version << std::endl;
}

std::vector<std::string> CLIArgumentParser::getArgumentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->arguments) {
        if (pair.first != "help" && pair.first != "version") {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> CLIArgumentParser::getFlagNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->flags) {
        names.push_back(pair.first);
    }
    return names;
}

std::string CLIArgumentParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string CLIArgumentParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Convenience functions implementation
namespace CLI {

bool parseInt(const std::string& value, int& output) {
    try {
        output = std::stoi(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseDouble(const std::string& value, double& output) {
    try {
        output = std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseBool(const std::string& value, bool& output) {
    if (value == "true" || value == "1" || value == "yes" || value == "on") {
        output = true;
        return true;
    } else if (value == "false" || value == "0" || value == "no" || value == "off") {
        output = false;
        return true;
    }
    return false;
}

bool parseString(const std::string& value, std::string& output) {
    output = value;
    return true;
}

bool validatePositiveInt(const std::string& value) {
    try {
        int val = std::stoi(value);
        return val > 0;
    } catch (...) {
        return false;
    }
}

bool validatePositiveDouble(const std::string& value) {
    try {
        double val = std::stod(value);
        return val > 0.0;
    } catch (...) {
        return false;
    }
}

bool validateNonEmptyString(const std::string& value) {
    return !value.empty();
}

bool validateFileExists(const std::string& value) {
    std::ifstream file(value);
    return file.good();
}

void setLogLevel(const std::string& level) {
    std::cout << "[CLI] Setting log level to: " << level << std::endl;
}

void setLogFile(const std::string& filename) {
    std::cout << "[CLI] Setting log file to: " << filename << std::endl;
}

void enableBenchmark(const std::string& name) {
    std::cout << "[CLI] Enabling benchmark mode: " << name << std::endl;
}

void setCheckpointFile(const std::string& filename) {
    std::cout << "[CLI] Setting checkpoint file: " << filename << std::endl;
}

} // namespace CLI

// Global CLI implementation
namespace nlm {

struct GlobalCLI::Impl {
    std::map<std::string, CLIArgumentParser*> parsers;
};

CLIArgumentParser& GlobalCLI::getParser(const std::string& programName, 
                                        const std::string& description) {
    static std::map<std::string, CLIArgumentParser> parsers;
    auto it = parsers.find(programName);
    if (it == parsers.end()) {
        it = parsers.emplace(programName, programName, description).first;
    }
    return it->second;
}

void GlobalCLI::registerStandardArguments(CLIArgumentParser& parser) {
    // Register standard NLM arguments
    parser.addArgument("help", "h", "Print help information", "", nullptr, nullptr, false, "flag");
    parser.addArgument("version", "v", "Print version information", "", nullptr, nullptr, false, "flag");
    
    parser.addArgument("verbose", "", "Enable verbose output", "", nullptr, nullptr, false, "flag");
    parser.addArgument("quiet", "", "Suppress output", "", nullptr, nullptr, false, "flag");
    parser.addArgument("log-level", "", "Set logging level (DEBUG, INFO, WARNING, ERROR)", "INFO", 
                     CLI::validateNonEmptyString, CLI::setLogLevel, false, "string");
    parser.addArgument("log-file", "", "Set log file output", "",
                     CLI::validateFileExists, CLI::setLogFile, false, "string");
    
    parser.addArgument("config", "", "Load configuration from file", "",
                     CLI::validateFileExists, nullptr, false, "string");
    parser.addArgument("set", "", "Override configuration value (format: key=value)", "",
                     nullptr, nullptr, false, "string");
    
    parser.addArgument("test", "", "Run specific test (1, 2, or 3)", "",
                     CLI::validatePositiveInt, nullptr, false, "int");
    parser.addArgument("steps", "", "Number of simulation steps", "100",
                     CLI::validatePositiveInt, nullptr, false, "int");
    parser.addArgument("duration", "", "Simulation duration in seconds", "10",
                     CLI::validatePositiveDouble, nullptr, false, "double");
    
    parser.addArgument("output", "", "Output file path", "",
                     CLI::validateNonEmptyString, nullptr, false, "string");
    parser.addArgument("benchmark", "", "Enable benchmark mode", "",
                     nullptr, CLI::enableBenchmark, false, "flag");
    parser.addArgument("checkpoint", "", "Checkpoint file path", "",
                     CLI::validateNonEmptyString, CLI::setCheckpointFile, false, "string");
    
    parser.addArgument("mode", "", "Operating mode (demo, test, benchmark)", "demo",
                     [](const std::string& v) {
                         return v == "demo" || v == "test" || v == "benchmark";
                     }, nullptr, false, "string");
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    template std::optional<int> CLIArgumentParser::getArgument<int>(const std::string&) const;
    template std::optional<double> CLIArgumentParser::getArgument<double>(const std::string&) const;
    template std::optional<bool> CLIArgumentParser::getArgument<bool>(const std::string&) const;
    template std::optional<std::string> CLIArgumentParser::getArgument<std::string>(const std::string&) const;
}
