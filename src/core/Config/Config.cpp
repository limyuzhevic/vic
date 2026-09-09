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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else if (extension == ".cfg" || extension == ".yaml" || extension == ".yml") {
        return loadFromKeyValue(filepath);
    } else {
        // Default to key=value format
        return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        // Clear existing entries
        clear();
        
        // Load JSON object recursively
        loadJSONToConfig(json, "");
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Log error but don't fail hard - fall back to key=value format
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
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

void Config::loadJSONToConfig(const nlohmann::json& json, const std::string& prefix) {
    if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            loadJSONValueToConfig(fullKey, value);
        }
    } else {
        // For top-level values or nested values, treat as single value
        if (!prefix.empty()) {
            loadJSONValueToConfig(prefix, json);
        }
    }
}

void Config::loadJSONValueToConfig(const std::string& key, const nlohmann::json& value) {
    ConfigSource source = ConfigSource::File;
    
    // Convert based on JSON type
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int>(), source);
    } else if (value.is_number_unsigned()) {
        set(key, static_cast<int>(value.get<uint64_t>()), source);
    } else if (value.is_number()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Handle arrays
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& item : value) {
            if (item.is_string()) {
                allInts = false;
                allDoubles = false;
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                allStrings = false;
                allDoubles = false;
                intArray.push_back(item.get<int>());
            } else if (item.is_number()) {
                allStrings = false;
                allInts = false;
                doubleArray.push_back(item.get<double>());
            } else {
                // Unsupported type in array
                allInts = false;
                allDoubles = false;
                allStrings = false;
                break;
            }
        }
        
        if (allStrings) {
            set(key, stringArray, source);
        } else if (allInts) {
            set(key, intArray, source);
        } else if (allDoubles) {
            set(key, doubleArray, source);
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
        return false;
    }
    
    // Detect file format by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else {
        return saveToKeyValue(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json json;
        
        // Convert Config entries to JSON structure
        for (const auto& entry : pImpl->entries) {
            // Convert key from dot notation to nested object if needed
            std::vector<std::string> keyParts;
            std::string remaining = entry.key;
            size_t dotPos = remaining.find('.');
            while (dotPos != std::string::npos) {
                keyParts.push_back(remaining.substr(0, dotPos));
                remaining = remaining.substr(dotPos + 1);
                dotPos = remaining.find('.');
            }
            keyParts.push_back(remaining);
            
            nlohmann::json* current = &json;
            
            // Navigate/create nested objects
            for (size_t i = 0; i < keyParts.size() - 1; ++i) {
                if (!current->contains(keyParts[i]) || !current->at(keyParts[i]).is_object()) {
                    (*current)[keyParts[i]] = nlohmann::json::object();
                }
                current = &((*current)[keyParts[i]]);
            }
            
            // Set the final value
            setJSONValue(*current, keyParts.back(), entry.value);
        }
        
        // Write JSON to file with indentation
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }
        
        outFile << json.dump(4);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        file << serializeValue(entry.value);
        file << "\n";
    }
    
    return true;
}

void Config::setJSONValue(nlohmann::json& json, const std::string& key, const ConfigValue& value) {
    std::visit([&json, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json[key] = arg;
        }
    }, value);
}

std::string Config::serializeValue(const ConfigValue& value) const {
    std::stringstream ss;
    
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else {
            ss << arg;
        }
    }, value);
    
    return ss.str();
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
