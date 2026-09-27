#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to load as JSON first (Phase 2 implementation)
    auto jsonContent = tryLoadJsonFile(filepath);
    if (jsonContent) {
        parseJsonConfig(jsonContent.value());
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
        }
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
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")\n";
        file << entry.key << " = " << getValueDescription(entry) << "\n";
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
        std::visit([&oss](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << v << "\"";
            } else {
                oss << v;
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

std::string Config::getValueDescription(const ConfigEntry& entry) {
    std::ostringstream oss;
    std::visit([&oss](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << v << "\"";
        } else {
            oss << v;
        }
    }, entry.value);
    return oss.str();
}

inline std::string nlm::configTypeToString(const ConfigValue& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<std::string>";
        else return "unknown";
    }, value);
}

inline std::string nlm::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << v << "\"";
        } else {
            oss << v;
        }
    }, value);
    return oss.str();
}

inline ConfigValue nlm::stringToValue(const std::string& str, const std::string& typeHint) {
    if (typeHint == "int") {
        try { return std::stoi(str); } catch (...) {}
    } else if (typeHint == "int64_t") {
        try { return std::stoll(str); } catch (...) {}
    } else if (typeHint == "double") {
        try { return std::stod(str); } catch (...) {}
    } else if (typeHint == "bool") {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "yes" || lower == "1") return true;
        if (lower == "false" || lower == "no" || lower == "0") return false;
    } else if (typeHint == "string") {
        return str;
    } else if (typeHint == "std::vector<int>") {
        return parseVectorConfig<std::vector<int>>(str);
    } else if (typeHint == "std::vector<double>") {
        return parseVectorConfig<std::vector<double>>(str);
    } else if (typeHint == "std::vector<std::string>") {
        return parseVectorConfig<std::vector<std::string>>(str);
    }
    
    // Default: try to parse as most common types
    try { return std::stoi(str); } catch (...) {}
    try { return std::stod(str); } catch (...) {}
    return str;
}

inline std::string nlm::toConfigString(bool value) {
    return value ? "true" : "false";
}

inline std::string nlm::toConfigString(int value) {
    return std::to_string(value);
}

inline std::string nlm::toConfigString(int64_t value) {
    return std::to_string(value);
}

inline std::string nlm::toConfigString(double value) {
    return std::to_string(value);
}

inline bool nlm::validateConfigEntry(const std::string& key, const ConfigValue& value) {
    // Basic validation - can be expanded with more sophisticated rules
    return !key.empty() && !std::holds_alternative<std::monostate>(value);
}

// Helper function to parse vector config values
namespace {
    template<typename T>
    ConfigValue parseVectorConfig(const std::string& str) {
        // Parse vector format: [1,2,3] or "1,2,3"
        std::vector<T> result;
        
        if (str.empty()) return result;
        
        // Try different formats
        if (str.front() == '[' && str.back() == ']') {
            std::string content = str.substr(1, str.size() - 2);
            // Parse comma-separated values
            std::stringstream ss(content);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = Config::trim(item);
                if (item.empty()) continue;
                try {
                    if constexpr (std::is_same_v<T, int>) {
                        result.push_back(std::stoi(item));
                    } else if constexpr (std::is_same_v<T, double>) {
                        result.push_back(std::stod(item));
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        // Remove quotes if present
                        std::string trimmed = Config::trim(item);
                        if (trimmed.size() >= 2 && 
                            ((trimmed.front() == '"' && trimmed.back() == '"') ||
                             (trimmed.front() == '\'' && trimmed.back() == '\''))) {
                            trimmed = trimmed.substr(1, trimmed.size() - 2);
                        }
                        result.push_back(trimmed);
                    }
                } catch (...) {
                    // Skip invalid items
                }
            }
        } else {
            // Comma-separated format
            std::stringstream ss(str);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = Config::trim(item);
                if (item.empty()) continue;
                try {
                    if constexpr (std::is_same_v<T, int>) {
                        result.push_back(std::stoi(item));
                    } else if constexpr (std::is_same_v<T, double>) {
                        result.push_back(std::stod(item));
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        result.push_back(item);
                    }
                } catch (...) {
                    // Skip invalid items
                }
            }
        }
        
        return result;
    }
    
    // Helper functions for JSON parsing (Phase 2)
    std::optional<std::string> tryLoadJsonFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    void parseJsonConfig(const std::string& jsonContent) {
        // TODO PHASE 2: Implement proper JSON parsing
        // For now, just log that JSON support is available
        // This would use a proper JSON library like nlohmann/json
        std::cerr << "Warning: JSON configuration parsing not yet implemented (Phase 2)" << std::endl;
    }
}

// Explicit template instantiations

} // namespace nlm
