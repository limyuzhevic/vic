#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), 
    validationEnabled_(true), 
    logErrors_(true), 
    paramRanges_(setupParameterRanges()),
    paramValidators_(setupParameterValidators()),
    paramDefaults_(setupDefaultParameters()) {
    // Initialize with default parameters
    for (const auto& [key, value] : paramDefaults_) {
        set(key, value, ConfigSource::Default);
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ValidationResult Config::validateParameter(const std::string& key, const ConfigValue& value) const {
    if (!validationEnabled_) {
        return ValidationResult();
    }
    
    // Check if key exists
    if (!has(key)) {
        return ValidationResult(true, "Unknown parameter: " + key);
    }
    
    // Apply type validation
    auto typeValidator = getTypeValidator(key);
    if (typeValidator) {
        ValidationResult result = typeValidator(value);
        if (!result.isValid) {
            return result;
        }
    }
    
    // Apply range constraint validation
    auto rangeValidator = getRangeValidator(key);
    if (rangeValidator) {
        ValidationResult result = rangeValidator(value);
        if (!result.isValid) {
            return result;
        }
    }
    
    // Apply custom validator
    auto customValidator = getCustomValidator(key);
    if (customValidator) {
        ValidationResult result = customValidator(value);
        if (!result.isValid) {
            return result;
        }
    }
    
    return ValidationResult();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        if (logErrors_) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
        }
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key format
            if (key.empty() || key.find(' ') != std::string::npos) {
                if (logErrors_) {
                    NLM_LOG_WARNING("Invalid config key format at line " + std::to_string(lineNum) + ": " + key);
                }
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
            // Validate after setting
            ValidationResult validation = validateParameter(key, getValue(key));
            if (!validation.isValid && logErrors_) {
                NLM_LOG_ERROR("Invalid value for parameter '" + key + "' at line " + 
                             std::to_string(lineNum) + ": " + validation.errorMessage);
            }
        } else {
            if (logErrors_) {
                NLM_LOG_WARNING("Invalid format in config file line " + std::to_string(lineNum) + ": " + line);
            }
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

// Improved saveToFile that saves actual values with proper formatting
template<typename T>
void Config::saveValueToStream(std::ofstream& file, const T& value) const {
    if constexpr (std::is_same_v<T, std::string>) {
        file << '"' << value << '"';
    } else if constexpr (std::is_same_v<T, bool>) {
        file << (value ? "true" : "false");
    } else if constexpr (std::is_same_v<T, int64_t> || 
                        std::is_same_v<T, int> || 
                        std::is_same_v<T, double>) {
        file << value;
    } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
        file << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) file << ", ";
            file << value[i];
        }
        file << "]";
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        file << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) file << ", ";
            file << value[i];
        }
        file << "]";
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        file << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) file << ", ";
            file << '"' << value[i] << '"';
        }
        file << "]";
    } else {
        // Fallback for unknown types
        file << "";
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        if (logErrors_) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        }
        return false;
    }
    
    // Add header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# This file contains all configuration parameters with their current values\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Add parameter description as comment
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Add parameter source as comment
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Write the parameter key and value
        file << entry.key << " = ";
        
        // Visit the variant and save the appropriate value
        std::visit([&file, this](auto&& arg) {
            saveValueToStream(file, arg);
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Helper function for JSON/YAML parsing (placeholder)
bool Config::parseJSONFile(const std::string& filepath) {
    // TODO: Implement proper JSON parsing using a library like nlohmann::json
    // For now, fall back to simple parser
    return loadFromFile(filepath);
}

bool Config::parseYAMLFile(const std::string& filepath) {
    // TODO: Implement proper YAML parsing using a library like yaml-cpp
    // For now, fall back to simple parser
    return loadFromFile(filepath);
}

Config::Config(const DefaultConfigParams& defaults) : Config() {
    // Override defaults with provided parameters
    for (const auto& [key, value] : paramDefaults_) {
        set(key, value, ConfigSource::Default);
    }
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = parameterDescriptions_.find(key);
    if (it != parameterDescriptions_.end()) {
        return it->second;
    }
    return "";
}

void Config::setParameterDescription(const std::string& key, const std::string& description) {
    parameterDescriptions_[key] = description;
}

Config::ParameterRange Config::getParameterRange(const std::string& key) const {
    auto it = paramRanges_.find(key);
    if (it != paramRanges_.end()) {
        return it->second;
    }
    return ParameterRange();
}

void Config::setParameterRange(const std::string& key, const ParameterRange& range) {
    paramRanges_[key] = range;
}

void Config::enableValidation(bool enable) {
    validationEnabled_ = enable;
}

bool Config::isValidationEnabled() const {
    return validationEnabled_;
}

void Config::setLogErrors(bool enable) {
    logErrors_ = enable;
}

bool Config::getLogErrors() const {
    return logErrors_;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ValidationResult validation = validateParameter(key, value);
    if (!validation.isValid && logErrors_) {
        NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + validation.errorMessage);
        if (!validation.warningMessage.empty()) {
            NLM_LOG_WARNING("Warning: " + validation.warningMessage);
        }
        return; // Don't apply invalid values
    }
    
    // Update entry if exists, otherwise create new one
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Get description if available
        std::string description = getParameterDescription(key);
        pImpl->entries.emplace_back(key, value, source, description);
    }
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
