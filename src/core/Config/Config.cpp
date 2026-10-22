#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ParameterMetadata> parameterRegistry;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Template method implementations for parameter registration
template<typename T>
void Config::registerParameter(const std::string& key, const std::string& description,
                               const T& defaultValue, ConfigSource source, bool required,
                               const ValidationFunction& validator) {
    ParameterMetadata metadata(key, description, source, required, validator);
    pImpl->parameterRegistry[key] = metadata;
    
    // Set the default value
    set(key, defaultValue, source);
}

std::optional<ParameterMetadata> Config::getParameterMetadata(const std::string& key) const {
    auto it = pImpl->parameterRegistry.find(key);
    if (it != pImpl->parameterRegistry.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to open the file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with better validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            addError("Invalid configuration line " + std::to_string(lineNumber) + 
                    " in " + filepath + ": missing '='");
            success = false;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Check if key is registered
        auto metadata = getParameterMetadata(key);
        if (!metadata && !key.empty()) {
            addError("Unknown configuration key '" + key + "' in line " + 
                    std::to_string(lineNumber) + " of " + filepath);
            success = false;
            continue;
        }
        
        // Parse and validate the value
        ConfigValue configValue;
        try {
            configValue = parseValue(value, metadata ? metadata->validator : nullptr);
        } catch (const std::exception& e) {
            addError("Failed to parse value for key '" + key + "' in line " + 
                    std::to_string(lineNumber) + " of " + filepath + ": " + e.what());
            success = false;
            continue;
        }
        
        // Validate against parameter metadata if available
        if (metadata && !metadata->validate(configValue)) {
            addError("Validation failed for key '" + key + "' in line " + 
                    std::to_string(lineNumber) + " of " + filepath);
            success = false;
            continue;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    if (!success) {
        return false;
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
                
                // Check if key is registered
                auto metadata = getParameterMetadata(key);
                if (!metadata && !key.empty()) {
                    addError("Unknown command line argument '--' + key + "\"");
                    return false;
                }
                
                ConfigValue configValue;
                try {
                    configValue = parseValue(value, metadata ? metadata->validator : nullptr);
                } catch (const std::exception& e) {
                    addError("Failed to parse value for argument '--' + key + "\": " + std::string(e.what()));
                    return false;
                }
                
                if (metadata && !metadata->validate(configValue)) {
                    addError("Validation failed for command line argument '--' + key + "\"");
                    return false;
                }
                
                set(key, configValue, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            auto metadata = getParameterMetadata(key);
            if (!metadata && !key.empty()) {
                addError("Unknown command line argument '-' + key + "\"");
                return false;
            }
            
            ConfigValue configValue;
            try {
                configValue = parseValue(value, metadata ? metadata->validator : nullptr);
            } catch (const std::exception& e) {
                addError("Failed to parse value for argument '-' + key + "\": " + std::string(e.what()));
                return false;
            }
            
            if (metadata && !metadata->validate(configValue)) {
                addError("Validation failed for command line argument '-' + key + "\"");
                return false;
            }
            
            set(key, configValue, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot write to configuration file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    ConfigValue configValue = value;
    
    // Validate against parameter metadata if available
    auto metadata = getParameterMetadata(key);
    if (metadata && !metadata->validate(configValue)) {
        addError("Configuration validation failed for key '" + key + "'");
        return;
    }
    
    set(key, configValue, source);
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
    parameterRegistry_.clear();
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
    
    // Show validation errors if any
    if (!errors_.empty()) {
        oss << "\nConfiguration Validation Errors:\n";
        for (const auto& error : errors_) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> validationErrors;
    
    // Check for required parameters
    for (const auto& metadata : parameterRegistry_) {
        if (metadata.required && !has(metadata.key)) {
            validationErrors.push_back("Required parameter missing: " + metadata.key);
        }
    }
    
    // Validate all configured values
    for (const auto& entry : pImpl->entries) {
        auto metadata = getParameterMetadata(entry.key);
        if (metadata && metadata->validator) {
            try {
                ConfigValue tempValue;
                std::visit([&tempValue](auto&& arg) { tempValue = arg; }, entry.value);
                if (!metadata->validate(tempValue)) {
                    validationErrors.push_back("Validation failed for parameter: " + entry.key);
                }
            } catch (const std::exception& e) {
                validationErrors.push_back("Error validating parameter " + entry.key + ": " + e.what());
            }
        }
    }
    
    return validationErrors;
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

bool Config::parseBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    throw std::runtime_error("Cannot parse boolean value: " + str);
}

ConfigValue Config::parseValue(const std::string& str, const ValidationFunction& validator) {
    std::string trimmed = trim(str);
    
    // Try to parse as boolean
    try {
        bool boolValue = parseBool(trimmed);
        ConfigValue result = boolValue;
        if (validator && !validator(result)) {
            throw std::runtime_error("Validation failed for boolean value");
        }
        return result;
    } catch (...) {}
    
    // Try to parse as integer
    try {
        // Check if it's a simple integer
        if (std::regex_match(trimmed, std::regex("^-?\d+$"))) {
            int intValue = std::stoi(trimmed);
            ConfigValue result = intValue;
            if (validator && !validator(result)) {
                throw std::runtime_error("Validation failed for integer value");
            }
            return result;
        }
        
        // Check if it's a double
        if (std::regex_match(trimmed, std::regex("^-?\d+(\.\d+)?$"))) {
            double doubleValue = std::stod(trimmed);
            ConfigValue result = doubleValue;
            if (validator && !validator(result)) {
                throw std::runtime_error("Validation failed for double value");
            }
            return result;
        }
    } catch (...) {}
    
    // Parse as string (remove quotes if present)
    std::string result = trimmed;
    if (result.size() >= 2 && 
        ((result.front() == '"' && result.back() == '"') ||
         (result.front() == '\'' && result.back() == '\''))) {
        result = result.substr(1, result.size() - 2);
    }
    
    ConfigValue finalResult = result;
    if (validator && !validator(finalResult)) {
        throw std::runtime_error("Validation failed for string value");
    }
    
    return finalResult;
}

template<typename T>
bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto metadata = getParameterMetadata(key);
    if (!metadata) {
        return true; // No validation required
    }
    
    return metadata->validate(value);
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
