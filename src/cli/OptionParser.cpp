// OptionParser implementation

#include "OptionParser.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nlm {
namespace cli {

void OptionParser::addOption(const AdvancedOption& option) {
    options.push_back(option);
}

void OptionParser::addFlag(const std::string& longName, char shortName,
                          const std::string& description,
                          bool defaultValue) {
    std::string defaultStr = defaultValue ? "true" : "false";
    options.push_back(AdvancedOption(longName, shortName, description, defaultStr, false));
}

void OptionParser::addOptionWithValue(const std::string& longName, char shortName,
                                     const std::string& description,
                                     const std::string& defaultValue,
                                     bool required) {
    options.push_back(AdvancedOption(longName, shortName, description, defaultValue, required));
}

void OptionParser::addPositionalArg(const PositionalArg& arg) {
    positionalSpecs.push_back(arg);
}

ParseResult OptionParser::parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

ParseResult OptionParser::parse(const std::vector<std::string>& args) {
    // Clear previous parsing state
    parsedValues.clear();
    positionalArgs.clear();
    errors.clear();
    parseResult = ParseResult::SUCCESS;
    
    size_t i = 1; // Skip program name
    while (i < args.size()) {
        std::string arg = args[i];
        
        if (arg == "--" || arg == "-") {
            // End of options, everything after is positional
            for (size_t j = i + 1; j < args.size(); ++j) {
                positionalArgs.push_back(args[j]);
            }
            break;
        }
        
        if (arg.substr(0, 2) == "--") {
            // Long option (--option)
            std::string optionName = arg.substr(2);
            bool found = false;
            
            for (auto& opt : options) {
                if (opt.longName == optionName) {
                    found = true;
                    if (!parseOption(arg, opt, i, args)) {
                        return ParseResult::PARSE_ERROR;
                    }
                    break;
                }
            }
            
            if (!found) {
                addError("Unknown option: " + arg);
                return ParseResult::PARSE_ERROR;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option (-o)
            std::string optionName(1, arg[1]);
            bool found = false;
            
            for (auto& opt : options) {
                if (opt.shortName != '\0' && opt.shortName == arg[1]) {
                    found = true;
                    if (!parseOption(arg, opt, i, args)) {
                        return ParseResult::PARSE_ERROR;
                    }
                    break;
                }
            }
            
            if (!found) {
                addError("Unknown option: " + arg);
                return ParseResult::PARSE_ERROR;
            }
        } else {
            // Positional argument
            positionalArgs.push_back(arg);
        }
        
        i++;
    }
    
    // Parse any remaining options
    while (i < args.size()) {
        if (parseArgument(args[i], i, args)) {
            i++;
        } else {
            return ParseResult::PARSE_ERROR;
        }
    }
    
    // Validate parsed results
    validate();
    
    return parseResult;
}

bool OptionParser::parseArgument(const std::string& arg, size_t& currentPos,
                                const std::vector<std::string>& args) {
    if (arg == "--" || arg == "-") {
        currentPos++;
        return true;
    }
    
    if (arg.substr(0, 2) == "--") {
        std::string optionName = arg.substr(2);
        for (auto& opt : options) {
            if (opt.longName == optionName) {
                return parseOption(arg, opt, currentPos, args);
            }
        }
        addError("Unknown option: " + arg);
        return false;
    } else if (arg.substr(0, 1) == "-") {
        std::string optionName(1, arg[1]);
        for (auto& opt : options) {
            if (opt.shortName != '\0' && opt.shortName == arg[1]) {
                return parseOption(arg, opt, currentPos, args);
            }
        }
        addError("Unknown option: " + arg);
        return false;
    }
    
    // It's a positional argument
    positionalArgs.push_back(arg);
    return true;
}

bool OptionParser::parseOption(const std::string& arg, AdvancedOption& option,
                              size_t& currentPos,
                              const std::vector<std::string>& args) {
    if (option.hasValue) {
        return parseValue(arg, option, currentPos, args);
    } else {
        // Flag option
        option.value = "true";
        parsedValues[option.longName] = "true";
        return true;
    }
}

bool OptionParser::parseValue(const std::string& arg, AdvancedOption& option,
                             size_t& currentPos,
                             const std::vector<std::string>& args) {
    size_t nextPos = currentPos + 1;
    
    if (nextPos < args.size() && args[nextPos].substr(0, 2) != "--") {
        // Next argument is the value
        option.value = args[nextPos];
        parsedValues[option.longName] = option.value;
        currentPos = nextPos;
        return true;
    } else {
        addError("Option " + option.longName + " requires a value");
        return false;
    }
}

template<typename T>
T OptionParser::getValue(const std::string& optionName, const T& defaultValue) const {
    auto it = parsedValues.find(optionName);
    if (it == parsedValues.end()) {
        return defaultValue;
    }
    
    const std::string& strValue = it->second;
    
    try {
        if constexpr (std::is_same_v<T, std::string>) {
            return strValue;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(strValue);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(strValue);
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string lower = toLower(strValue);
            return (lower == "true" || lower == "1" || lower == "yes");
        }
    } catch (const std::exception&) {
        // Return default value if parsing fails
    }
    
    return defaultValue;
}

bool OptionParser::hasOption(const std::string& optionName) const {
    return parsedValues.find(optionName) != parsedValues.end();
}

std::string OptionParser::getString(const std::string& optionName) const {
    auto it = parsedValues.find(optionName);
    if (it != parsedValues.end()) {
        return it->second;
    }
    return "";
}

int OptionParser::getInt(const std::string& optionName) const {
    return getValue<int>(optionName, 0);
}

double OptionParser::getDouble(const std::string& optionName) const {
    return getValue<double>(optionName, 0.0);
}

bool OptionParser::getBool(const std::string& optionName) const {
    return getValue<bool>(optionName, false);
}

std::string OptionParser::getPositionalArg(size_t index) const {
    if (index < positionalArgs.size()) {
        return positionalArgs[index];
    }
    return "";
}

bool OptionParser::validate() const {
    bool valid = true;
    
    // Check required options
    for (const auto& opt : options) {
        if (opt.required && !hasOption(opt.longName)) {
            errors.push_back("Missing required option: --" + opt.longName);
            valid = false;
        }
    }
    
    // Check option-specific validation
    for (const auto& opt : options) {
        if (hasOption(opt.longName) && opt.validator) {
            std::string value = getString(opt.longName);
            if (!opt.validator(value)) {
                errors.push_back("Invalid value for option --" + opt.longName + ": " + value);
                valid = false;
            }
        }
    }
    
    // Check allowed values
    for (const auto& opt : options) {
        if (hasOption(opt.longName) && !opt.allowedValues.empty()) {
            std::string value = getString(opt.longName);
            if (std::find(opt.allowedValues.begin(), opt.allowedValues.end(), value) == opt.allowedValues.end()) {
                errors.push_back("Invalid value for option --" + opt.longName + ": " + value +
                               ". Allowed values: " + join(opt.allowedValues, ", "));
                valid = false;
            }
        }
    }
    
    // Check positional arguments
    if (!validatePositionalArgs()) {
        valid = false;
    }
    
    if (!valid) {
        parseResult = ParseResult::VALIDATION_ERROR;
    }
    
    return valid;
}

bool OptionParser::validatePositionalArgs() const {
    bool valid = true;
    
    for (const auto& spec : positionalSpecs) {
        size_t count = 0;
        // Count how many arguments match the spec (simplified implementation)
        // In a real implementation, this would check based on the spec
        count = positionalArgs.size(); // Simplified: count all as positional
        
        if (count < spec.minCount) {
            errors.push_back("Argument " + spec.name + " requires at least " + 
                           std::to_string(spec.minCount) + " values, got " + std::to_string(count));
            valid = false;
        }
        
        if (count > spec.maxCount) {
            errors.push_back("Argument " + spec.name + " requires at most " + 
                           std::to_string(spec.maxCount) + " values, got " + std::to_string(count));
            valid = false;
        }
    }
    
    return valid;
}

void OptionParser::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [OPTIONS] [ARGUMENTS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& opt : options) {
        std::cout << "  ";
        if (opt.shortName != '\0') {
            std::cout << "-" << opt.shortName << ", ";
        }
        std::cout << "--" << opt.longName;
        
        if (opt.hasValue) {
            std::cout << " VALUE";
        }
        
        std::cout << std::endl;
        std::cout << "      " << opt.description;
        
        if (!opt.defaultValue.empty()) {
            std::cout << " (default: " << opt.defaultValue << ")";
        }
        
        if (opt.required) {
            std::cout << " (required)";
        }
        
        std::cout << std::endl << std::endl;
    }
}

void OptionParser::printHelp(const std::string& programName) const {
    printUsage(programName);
    
    if (!errors.empty()) {
        std::cout << "Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
        std::cout << std::endl;
    }
}

std::string OptionParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string OptionParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string OptionParser::join(const std::vector<std::string>& vec, const std::string& separator) {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << separator;
        ss << vec[i];
    }
    return ss.str();
}

// ConfigFileParser implementation

bool ConfigFileParser::parse(const std::string& filePath) {
    std::string path = filePath.empty() ? configFilePath : filePath;
    return parseFromString(readFile(path, content));
}

bool ConfigFileParser::parseFromString(const std::string& content) {
    configValues.clear();
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        std::string key, value;
        if (parseLine(line, key, value)) {
            configValues[key] = value;
        }
    }
    
    return true;
}

template<typename T>
T ConfigFileParser::getValue(const std::string& key, const T& defaultValue) const {
    auto it = configValues.find(key);
    if (it == configValues.end()) {
        return defaultValue;
    }
    
    const std::string& strValue = it->second;
    
    try {
        if constexpr (std::is_same_v<T, std::string>) {
            return strValue;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(strValue);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(strValue);
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string lower = toLower(strValue);
            return (lower == "true" || lower == "1" || lower == "yes");
        }
    } catch (const std::exception&) {
        // Return default value if parsing fails
    }
    
    return defaultValue;
}

std::string ConfigFileParser::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        return it->second;
    }
    return defaultValue;
}

int ConfigFileParser::getInt(const std::string& key, int defaultValue) const {
    return getValue<int>(key, defaultValue);
}

double ConfigFileParser::getDouble(const std::string& key, double defaultValue) const {
    return getValue<double>(key, defaultValue);
}

bool ConfigFileParser::getBool(const std::string& key, bool defaultValue) const {
    return getValue<bool>(key, defaultValue);
}

std::string ConfigFileParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string ConfigFileParser::normalizeKey(const std::string& key) {
    std::string result = key;
    // Convert to lowercase and replace spaces with underscores
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    size_t pos = 0;
    while ((pos = result.find(' ', pos)) != std::string::npos) {
        result.replace(pos, 1, "_");
        pos += 1;
    }
    
    return result;
}

bool ConfigFileParser::parseLine(const std::string& line, std::string& key, std::string& value) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) {
        line.substr(0, commentPos);
    }
    
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, equalsPos));
    value = trim(line.substr(equalsPos + 1));
    
    return !key.empty();
}

bool ConfigFileParser::readFile(const std::string& filePath, std::string& content) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

// ArgumentMerger implementation

bool ArgumentMerger::merge(int argc, char** argv) {
    return configParser->parse() && optionParser->parse(argc, argv);
}

template<typename T>
T ArgumentMerger::getFinalValue(const std::string& optionName, const T& defaultValue) const {
    // Command line takes precedence over config file
    if (optionParser->hasOption(optionName)) {
        return optionParser->getValue<T>(optionName, defaultValue);
    }
    return configParser->getValue<T>(optionName, defaultValue);
}

bool ArgumentMerger::hasOption(const std::string& optionName) const {
    return optionParser->hasOption(optionName);
}

std::string ArgumentMerger::getString(const std::string& optionName) const {
    if (optionParser->hasOption(optionName)) {
        return optionParser->getString(optionName);
    }
    return configParser->getString(optionName);
}

int ArgumentMerger::getInt(const std::string& optionName) const {
    if (optionParser->hasOption(optionName)) {
        return optionParser->getInt(optionName);
    }
    return configParser->getInt(optionName);
}

double ArgumentMerger::getDouble(const std::string& optionName) const {
    if (optionParser->hasOption(optionName)) {
        return optionParser->getDouble(optionName);
    }
    return configParser->getDouble(optionName);
}

bool ArgumentMerger::getBool(const std::string& optionName) const {
    if (optionParser->hasOption(optionName)) {
        return optionParser->getBool(optionName);
    }
    return configParser->getBool(optionName);
}

std::string ArgumentMerger::getErrors() const {
    std::stringstream ss;
    
    if (!configParser->isSuccessful()) {
        ss << "Config file errors:" << std::endl;
        for (const auto& error : configParser->getValidationErrors()) {
            ss << "  " << error << std::endl;
        }
    }
    
    if (!optionParser->isSuccessful()) {
        if (!configParser->isSuccessful()) ss << std::endl;
        ss << "Option parser errors:" << std::endl;
        for (const auto& error : optionParser->getValidationErrors()) {
            ss << "  " << error << std::endl;
        }
    }
    
    return ss.str();
}

} // namespace cli
} // namespace nlm
