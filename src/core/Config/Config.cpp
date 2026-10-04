#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ConfigFileFormat Config::detectFormatFromExtension(const std::string& filepath) {
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return ConfigFileFormat::JSON;
    } else if (extension == ".yaml" || extension == ".yml") {
        return ConfigFileFormat::YAML;
    } else if (extension == ".cfg" || extension == ".ini") {
        return ConfigFileFormat::KeyValue;
    }
    
    // Default to key=value for backward compatibility
    return ConfigFileFormat::KeyValue;
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, detectFormatFromExtension(filepath));
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFileFormat::KeyValue:
            return parseKeyValue(std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
        case ConfigFileFormat::JSON:
            try {
                nlohmann::json jsonData = nlohmann::json::parse(file);
                return parseJSON(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                return false;
            }
        case ConfigFileFormat::YAML:
            try {
                YAML::Node yamlData = YAML::Load(file);
                return parseYAML(yamlData);
            } catch (const YAML::ParserException& e) {
                return false;
            }
        default:
            return false;
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        return parseJSON(jsonData);
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        return parseYAML(yamlData);
    } catch (const YAML::ParserException& e) {
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
    return saveToFile(filepath, detectFormatFromExtension(filepath));
}

bool Config::saveToFile(const std::string& filepath, ConfigFileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFileFormat::KeyValue:
            file << toKeyValue();
            break;
        case ConfigFileFormat::JSON:
            file << toJSON().dump(4);
            break;
        case ConfigFileFormat::YAML:
            file << toYAML();
            break;
        default:
            return false;
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJSON().dump(4);
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toYAML();
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
    // Validate the value if schema validation is enabled
    if (schema_.validateOnSet && !validateValue(key, value)) {
        throw std::invalid_argument("Validation failed for key: " + key);
    }
    
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
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
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

bool Config::parseKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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

bool Config::parseJSON(const nlohmann::json& jsonData) {
    if (!jsonData.is_object()) {
        return false;
    }
    
    for (const auto& [key, value] : jsonData.items()) {
        ConfigValue configValue;
        
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Handle arrays of different types
            if (!value.empty()) {
                if (value[0].is_string()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                } else if (value[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                }
            }
        }
        
        if (!configValue.index()) { // Check if it's a valid variant
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseYAML(const YAML::Node& yamlData) {
    if (!yamlData.IsMap()) {
        return false;
    }
    
    for (const auto& node : yamlData) {
        std::string key = node.first.as<std::string>();
        YAML::Node value = node.second;
        
        ConfigValue configValue;
        
        if (value.IsScalar()) {
            const std::string& scalarValue = value.as<std::string>();
            
            // Try to infer type
            if (scalarValue == "true" || scalarValue == "false") {
                configValue = scalarValue == "true";
            } else {
                try {
                    size_t pos;
                    int intVal = std::stoi(scalarValue, &pos);
                    if (pos == scalarValue.size()) {
                        configValue = intVal;
                    } else {
                        double doubleVal = std::stod(scalarValue, &pos);
                        if (pos == scalarValue.size()) {
                            configValue = doubleVal;
                        } else {
                            configValue = scalarValue;
                        }
                    }
                } catch (const std::exception&) {
                    configValue = scalarValue;
                }
            }
        } else if (value.IsSequence()) {
            // Handle arrays
            if (!value.empty()) {
                std::vector<std::string> strArray;
                bool allStrings = true;
                
                for (const auto& item : value) {
                    if (item.IsScalar()) {
                        strArray.push_back(item.as<std::string>());
                    } else {
                        allStrings = false;
                        break;
                    }
                }
                
                if (allStrings) {
                    configValue = strArray;
                }
            }
        }
        
        if (configValue.index()) { // Check if it's a valid variant
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

std::string Config::toKeyValue() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
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
        
        oss << "\n\n";
    }
    
    return oss.str();
}

nlohmann::json Config::toJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return jsonData;
}

YAML::Node Config::toYAML() const {
    YAML::Node yamlData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&yamlData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                yamlData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                yamlData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return yamlData;
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    for (const auto& rule : schema_.validationRules) {
        if (rule.key == key) {
            if (!rule.validator(value)) {
                return false;
            }
        }
    }
    return true;
}

void Config::applyDefaults() {
    for (const auto& [key, defaultValue] : schema_.defaultValues) {
        if (!has(key)) {
            set(key, defaultValue, ConfigSource::Default);
        }
    }
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