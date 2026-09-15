#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace {
    // Helper to convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& jsonVal) {
        if (jsonVal.is_string()) {
            return jsonVal.get<std::string>();
        } else if (jsonVal.is_number_integer()) {
            return jsonVal.get<int64_t>();
        } else if (jsonVal.is_number_unsigned()) {
            return jsonVal.get<uint64_t>();
        } else if (jsonVal.is_number_float()) {
            return jsonVal.get<double>();
        } else if (jsonVal.is_boolean()) {
            return jsonVal.get<bool>();
        } else if (jsonVal.is_array()) {
            // Handle arrays - convert to vector based on element types
            const auto& arr = jsonVal;
            if (arr.empty()) {
                return std::vector<int>{};
            }
            
            // Try to infer array type
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& elem : arr) {
                if (!elem.is_number_integer()) allInts = false;
                if (!elem.is_number_float() && !elem.is_number_integer()) allDoubles = false;
                if (!elem.is_string()) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int64_t> intVec;
                intVec.reserve(arr.size());
                for (const auto& elem : arr) intVec.push_back(elem.get<int64_t>());
                return intVec;
            } else if (allDoubles) {
                std::vector<double> doubleVec;
                doubleVec.reserve(arr.size());
                for (const auto& elem : arr) doubleVec.push_back(elem.get<double>());
                return doubleVec;
            } else if (allStrings) {
                std::vector<std::string> stringVec;
                stringVec.reserve(arr.size());
                for (const auto& elem : arr) stringVec.push_back(elem.get<std::string>());
                return stringVec;
            }
            
            // Fallback to vector<int>
            std::vector<int64_t> intVec;
            intVec.reserve(arr.size());
            for (const auto& elem : arr) intVec.push_back(elem.get<int64_t>());
            return intVec;
        } else if (jsonVal.is_object()) {
            // Convert object to string representation for now
            // Could be enhanced to store as nested structure
            return jsonVal.dump();
        } else {
            // null or other types
            return std::string();
        }
    }
    
    // Helper to convert YAML value to ConfigValue
    ConfigValue yamlToConfigValue(const YAML::Node& yamlVal) {
        if (yamlVal.IsScalar()) {
            if (yamlVal.IsInteger()) {
                return yamlVal.as<int64_t>();
            } else if (yamlVal.IsDouble()) {
                return yamlVal.as<double>();
            } else if (yamlVal.IsBoolean()) {
                return yamlVal.as<bool>();
            } else {
                // String scalar
                return yamlVal.as<std::string>();
            }
        } else if (yamlVal.IsSequence()) {
            // Handle sequences - infer type
            if (yamlVal.empty()) {
                return std::vector<int>{};
            }
            
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& elem : yamlVal) {
                if (!elem.IsInteger()) allInts = false;
                if (!elem.IsDouble() && !elem.IsInteger()) allDoubles = false;
                if (!elem.IsScalar() || (elem.ScalarType() != YAML::NodeType::Scalar)) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int64_t> intVec;
                intVec.reserve(yamlVal.size());
                for (const auto& elem : yamlVal) intVec.push_back(elem.as<int64_t>());
                return intVec;
            } else if (allDoubles) {
                std::vector<double> doubleVec;
                doubleVec.reserve(yamlVal.size());
                for (const auto& elem : yamlVal) doubleVec.push_back(elem.as<double>());
                return doubleVec;
            } else if (allStrings) {
                std::vector<std::string> stringVec;
                stringVec.reserve(yamlVal.size());
                for (const auto& elem : yamlVal) stringVec.push_back(elem.as<std::string>());
                return stringVec;
            }
            
            // Fallback to vector<int>
            std::vector<int64_t> intVec;
            intVec.reserve(yamlVal.size());
            for (const auto& elem : yamlVal) intVec.push_back(elem.as<int64_t>());
            return intVec;
        } else if (yamlVal.IsMap()) {
            // Convert map to string representation for now
            std::stringstream ss;
            ss << "{";
            bool first = true;
            for (const auto& pair : yamlVal) {
                if (!first) ss << ", ";
                ss << "\"" << pair.first.as<std::string>() << "\": ";
                if (pair.second.IsScalar()) {
                    if (pair.second.IsInteger()) ss << pair.second.as<int64_t>();
                    else if (pair.second.IsDouble()) ss << pair.second.as<double>();
                    else if (pair.second.IsBoolean()) ss << (pair.second.as<bool>() ? "true" : "false");
                    else ss << "\"" << pair.second.as<std::string>() << "\"";
                } else {
                    ss << pair.second;
                }
                first = false;
            }
            ss << "}";
            return ss.str();
        } else {
            // null or other types
            return std::string();
        }
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
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
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        if (!jsonData.is_object()) {
            return false;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            set(key, jsonToConfigValue(value), ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error if logger is available
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        if (!yamlData.IsMap()) {
            return false;
        }
        
        for (const auto& node : yamlData) {
            set(node.first.as<std::string>(), yamlToConfigValue(node.second), ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error if logger is available
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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