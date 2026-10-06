#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
            
            // Try to infer type from value
                setFromString(key, value, ConfigSource::File);
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Advanced configuration methods

// Get configuration value as string
std::string Config::getAsString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Check if configuration has changed
bool Config::hasChangedSince(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return true;
    }
    
    for (const auto& thisEntry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&thisEntry](const ConfigEntry& otherEntry) {
                return otherEntry.key == thisEntry.key;
            });
        
        if (it == other.pImpl->entries.end()) {
            return true;
        }
        
        if (it->value != thisEntry.value || it->source != thisEntry.source) {
            return true;
        }
    }
    return false;
}

// Get configuration diff
std::vector<std::string> Config::getDiff(const Config& other) const {
    std::vector<std::string> diffs;
    
    // Check for removed keys
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            diffs.push_back("REMOVED: " + otherEntry.key);
        }
    }
    
    // Check for changed keys
    for (const auto& thisEntry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&thisEntry](const ConfigEntry& otherEntry) {
                return otherEntry.key == thisEntry.key;
            });
        
        if (it != other.pImpl->entries.end() && 
            (it->value != thisEntry.value || it->source != thisEntry.source)) {
            diffs.push_back("CHANGED: " + thisEntry.key);
        }
    }
    
    return diffs;
}

// Merge configuration from another config
void Config::merge(const Config& other, bool overrideSource) {
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& thisEntry) {
                return thisEntry.key == otherEntry.key;
            });
        
        if (it != pImpl->entries.end()) {
            if (overrideSource) {
                it->value = otherEntry.value;
                it->source = otherEntry.source;
            }
        } else {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

// Get all values of a specific type
template<typename T>
std::vector<std::pair<std::string, T>> Config::getAllOfType() const {
    std::vector<std::pair<std::string, T>> result;
    
    for (const auto& entry : pImpl->entries) {
        try {
            auto value = std::get<T>(entry.value);
            result.emplace_back(entry.key, value);
        } catch (const std::bad_variant_access&) {
            // Skip entries that don't match the type
        }
    }
    
    return result;
}

// Try to infer type from string value
void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Trim whitespace
    std::string trimmed = trim(valueStr);
    
    // Check for boolean
    if (trimmed == "true" || trimmed == "false") {
        bool boolValue = (trimmed == "true");
        set(key, boolValue, source);
        return;
    }
    
    // Check for integer
    if (std::all_of(trimmed.begin(), trimmed.end(), ::isdigit) ||
        (trimmed.size() > 0 && trimmed[0] == '-' && 
         std::all_of(trimmed.begin() + 1, trimmed.end(), ::isdigit))) {
        try {
            int intValue = std::stoi(trimmed);
            set(key, intValue, source);
            return;
        } catch (...) {
            // Fall through to double
        }
    }
    
    // Check for double
    try {
        size_t pos;
        double doubleValue = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            set(key, doubleValue, source);
            return;
        }
    } catch (...) {
        // Fall through to string
    }
    
    // Default to string
    set(key, trimmed, source);
}

// Save to file (improved)
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
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

// New method explicit template instantiations
template std::string Config::getAsString(const std::string&, const std::string&) const;
template std::vector<std::pair<std::string, int>> Config::getAllOfType<int>() const;
template std::vector<std::pair<std::string, double>> Config::getAllOfType<double>() const;
template std::vector<std::pair<std::string, bool>> Config::getAllOfType<bool>() const;
template std::vector<std::pair<std::string, std::string>> Config::getAllOfType<std::string>() const;
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
