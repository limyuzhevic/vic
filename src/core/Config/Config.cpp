#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <unordered_map>
#include <vector>
#include <memory>
#include <limits>
#include <stdexcept>
#include <cstdint>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load JSON file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Parse JSON file
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON recursively
        processJsonObject(j, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // In case nlohmann::json is not available or parsing fails, 
        // fallback to simple parser
        return fallbackLoadFromFile(filepath);
    }
}

// Process JSON and convert to config entries
void Config::processJsonObject(const nlohmann::json& j, ConfigSource source, const std::string& parentKey) {
    if (j.is_object()) {
        for (const auto& element : j.items()) {
            std::string key = parentKey.empty() ? element.key() : parentKey + "." + element.key();
            
            if (element.value().is_object()) {
                // Nested object - recursively process
                processJsonObject(element.value(), source, key);
            } else if (element.value().is_array()) {
                // Array - process as single value
                setArrayConfig(key, element.value(), source);
            } else {
                // Primitive value - process directly
                setPrimitiveConfig(key, element.value(), source);
            }
        }
    }
}

// Set primitive config values from JSON
void Config::setPrimitiveConfig(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        // Handle different integer types
        if (value.get<int64_t>() <= std::numeric_limits<int>::max() && 
            value.get<int64_t>() >= std::numeric_limits<int>::min()) {
            set(key, value.get<int>(), source);
        } else {
            set(key, value.get<int64_t>(), source);
        }
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_null()) {
        // Treat null as empty string
        set(key, "", source);
    }
}

// Set array config values from JSON
void Config::setArrayConfig(const std::string& key, const nlohmann::json& array, ConfigSource source) {
    // Determine array type based on first element
    if (array.empty()) {
        // Empty array
        return;
    }
    
    // Determine element type by checking first element
    if (array[0].is_string()) {
        std::vector<std::string> strArray;
        for (const auto& element : array) {
            strArray.push_back(element.get<std::string>());
        }
        set(key, strArray, source);
    } else if (array[0].is_number_integer()) {
        // Check if all elements are integers
        bool allIntegers = std::all_of(array.begin(), array.end(), 
            [](const nlohmann::json& el) { return el.is_number_integer(); });
        if (allIntegers) {
            std::vector<int> intArray;
            for (const auto& element : array) {
                intArray.push_back(element.get<int>());
            }
            set(key, intArray, source);
        } else {
            // Mixed or float numbers
            std::vector<double> doubleArray;
            for (const auto& element : array) {
                doubleArray.push_back(element.get<double>());
            }
            set(key, doubleArray, source);
        }
    } else {
        // All numbers are treated as doubles for consistency
        std::vector<double> doubleArray;
        for (const auto& element : array) {
            doubleArray.push_back(element.get<double>());
        }
        set(key, doubleArray, source);
    }
}

// Fallback parser for when nlohmann::json is not available or parsing fails
bool Config::fallbackLoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse simple key=value format for backward compatibility
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
    
    nlohmann::json j;
    
    // Convert config entries to JSON structure
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find('.') != std::string::npos) {
            // Nested key (e.g., "nested.key") - create nested object
            std::string parentKey = entry.key.substr(0, entry.key.find('.'));
            std::string childKey = entry.key.substr(entry.key.find('.') + 1);
            
            if (!j.contains(parentKey)) {
                j[parentKey] = nlohmann::json::object();
            }
            
            // Recursively set nested values
            setJsonValue(j[parentKey], childKey, entry.value);
        } else {
            // Top-level key
            setJsonValue(j, entry.key, entry.value);
        }
    }
    
    // Write JSON to file with indentation for readability
    file << j.dump(2) << std::endl;
    
    return true;
}

void Config::setJsonValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            j[key] = val;
        } else if constexpr (std::is_same_v<T, int>) {
            j[key] = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j[key] = val;
        } else if constexpr (std::is_same_v<T, double>) {
            j[key] = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            j[key] = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            j[key] = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            j[key] = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            j[key] = array;
        }
    }, value);
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