#include "core/CLI/AdvancedCLI.h"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace nlm {

// Parser implementation
struct CLIParser::Impl {
    struct ArgumentComparator {
        bool operator()(const CLIArgument* a, const CLIArgument* b) const {
            return a->getName() < b->getName();
        }
    };

    std::vector<std::unique_ptr<CLIArgument>> arguments;
    std::unordered_map<std::string, std::unique_ptr<CLIArgument>> argumentMap;
    std::unordered_map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> validators;
    std::unordered_map<std::string, std::string> envVariableMap;
    std::unordered_map<std::string, std::string> configFileMap;

    std::vector<std::string> positionalArgs;
    std::vector<std::string> subcommands;
    std::unordered_map<std::string, std::vector<std::unique_ptr<CLIArgument>>> nestedArguments;
    std::vector<std::unique_ptr<CLICommand>> commands;
    std::unordered_map<std::string, std::unique_ptr<CLICommand>> commandMap;

    std::vector<ParsedArgument> parsedArgs;
    std::vector<std::string> errorMessages;
    std::unordered_map<std::string, std::vector<std::string>> suggestions;
    std::string currentProfile;
    std::vector<std::string> profiles;

    mutable std::mutex mutex;

    std::string outputFormat = "console";
    bool threadSafe = true;
    bool autoCompletionEnabled = false;
    std::string defaultsFilePath;
    std::string currentSubcommand;
};

CLIParser::CLIParser() : pImpl(std::make_unique<Impl>()) {}

// Helper function for parsing range arguments
static bool parseRange(const std::string& value, int& result) {
    if (value.find("..") != std::string::npos) {
        // Parse range like "100..1000"
        size_t pos = value.find("..");
        int min = std::stoi(value.substr(0, pos));
        int max = std::stoi(value.substr(pos + 2));
        // For range arguments, we typically use the min value
        result = min;
        return true;
    } else {
        result = std::stoi(value);
        return true;
    }
}

// Argument implementations
bool StringArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool StringArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string StringArgument::getUsage() const {
    return "[" + name + " <value>]";
}

std::string StringArgument::getHelp() const {
    return name + "=" + defaultValue + " - String argument";
}

bool IntArgument::parse(const std::string& value, ParsedArgument& parsed) {
    try {
        int val = std::stoi(value);
        parsed.key = name;
        parsed.value = val;
        parsed.source = ArgumentSource::CommandLine;
        parsed.timestamp = std::chrono::system_clock::now();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool IntArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<int>(parsed.value)) return false;
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string IntArgument::getUsage() const {
    return "[" + name + " <int>]";
}

std::string IntArgument::getHelp() const {
    return name + "=" + std::to_string(defaultValue) + " - Integer argument";
}

bool FloatArgument::parse(const std::string& value, ParsedArgument& parsed) {
    try {
        double val = std::stod(value);
        parsed.key = name;
        parsed.value = val;
        parsed.source = ArgumentSource::CommandLine;
        parsed.timestamp = std::chrono::system_clock::now();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool FloatArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<double>(parsed.value)) return false;
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string FloatArgument::getUsage() const {
    return "[" + name + " <float>]";
}

std::string FloatArgument::getHelp() const {
    return name + "=" + std::to_string(defaultValue) + " - Float argument";
}

bool BoolArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    bool val;
    if (value == "true" || value == "1" || value == "yes" || value == "on") {
        val = true;
    } else if (value == "false" || value == "0" || value == "no" || value == "off") {
        val = false;
    } else {
        return false;
    }
    parsed.value = val;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool BoolArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<bool>(parsed.value)) return false;
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string BoolArgument::getUsage() const {
    return "[" + name + " <bool>]";
}

std::string BoolArgument::getHelp() const {
    return "[" + name + "] - Flag argument (present/absent)";
}

bool EnumArgument::parse(const std::string& value, ParsedArgument& parsed) {
    auto it = std::find(allowedValues.begin(), allowedValues.end(), value);
    if (it == allowedValues.end()) {
        return false;
    }
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool EnumArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<std::string>(parsed.value)) return false;
    auto it = std::find(allowedValues.begin(), allowedValues.end(), std::get<std::string>(parsed.value));
    if (it == allowedValues.end()) return false;
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string EnumArgument::getUsage() const {
    std::string usage = "[" + name + " <";
    for (size_t i = 0; i < allowedValues.size(); ++i) {
        if (i > 0) usage += "|";
        usage += allowedValues[i];
    }
    usage += ">]";
    return usage;
}

std::string EnumArgument::getHelp() const {
    std::string help = name + "=" + defaultValue + " - Enum argument (";
    for (size_t i = 0; i < allowedValues.size(); ++i) {
        if (i > 0) help += ", ";
        help += allowedValues[i];
    }
    help += ")";
    return help;
}

bool RangeArgument::parse(const std::string& value, ParsedArgument& parsed) {
    int val;
    if (!parseRange(value, val)) {
        return false;
    }
    if (val < min || val > max) {
        return false;
    }
    parsed.key = name;
    parsed.value = val;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool RangeArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<int>(parsed.value)) return false;
    int val = std::get<int>(parsed.value);
    if (val < min || val > max) return false;
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string RangeArgument::getUsage() const {
    return "[" + name + " <" + std::to_string(min) + ".." + std::to_string(max) + ">]";
}

std::string RangeArgument::getHelp() const {
    return name + "=" + std::to_string(defaultValue) + " - Range argument (" + std::to_string(min) + ".." + std::to_string(max) + ")";
}

bool FileArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool FileArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<std::string>(parsed.value)) return false;
    std::string filepath = std::get<std::string>(parsed.value);
    if (mustExist) {
        std::ifstream file(filepath);
        if (!file.good()) {
            return false;
        }
    }
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string FileArgument::getUsage() const {
    return "[" + name + " <file>]";
}

std::string FileArgument::getHelp() const {
    return name + "=<file> - File argument";
}

bool PathArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool PathArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<std::string>(parsed.value)) return false;
    std::string path = std::get<std::string>(parsed.value);
    if (mustExist) {
        std::ifstream file(path);
        if (!file.good()) {
            return false;
        }
    }
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string PathArgument::getUsage() const {
    return "[" + name + " <path>]";
}

std::string PathArgument::getHelp() const {
    return name + "=<path> - Path argument";
}

bool VectorArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    // Parse comma-separated values
    std::vector<std::string> elements;
    size_t start = 0;
    size_t end = value.find(',');
    while (end != std::string::npos) {
        elements.push_back(value.substr(start, end - start));
        start = end + 1;
        end = value.find(',', start);
    }
    elements.push_back(value.substr(start));

    ConfigValue vectorValue;
    if (elementType == "int") {
        std::vector<int> intVector;
        for (const auto& elem : elements) {
            try {
                intVector.push_back(std::stoi(elem));
            } catch (const std::exception&) {
                return false;
            }
        }
        vectorValue = intVector;
    } else if (elementType == "float") {
        std::vector<double> floatVector;
        for (const auto& elem : elements) {
            try {
                floatVector.push_back(std::stod(elem));
            } catch (const std::exception&) {
                return false;
            }
        }
        vectorValue = floatVector;
    } else {
        std::vector<std::string> stringVector = elements;
        vectorValue = stringVector;
    }

    parsed.value = vectorValue;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool VectorArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string VectorArgument::getUsage() const {
    return "[" + name + " <" + elementType + ">,<" + elementType + ",...>]";
}

std::string VectorArgument::getHelp() const {
    return name + "=<val>,<val>,... - Vector of " + elementType + "s";
}

bool HyperparameterArgument::parse(const std::string& value, ParsedArgument& parsed) {
    // Parse hyperparameter format: name=value or name=value1:value2
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool HyperparameterArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string HyperparameterArgument::getUsage() const {
    return "[" + name + " <name=value>|<name=value1:value2>]";
}

std::string HyperparameterArgument::getHelp() const {
    return name + "=<name=value>|<name=value1:value2> - Neural network hyperparameters";
}

bool ConfigArgument::parse(const std::string& value, ParsedArgument& parsed) {
    parsed.key = name;
    parsed.value = value;
    parsed.source = ArgumentSource::CommandLine;
    parsed.timestamp = std::chrono::system_clock::now();
    return true;
}

bool ConfigArgument::validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const {
    if (!std::holds_alternative<std::string>(parsed.value)) return false;
    std::string filepath = std::get<std::string>(parsed.value);
    if (format == "json") {
        std::ifstream file(filepath);
        if (!file.good()) return false;
        try {
            nlohmann::json::parse(file);
        } catch (const std::exception&) {
            return false;
        }
    } else if (format == "yaml" || format == "yml") {
        // Would need YAML-CPP to validate
        // For now just check if file exists
        std::ifstream file(filepath);
        if (!file.good()) return false;
    } else {
        // Just check if file exists
        std::ifstream file(filepath);
        if (!file.good()) return false;
    }
    if (validation.validator && !validation.validator(parsed.value)) {
        return false;
    }
    return true;
}

std::string ConfigArgument::getUsage() const {
    return "[" + name + " <file>]";
}

std::string ConfigArgument::getHelp() const {
    return name + "=<file> - Configuration file (" + format + ")";
}

// Parser method implementations
void CLIParser::loadDefaultsFile() {
    if (defaultsFilePath.empty()) return;

    std::ifstream file(defaultsFilePath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            // Set default value if argument exists
            auto it = pImpl->argumentMap.find(key);
            if (it != pImpl->argumentMap.end()) {
                ParsedArgument parsed;
                parsed.key = key;
                parsed.value = value;
                parsed.source = ArgumentSource::Default;
                parsed.timestamp = std::chrono::system_clock::now();
                parsed.profile = currentProfile;

                if (it->second->parse(value, parsed)) {
                    pImpl->parsedArgs.push_back(parsed);
                }
            }
        }
    }
}

void CLIParser::loadConfigFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        errorMessages.push_back("Failed to load config file: " + filepath);
        return;
    }

    for (const auto& key : config->getKeys()) {
        ParsedArgument parsed;
        parsed.key = key;
        parsed.value = config->getRawValue(key);
        parsed.source = ArgumentSource::ConfigFile;
        parsed.timestamp = std::chrono::system_clock::now();;
        parsed.profile = currentProfile;

        auto it = pImpl->argumentMap.find(key);
        if (it != pImpl->argumentMap.end()) {
            pImpl->parsedArgs.push_back(parsed);
        }
    }
}

void CLIParser::resolveEnvVariables(std::vector<ParsedArgument>& parsedArgs) const {
    for (const auto& pair : pImpl->envVariableMap) {
        const std::string& argName = pair.first;
        const std::string& envVarName = pair.second;

        const char* envValue = std::getenv(envVarName.c_str());
        if (envValue) {
            ParsedArgument parsed;
            parsed.key = argName;
            parsed.value = std::string(envValue);
            parsed.source = ArgumentSource::EnvironmentVariable;
            parsed.timestamp = std::chrono::system_clock::now();
            parsed.profile = currentProfile;

            auto it = pImpl->argumentMap.find(argName);
            if (it != pImpl->argumentMap.end()) {
                parsedArgs.push_back(parsed);
            }
        }
    }
}

bool CLIParser::validateArguments(const std::vector<ParsedArgument>& parsedArgs) const {
    for (const auto& parsed : parsedArgs) {
        auto it = pImpl->argumentMap.find(parsed.key);
        if (it != pImpl->argumentMap.end()) {
            auto valIt = pImpl->validators.find(parsed.key);
            if (valIt != pImpl->validators.end()) {
                for (const auto& validator : valIt->second) {
                    if (!validator(parsed.value)) {
                        errorMessages.push_back("Validation failed for argument: " + parsed.key);
                        return false;
                    }
                }
            }

            auto argIt = pImpl->argumentMap.find(parsed.key);
            if (argIt != pImpl->argumentMap.end()) {
                ArgumentValidation validation;
                auto valIt = pImpl->validators.find(parsed.key);
                if (valIt != pImpl->validators.end()) {
                    validation.validator = valIt->second.front();
                }

                if (!argIt->second->validate(parsed, validation)) {
                    errorMessages.push_back("Invalid value for argument: " + parsed.key + 
                                          " (expected format: " + argIt->second->getUsage() + ")");
                    return false;
                }
            }
        }
    }
    return true;
}

std::string CLIParser::generateErrorMessage(const std::string& arg, const std::string& expected) const {
    std::string error = "Invalid argument: " + arg;
    if (!expected.empty()) {
        error += " (expected: " + expected + ")";
    }
    return error;
}

// Parser fluent interface methods
template<typename T>
CLIParser& CLIParser::addArgument(const std::string& name, const std::string& description, T defaultValue) {
    auto arg = std::make_unique<StringArgument>(name, description, defaultValue);
    pImpl->arguments.push_back(std::move(arg));
    return *this;
}

CLIParser& CLIParser::addPositional(const std::string& name, const std::string& description, bool required) {
    auto arg = std::make_unique<StringArgument>(name, description, "");
    arg->required = required;
    pImpl->arguments.push_back(std::move(arg));
    pImpl->positionalArgs.push_back(name);
    return *this;
}

CLIParser& CLIParser::addFlag(const std::string& name, const std::string& description) {
    auto arg = std::make_unique<BoolArgument>(name, description, false);
    pImpl->arguments.push_back(std::move(arg));
    return *this;
}

CLIParser& CLIParser::addOption(const std::string& name, const std::string& description, const std::string& defaultValue) {
    auto arg = std::make_unique<StringArgument>(name, description, defaultValue);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[name] = arg.get();
    return *this;
}

CLIParser& CLIParser::addEnum(const std::string& name, const std::string& description,
                             const std::vector<std::string>& allowedValues, const std::string& defaultValue) {
    auto arg = std::make_unique<EnumArgument>(name, description, allowedValues, defaultValue);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[name] = arg.get();
    return *this;
}

CLIParser& CLIParser::addRange(const std::string& name, const std::string& description,
                             int min, int max, int defaultValue) {
    auto arg = std::make_unique<RangeArgument>(name, description, min, max, defaultValue);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[name] = arg.get();
    return *this;
}

CLIParser& CLIParser::addFile(const std::string& name, const std::string& description, bool required) {
    auto arg = std::make_unique<FileArgument>(name, description, required);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[name] = arg.get();
    return *this;
}

CLIParser& CLIParser::addPath(const std::string& name, const std::string& description, bool required, bool mustExist) {
    auto arg = std::make_unique<PathArgument>(name, description, required, mustExist);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[name] = arg.get();
    return *this;
}

CLIParser& CLIParser::addValidator(const std::string& name, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[name].push_back(validator);
    return *this;
}

CLIParser& CLIParser::addErrorMessage(const std::string& name, const std::string& errorMessage) {
    pImpl->errorMessages.push_back("Argument " + name + ": " + errorMessage);
    return *this;
}

CLIParser& CLIParser::addSuggestions(const std::string& name, const std::vector<std::string>& suggestions) {
    pImpl->suggestions[name] = suggestions;
    return *this;
}

CLIParser& CLIParser::addEnvVariable(const std::string& argName, const std::string& envVarName) {
    pImpl->envVariableMap[argName] = envVarName;
    return *this;
}

CLIParser& CLIParser::addConfigFile(const std::string& configKey, const std::string& format) {
    pImpl->configFileMap[configKey] = format;
    return *this;
}

CLIParser& CLIParser::addProfile(const std::string& profileName) {
    pImpl->profiles.push_back(profileName);
    if (currentProfile.empty()) {
        currentProfile = profileName;
    }
    return *this;
}

CLIParser& CLIParser::setProfile(const std::string& profileName) {
    currentProfile = profileName;
    return *this;
}

CLIParser& CLIParser::addSubcommand(const std::string& name, const std::string& description) {
    auto command = std::make_unique<CLICommand>(name, description);
    pImpl->commands.push_back(std::move(command));
    pImpl->subcommands.push_back(name);
    return *this;
}

CLIParser& CLIParser::addNestedOption(const std::string& parent, const std::string& name,
                                     const std::string& description, const std::string& defaultValue) {
    auto arg = std::make_unique<StringArgument>(parent + "." + name, description, defaultValue);
    pImpl->arguments.push_back(std::move(arg));
    pImpl->argumentMap[parent + "." + name] = arg.get();
    return *this;
}

std::vector<std::string> CLIParser::parseCommandLineArgs(int argc, char** argv) const {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return args;
}

bool CLIParser::parse(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);

    pImpl->parsedArgs.clear();
    pImpl->errorMessages.clear();

    std::vector<std::string> args = parseCommandLineArgs(argc, argv);

    return parse(args);
}

bool CLIParser::parse(const std::vector<std::string>& args) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);

    pImpl->parsedArgs.clear();
    pImpl->errorMessages.clear();

    // Load defaults file
    loadDefaultsFile();

    // Parse arguments
    bool parsingSuccess = true;
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            // Option argument
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);

                // Check if this is a subcommand
                if (std::find(pImpl->subcommands.begin(), pImpl->subcommands.end(), key) != pImpl->subcommands.end()) {
                    pImpl->currentSubcommand = key;
                    continue;
                }

                auto it = pImpl->argumentMap.find(key);
                if (it != pImpl->argumentMap.end()) {
                    ParsedArgument parsed;
                    if (it->second->parse(value, parsed)) {
                        parsedArgs.push_back(parsed);
                    } else {
                        errorMessages.push_back(generateErrorMessage(key, it->second->getUsage()));
                        parsingSuccess = false;
                    }
                } else {
                    errorMessages.push_back("Unknown argument: " + key);
                    parsingSuccess = false;
                }
            } else {
                // Flag argument
                std::string key = arg.substr(2);
                auto it = pImpl->argumentMap.find(key);
                if (it != pImpl->argumentMap.end()) {
                    ParsedArgument parsed;
                    parsed.key = key;
                    parsed.value = true;
                    parsed.source = ArgumentSource::CommandLine;
                    parsed.timestamp = std::chrono::system_clock::now();
                    parsedArgs.push_back(parsed);
                } else {
                    errorMessages.push_back("Unknown flag: " + key);
                    parsingSuccess = false;
                }
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option argument
            std::string key = arg.substr(1);
            auto it = pImpl->argumentMap.find(key);
            if (it != pImpl->argumentMap.end()) {
                ParsedArgument parsed;
                parsed.key = key;
                parsed.value = true;
                parsed.source = ArgumentSource::CommandLine;
                parsed.timestamp = std::chrono::system_clock::now();
                parsedArgs.push_back(parsed);
            } else {
                errorMessages.push_back("Unknown flag: -" + key);
                parsingSuccess = false;
            }
        } else {
            // Positional argument
            if (!pImpl->positionalArgs.empty()) {
                ParsedArgument parsed;
                parsed.key = pImpl->positionalArgs[0];
                parsed.value = arg;
                parsed.source = ArgumentSource::CommandLine;
                parsed.timestamp = std::chrono::system_clock::now();
                parsedArgs.push_back(parsed);

                // Remove the used positional arg
                pImpl->positionalArgs.erase(pImpl->positionalArgs.begin());
            } else {
                errorMessages.push_back("Unexpected positional argument: " + arg);
                parsingSuccess = false;
            }
        }
    }

    // Check for required arguments
    for (const auto& argName : pImpl->positionalArgs) {
        if (std::find_if(pImpl->parsedArgs.begin(), pImpl->parsedArgs.end(),
                        [&argName](const ParsedArgument& p) { return p.key == argName; }) == pImpl->parsedArgs.end()) {
            errorMessages.push_back("Missing required positional argument: " + argName);
            parsingSuccess = false;
        }
    }

    // Resolve environment variables
    resolveEnvVariables(pImpl->parsedArgs);

    // Validate arguments
    if (!validateArguments(pImpl->parsedArgs)) {
        parsingSuccess = false;
    }

    // Process config files
    for (const auto& pair : pImpl->configFileMap) {
        auto it = std::find_if(pImpl->parsedArgs.begin(), pImpl->parsedArgs.end(),
                               [&pair](const ParsedArgument& p) { return p.key == pair.first; });
        if (it != pImpl->parsedArgs.end() && std::holds_alternative<std::string>(it->value)) {
            loadConfigFile(std::get<std::string>(it->value));
        }
    }

    return parsingSuccess;
}

bool CLIParser::applyToConfig(std::shared_ptr<Config> config) const {
    bool success = true;
    for (const auto& parsed : pImpl->parsedArgs) {
        try {
            if (std::holds_alternative<int>(parsed.value)) {
                config->set(parsed.key, std::get<int>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<int64_t>(parsed.value)) {
                config->set(parsed.key, std::get<int64_t>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<double>(parsed.value)) {
                config->set(parsed.key, std::get<double>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<bool>(parsed.value)) {
                config->set(parsed.key, std::get<bool>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<std::string>(parsed.value)) {
                config->set(parsed.key, std::get<std::string>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<std::vector<int>>(parsed.value)) {
                config->set(parsed.key, std::get<std::vector<int>>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<std::vector<double>>(parsed.value)) {
                config->set(parsed.key, std::get<std::vector<double>>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<std::vector<std::string>>(parsed.value)) {
                config->set(parsed.key, std::get<std::vector<std::string>>(parsed.value), static_cast<ConfigSource>(parsed.source));
            } else if (std::holds_alternative<std::map<std::string, nlohmann::json>>(parsed.value)) {
                config->set(parsed.key, std::get<std::map<std::string, nlohmann::json>>(parsed.value), static_cast<ConfigSource>(parsed.source));
            }
        } catch (const std::exception& e) {
            errorMessages.push_back("Error applying argument " + parsed.key + ": " + e.what());
            success = false;
        }
    }
    return success;
}

std::string CLIParser::getHelpText() const {
    std::ostringstream oss;

    oss << "Usage: program [options] [arguments]" << std::endl;
    oss << "" << std::endl;

    if (!pImpl->subcommands.empty()) {
        oss << "Subcommands:" << std::endl;
        for (const auto& cmd : pImpl->subcommands) {
            oss << "  " << cmd << std::endl;
        }
        oss << "" << std::endl;
    }

    if (!pImpl->arguments.empty()) {
        oss << "Options:" << std::endl;
        for (const auto& arg : pImpl->arguments) {
            oss << "  " << arg->getUsage() << std::endl;
            oss << "    " << arg->getHelp() << std::endl;
        }
        oss << "" << std::endl;
    }

    return oss.str();
}

std::string CLIParser::getCommandHelp(const std::string& command) const {
    auto it = std::find_if(pImpl->subcommands.begin(), pImpl->subcommands.end(),
                           [&command](const std::string& cmd) { return cmd == command; });n
    if (it == pImpl->subcommands.end()) {
        return "";
    }

    std::ostringstream oss;
    oss << "Command: " << command << std::endl;
    oss << "Description: " << "TODO: Add description" << std::endl;
    oss << "" << std::endl;

    oss << "Options:" << std::endl;
    for (const auto& arg : pImpl->arguments) {
        if (arg->getName().find(command + ".") == 0) {
            oss << "  " << arg->getUsage() << std::endl;
            oss << "    " << arg->getHelp() << std::endl;
        }
    }

    return oss.str();
}

std::string CLIParser::getArgumentHelp(const std::string& name) const {
    auto it = pImpl->argumentMap.find(name);
    if (it == pImpl->argumentMap.end()) {
        return "";
    }
    return it->second->getHelp();
}

std::string CLIParser::getErrorReport() const {
    std::ostringstream oss;
    oss << "Errors:" << std::endl;
    for (const auto& error : pImpl->errorMessages) {
        oss << "  - " << error << std::endl;
    }
    return oss.str();
}

std::vector<std::string> CLIParser::getCommandCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    for (const auto& cmd : pImpl->subcommands) {
        if (cmd.find(partial) == 0) {
            completions.push_back(cmd);
        }
    }
    return completions;
}

std::vector<std::string> CLIParser::getOptionCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    for (const auto& arg : pImpl->arguments) {
        if (arg->getName().find(partial) == 0) {
            completions.push_back(arg->getName());
        }
    }
    return completions;
}

bool CLIParser::hasArgument(const std::string& name) const {
    return pImpl->argumentMap.find(name) != pImpl->argumentMap.end();
}

ConfigValue CLIParser::getArgumentValue(const std::string& name) const {
    auto it = std::find_if(pImpl->parsedArgs.begin(), pImpl->parsedArgs.end(),
                           [&name](const ParsedArgument& p) { return p.key == name; });
    if (it != pImpl->parsedArgs.end()) {
        return it->value;
    }
    throw ConfigError("Argument not found: " + name, ConfigError::ErrorType::MissingKey);
}

std::string CLIParser::getArgumentSource(const std::string& name) const {
    auto it = std::find_if(pImpl->parsedArgs.begin(), pImpl->parsedArgs.end(),
                           [&name](const ParsedArgument& p) { return p.key == name; });n
    if (it != pImpl->parsedArgs.end()) {
        switch (it->source) {
            case ArgumentSource::CommandLine: return "command_line";
            case ArgumentSource::EnvironmentVariable: return "environment_variable";
            case ArgumentSource::ConfigFile: return "config_file";
            case ArgumentSource::Default: return "default";
            case ArgumentSource::Runtime: return "runtime";
        }
    }
    return "not_found";
}

// Command implementations
bool RunCommand::execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running neural network simulation...");

    // Extract configuration from command line args
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);

                // Set in config
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    NLM_LOG_INFO("Starting simulation with configuration:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  Timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");

    return true;
}

std::string RunCommand::getHelp() const {
    return "Run neural network simulation with specified configuration";
}

bool TrainCommand::execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Training neural network model...");

    // Extract configuration from command line args
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);

                // Set in config
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    NLM_LOG_INFO("Starting training with configuration:");
    NLM_LOG_INFO("  Learning rate: " + std::to_string(config->getOr<double>("learning_rate", 0.001)));
    NLM_LOG_INFO("  Batch size: " + std::to_string(config->getOr<int64_t>("batch_size", 32)));

    return true;
}

std::string TrainCommand::getHelp() const {
    return "Train neural network model with specified hyperparameters";
}

bool SimulateCommand::execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running simulation with current brain state...");

    // Extract configuration from command line args
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);

                // Set in config
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    NLM_LOG_INFO("Starting simulation with configuration:");
    NLM_LOG_INFO("  Steps: " + std::to_string(config->getOr<int64_t>("steps", 1000)));
    NLM_LOG_INFO("  Output format: " + config->getOr<std::string>("output_format", "console"));

    return true;
}

std::string SimulateCommand::getHelp() const {
    return "Run simulation with specified parameters";
}

bool ExploreCommand::execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Exploring neural network space...");

    // Extract configuration from command line args
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);

                // Set in config
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    NLM_LOG_INFO("Starting exploration with configuration:");
    NLM_LOG_INFO("  Search space: " + config->getOr<std::string>("search_space", "auto"));
    NLM_LOG_INFO("  Population size: " + std::to_string(config->getOr<int64_t>("population_size", 50)));

    return true;
}

std::string ExploreCommand::getHelp() const {
    return "Explore neural network space with genetic algorithms";
}

} // namespace nlm
