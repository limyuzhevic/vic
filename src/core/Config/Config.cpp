#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <iostream>

namespace nlm {

// Config implementation with enhanced JSON support and validation

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filePath;
    bool validateOnSet;
    bool logErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnSet = true;
    pImpl->logErrors = true;
    NLM_LOG_INFO("Config initialized with JSON support");
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept = default;

Config& Config::operator=(Config&& other) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filePath = filepath;
    
    if (!fileExists(filepath)) {
        logError("Config file not found: " + filepath);
        return false;
    }
    
    std::string extension = getFileExtension(filepath);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    if (extension == "json" || extension.empty()) {
        // Try JSON parsing first
        try {
            auto json = nlohmann::json::parse(content);
            return loadFromJson(json);
        } catch (const nlohmann::json::parse_error& e) {
            logError("JSON parse error in " + filepath + ": " + std::string(e.what()));
            if (pImpl->logErrors) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }
    }
    
    // Fallback to simple format
    return loadFromSimpleFormat(content);
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return loadFromJson(json);
    } catch (const nlohmann::json::parse_error& e) {
        logError("JSON parse error: " + std::string(e.what()));
        if (pImpl->logErrors) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
        return false;
    }
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
    if (schema_.strictMode) {
        if (!validate()) {
            logError("Configuration validation failed before saving");
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot create config file: " + filepath);
        return false;
    }
    
    // Check file extension and write appropriate format
    std::string extension = getFileExtension(filepath);
    if (extension == "json") {
        auto json = saveToJsonFormat();
        file << json.dump(4);  // Pretty print with indentation
    } else {
        file << saveToSimpleFormat();
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

std::string Config::toJsonString() const {
    return saveToJsonFormat().dump(4);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return variantToValue<T>(it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setWithValidation(key, value, source);
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

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        logError("Invalid config key: " + key);
        return false;
    }
    
    bool isValid = true;
    std::string validationError;
    
    // Type-specific validation
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                     std::is_same_v<T, double>) {
            double doubleVal = (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) 
                             ? static_cast<double>(val) : val;
            if (doubleVal < -1e6 || doubleVal > 1e6) {
                isValid = false;
                validationError = "Value out of reasonable range for " + key;
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (val.empty() && !schema_.requiredKeys.empty()) {
                isValid = false;
                validationError = "Empty string value for required key: " + key;
            }
        }
    }, value);
    
    // Schema validation
    if (pImpl->validateOnSet) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            if (!it->isValid()) {
                isValid = false;
                validationError = "Schema validation failed for key: " + key;
            }
        }
    }
    
    // Custom validation callbacks
    if (isValid) {
        auto schemaIt = schema_.validations.find(key);
        if (schemaIt != schema_.validations.end()) {
            ConfigEntry entry(key, value, source);
            if (!schemaIt->second(entry)) {
                isValid = false;
                validationError = "Custom validation failed for key: " + key;
            }
        }
    }
    
    if (!isValid) {
        logError(validationError);
        if (pImpl->logErrors) {
            std::cerr << "Validation error: " << validationError << std::endl;
        }
        return false;
    }
    
    // Update or add entry
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (existingIt != pImpl->entries.end()) {
        existingIt->value = value;
        existingIt->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    return true;
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            logError("Validation failed for key: " + entry.key);
            return false;
        }
    }
    
    // Check required keys
    for (const auto& requiredKey : schema_.requiredKeys) {
        if (!has(requiredKey)) {
            logError("Missing required key: " + requiredKey);
            return false;
        }
    }
    
    // Check strict mode (reject unknown keys)
    if (schema_.strictMode) {
        // In strict mode, all keys should be known (no validation in this simple implementation)
        // In practice, you would have a complete schema definition
    }
    
    return true;
}

bool Config::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    std::string extension = filepath.substr(dotPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
        } else {
            logError("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromJson(const nlohmann::json& json) {
    if (!json.is_object()) {
        logError("JSON root is not an object");
        return false;
    }
    
    for (const auto& [key, value] : json.items()) {
        ConfigValue configValue;
        
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Handle arrays based on element type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    configValue = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else if (value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                } else {
                    logError("Unsupported array type in JSON for key: " + key);
                    continue;
                }
            } else {
                configValue = std::vector<std::string>{};
            }
        } else {
            logError("Unsupported JSON type for key: " + key);
            continue;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

std::string Config::saveToSimpleFormat() const {
    std::ostringstream oss;
    oss << "# Configuration generated by NLM\n";
    oss << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "\n\n";
    }
    
    return oss.str();
}

nlohmann::json Config::saveToJsonFormat() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json array = arg;
                json[entry.key] = array;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json array = arg;
                json[entry.key] = array;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json array = arg;
                json[entry.key] = array;
            }
        }, entry.value);
    }
    
    return json;
}

void Config::logError(const std::string& message) {
    if (message.empty()) return;
    
    // In practice, use the actual logger
    // For now, use standard error output
    std::cerr << "[Config Error] " << message << std::endl;
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    if (key.find(' ') != std::string::npos) return false;
    if (key.find('=') != std::string::npos) return false;
    if (key.find('"') != std::string::npos) return false;
    return true;
}

template<typename T>
std::optional<T> Config::variantToValue(const ConfigValue& variant) {
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(variant);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(variant);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(variant);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(variant);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(variant);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::get<std::vector<int>>(variant);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::get<std::vector<double>>(variant);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::get<std::vector<std::string>>(variant);
        }
    } catch (const std::bad_variant_access&) {
        // Return nullopt if type mismatch
    }
    return std::nullopt;
}

void Config::addValidation(const std::string& key, const ValidationCallback& callback) {
    schema_.validations[key] = callback;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

} // namespace nlm
