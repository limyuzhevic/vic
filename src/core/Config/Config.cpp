#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <string_view>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::unordered_map<std::string, size_t> keyToIndex;  // Optimized lookup
    bool isSorted = false;  // For maintaining sorted keys
    
    // Performance optimization: cache validation results
    mutable bool validationCached = false;
    mutable std::string validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

[[nodiscard]] bool Config::loadFromFile(const std::string& filepath) noexcept {
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
        
        // Parse simple key=value pairs with better error handling
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
            
            // Try to infer type from value
            ConfigValue configValue;
            if (tryConvertFromString(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            } else {
                // If conversion fails, store as string
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    // Invalidate validation cache
    pImpl->validationCached = false;
    
    return true;
}

[[nodiscard]] bool Config::loadFromArgs(int argc, char** argv) noexcept {
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
        // Handle -key value format with bounds checking
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Invalidate validation cache
    pImpl->validationCached = false;
    
    return true;
}

[[nodiscard]] bool Config::saveToFile(const std::string& filepath) const noexcept {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Sort entries for consistent output
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    
    for (const auto& entry : sortedEntries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value back to string representation
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

template<typename T>
[[nodiscard]] std::optional<T> Config::get(const std::string& key) const noexcept {
    // Fast path: check if key exists in map
    const auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(entry.value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(entry.value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(entry.value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(entry.value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(entry.value);
        } else {
            return std::nullopt;
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
[[nodiscard]] T Config::getOr(const std::string& key, const T& defaultValue) const noexcept {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) noexcept {
    // Validate key
    if (key.empty()) {
        return;
    }
    
    // Find or create entry
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            pImpl->isSorted = false;  // Mark as unsorted
            pImpl->validationCached = false;  // Invalidate validation cache
        }
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
        pImpl->isSorted = false;
        pImpl->validationCached = false;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

[[nodiscard]] bool Config::has(const std::string& key) const noexcept {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) noexcept {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        
        // Remove from entries vector
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Update indices in keyToIndex map
        for (auto& pair : pImpl->keyToIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        // Remove from map
        pImpl->keyToIndex.erase(it);
        pImpl->isSorted = false;
        pImpl->validationCached = false;
    }
}

[[nodiscard]] std::vector<std::string> Config::getKeys() const noexcept {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

void Config::clear() noexcept {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    pImpl->isSorted = false;
    pImpl->validationCached = false;
}

[[nodiscard]] std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort entries for consistent output
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    
    for (const auto& entry : sortedEntries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
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

[[nodiscard]] std::optional<ConfigSource> Config::getSource(const std::string& key) const noexcept {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            return pImpl->entries[index].source;
        }
    }
    return std::nullopt;
}

void Config::update(const std::string& key, const std::string& value) noexcept {
    if (has(key)) {
        set(key, value);
    }
}

void Config::merge(const Config& other) noexcept {
    for (const auto& entry : other.pImpl->entries) {
        // Don't overwrite existing entries unless they're from the same source
        if (!has(entry.key) || getSource(entry.key) != entry.source) {
            set(entry.key, entry.value, entry.source);
        }
    }
    pImpl->isSorted = false;
    pImpl->validationCached = false;
}

[[nodiscard]] bool Config::validate() const noexcept {
    if (pImpl->validationCached) {
        return pImpl->validationErrors.empty();
    }
    
    std::string errors;
    bool isValid = true;
    
    // Check for duplicate keys (shouldn't happen with our implementation)
    // Check for missing required values
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors += "Empty key found.\n";
            isValid = false;
        }
        
        // Type validation could be added here
    }
    
    pImpl->validationErrors = errors;
    pImpl->validationCached = true;
    
    return isValid;
}

[[nodiscard]] std::string Config::getValidationErrors() const {
    validate();  // Ensure validation is run
    return pImpl->validationErrors;
}

std::string Config::trim(const std::string& str) noexcept {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) noexcept {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::tryConvertToString(const ConfigValue& value, std::string& result) noexcept {
    try {
        std::visit([&result](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result = val;
            } else if constexpr (std::is_same_v<T, int>) {
                result = std::to_string(val);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result = std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                // Use consistent formatting
                std::ostringstream oss;
                oss.precision(6);
                oss << std::fixed << val;
                result = oss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                result = val ? "true" : "false";
            } else {
                result.clear();
                return false;
            }
        }, value);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::tryConvertFromString(const std::string& str, ConfigValue& value) noexcept {
    if (str.empty()) {
        return false;
    }
    
    // Try boolean first (case-insensitive)
    std::string lower = toLower(str);
    if (lower == "true") {
        value = true;
        return true;
    } else if (lower == "false") {
        value = false;
        return true;
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    value = str;
    return true;
}

} // namespace nlm
