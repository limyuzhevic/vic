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
    // Try JSON parsing first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to Config entries
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                std::string description = "Loaded from JSON file: " + key;
                
                // Handle different JSON types
                if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), ConfigSource::File, description);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), ConfigSource::File, description);
                } else if (it->is_boolean()) {
                    set(key, it->get<bool>(), ConfigSource::File, description);
                } else if (it->is_string()) {
                    set(key, it->get<std::string>(), ConfigSource::File, description);
                } else if (it->is_array()) {
                    // Convert array to vector
                    std::vector<double> vec;
                    for (const auto& item : *it) {
                        if (item.is_number()) {
                            vec.push_back(item.get<double>());
                        }
                    }
                    set(key, vec, ConfigSource::File, description);
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
    }
    
    return false;
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Original implementation for backward compatibility
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
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with proper validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate and set value
                if (!setFromString(key, value)) {
                    NLM_LOG_ERROR("Invalid command line argument: " + arg);
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!setFromString(key, value)) {
                NLM_LOG_ERROR("Invalid command line argument: -" + key + " " + value);
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::setFromString(const std::string& key, const std::string& value) {
    // Try different type conversions
    try {
        // Try int64
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, ConfigSource::CommandLine);
            return true;
        }
        
        // Try double
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            set(key, doubleVal, ConfigSource::CommandLine);
            return true;
        }
        
        // Try boolean
        if (value == "true" || value == "1" || value == "yes" || value == "on") {
            set(key, true, ConfigSource::CommandLine);
            return true;
        } else if (value == "false" || value == "0" || value == "no" || value == "off") {
            set(key, false, ConfigSource::CommandLine);
            return true;
        }
        
        // Assume string
        set(key, value, ConfigSource::CommandLine);
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for better readability
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                jsonData[entry.key] = arg;  // Note: this won't work, need to fix
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else {
                jsonData[entry.key] = "UNKNOWN_TYPE";
            }
        }, entry.value);
    }
    
    // Write JSON with pretty formatting
    file << jsonData.dump(2);
    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
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