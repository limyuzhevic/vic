#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>

// Try to include nlohmann/json for JSON parsing
#ifdef NLM_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#define HAS_NLOHMANN_JSON 1
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    // Try to detect file format and parse accordingly
    file.clear();
    file.seekg(0, std::ios::beg);
    
    // Read first few lines to detect format
    std::string line;
    bool has_braces = false;
    bool is_json_like = false;
    int brace_count = 0;
    
    for (int i = 0; i < 10 && std::getline(file, line); ++i) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        
        // Check for JSON-like structure
        if (trimmed.find('{') != std::string::npos) {
            has_braces = true;
            for (char c : trimmed) {
                if (c == '{') ++brace_count;
                else if (c == '}') ++brace_count;
            }
        }
        
        // Check for JSON object structure
        if (trimmed.find('"') != std::string::npos || 
            (trimmed.find('{') != std::string::npos && trimmed.find('}') != std::string::npos)) {
            is_json_like = true;
        }
    }
    
    // Reset file for actual parsing
    file.clear();
    file.seekg(0, std::ios::beg);
    
    if (has_braces || is_json_like) {
        return loadFromJSONFile(filepath);
    } else {
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open JSON config file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON recursively
        parseJSON(jsonData, "");
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON config file ") + filepath + ": " + e.what());
        return false;
    }
#else
    // Fallback to simple parsing
    NLM_LOG_WARNING("JSON parsing requested but nlohmann/json not available. Using simple format.");
    return loadFromSimpleFormat(filepath);
#endif
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            // Try to convert value to appropriate type
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING(std::string("Invalid config line ") + std::to_string(lineNum) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config file had parsing warnings. Some entries may be missing.");
    }
    
    NLM_LOG_INFO(std::string("Loaded configuration from: ") + filepath + " (simple format)");
    return true;
}

void Config::parseJSON(const nlohmann::json& json, const std::string& prefix) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            
            if (it->is_object()) {
                // Recursively parse nested objects
                parseJSON(*it, fullKey);
            } else if (it->is_array()) {
                // Parse arrays
                std::vector<ConfigValue> values;
                for (const auto& item : *it) {
                    if (item.is_number_integer()) {
                        values.push_back(item.get<int64_t>());
                    } else if (item.is_number_float()) {
                        values.push_back(item.get<double>());
                    } else if (item.is_boolean()) {
                        values.push_back(item.get<bool>());
                    } else if (item.is_string()) {
                        values.push_back(item.get<std::string>());
                    } else {
                        values.push_back(std::string("[unknown]"));
                    }
                }
                set(fullKey, values, ConfigSource::File);
            } else {
                // Handle primitive values
                ConfigValue value;
                if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_string()) {
                    value = it->get<std::string>();
                } else {
                    value = std::string("[unknown]");
                }
                set(fullKey, value, ConfigSource::File);
            }
        }
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
        NLM_LOG_ERROR(std::string("Failed to open config file for writing: ") + filepath);
        return false;
    }
    
    // Write in JSON format for better readability and compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add comment if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Set key in JSON
        std::visit([&](const auto& val) {
            jsonData[entry.key] = val;
        }, entry.value);
        
        file << "  \"" << entry.key << "\" : ";
        
        // Output the value based on its type
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, entry.value);
        
        file << ",\n";
    }
    
    // Write JSON to file
    std::ofstream jsonFile(filepath);
    if (!jsonFile.is_open()) {
        return false;
    }
    
    jsonFile << jsonData.dump(2); // Indent with 2 spaces
    
    NLM_LOG_INFO(std::string("Configuration saved to: ") + filepath);
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
        // Try to convert from string representation
        try {
            if constexpr (std::is_same_v<T, int64_t>) {
                std::string strValue = std::get<std::string>(it->value);
                try {
                    return std::stoll(strValue);
                } catch (...) {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                std::string strValue = std::get<std::string>(it->value);
                try {
                    return std::stod(strValue);
                } catch (...) {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                std::string strValue = std::get<std::string>(it->value);
                if (strValue == "true" || strValue == "1" || strValue == "yes") return true;
                if (strValue == "false" || strValue == "0" || strValue == "no") return false;
                return std::nullopt;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::get<std::string>(it->value);
            }
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
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
