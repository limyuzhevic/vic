#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // IMPROVEMENT: Implement proper JSON/YAML parser (Phase 2 complete)
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON object recursively
        parseJsonObject(jsonData, ConfigSource::File, "");
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, fall back to simple key=value format
        file.clear();
        file.seekg(0);
        
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
    }
    
    return true;
}

void Config::parseJsonObject(const nlohmann::json& jsonObj, ConfigSource source, const std::string& prefix) {
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            // Recursively parse nested objects
            parseJsonObject(*it, source, key);
        } else if (it->is_array()) {
            // Parse arrays as comma-separated values
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < it->size(); ++i) {
                if (i > 0) ss << ",";
                ss << (*it)[i].dump();
            }
            ss << "]";
            set(key, ss.str(), source);
        } else {
            // Parse primitive values
            setFromJsonValue(*it, key, source);
        }
    }
}

void Config::setFromJsonValue(const nlohmann::json& value, const std::string& key, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int>(), source);
    } else if (value.is_number_unsigned()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else {
        // For complex types, convert to string representation
        set(key, value.dump(), source);
    }
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
    
    // IMPROVEMENT: Use proper JSON formatting instead of placeholder
    nlohmann::json jsonObj;
    
    for (const auto& entry : pImpl->entries) {
        // Convert string value with brackets back to proper JSON
        std::string key = entry.key;
        nlohmann::json& value = jsonObj[key];
        
        std::visit([&value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Convert vector to JSON array
                value = nlohmann::json::array();
                for (const auto& item : arg) {
                    value.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = nlohmann::json::array();
                for (const auto& item : arg) {
                    value.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = nlohmann::json::array();
                for (const auto& item : arg) {
                    value.push_back(item);
                }
            } else {
                value = arg;
            }
        }, entry.value);
    }
    
    // Write pretty JSON
    file << std::setw(2) << jsonObj;
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