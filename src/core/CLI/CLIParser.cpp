#include "CLIParser.hpp"

namespace nlm {

class ConfigCLI::Impl {
public:
    std::string appName;
    std::string version;
    std::shared_ptr<Config> config;
    bool showHelp = false;
    bool showVersion = false;
    bool validateOnly = false;
    bool generateOnly = false;
    std::string configFile;
    std::vector<std::pair<std::string, std::string>> setValues;
    std::vector<std::tuple<std::string, std::string, std::string>> setTypedValues;
    std::vector<std::string> validationErrors;
};

ConfigCLI::ConfigCLI() : pImpl(std::make_unique<Impl>()) {
    pImpl->appName = "NLM";
    pImpl->version = "0.1.0";
    pImpl->config = std::make_shared<Config>();
}

bool ConfigCLI::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Parse --option=value format
        if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            pImpl->setValues.emplace_back(key, value);
        }
        else if (arg == "--help") {
            pImpl->showHelp = true;
        }
        else if (arg == "--version") {
            pImpl->showVersion = true;
        }
        else if (arg == "--validate-config") {
            pImpl->validateOnly = true;
        }
        else if (arg.find("--generate-config=") == 0) {
            pImpl->generateOnly = true;
            pImpl->configFile = arg.substr(18);
        }
        else if (arg.find("--set=") == 0) {
            std::string setArg = arg.substr(6);
            size_t pos = setArg.find('=');
            if (pos != std::string::npos) {
                std::string key = setArg.substr(0, pos);
                std::string value = setArg.substr(pos + 1);
                pImpl->setValues.emplace_back(key, value);
            }
        }
        else if (arg.find("--set-type=") == 0) {
            std::string setTypeArg = arg.substr(11);
            size_t pos1 = setTypeArg.find('=');
            size_t pos2 = setTypeArg.find(':', pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string key = setTypeArg.substr(0, pos1);
                std::string type = setTypeArg.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string value = setTypeArg.substr(pos2 + 1);
                pImpl->setTypedValues.emplace_back(key, type, value);
            }
        }
    }
    
    return true;
}

bool ConfigCLI::execute() {
    // Load existing config if file specified
    if (!pImpl->configFile.empty() && !pImpl->config->loadFromFile(pImpl->configFile)) {
        return false;
    }
    
    // Apply set values
    for (const auto& pair : pImpl->setValues) {
        if (!setValue(pair.first, pair.second)) {
            return false;
        }
    }
    
    // Apply set typed values
    for (const auto& tuple : pImpl->setTypedValues) {
        const auto& [key, type, value] = tuple;
        if (!setTypedValueFromString(key, type, value)) {
            return false;
        }
    }
    
    // Show help if requested
    if (pImpl->showHelp) {
        showHelp();
        return true;
    }
    
    // Show version if requested
    if (pImpl->showVersion) {
        showVersion();
        return true;
    }
    
    // Validate if requested
    if (pImpl->validateOnly) {
        if (!validateConfig()) {
            return false;
        }
    }
    
    // Generate if requested
    if (pImpl->generateOnly) {
        return generateConfig(pImpl->configFile);
    }
    
    return true;
}

std::string ConfigCLI::getHelp() const {
    return "Usage: nlm [OPTIONS]\n\nOptions:\n"
           "  --help                     Show this help message\n"
           "  --version                  Show version information\n"
           "  --validate-config          Validate configuration file\n"
           "  --generate-config=<file>   Generate configuration file\n"
           "  --set=<key>=<value>        Set configuration value\n"
           "  --set-type=<key>:<type>=<value> Set configuration value with type";
}

std::string ConfigCLI::getVersion() const {
    return "NLM " + pImpl->version;
}

bool ConfigCLI::validateConfig() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->config->getEntries()) {
        // Basic validation for required fields
        if (entry.source == ConfigSource::File && entry.key.empty()) {
            pImpl->validationErrors.push_back("Empty key in configuration");
        }
    }
    
    if (!pImpl->validationErrors.empty()) {
        showValidationErrors(pImpl->validationErrors);
        return false;
    }
    
    return true;
}

bool ConfigCLI::generateConfig(const std::string& filepath) const {
    return pImpl->config->saveToFile(filepath);
}

bool ConfigCLI::setValue(const std::string& key, const std::string& valueStr) {
    // Try bool first
    if (valueStr == "true") {
        pImpl->config->set(key, true, ConfigSource::CommandLine);
        return true;
    } else if (valueStr == "false") {
        pImpl->config->set(key, false, ConfigSource::CommandLine);
        return true;
    }
    
    // Try int
    try {
        size_t pos;
        int value = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            pImpl->config->set(key, value, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double value = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            pImpl->config->set(key, value, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {}
    
    // Fall back to string
    pImpl->config->set(key, valueStr, ConfigSource::CommandLine);
    return true;
}

template<typename T>
bool ConfigCLI::setTypedValue(const std::string& key, const std::string& valueStr) {
    return setTypedValueFromString(key, getTypeName<T>(), valueStr);
}

bool ConfigCLI::setTypedValueFromString(const std::string& key, const std::string& typeStr, const std::string& valueStr) {
    try {
        if (typeStr == "int") {
            size_t pos;
            int value = std::stoi(valueStr, &pos);
            if (pos != valueStr.size()) return false;
            pImpl->config->set(key, value, ConfigSource::CommandLine);
            return true;
        }
        else if (typeStr == "bool") {
            if (valueStr == "true") {
                pImpl->config->set(key, true, ConfigSource::CommandLine);
                return true;
            }
            else if (valueStr == "false") {
                pImpl->config->set(key, false, ConfigSource::CommandLine);
                return true;
            }
            return false;
        }
        else if (typeStr == "double") {
            size_t pos;
            double value = std::stod(valueStr, &pos);
            if (pos != valueStr.size()) return false;
            pImpl->config->set(key, value, ConfigSource::CommandLine);
            return true;
        }
        else if (typeStr == "string") {
            pImpl->config->set(key, valueStr, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {
    }
    return false;
}

std::string ConfigCLI::getTypeName() const {
    return "int";
}

void ConfigCLI::showHelp() const {
    std::cout << getHelp() << std::endl;
}

void ConfigCLI::showVersion() const {
    std::cout << getVersion() << std::endl;
}

void ConfigCLI::showValidationErrors(const std::vector<std::string>& errors) const {
    std::cout << "Validation errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
}

} // namespace nlm