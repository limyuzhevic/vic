#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigValidator> validators;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, ValidationResult> validationCache;
    bool validationEnabled = true;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    return loadFromString(content, "auto");
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setValueFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Failed to parse command line argument: " + arg);
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing value): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setValueFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_WARNING("Failed to parse command line argument: " + arg);
                success = false;
            }
        }
        else {
            // Unknown argument format, ignore but warn
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    return success;
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    // Clear existing configuration
    clear();
    
    // Auto-detect format
    std::string detectedFormat = format;
    if (format == "auto") {
        detectedFormat = detectFormat(content);
    }
    
    try {
        if (detectedFormat == "json") {
            return parseJSON(content);
        } else if (detectedFormat == "ini") {
            return parseINI(content);
        } else if (detectedFormat == "toml") {
            return parseTOML(content);
        } else {
            NLM_LOG_ERROR("Unsupported configuration format: " + detectedFormat);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse configuration: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write in JSON format
    file << toJSON();
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return getValue<T>(it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
std::optional<T> Config::getValidated(const std::string& key,
                                     const std::vector<std::string>& allowedValues) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return std::nullopt;
    }
    
    // Validate the value
    std::vector<std::string> errors;
    bool isValid = validateValue(key, val.value(), errors);
    
    if (!isValid && pImpl->validationEnabled) {
        std::ostringstream oss;
        oss << "Configuration validation failed for key '" << key << "':";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        NLM_LOG_WARNING(oss.str());
    }
    
    return val;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // Invalidate validation cache for this key
        pImpl->validationCache.erase(key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate the new value
    std::vector<std::string> errors;
    validateValue(key, value, errors);
    if (!errors.empty() && pImpl->validationEnabled) {
        NLM_LOG_WARNING("Configuration validation warning for key '" + key + "':" + 
                       getValidationErrorString(errors));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setValueFromString(key, value, source);
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

void Config::setDefault(const std::string& key, const ConfigValue& value, const std::string& description) {
    set(key, value, ConfigSource::Default);
    if (!description.empty()) {
        setDescription(key, description);
    }
    
    // Validate default value
    std::vector<std::string> errors;
    if (!validateValue(key, value, errors)) {
        NLM_LOG_ERROR("Invalid default configuration for key '" + key + "':" + 
                     getValidationErrorString(errors));
    }
}

void Config::setDefault(const std::string& key, const std::string& value, const std::string& description) {
    setDefault(key, ConfigValue(value), description);
}

void Config::setDefault(const std::string& key, int value, const std::string& description) {
    setDefault(key, ConfigValue(value), description);
}

void Config::setDefault(const std::string& key, double value, const std::string& description) {
    setDefault(key, ConfigValue(value), description);
}

void Config::setDefault(const std::string& key, bool value, const std::string& description) {
    setDefault(key, ConfigValue(value), description);
}

void Config::setValidator(const std::string& key, ConfigValidator validator) {
    pImpl->validators[key] = validator;
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
    pImpl->validators.erase(key);
    pImpl->validationCache.erase(key);
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
    pImpl->validators.clear();
    pImpl->validationCache.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Validation enabled: " << (pImpl->validationEnabled ? "yes" : "no") << "\n";
    
    if (pImpl->entries.empty()) {
        return oss.str();
    }
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        oss << valueToString(entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    if (!pImpl->validationEnabled) {
        return result;
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> errors;
        if (!validateValue(entry.key, entry.value, errors)) {
            for (const auto& error : errors) {
                result.addError("Key '" + entry.key + "': " + error);
            }
        }
        
        // Check validator if present
        auto validatorIt = pImpl->validators.find(entry.key);
        if (validatorIt != pImpl->validators.end()) {
            std::vector<std::string> validatorErrors;
            if (!validatorIt->second(entry.value, validatorErrors)) {
                for (const auto& error : validatorErrors) {
                    result.addError("Validator failed for key '" + entry.key + "': " + error);
                }
            }
        }
    }
    
    return result;
}

std::vector<std::string> Config::getValidationErrors(const std::string& key) const {
    std::vector<std::string> errors;
    if (has(key)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        if (it != pImpl->entries.end()) {
            validateValue(key, it->value, errors);
        }
    }
    return errors;
}

bool Config::isConsistent() const {
    std::vector<std::string> errors;
    return validateConsistency(errors);
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

void Config::setSource(const std::string& key, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->source = source;
    }
}

std::string Config::getAsString(const std::string& key) const {
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return val.value();
    }
    return "";
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        bool exists = false;
        for (const auto& existing : pImpl->entries) {
            if (existing.key == entry.key) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            pImpl->entries.push_back(entry);
        }
    }
    
    // Merge validators
    for (const auto& [key, validator] : other.pImpl->validators) {
        pImpl->validators[key] = validator;
    }
}

void Config::resetToDefaults() {
    // Find entries with Default source and restore them
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source != ConfigSource::Default; });n    
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << valueToString(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::fromJSON(const std::string& json) {
    try {
        return parseJSON(json);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON configuration: ") + e.what());
        return false;
    }
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::loadFromEnv(const std::string& prefix) {
    // Load environment variables that start with the given prefix
    for (const auto& entry : pImpl->entries) {
        std::string envVar = getEnvVarName(entry.key);
        if (std::getenv(envVar.c_str()) != nullptr) {
            std::string value = std::getenv(envVar.c_str());
            setValueFromString(entry.key, value, ConfigSource::Runtime);
        }
    }
}

std::string Config::getEnvVarName(const std::string& key) {
    return "NLM_" + toLower(key);
}

void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptions[key] = description;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

// Implementation helper functions
template<typename T>
std::optional<T> Config::getValue(const ConfigValue& value) const {
    try {
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

bool Config::setValueFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse the value based on the key type
    // For now, just store as string
    set(key, ConfigValue(value), source);
    return true;
}

bool Config::parseJSON(const std::string& content) {
    // Simple JSON parsing - in a real implementation, use a proper JSON library
    std::vector<ConfigEntry> entries;
    
    // This is a simplified parser that handles basic key-value pairs
    // For production use, integrate with a proper JSON parser like nlohmann/json
    
    std::string::size_type pos = 0;
    while (pos < content.size()) {
        // Find key
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        std::string::size_type keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        
        // Find value
        pos = content.find(':', keyEnd);
        if (pos == std::string::npos) break;
        
        std::string::size_type valueStart = content.find_first_not_of(' ', pos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string::size_type valueEnd = content.find_first_of(",}", valueStart);
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
        
        // Remove quotes from string values
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        set(key, valueStr, ConfigSource::File);
        
        // Move to next item
        pos = valueEnd;
    }
    
    return true;
}

bool Config::parseINI(const std::string& content) {
    // Simple INI parsing
    std::vector<ConfigEntry> entries;
    
    std::string currentSection = "";
    
    std::string::size_type pos = 0;
    while (pos < content.size()) {
        std::string line;
        
        // Find end of line
        std::string::size_type newline = content.find('\n', pos);
        if (newline == std::string::npos) {
            line = content.substr(pos);
            pos = content.size();
        } else {
            line = content.substr(pos, newline - pos);
            pos = newline + 1;
        }
        
        // Trim whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Check for section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key=value pair
        std::string::size_type equal = line.find('=');
        if (equal == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, equal));
        std::string value = trim(line.substr(equal + 1));
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Append section prefix if not empty
        if (!currentSection.empty()) {
            key = currentSection + "." + key;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseTOML(const std::string& content) {
    // Simplified TOML parsing - real implementation would use a TOML parser
    // For now, fall back to INI parsing
    return parseINI(content);
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

bool Config::stringToValue(const std::string& str, ConfigValue& value) {
    // Try to parse string as different types
    if (str.empty()) {
        return false;
    }
    
    // Check if it's a boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Check if it's an integer
    try {
        size_t pos;
        int intValue = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intValue;
            return true;
        }
    } catch (...) {}
    
    // Check if it's a floating point
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleValue;
            return true;
        }
    } catch (...) {}
    
    // Treat as string
    value = str;
    return true;
}

std::string Config::detectFormat(const std::string& content) {
    // Simple format detection
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return "json";
    } else if (content.find('[') != std::string::npos) {
        return "ini";
    }
    return "ini"; // Default to INI
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, std::vector<std::string>& errors) const {
    bool isValid = true;
    
    // Get validation cache
    auto cacheIt = pImpl->validationCache.find(key);
    if (cacheIt != pImpl->validationCache.end()) {
        return cacheIt->second.isValid;
    }
    
    // Basic type validation
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        if (std::get<int64_t>(value) < 0) {
            errors.push_back("Value must be non-negative");
            isValid = false;
        }
    }
    
    if (std::holds_alternative<double>(value)) {
        if (std::get<double>(value) < 0.0) {
            errors.push_back("Value must be non-negative");
            isValid = false;
        }
    }
    
    if (std::holds_alternative<std::string>(value)) {
        const std::string& str = std::get<std::string>(value);
        if (str.empty()) {
            errors.push_back("Value must not be empty");
            isValid = false;
        }
    }
    
    // Vector validation
    if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        if (vec.size() > 1000) {
            errors.push_back("Vector size too large (max: 1000)");
            isValid = false;
        }
    }
    
    if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        if (vec.size() > 1000) {
            errors.push_back("Vector size too large (max: 1000)");
            isValid = false;
        }
    }
    
    if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        if (vec.size() > 1000) {
            errors.push_back("Vector size too large (max: 1000)");
            isValid = false;
        }
        for (const auto& item : vec) {
            if (item.empty()) {
                errors.push_back("String items in vector must not be empty");
                isValid = false;
                break;
            }
        }
    }
    
    // Custom validator
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        std::vector<std::string> validatorErrors;
        if (!validatorIt->second(value, validatorErrors)) {
            errors.insert(errors.end(), validatorErrors.begin(), validatorErrors.end());
            isValid = false;
        }
    }
    
    // Cache the result
    ValidationResult cacheResult;
    cacheResult.isValid = isValid;
    cacheResult.errors = errors;
    pImpl->validationCache[key] = cacheResult;
    
    return isValid;
}

bool Config::validateConsistency(const std::vector<std::string>& errors) const {
    // Check for consistency between related parameters
    std::vector<std::string> localErrors;
    
    // Example: Check that neuron_count and region_count are reasonable
    auto neuronCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "neuron_count"; });
    auto regionCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "region_count"; });
    
    if (neuronCountIt != pImpl->entries.end() && regionCountIt != pImpl->entries.end()) {
        if (std::holds_alternative<int64_t>(neuronCountIt->value) && 
            std::holds_alternative<int64_t>(regionCountIt->value)) {
            
            int64_t neuronCount = std::get<int64_t>(neuronCountIt->value);
            int64_t regionCount = std::get<int64_t>(regionCountIt->value);
            
            if (neuronCount < regionCount) {
                localErrors.push_back("neuron_count must be greater than or equal to region_count");
            }
        }
    }
    
    errors.insert(errors.end(), localErrors.begin(), localErrors.end());
    return localErrors.empty();
}

std::string Config::getValidationErrorString(const std::vector<std::string>& errors) const {
    if (errors.empty()) return "";
    
    std::ostringstream oss;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << errors[i];
    }
    return oss.str();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
