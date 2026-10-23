#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <chrono>
#include <ctime>
#include <set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ValidationCallback> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check if file is readable
    if (!file.good()) {
        NLM_LOG_ERROR("Config file is not in a good state: " + filepath);
        return false;
    }
    
    // Check file size to avoid loading huge files (100MB limit)
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Warn if file is very large (potential DoS)
    if (fileSize > 100 * 1024 * 1024) {  // 100MB
        NLM_LOG_WARNING("Config file is very large (" + std::to_string(fileSize) + " bytes). This may impact performance.");
    }
    
    // Get file modification time for logging
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(filepath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto mtime = std::chrono::system_clock::to_time_t(sctp);
    
    std::string line;
    size_t lineNumber = 0;
    size_t totalLinesProcessed = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        totalLinesProcessed++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": no '=' found: " + line);
            continue;
        }
        
        if (pos == 0) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": key is empty: " + line);
            continue;
        }
        
        if (pos == line.length() - 1) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": value is empty: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": key is empty: " + line);
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value is not empty after trimming
        if (value.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": value is empty after trimming: " + line);
            continue;
        }
        
        // Try to infer type from value
        ConfigValue configValue;
        if (value == "true" || value == "false") {
            configValue = (value == "true");
        } else if (value.find('.') != std::string::npos) {
            configValue = std::stod(value);
        } else {
            try {
                // Try integer first
                configValue = std::stoll(value);
            } catch (...) {
                // Fall back to string
                configValue = value;
            }
        }
        
        ConfigEntry entry(key, configValue, ConfigSource::File);
        
        // Validate against registered validators
        bool validationPassed = true;
        auto it = pImpl->validators.find(key);
        if (it != pImpl->validators.end()) {
            validationPassed = it->second(key, configValue);
            if (!validationPassed) {
                NLM_LOG_WARNING("Validation failed for config key '" + key + "' with value '" + value + "'");
                continue;
            }
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    // Log summary of config file processing
    NLM_LOG_INFO("Config file loaded: " + filepath + " (" + std::to_string(totalLinesProcessed) + " lines processed, modified: " + std::ctime(&mtime) + ")");
    
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
            } else {
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
            }
        } 
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            NLM_LOG_WARNING("Invalid command line argument (missing value): " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    if (!file.good()) {
        NLM_LOG_ERROR("Config file is not in a good state for writing: " + filepath);
        return false;
    }
    
    // Add header comment
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated at: " << std::ctime(&std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = " << getValueAsString(entry.value) << std::endl;
        file << std::endl;
    }
    
    // Check if write was successful
    if (!file.good()) {
        NLM_LOG_ERROR("Failed to write to config file: " + filepath);
        return false;
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

int Config::getInt(const std::string& key, int defaultValue) const {
    return getOr<int>(key, defaultValue);
}

int64_t Config::getInt64(const std::string& key, int64_t defaultValue) const {
    return getOr<int64_t>(key, defaultValue);
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    return getOr<double>(key, defaultValue);
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    return getOr<bool>(key, defaultValue);
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    return getOr<std::string>(key, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue;
    if (value == "true") {
        configValue = true;
    } else if (value == "false") {
        configValue = false;
    } else if (value.find('.') != std::string::npos) {
        configValue = std::stod(value);
    } else {
        try {
            configValue = std::stoll(value);
        } catch (...) {
            configValue = value;
        }
    }
    set(key, configValue, source);
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

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::validate() const {
    std::set<std::string> errors;
    
    // Check for required fields
    for (const auto& entry : pImpl->entries) {
        if (entry.required && 
            (entry.value.index() == typeid(std::string) && std::get<std::string>(entry.value).empty()) ||
            (entry.value.index() == typeid(int) && std::get<int>(entry.value) == 0) ||
            (entry.value.index() == typeid(double) && std::get<double>(entry.value) == 0.0) ||
            (entry.value.index() == typeid(bool) && std::get<bool>(entry.value) == false)) {
            errors.insert("Required field '" + entry.key + "' is not set");
        }
        
        // Run registered validators
        auto it = pImpl->validators.find(entry.key);
        if (it != pImpl->validators.end() && !it->second(entry.key, entry.value)) {
            errors.insert("Validation failed for field '" + entry.key + "'");
        }
    }
    
    // Check for duplicate keys
    std::set<std::string> uniqueKeys;
    for (const auto& entry : pImpl->entries) {
        if (!uniqueKeys.insert(entry.key).second) {
            errors.insert("Duplicate key in configuration: '" + entry.key + "'");
        }
    }
    
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Config validation error: " + error);
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return std::vector<std::string>(); // Would collect and return errors
}

void Config::addValidation(const std::string& key, ValidationCallback validator) {
    pImpl->validators[key] = validator;
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->validators.erase(key);
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << getValueAsString(entry.value) << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    if (!pImpl->validators.empty()) {
        oss << "  Validators: " << pImpl->validators.size() << " active" << std::endl;
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

std::string Config::getValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

} // namespace nlm