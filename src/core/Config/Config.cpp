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
        // Write description as comment
        file << "# " << entry.description << "\n";
        
        // Write key-value pair
        file << entry.key << " = ";
        
        // Helper lambda to write config value
        auto writeValue = [&file](const ConfigValue& value) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << "\"" << arg[i] << "\"";
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, value);
        };
        
        writeValue(entry.value);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[int list]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[double list]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[string list]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Enhanced configuration features
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            return false;
        }
        
        // Validate based on value type
        bool valid = std::visit([](const auto& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // String validation - check for null bytes or other problematic characters
                return arg.find('\0') == std::string::npos;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                // Vector validation - ensure no empty strings in string vector
                if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    for (const auto& str : arg) {
                        if (str.find('\0') != std::string::npos) return false;
                    }
                }
                return true;
            } else {
                return true;  // int, double, bool - always valid
            }
        }, entry.value);
        
        if (!valid) {
            return false;
        }
    }
    return true;
}

std::string Config::getHelp() const {
    std::ostringstream oss;
    oss << "NLM Configuration Help:\n";
    oss << "=======================\n\n";
    oss << "Available configuration options:\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "\n" << entry.key << ":";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << " (string, default: \"\" )";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << " (integer)";
            } else if constexpr (std::is_same_v<T, double>) {
                oss << " (floating point)";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << " (boolean)";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << " (integer list)";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << " (floating point list)";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << " (string list)";
            }
            oss << " (source: " << static_cast<int>(entry.source) << ")";
        }, entry.value);
    }
    
    oss << "\n\nUsage examples:\n";
    oss << "  --config.key=value       Set configuration value\n";
    oss << "  --config.file=configs/default.cfg  Load from file\n";
    oss << "  nlm --help              Show this help\n";
    oss << "  nlm --config.list       List all configuration options\n";
    
    return oss.str();
}

std::vector<std::string> Config::getConfigSuggestions(const std::string& partialKey) const {
    std::vector<std::string> suggestions;
    std::string lowerPartial = toLower(partialKey);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerKey = toLower(entry.key);
        if (lowerKey.find(lowerPartial) != std::string::npos) {
            suggestions.push_back(entry.key);
        }
    }
    
    return suggestions;
}

// Configuration statistics and diagnostics
size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

bool Config::hasDefaultValues() const {
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default || entry.source == ConfigSource::CommandLine) {
            return true;
        }
    }
    return false;
}

ConfigSource Config::getHighestSource() const {
    int maxSourceValue = -1;
    ConfigSource highestSource = ConfigSource::Default;
    
    for (const auto& entry : pImpl->entries) {
        int sourceValue = static_cast<int>(entry.source);
        if (sourceValue > maxSourceValue) {
            maxSourceValue = sourceValue;
            highestSource = entry.source;
        }
    }
    
    return highestSource;
}

// Advanced value access
template<typename T>
T Config::getStrict(const std::string& key, T defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Configuration migration and updates
void Config::migrateFrom(const Config& oldConfig) {
    // Migrate entries from old config, preferring new values
    for (const auto& oldEntry : oldConfig.pImpl->entries) {
        if (!has(oldEntry.key)) {
            pImpl->entries.push_back(oldEntry);
        }
    }
}

void Config::applyPatch(const std::string& patchFile) {
    // Apply configuration patches from file (key=value format)
    std::ifstream file(patchFile);
    if (!file.is_open()) {
        return;
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::Runtime);
        }
    }
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
