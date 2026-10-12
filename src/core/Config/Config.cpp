#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;
    
    Impl() : entries() {}
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
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = ConfigHelper::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = ConfigHelper::trim(line.substr(0, pos));
            std::string value = ConfigHelper::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to determine value type
            bool success = false;
            
            // Try integer
            try {
                size_t posInt;
                int64_t intValue = std::stoll(value, &posInt);
                if (posInt == value.size()) {
                    set(key, intValue, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
            
            // Try double
            if (!success) {
                try {
                    size_t posDouble;
                    double doubleValue = std::stod(value, &posDouble);
                    if (posDouble == value.size()) {
                        set(key, doubleValue, ConfigSource::File);
                        success = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean
            if (!success) {
                if (value == "true" || value == "1" || value == "yes" || value == "on") {
                    set(key, true, ConfigSource::File);
                    success = true;
                } else if (value == "false" || value == "0" || value == "no" || value == "off") {
                    set(key, false, ConfigSource::File);
                    success = true;
                }
            }
            
            // Try vector
            if (!success && (value.front() == '[' && value.back() == ']')) {
                std::string inner = value.substr(1, value.size() - 2);
                std::vector<std::string> tokens;
                std::stringstream ss(inner);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token = ConfigHelper::trim(token);
                    if (!token.empty()) {
                        tokens.push_back(token);
                    }
                }
                
                if (!tokens.empty()) {
                    // Try to parse as integers
                    bool allInt = true;
                    std::vector<int64_t> intVector;
                    for (const auto& token : tokens) {
                        try {
                            size_t posToken;
                            int64_t intVal = std::stoll(token, &posToken);
                            if (posToken == token.size()) {
                                intVector.push_back(intVal);
                            } else {
                                allInt = false;
                                break;
                            }
                        } catch (...) {
                            allInt = false;
                            break;
                        }
                    }
                    
                    if (allInt) {
                        set(key, intVector, ConfigSource::File);
                        success = true;
                    } else {
                        // Try to parse as doubles
                        bool allDouble = true;
                        std::vector<double> doubleVector;
                        for (const auto& token : tokens) {
                            try {
                                size_t posToken;
                                double doubleVal = std::stod(token, &posToken);
                                if (posToken == token.size()) {
                                    doubleVector.push_back(doubleVal);
                                } else {
                                    allDouble = false;
                                    break;
                                }
                            } catch (...) {
                                allDouble = false;
                                break;
                            }
                        }
                        
                        if (allDouble) {
                            set(key, doubleVector, ConfigSource::File);
                            success = true;
                        } else {
                            // Keep as string vector
                            std::vector<std::string> stringVector = tokens;
                            set(key, stringVector, ConfigSource::File);
                            success = true;
                        }
                    }
                }
            }
            
            // Fallback to string
            if (!success) {
                set(key, value, ConfigSource::File);
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        switch (entry.value.getType()) {
            case ConfigValue::Type::String:
                file << "\"" << entry.value.getString() << "\"";
                break;
            case ConfigValue::Type::Int:
                file << entry.value.getInt();
                break;
            case ConfigValue::Type::Double:
                file << entry.value.getDouble();
                break;
            case ConfigValue::Type::Bool:
                file << (entry.value.getBool() ? "true" : "false");
                break;
            case ConfigValue::Type::VectorInt: {
                const auto& vec = entry.value.getVectorInt();
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case ConfigValue::Type::VectorDouble: {
                const auto& vec = entry.value.getVectorDouble();
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case ConfigValue::Type::VectorString: {
                const auto& vec = entry.value.getVectorString();
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << vec[i] << "\"";
                }
                file << "]";
                break;
            }
            default:
                file << "unknown";
                break;
        }
        
        file << "\n\n";
    }
    
    return true;
}

// Get values with type safety
std::optional<std::string> Config::getString(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::String) {
        return it->value.getString();
    }
    return std::nullopt;
}

std::optional<int64_t> Config::getInt(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::Int) {
        return it->value.getInt();
    }
    return std::nullopt;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::Double) {
        return it->value.getDouble();
    }
    return std::nullopt;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::Bool) {
        return it->value.getBool();
    }
    return std::nullopt;
}

std::optional<std::vector<int64_t>> Config::getVectorInt(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::VectorInt) {
        return it->value.getVectorInt();
    }
    return std::nullopt;
}

std::optional<std::vector<double>> Config::getVectorDouble(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::VectorDouble) {
        return it->value.getVectorDouble();
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> Config::getVectorString(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    if (it->value.getType() == ConfigValue::Type::VectorString) {
        return it->value.getVectorString();
    }
    return std::nullopt;
}

// Get values with default values
std::string Config::getStringOr(const std::string& key, const std::string& defaultValue) const {
    auto val = getString(key);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t Config::getIntOr(const std::string& key, int64_t defaultValue) const {
    auto val = getInt(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDoubleOr(const std::string& key, double defaultValue) const {
    auto val = getDouble(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBoolOr(const std::string& key, bool defaultValue) const {
    auto val = getBool(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<int64_t> Config::getVectorIntOr(const std::string& key, const std::vector<int64_t>& defaultValue) const {
    auto val = getVectorInt(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<double> Config::getVectorDoubleOr(const std::string& key, const std::vector<double>& defaultValue) const {
    auto val = getVectorDouble(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<std::string> Config::getVectorStringOr(const std::string& key, const std::vector<std::string>& defaultValue) const {
    auto val = getVectorString(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set values
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        switch (entry.value.getType()) {
            case ConfigValue::Type::String:
                oss << "\"" << entry.value.getString() << "\"";
                break;
            case ConfigValue::Type::Int:
                oss << entry.value.getInt();
                break;
            case ConfigValue::Type::Double:
                oss << entry.value.getDouble();
                break;
            case ConfigValue::Type::Bool:
                oss << (entry.value.getBool() ? "true" : "false");
                break;
            case ConfigValue::Type::VectorInt: {
                const auto& vec = entry.value.getVectorInt();
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
                break;
            }
            case ConfigValue::Type::VectorDouble: {
                const auto& vec = entry.value.getVectorDouble();
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
                break;
            }
            case ConfigValue::Type::VectorString: {
                const auto& vec = entry.value.getVectorString();
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << vec[i] << "\"";
                }
                oss << "]";
                break;
            }
            default:
                oss << "unknown";
                break;
        }
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Basic validation - can be extended with more sophisticated rules
    for (const auto& entry : pImpl->entries) {
        // Check for empty key
        if (entry.key.empty()) {
            return false;
        }
        
        // Check for invalid characters in key
        if (!ConfigHelper::isValidKey(entry.key)) {
            return false;
        }
        
        // Check for value constraints based on type
        switch (entry.value.getType()) {
            case ConfigValue::Type::Int: {
                int64_t intVal = entry.value.getInt();
                if (intVal < INT64_MIN || intVal > INT64_MAX) {
                    return false;
                }
                break;
            }
            case ConfigValue::Type::Double: {
                double doubleVal = entry.value.getDouble();
                if (std::isinf(doubleVal) || std::isnan(doubleVal)) {
                    return false;
                }
                break;
            }
            case ConfigValue::Type::VectorInt: {
                const auto& vec = entry.value.getVectorInt();
                for (const auto& val : vec) {
                    if (val < INT64_MIN || val > INT64_MAX) {
                        return false;
                    }
                }
                break;
            }
            case ConfigValue::Type::VectorDouble: {
                const auto& vec = entry.value.getVectorDouble();
                for (const auto& val : vec) {
                    if (std::isinf(val) || std::isnan(val)) {
                        return false;
                    }
                }
                break;
            }
            default:
                // No validation for other types
                break;
        }
    }
    
    return true;
}

Config::ConfigStats Config::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    ConfigStats stats = {0};
    
    for (const auto& entry : pImpl->entries) {
        stats.totalEntries++;
        
        switch (entry.source) {
            case ConfigSource::Default:
                stats.defaultEntries++;
                break;
            case ConfigSource::File:
                stats.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                stats.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                stats.runtimeEntries++;
                break;
        }
        
        switch (entry.value.getType()) {
            case ConfigValue::Type::String:
                stats.stringEntries++;
                break;
            case ConfigValue::Type::Int:
                stats.intEntries++;
                break;
            case ConfigValue::Type::Double:
                stats.doubleEntries++;
                break;
            case ConfigValue::Type::Bool:
                stats.boolEntries++;
                break;
            case ConfigValue::Type::VectorInt:
            case ConfigValue::Type::VectorDouble:
            case ConfigValue::Type::VectorString:
                stats.vectorEntries++;
                break;
            default:
                break;
        }
    }
    
    return stats;
}

// Internal implementation
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

std::string ConfigHelper::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigHelper::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigHelper::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    
    // Allow alphanumeric characters, underscores, and hyphens
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

std::string ConfigHelper::sanitizeKey(const std::string& key) {
    std::string result;
    for (char c : key) {
        if (std::isalnum(c) || c == '_' || c == '-') {
            result += c;
        } else {
            result += '_';
        }
    }
    return result;
}

} // namespace nlm