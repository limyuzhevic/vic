// Enhanced configuration implementation with validation support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::set<std::string>> dependencies;
    std::map<std::string, std::vector<std::string>> validationErrors;
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
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnv(const std::map<std::string, std::string>& envVars) {
    for (const auto& [key, value] : envVars) {
        if (key.find("NLM_") == 0) {
            std::string normalizedKey = key.substr(4); // Remove "NLM_" prefix
            std::transform(normalizedKey.begin(), normalizedKey.end(), normalizedKey.begin(), ::tolower);
            set(normalizedKey, value, ConfigSource::Runtime);
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
        std::visit([&file](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                file << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << "\", \"";
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = " << val << "\n";
            }
        }, entry.value);
        file << "\n";
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
        it->validated = false; // Mark as needing re-validation
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
    pImpl->dependencies.erase(key);
    pImpl->validationErrors.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->dependencies.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
                for (size_t i = 0; i < val.size(); ++i) {
                    oss << val[i];
                    if (i < val.size() - 1) oss << ", ";
                }
            } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
                for (size_t i = 0; i < val.size(); ++i) {
                    oss << val[i];
                    if (i < val.size() - 1) oss << ", ";
                }
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (size_t i = 0; i < val.size(); ++i) {
                    oss << "\"" << val[i] << "\"";
                    if (i < val.size() - 1) oss << ", \";
                }
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate(const ConfigSchema& schema) const {
    ValidationResult result;
    
    // Validate each entry against schema
    for (const auto& entry : pImpl->entries) {
        auto errors = validateEntry(entry, schema);
        if (!errors.empty()) {
            result.valid = false;
            for (const auto& error : errors) {
                result.addError(error);
            }
        }
    }
    
    return result;
}

bool Config::isValid(const ConfigSchema& schema) const {
    ValidationResult result = validate(schema);
    return result.valid;
}

std::vector<std::string> Config::getValidationErrors(const ConfigSchema& schema, const std::string& key) const {
    std::vector<std::string> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        errors = validateEntry(*it, schema);
    }
    
    return errors;
}

void Config::applyBatch(const std::map<std::string, ConfigValue>& batch, ConfigSource source) {
    for (const auto& [key, value] : batch) {
        set(key, value, source);
    }
}

std::vector<std::string> Config::validateEntry(const ConfigEntry& entry, const ConfigSchema& schema) const {
    std::vector<std::string> errors;
    
    // Check if key exists in schema
    // For now, we'll just check for basic type validation
    // A more complete implementation would check schema entries
    
    // Basic type validation
    try {
        if (entry.value.index() == typeid(int)) {
            int val = std::get<int>(entry.value);
            if (val < 0) errors.push_back("Value for key '" + entry.key + "' must be non-negative");
        } else if (entry.value.index() == typeid(double)) {
            double val = std::get<double>(entry.value);
            if (val < 0.0) errors.push_back("Value for key '" + entry.key + "' must be non-negative");
        } else if (entry.value.index() == typeid(std::string)) {
            // String validation
        } else if (entry.value.index() == typeid(bool)) {
            // Boolean validation
        }
    } catch (const std::bad_variant_access&) {
        errors.push_back("Invalid configuration value type for key '" + entry.key + "'");
    }
    
    return errors;
}

void Config::updateDependencies(const std::string& key, const std::set<std::string>& newDeps) {
    pImpl->dependencies[key] = newDeps;
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
// This section provides compiled versions of the template functions
// to reduce runtime compilation overhead

// Integer type
template std::optional<int> Config::get<int>(const std::string&) const;
template int Config::getOr<int>(const std::string&, const int&) const;

// 64-bit integer type  
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;

// Double/floating point type
template std::optional<double> Config::get<double>(const std::string&) const;
template double Config::getOr<double>(const std::string&, const double&) const;

// Boolean type
template std::optional<bool> Config::get<bool>(const std::string&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;

// String type
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm