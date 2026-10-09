#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ConfigFileFormat Config::detectFileFormat(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return ConfigFileFormat::UNKNOWN;
    }
    
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension.empty()) {
        return ConfigFileFormat::UNKNOWN;
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return ConfigFileFormat::JSON;
    } else if (extension == ".yaml" || extension == ".yml") {
        return ConfigFileFormat::YAML;
    } else if (extension == ".cfg" || extension == ".ini" || extension == ".txt" || extension.empty()) {
        // Default to key-value format for files without extension or with common config extensions
        return ConfigFileFormat::KEY_VALUE;
    }
    
    return ConfigFileFormat::UNKNOWN;
}

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFileFormat format = detectFileFormat(filepath);
    return loadFromFile(filepath, format);
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    clear();
    
    switch (format) {
        case ConfigFileFormat::KEY_VALUE:
            return loadFromKeyValue(filepath, *this);
        case ConfigFileFormat::JSON:
            return loadFromJSON(filepath, *this);
        case ConfigFileFormat::YAML:
            return loadFromYAML(filepath, *this);
        case ConfigFileFormat::UNKNOWN:
            throw ConfigFileError("Unknown file format for " + filepath);
    }
    return false;
}

bool Config::loadFromKeyValue(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file: " + filepath);
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            config.set(key, value, ConfigSource::File);
        } else {
            throw ConfigFormatError("Invalid format at line " + std::to_string(lineNumber) + 
                                   " in " + filepath + ": expected 'key = value' format");
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath, Config& config, const ConfigSchema* schema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open JSON file: " + filepath);
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        if (!jsonData.is_object()) {
            throw ConfigFormatError("JSON file must contain an object (key-value pairs)");
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            if (!parseJSONValue(value, configValue)) {
                throw ConfigFormatError("Failed to parse JSON value for key '" + key + "'");
            }
            config.set(key, configValue, ConfigSource::File);
        }
        
        // Validate against schema if provided
        if (schema) {
            std::unordered_map<std::string, std::any> configData;
            for (const auto& entry : config.pImpl->entries) {
                configData[entry.key] = entry.value;
            }
            
            if (!schema->validate(configData)) {
                auto missing = schema->getMissingElements(configData);
                throw ConfigValidationError("Missing required fields: " + 
                    (missing.empty() ? "unknown validation error" : 
                     std::accumulate(missing.begin(), missing.end(), std::string(),
                                 [](const std::string& a, const std::string& b) {
                                     return a.empty() ? b : a + ", " + b;
                                 })));
            }
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFormatError("JSON parse error: " + std::string(e.what()) + 
                               " at position " + std::to_string(e.byte));
    } catch (const nlohmann::json::type_error& e) {
        throw ConfigFormatError("JSON type error: " + std::string(e.what()) + 
                               " at position " + std::to_string(e.byte));
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath, Config& config, const ConfigSchema* schema) {
    return loadFromJSON(filepath, config, schema);
}

bool Config::saveToFile(const std::string& filepath) const {
    ConfigFileFormat format = detectFileFormat(filepath);
    return saveToFile(filepath, format);
}

bool Config::saveToFile(const std::string& filepath, ConfigFileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFileFormat::KEY_VALUE:
            return saveToKeyValue(filepath, *this);
        case ConfigFileFormat::JSON:
            return saveToJSON(filepath, *this);
        case ConfigFileFormat::YAML:
            return saveToYAML(filepath, *this);
        case ConfigFileFormat::UNKNOWN:
            return false;
    }
    return false;
}

bool Config::saveToKeyValue(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Add file header
    file << "# NLM Configuration File\n";
    file << "# Generated from configuration data\n\n";
    
    for (const auto& entry : config.pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath, const Config& config) {
    nlohmann::json jsonData;
    
    for (const auto& entry : config.pImpl->entries) {
        jsonData[entry.key] = serializeToJSON(entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write with pretty formatting
    file << jsonData.dump(4);
    
    return true;
}

bool Config::saveToYAML(const std::string& filepath, const Config& config) {
    // For now, YAML is implemented as JSON
    return saveToJSON(filepath, config);
}

bool Config::parseJSONValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number()) {
        // Check if it's an integer that fits in int64_t
        if (jsonValue.is_number_integer()) {
            configValue = jsonValue.get<int64_t>();
        } else {
            configValue = jsonValue.get<double>();
        }
    } else if (jsonValue.is_array()) {
        std::vector<std::string> stringArray;
        bool allStrings = true;
        
        for (const auto& item : jsonValue) {
            if (item.is_string()) {
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                stringArray.push_back(std::to_string(item.get<int64_t>()));
            } else if (item.is_number()) {
                stringArray.push_back(std::to_string(item.get<double>()));
            } else if (item.is_boolean()) {
                stringArray.push_back(item.get<bool>() ? "true" : "false");
            } else {
                allStrings = false;
                break;
            }
        }
        
        if (allStrings) {
            configValue = stringArray;
        } else {
            // Try to detect type based on first element
            if (!jsonValue.empty()) {
                if (jsonValue[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : jsonValue) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (jsonValue[0].is_number()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : jsonValue) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else {
                    return false;
                }
            }
        }
    } else {
        return false;
    }
    
    return true;
}

bool Config::parseYAMLValue(const nlohmann::json& yamlValue, ConfigValue& configValue) {
    return parseJSONValue(yamlValue, configValue);
}

nlohmann::json Config::serializeToJSON(const ConfigValue& configValue) const {
    nlohmann::json result;
    
    std::visit([&result](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = value;
        } else if constexpr (std::is_same_v<T, int>) {
            result = value;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = value;
        } else if constexpr (std::is_same_v<T, double>) {
            result = value;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = value;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = value;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = value;
        }
    }, configValue);
    
    return result;
}

bool Config::setValueFromAny(Config& config, const std::string& key, const std::any& value, ConfigSource source) {
    if (value.type() == typeid(int)) {
        config.set(key, std::any_cast<int>(value), source);
    } else if (value.type() == typeid(int64_t)) {
        config.set(key, std::any_cast<int64_t>(value), source);
    } else if (value.type() == typeid(double)) {
        config.set(key, std::any_cast<double>(value), source);
    } else if (value.type() == typeid(bool)) {
        config.set(key, std::any_cast<bool>(value), source);
    } else if (value.type() == typeid(std::string)) {
        config.set(key, std::any_cast<std::string>(value), source);
    } else if (value.type() == typeid(std::vector<int>)) {
        config.set(key, std::any_cast<std::vector<int>>(value), source);
    } else if (value.type() == typeid(std::vector<double>)) {
        config.set(key, std::any_cast<std::vector<double>>(value), source);
    } else if (value.type() == typeid(std::vector<std::string>)) {
        config.set(key, std::any_cast<std::vector<std::string>>(value), source);
    } else {
        return false;
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
