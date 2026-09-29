#include "CommandLineParser.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()), parseState(ParseState::NORMAL) {
    loadDefaultConfiguration();
}

CommandLineParser::~CommandLineParser() = default;

bool CommandLineParser::parse(int argc, char** argv) {
    reset();
    pImpl->arguments.clear();
    pImpl->positionalArguments.clear();
    pImpl->errors.clear();
    
    pImpl->statistics.totalArguments = argc - 1;  // Exclude program name
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --help and -h
        if (arg == "--help" || arg == "-h") {
            std::cout << getHelp() << std::endl;
            exit(0);
        }
        
        // Handle --version
        if (arg == "--version") {
            std::cout << "NLM Phase 2 Demo System v1.0" << std::endl;
            exit(0);
        }
        
        // Parse argument
        if (!parseArgument(arg) && i + 1 < argc) {
            // Try next argument
            std::string nextArg = argv[++i];
            if (!parseArgument(nextArg)) {
                addError(ErrorType::UNKNOWN_ARGUMENT, "Unknown argument: " + nextArg, nextArg);
            }
        }
    }
    
    // Validate all arguments
    std::vector<std::string> validationErrors = validateAllArguments();
    for (const auto& error : validationErrors) {
        addError(ErrorType::VALIDATION_FAILED, error);
    }
    
    updateStatistics();
    return pImpl->errors.empty();
}

const std::map<std::string, std::string>& CommandLineParser::getArguments() const {
    return pImpl->arguments;
}

const std::vector<std::string>& CommandLineParser::getPositionalArguments() const {
    return pImpl->positionalArguments;
}

bool CommandLineParser::hasArgument(const std::string& name) const {
    return pImpl->arguments.find(name) != pImpl->arguments.end();
}

std::string CommandLineParser::getArgument(const std::string& name) const {
    auto it = pImpl->arguments.find(name);
    if (it != pImpl->arguments.end()) {
        return it->second;
    }
    
    // Check if there's a default value
    auto configIt = pImpl->argumentConfigs.find(name);
    if (configIt != pImpl->argumentConfigs.end()) {
        return configIt->second.defaultValue;
    }
    
    throw std::runtime_error("Argument not found: " + name);
}

int CommandLineParser::getArgumentInt(const std::string& name) const {
    std::string value = getArgument(name);
    return stringToInt(value);
}

double CommandLineParser::getArgumentDouble(const std::string& name) const {
    std::string value = getArgument(name);
    return stringToDouble(value);
}

bool CommandLineParser::getArgumentBool(const std::string& name) const {
    std::string value = getArgument(name);
    return stringToBool(value);
}

std::string CommandLineParser::getPositionalArgument(size_t index) const {
    if (index < pImpl->positionalArguments.size()) {
        return pImpl->positionalArguments[index];
    }
    throw std::out_of_range("Positional argument index out of range");
}

bool CommandLineParser::validateArguments() const {
    return pImpl->errors.empty();
}

std::vector<std::string> CommandLineParser::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& error : pImpl->errors) {
        errors.push_back(error.message);
    }
    return errors;
}

std::string CommandLineParser::getUsage() const {
    return generateUsage();
}

std::string CommandLineParser::getHelp() const {
    return generateHelp();
}

void CommandLineParser::addArgument(const std::string& name, const std::string& defaultValue,
                                   const std::string& help, const std::string& type) {
    ArgumentConfig config(name, type, defaultValue, help, false);
    pImpl->argumentConfigs[name] = config;
}

void CommandLineParser::addRequiredArgument(const std::string& name, const std::string& help) {
    ArgumentConfig config(name, "string", "", help, true);
    pImpl->argumentConfigs[name] = config;
}

void CommandLineParser::addOptionalArgument(const std::string& name, const std::string& defaultValue,
                                          const std::string& help) {
    ArgumentConfig config(name, "string", defaultValue, help, false);
    pImpl->argumentConfigs[name] = config;
}

void CommandLineParser::addFlag(const std::string& name, const std::string& help) {
    ArgumentConfig config(name, "bool", "false", help, false);
    pImpl->argumentConfigs[name] = config;
}

CommandLineParser::Statistics CommandLineParser::getStatistics() const {
    return pImpl->statistics;
}

void CommandLineParser::reset() {
    pImpl->arguments.clear();
    pImpl->positionalArguments.clear();
    pImpl->errors.clear();
    pImpl->statistics = Statistics();
    parseState = ParseState::NORMAL;
    currentArgumentName.clear();
}

std::string CommandLineParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool CommandLineParser::stringToBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    
    throw std::invalid_argument("Cannot convert string to bool: " + str);
}

int CommandLineParser::stringToInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("Cannot convert string to int: " + str);
    }
}

double CommandLineParser::stringToDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("Cannot convert string to double: " + str);
    }
}

bool CommandLineParser::validatePattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) return true;
    
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

bool CommandLineParser::parseArgument(const std::string& arg) {
    if (arg.empty()) return false;
    
    // Handle long options (--name)
    if (matchesLongOptionPattern(arg)) {
        return parseLongOption(arg);
    }
    
    // Handle short options (-n)
    if (matchesShortOptionPattern(arg)) {
        return parseShortOption(arg);
    }
    
    // Handle equals format (--name=value)
    if (matchesEqualPattern(arg)) {
        return parseEqualFormat(arg);
    }
    
    // Handle space format (--name value or -n value)
    if (parseState == ParseState::NORMAL && (arg[0] == '-' || arg[0] == '/')) {
        parseState = ParseState::EXPECTING_VALUE;
        currentArgumentName = arg.substr(1);
        if (currentArgumentName.empty()) return false;
        return true;
    }
    
    // Handle positional arguments
    if (parseState == ParseState::NORMAL) {
        pImpl->positionalArguments.push_back(arg);
        return true;
    }
    
    return false;
}

bool CommandLineParser::parseLongOption(const std::string& arg) {
    size_t equalsPos = arg.find('=');
    std::string name;
    std::string value;
    
    if (equalsPos != std::string::npos) {
        // --name=value format
        name = arg.substr(2, equalsPos - 2);
        value = arg.substr(equalsPos + 1);
    } else {
        // --name format (flag)
        name = arg.substr(2);
        value = "true";
    }
    
    if (name.empty()) return false;
    
    // Check if argument is defined
    if (pImpl->argumentConfigs.find(name) == pImpl->argumentConfigs.end()) {
        addError(ErrorType::UNKNOWN_ARGUMENT, "Unknown argument: --" + name, "--" + name);
        return false;
    }
    
    // Set argument value
    pImpl->arguments[name] = value;
    pImpl->statistics.argumentCounts[name]++;
    
    // Reset parse state
    parseState = ParseState::NORMAL;
    currentArgumentName.clear();
    
    return true;
}

bool CommandLineParser::parseShortOption(const std::string& arg) {
    if (arg.size() < 2 || arg[0] != '-') return false;
    
    std::string name = arg.substr(1);
    std::string value = "true";  // Flag by default
    
    // Check if argument is defined
    if (pImpl->argumentConfigs.find(name) == pImpl->argumentConfigs.end()) {
        addError(ErrorType::UNKNOWN_ARGUMENT, "Unknown argument: -" + name, "-" + name);
        return false;
    }
    
    // Set argument value
    pImpl->arguments[name] = value;
    pImpl->statistics.argumentCounts[name]++;
    
    // Reset parse state
    parseState = ParseState::NORMAL;
    currentArgumentName.clear();
    
    return true;
}

bool CommandLineParser::parseEqualFormat(const std::string& arg) {
    size_t equalsPos = arg.find('=');
    if (equalsPos == std::string::npos || equalsPos == 0 || equalsPos == arg.size() - 1) {
        return false;
    }
    
    std::string name = arg.substr(0, equalsPos);
    std::string value = arg.substr(equalsPos + 1);
    
    if (name.empty()) return false;
    
    // Check if argument is defined
    if (pImpl->argumentConfigs.find(name) == pImpl->argumentConfigs.end()) {
        addError(ErrorType::UNKNOWN_ARGUMENT, "Unknown argument: --" + name, "--" + name);
        return false;
    }
    
    // Set argument value
    pImpl->arguments[name] = value;
    pImpl->statistics.argumentCounts[name]++;
    
    return true;
}

bool CommandLineParser::validateAllArguments() const {
    std::vector<std::string> errors;
    
    // Check required arguments
    for (const auto& pair : pImpl->argumentConfigs) {
        const std::string& name = pair.first;
        const ArgumentConfig& config = pair.second;
        
        if (config.required && pImpl->arguments.find(name) == pImpl->arguments.end()) {
            errors.push_back("Missing required argument: --" + name);
        }
    }
    
    // Validate argument values
    for (const auto& pair : pImpl->arguments) {
        const std::string& name = pair.first;
        const std::string& value = pair.second;
        
        auto configIt = pImpl->argumentConfigs.find(name);
        if (configIt != pImpl->argumentConfigs.end()) {
            const ArgumentConfig& config = configIt->second;
            
            if (!validatePattern(value, config.validationPattern)) {
                errors.push_back("Invalid value for argument --" + name + ": " + value);
            }
        }
    }
    
    return errors.empty();
}

std::string CommandLineParser::generateUsage() const {
    std::ostringstream oss;
    oss << "Usage: nlm_demo [options]" << std::endl;
    oss << "  --help, -h                    Show this help message" << std::endl;
    oss << "  --version                     Show version information" << std::endl;
    oss << "  --config <file>               Load configuration from file" << std::endl;
    oss << "  --neurons <int>               Number of neurons (default: " << DEFAULT_NEURON_COUNT << ")" << std::endl;
    oss << "  --regions <int>               Number of regions (default: " << DEFAULT_REGION_COUNT << ")" << std::endl;
    oss << "  --steps <int>                 Maximum simulation steps (default: " << DEFAULT_MAX_STEPS << ")" << std::endl;
    oss << "  --timestep <float>             Simulation timestep (default: " << DEFAULT_TIME_STEP << ")" << std::endl;
    oss << "  --seed <int>                  Random seed (default: " << DEFAULT_RANDOM_SEED << ")" << std::endl;
    oss << "  --enable-plasticity           Enable neural plasticity" << std::endl;
    oss << "  --enable-development          Enable developmental processes" << std::endl;
    oss << "  --checkpoint <path>            Set checkpoint directory" << std::endl;
    
    return oss.str();
}

std::string CommandLineParser::generateHelp() const {
    std::ostringstream oss;
    oss << DEFAULT_HELP_HEADER << std::endl;
    oss << std::endl;
    oss << generateUsage() << std::endl;
    oss << std::endl;
    
    // Add argument details
    for (const auto& pair : pImpl->argumentConfigs) {
        const ArgumentConfig& config = pair.second;
        oss << "  --" << config.name;
        
        if (config.type != "bool") {
            oss << " <" << config.type << ">";
        }
        
        if (config.required) {
            oss << " (required)";
        }
        
        oss << std::endl;
        
        if (!config.help.empty()) {
            oss << "    " << config.help << std::endl;
        }
        
        if (!config.defaultValue.empty() && !config.required) {
            oss << "    Default: " << config.defaultValue << std::endl;
        }
        
        oss << std::endl;
    }
    
    oss << DEFAULT_HELP_FOOTER << std::endl;
    return oss.str();
}

void CommandLineParser::loadDefaultConfiguration() {
    addArgument("config", "configs/default.cfg", "Load configuration from file");
    addArgument("neurons", std::to_string(DEFAULT_NEURON_COUNT), "Number of neurons");
    addArgument("regions", std::to_string(DEFAULT_REGION_COUNT), "Number of regions");
    addArgument("steps", std::to_string(DEFAULT_MAX_STEPS), "Maximum simulation steps");
    addArgument("timestep", std::to_string(DEFAULT_TIME_STEP), "Simulation timestep");
    addArgument("seed", std::to_string(DEFAULT_RANDOM_SEED), "Random seed");
    addFlag("enable-plasticity", "Enable neural plasticity");
    addFlag("enable-development", "Enable developmental processes");
    addArgument("checkpoint", "", "Set checkpoint directory");
    
    // Add required arguments
    addRequiredArgument("help", "Show this help message");
    addRequiredArgument("version", "Show version information");
}

void CommandLineParser::updateStatistics() {
    pImpl->statistics.parsedArguments = pImpl->arguments.size();
    pImpl->statistics.validationErrors = getValidationErrors().size();
    
    for (const auto& error : pImpl->errors) {
        if (error.type == ErrorType::UNKNOWN_ARGUMENT) {
            pImpl->statistics.unknownArguments++;
        }
    }
}

void CommandLineParser::addError(ErrorType type, const std::string& message, const std::string& argumentName) {
    Error error(type, message, argumentName);
    pImpl->errors.push_back(error);
}

bool CommandLineParser::isBooleanType(const std::string& type) {
    return type == "bool" || type == "flag";
}

bool CommandLineParser::isNumericType(const std::string& type) {
    return type == "int" || type == "float" || type == "double";
}

bool CommandLineParser::isStringType(const std::string& type) {
    return type == "string" || type == "path" || type.empty();
}

bool CommandLineParser::matchesLongOptionPattern(const std::string& arg) {
    return arg.size() >= 3 && arg[0] == '-' && arg[1] == '-';
}

bool CommandLineParser::matchesShortOptionPattern(const std::string& arg) {
    return arg.size() >= 2 && arg[0] == '-' && arg[1] != '-';
}

bool CommandLineParser::matchesEqualPattern(const std::string& arg) {
    return arg.find('=') != std::string::npos;
}

} // namespace nlm

// Global command line parser instance
CommandLineParser& getCommandLineParser() {
    static CommandLineParser parser;
    return parser;
}