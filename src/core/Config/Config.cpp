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

// Helper function to convert string to ConfigValue
ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to parse as boolean
    if (str == "true") return true;
    if (str == "false") return false;
    
    // Try to parse as integer
    try {
        if (str.find('.') == std::string::npos) {
            size_t pos;
            int val = std::stoi(str, &pos);
            if (pos == str.size()) return val;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) return val;
    } catch (...) {}
    
    // Try to parse as vector (simple format: [1,2,3])
    if (str.front() == '[' && str.back() == ']') {
        std::string content = str.substr(1, str.size() - 2);
        std::vector<int> intVec;
        std::vector<double> doubleVec;
        
        size_t pos = 0;
        bool hasDouble = false;
        
        try {
            while (pos < content.size()) {
                size_t comma = content.find(',', pos);
                if (comma == std::string::npos) {
                    comma = content.size();
                }
                
                std::string numStr = trim(content.substr(pos, comma - pos));
                
                // Try to parse as int first
                try {
                    size_t endpos;
                    int val = std::stoi(numStr, &endpos);
                    if (endpos == numStr.size()) {
                        intVec.push_back(val);
                    } else {
                        double val = std::stod(numStr, &endpos);
                        if (endpos == numStr.size()) {
                            doubleVec.push_back(val);
                            hasDouble = true;
                        }
                    }
                } catch (...) {
                    double val = std::stod(numStr);
                    doubleVec.push_back(val);
                    hasDouble = true;
                }
                
                pos = comma + 1;
                if (pos < content.size() && content[pos] == ' ') {
                    ++pos;
                }
            }
            
            if (hasDouble) {
                return doubleVec;
            } else if (!intVec.empty()) {
                return intVec;
            }
        } catch (...) {}
    }
    
    // Default to string
    return str;
}

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
            
            set(key, stringToConfigValue(value), ConfigSource::File);
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
                set(key, stringToConfigValue(value), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, stringToConfigValue(value), ConfigSource::CommandLine);
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
        
        // Helper to convert ConfigValue to string
        auto valueToString = [](const ConfigValue& value) -> std::string {
            std::stringstream ss;
            std::visit([&ss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << arg[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << arg[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << arg[i] << "\"";
                    }
                    ss << "]";
                } else {
                    ss << arg;
                }
            }, entry.value);
            return ss.str();
        };
        
        file << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    return true;
}

// Safe get method with better type checking and error handling
// This provides better error messages and validation
template<typename T>
std::optional<T> Config::safeGet(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Check if value type matches requested type
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(it->value)) {
            return std::get<int>(it->value);
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(it->value)) {
            return std::get<int64_t>(it->value);
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(it->value)) {
            return std::get<double>(it->value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(it->value)) {
            return std::get<bool>(it->value);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(it->value)) {
            return std::get<std::string>(it->value);
        }
    }
    
    return std::nullopt;
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
// These need to match the definitions in the header
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
