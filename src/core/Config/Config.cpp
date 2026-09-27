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

// File format detection helper
ConfigFormat Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigFormat::Simple;
    }
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Check for JSON pattern (starts with { or [)
        if (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '[')) {
            return ConfigFormat::JSON;
        }
        
        // Check for YAML pattern (contains : or key: value pattern)
        if (!firstLine.empty() && (firstLine.find(':') != std::string::npos || 
                                 (firstLine.size() >= 2 && firstLine[0] == '-' && firstLine[1] == ' ') ||
                                 (firstLine.find(':') != std::string::npos && firstLine.find('=') == std::string::npos))) {
            return ConfigFormat::YAML;
        }
    }
    
    // Default to simple key=value format
    return ConfigFormat::Simple;
}

// JSON parsing helper
bool Config::parseJSONFile(const std::string& filepath, Config& config) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Parse JSON recursively
        if (jsonData.is_object()) {
            for (const auto& item : jsonData.items()) {
                const std::string& key = item.key();
                ConfigValue value = convertJSONToConfigValue(item.value());
                config.set(key, value, ConfigSource::File);
            }
        } else if (jsonData.is_array()) {
            // Handle array of objects
            for (const auto& item : jsonData) {
                if (item.is_object() && item.contains("key") && item.contains("value")) {
                    std::string key = item["key"].get<std::string>();
                    ConfigValue value = convertJSONToConfigValue(item["value"]);
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// YAML parsing helper
bool Config::parseYAMLFile(const std::string& filepath, Config& config) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node doc = YAML::Load(file);
        
        // Parse as map (key-value pairs)
        if (doc.IsMap()) {
            for (const auto& node : doc) {
                std::string key = node.first.as<std::string>();
                ConfigValue value = convertYAMLToConfigValue(node.second);
                config.set(key, value, ConfigSource::File);
            }
        } else if (doc.IsSequence()) {
            // Handle array of key-value objects
            for (const auto& node : doc) {
                if (node.IsMap() && node["key"] && node["value"]) {
                    std::string key = node["key"].as<std::string>();
                    ConfigValue value = convertYAMLToConfigValue(node["value"]);
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper to convert JSON value to ConfigValue
ConfigValue Config::convertJSONToConfigValue(const nlohmann::json& value) {
    if (value.is_null()) {
        return std::string("");
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_number_integer()) {
        return value.get<int64_t>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_array()) {
        // Try to determine array type
        if (!value.empty()) {
            if (value[0].is_number_integer()) {
                std::vector<int64_t> intArray;
                for (const auto& item : value) {
                    intArray.push_back(item.get<int64_t>());
                }
                return intArray;
            } else if (value[0].is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (value[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Default to string representation
        return value.dump();
    } else {
        // Fallback to string representation
        return value.dump();
    }
}

// Helper to convert YAML node to ConfigValue
ConfigValue Config::convertYAMLToConfigValue(const YAML::Node& node) {
    if (node.IsNull()) {
        return std::string("");
    } else if (node.IsScalar()) {
        const std::string& value = node.as<std::string>();
        
        // Try to determine the type
        if (value == "true" || value == "false") {
            return value == "true";
        }
        
        try {
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.length()) {
                return intVal;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.length()) {
                return doubleVal;
            }
        } catch (...) {}
        
        return value;
    } else if (node.IsSequence()) {
        // Try to determine array type
        if (!node.empty()) {
            const YAML::Node& firstNode = node[0];
            if (firstNode.IsScalar()) {
                const std::string& firstVal = firstNode.as<std::string>();
                
                try {
                    bool allInt = true;
                    bool allDouble = true;
                    std::vector<int64_t> intArray;
                    std::vector<double> doubleArray;
                    std::vector<std::string> stringArray;
                    
                    for (const auto& item : node) {
                        if (item.IsScalar()) {
                            const std::string& val = item.as<std::string>();
                            
                            // Check integer
                            try {
                                size_t pos;
                                int64_t intVal = std::stoll(val, &pos);
                                if (pos == val.length()) {
                                    intArray.push_back(intVal);
                                    allDouble = false;
                                    continue;
                                }
                            } catch (...) {}
                            
                            // Check double
                            try {
                                size_t pos;
                                double doubleVal = std::stod(val, &pos);
                                if (pos == val.length()) {
                                    doubleArray.push_back(doubleVal);
                                    allInt = false;
                                    continue;
                                }
                            } catch (...) {}
                            
                            // Treat as string
                            stringArray.push_back(val);
                            allInt = false;
                            allDouble = false;
                        } else {
                            allInt = false;
                            allDouble = false;
                        }
                    }
                    
                    if (allInt) return intArray;
                    if (allDouble) return doubleArray;
                    if (!stringArray.empty()) return stringArray;
                } catch (...) {}
            }
        }
        
        // Fallback to string representation
        std::ostringstream oss;
        oss << node;
        return oss.str();
    } else {
        // Fallback to string representation
        std::ostringstream oss;
        oss << node;
        return oss.str();
    }
}

// Helper to convert ConfigValue to JSON value
nlohmann::json Config::convertConfigValueToJSON(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return "";
        }
    }, value);
}

// Helper to convert ConfigValue to simple format string
std::string Config::convertConfigValueToSimpleFormat(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, value);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format
    ConfigFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case ConfigFormat::JSON:
            return parseJSONFile(filepath, *this);
        case ConfigFormat::YAML:
            return parseYAMLFile(filepath, *this);
        case ConfigFormat::Simple:
            // Fall back to original simple parser
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
        default:
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
    // Determine file format based on file extension
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (extension == ".json") {
        // Save in JSON format
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = convertConfigValueToJSON(entry.value);
        }
        
        file << jsonData.dump(2);
    } else if (extension == ".yaml" || extension == ".yml") {
        // For YAML, we could save in YAML format, but for simplicity,
        // we'll save a subset in JSON format (convertible)
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = convertConfigValueToJSON(entry.value);
        }
        
        file << jsonData.dump(2);
    } else {
        // Save in simple key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << convertConfigValueToSimpleFormat(entry.value) << "\n\n";
        }
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
