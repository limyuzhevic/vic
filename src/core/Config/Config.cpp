#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

namespace nlm {

// Add error handling for configuration
    bool hasErrors() const {
        return !pImpl->errorMessages.empty();
    }
    
    const std::vector<std::string>& getErrorMessages() const {
        return pImpl->errorMessages;
    }
    
    void clearErrors() {
        pImpl->errorMessages.clear();
    }
    
    void addError(const std::string& message) {
        pImpl->errorMessages.push_back(message);
    }
    
    // Validation
    bool validateAll() const {
        clearErrors();
        bool allValid = true;
        
        for (const auto& entry : pImpl->entries) {
            // Check key format
            if (!isValidKeyFormat(entry.key)) {
                addError("Invalid key format: '" + entry.key + "'. Keys must contain only alphanumeric characters, underscores, and hyphens.");
                allValid = false;
            }
            
            // Check if key is known
            if (!isValidKey(entry.key)) {
                addError("Unknown configuration key: '" + entry.key + "'. Known keys: " + getKnownKeysString());
                allValid = false;
            }
            
            // Validate value against known rules
            if (!validateValue(entry.key, entry.value)) {
                addError("Invalid value for key '" + entry.key + "': " + formatValue(entry.value) + ". " + 
                        "Expected: " + getDescription(entry.key));
                allValid = false;
            }
        }
        
        // Validate required relationships
        if (has("neuron_count") && has("region_count")) {
            auto neuronCount = getOr<int64_t>("neuron_count", 0);
            auto regionCount = getOr<int64_t>("region_count", 0);
            if (regionCount > neuronCount) {
                addError("Invalid configuration: region_count (" + std::to_string(regionCount) + 
                        ") cannot be greater than neuron_count (" + std::to_string(neuronCount) + ").");
                allValid = false;
            }
        }
        
        return allValid;
    }
    
    std::string getKnownKeysString() const {
        std::ostringstream oss;
        size_t count = 0;
        for (const auto& pair : CONFIG_DESCRIPTIONS) {
            if (count++ > 0) oss << ", ";
            oss << pair.first;
        }
        return oss.str();
    }
    
    // Set strict validation mode
    void setStrictValidation(bool strict) {
        pImpl->strictValidation = strict;
        if (strict) {
            validateAll();
        }
    }
    
    bool getStrictValidation() const {
        return pImpl->strictValidation;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->strictValidation = false;  // Default to non-strict mode
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->strictValidation = false;  // Default to non-strict mode
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

bool Config::validate() const {
    return pImpl->validateAll();
}

bool Config::hasErrors() const {
    return pImpl->hasErrors();
}

const std::vector<std::string>& Config::getErrorMessages() const {
    return pImpl->getErrorMessages();
}

void Config::clearErrors() {
    pImpl->clearErrors();
}

void Config::addError(const std::string& message) {
    pImpl->addError(message);
}

std::string Config::getValidationSummary() const {
    if (pImpl->hasErrors()) {
        std::ostringstream oss;
        oss << "Configuration validation FAILED with " << pImpl->getErrorMessages().size() << " error(s):\n";
        for (const auto& error : pImpl->getErrorMessages()) {
            oss << "  ERROR: " << error << "\n";
        }
        return oss.str();
    } else {
        return "Configuration validation PASSED. All " + std::to_string(pImpl->entries.size()) + 
               " configuration entries are valid.";
    }
}

void Config::setStrictValidation(bool strict) {
    pImpl->setStrictValidation(strict);
}

bool Config::getStrictValidation() const {
    return pImpl->getStrictValidation();
}

bool Config::validateKeyFormat(const std::string& key) {
    return isValidKeyFormat(key);
}

bool Config::isValidKey(const std::string& key) {
    return ::nlm::isValidKey(key);
}

std::string Config::getKeyDescription(const std::string& key) {
    return ::nlm::getDescription(key);
}

std::vector<std::string> Config::getKnownKeys() {
    std::vector<std::string> keys;
    for (const auto& pair : CONFIG_DESCRIPTIONS) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string Config::formatValue(const ConfigValue& value) {
    return ::nlm::formatValue(value);
}

std::string Config::getKnownKeysString() {
    return ::nlm::getKnownKeysString();
}
