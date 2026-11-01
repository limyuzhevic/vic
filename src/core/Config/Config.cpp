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
    // Implement proper JSON/YAML parser
    // Try JSON first (requires nlohmann/json)
    try {
        // Check if file exists and has .json extension
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            std::ifstream file(filepath);
            if (file.is_open()) {
                // For now, use existing key=value parser for .json files too
                // In a full implementation, you would use a proper JSON parser
                return loadFromSimpleFormat(file);
            }
        }
        
        // Try YAML (requires yaml-cpp)
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
            std::ifstream file(filepath);
            if (file.is_open()) {
                // For now, use existing key=value parser for .yaml files too
                // In a full implementation, you would use a proper YAML parser
                return loadFromSimpleFormat(file);
            }
        }
        
        // Try simple key=value format for .cfg files
        if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
            return loadFromSimpleFormat(std::ifstream(filepath));
        }
        
    } catch (const std::exception& e) {
        // If parsing fails, fall back to simple format
        std::ifstream file(filepath);
        if (file.is_open()) {
            return loadFromSimpleFormat(file);
        }
    }
    
    return false;
}

// Helper method for parsing simple key=value format
bool Config::loadFromSimpleFormat(std::ifstream& file) {
    if (!file.is_open()) return false;
    
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
                ((value.front() == '\"' && value.back() == '\"') ||
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
    // Try JSON first (requires nlohmann/json)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return saveToJsonFormat(filepath);
    }
    
    // Try YAML (requires yaml-cpp)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return saveToYamlFormat(filepath);
    }
    
    // Default to simple key=value format
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

// Helper method to convert ConfigValue to string
static std::string configValueToString(const ConfigValue& value) {
    std::string result;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            // Format to avoid scientific notation
            result = std::to_string(val);
            // Remove trailing zeros for cleaner output
            size_t dotPos = result.find('.');
            if (dotPos != std::string::npos) {
                // Remove trailing zeros
                while (!result.empty() && result.back() == '0') {
                    result.pop_back();
                }
                // Remove decimal point if no fractional part left
                if (!result.empty() && result.back() == '.') {
                    result.pop_back();
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(val[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                // Format double similarly
                std::string num = std::to_string(val[i]);
                size_t dotPos = num.find('.');
                if (dotPos != std::string::npos) {
                    while (!num.empty() && num.back() == '0') {
                        num.pop_back();
                    }
                    if (!num.empty() && num.back() == '.') {
                        num.pop_back();
                    }
                }
                result += num;
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + val[i] + "\"";
            }
            result += "]";
        } else {
            result = "UNKNOWN_TYPE";
        }
    }, value);
    
    return result;
}

// Simple JSON format saving (minimal implementation)
bool Config::saveToJsonFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": " << configValueToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

// Simple YAML format saving (minimal implementation)
bool Config::saveToYamlFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration\n";
    file << "# Generated from NLM config system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": " << configValueToString(entry.value) << "\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
