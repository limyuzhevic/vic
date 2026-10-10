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
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            // Log error for debugging
            return false;
        }
        
        // Check if file is empty
        if (file.peek() == std::ifstream::traits_type::eof()) {
            // Empty file is valid (just means no configuration)
            return true;
        }
        
        // Try to parse as JSON first
        nlohmann::json j;
        try {
            file >> j;
        } catch (const nlohmann::json::parse_error& e) {
            // JSON parsing failed, try legacy key=value format
            file.clear();
            file.seekg(0);
            std::string line;
            size_t lineNumber = 0;
            while (std::getline(file, line)) {
                ++lineNumber;
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
                    
                    // Validate key
                    if (key.empty()) {
                        // Skip invalid lines with empty keys
                        continue;
                    }
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                } else {
                    // Skip lines without '='
                    continue;
                }
            }
            return true;
        }
        
        // Validate JSON structure
        if (!j.is_object()) {
            return false;
        }
        
        // Parse JSON object and populate configuration
        for (const auto& [key, value] : j.items()) {
            ConfigSource source = ConfigSource::File;
            
            // Validate key
            if (key.empty()) {
                continue;  // Skip empty keys
            }
            
            try {
                // Handle different value types with validation
                if (value.is_string()) {
                    set(key, value.get<std::string>(), source);
                } else if (value.is_number_integer()) {
                    // Check for integer overflow
                    int64_t intVal = value.get<int64_t>();
                    set(key, intVal, source);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), source);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), source);
                } else if (value.is_array()) {
                    // Handle array types
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        if (item.is_string()) {
                            strArray.push_back(item.get<std::string>());
                        } else {
                            // Try to convert to string
                            strArray.push_back(value.dump());
                        }
                    }
                    set(key, strArray, source);
                } else {
                    // Unknown type, treat as string with error handling
                    std::string strValue;
                    try {
                        strValue = value.dump();
                        set(key, strValue, source);
                    } catch (...) {
                        // Skip problematic entries
                        continue;
                    }
                }
            } catch (const std::exception& e) {
                // Log error and skip this entry
                continue;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Any exception during file reading/parsing should be caught and handled gracefully
        return false;
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
    
    // Create JSON object from configuration entries
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        // Store the key and value as JSON with metadata
        nlohmann::json entryJson;
        
        // Convert ConfigValue to JSON
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                entryJson["value"] = val;
                entryJson["type"] = "string";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                entryJson["value"] = val;
                entryJson["type"] = "int64";
            } else if constexpr (std::is_same_v<T, double>) {
                entryJson["value"] = val;
                entryJson["type"] = "double";
            } else if constexpr (std::is_same_v<T, bool>) {
                entryJson["value"] = val;
                entryJson["type"] = "bool";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                entryJson["value"] = val;
                entryJson["type"] = "int64_array";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                entryJson["value"] = val;
                entryJson["type"] = "double_array";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                entryJson["value"] = val;
                entryJson["type"] = "string_array";
            } else {
                entryJson["value"] = "";
                entryJson["type"] = "unknown";
            }
            entryJson["source"] = static_cast<int>(entry.source);
            if (!entry.description.empty()) {
                entryJson["description"] = entry.description;
            }
        }, entry.value);
        
        j[entry.key] = entryJson;
    }
    
    // Write JSON with indentation for readability
    file << j.dump(4);
    
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
