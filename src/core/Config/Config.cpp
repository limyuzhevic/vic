#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
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

// File format detection implementation
FileFormat detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return FileFormat::Unknown;
    }
    
    char firstChar;
    file.get(firstChar);
    if (file.eof()) {
        return FileFormat::Unknown;
    }
    
    file.clear();
    
    if (firstChar == '{') {
        // Try to parse as JSON
        try {
            nlohmann::json::parse(file);
            return FileFormat::JSON;
        } catch (...) {
            return FileFormat::Unknown;
        }
    } else if (firstChar == '[') {
        return FileFormat::JSON;
    } else if (firstChar == '-') {
        // Check if it's YAML (starts with dash)
        return FileFormat::YAML;
    } else if (std::isalpha(firstChar) || firstChar == '_' || firstChar == '"' || firstChar == '\'') {
        // Check for key=value format
        return FileFormat::KeyValue;
    }
    
    return FileFormat::Unknown;
}

// Convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    
    return jsonValue;
}

// Convert JSON to ConfigValue
bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& outValue) {
    try {
        if (jsonValue.is_boolean()) {
            outValue = jsonValue.get<bool>();
        } else if (jsonValue.is_number_integer()) {
            outValue = jsonValue.get<int>();
        } else if (jsonValue.is_number_float()) {
            outValue = jsonValue.get<double>();
        } else if (jsonValue.is_string()) {
            outValue = jsonValue.get<std::string>();
        } else if (jsonValue.is_array()) {
            // Check array element type
            if (jsonValue.empty()) {
                outValue = std::vector<std::string>{};
            } else {
                const auto& firstElement = jsonValue[0];
                if (firstElement.is_boolean()) {
                    std::vector<bool> vec;
                    for (const auto& elem : jsonValue) {
                        vec.push_back(elem.get<bool>());
                    }
                    outValue = vec;
                } else if (firstElement.is_number_integer()) {
                    std::vector<int> vec;
                    for (const auto& elem : jsonValue) {
                        vec.push_back(elem.get<int>());
                    }
                    outValue = vec;
                } else if (firstElement.is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& elem : jsonValue) {
                        vec.push_back(elem.get<double>());
                    }
                    outValue = vec;
                } else if (firstElement.is_string()) {
                    std::vector<std::string> vec;
                    for (const auto& elem : jsonValue) {
                        vec.push_back(elem.get<std::string>());
                    }
                    outValue = vec;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// Convert ConfigValue to YAML
void configValueToYaml(YAML::Emitter& emitter, const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            emitter << YAML::Double(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            emitter << YAML::Double(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << YAML::Double(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            emitter << YAML::Double(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << YAML::Double(item);
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << YAML::Double(item);
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        }
    }, value);
}

// Convert YAML to ConfigValue
bool yamlToConfigValue(const YAML::Node& node, ConfigValue& outValue) {
    try {
        if (node.IsScalar()) {
            std::string value = node.Scalar();
            
            // Try to convert to different types based on content
            if (value == "true" || value == "false") {
                outValue = (value == "true");
            } else {
                // Try to parse as number
                try {
                    if (value.find('.') != std::string::npos) {
                        double d = std::stod(value);
                        outValue = d;
                    } else {
                        int i = std::stoi(value);
                        outValue = i;
                    }
                } catch (...) {
                    outValue = value;
                }
            }
        } else if (node.IsSequence()) {
            // Determine element type
            if (node.size() == 0) {
                outValue = std::vector<std::string>{};
            } else {
                const auto& firstElement = node[0];
                if (firstElement.IsScalar()) {
                    std::string firstValue = firstElement.Scalar();
                    if (firstValue == "true" || firstValue == "false") {
                        std::vector<bool> vec;
                        for (const auto& elem : node) {
                            vec.push_back(elem.as<bool>());
                        }
                        outValue = vec;
                    } else {
                        try {
                            if (firstValue.find('.') != std::string::npos) {
                                std::vector<double> vec;
                                for (const auto& elem : node) {
                                    vec.push_back(elem.as<double>());
                                }
                                outValue = vec;
                            } else {
                                std::vector<int> vec;
                                for (const auto& elem : node) {
                                    vec.push_back(elem.as<int>());
                                }
                                outValue = vec;
                            }
                        } catch (...) {
                            std::vector<std::string> vec;
                            for (const auto& elem : node) {
                                vec.push_back(elem.Scalar());
                            }
                            outValue = vec;
                        }
                    }
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// Get file extension
std::string getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filepath.substr(pos + 1);
}

// Check if file is empty
bool isFileEmpty(const std::string& filepath) {
    std::ifstream file(filepath);
    return !file.good();
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    FileFormat format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse based on format
    switch (format) {
        case FileFormat::KeyValue: {
            // Backward compatibility with simple key=value format
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
            break;
        }
        case FileFormat::JSON: {
            try {
                auto jsonData = nlohmann::json::parse(file);
                
                // Handle both object and array formats
                if (jsonData.is_object()) {
                    for (const auto& [key, value] : jsonData.items()) {
                        ConfigValue configValue;
                        if (jsonToConfigValue(value, configValue)) {
                            set(key, configValue, ConfigSource::File);
                        }
                    }
                } else if (jsonData.is_array()) {
                    // For arrays, create keys like key0, key1, etc.
                    int index = 0;
                    for (const auto& value : jsonData) {
                        std::string key = "key" + std::to_string(index++);
                        ConfigValue configValue;
                        if (jsonToConfigValue(value, configValue)) {
                            set(key, configValue, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                return false;
            }
            break;
        }
        case FileFormat::YAML: {
            try {
                auto yamlData = YAML::Load(file);
                
                // Handle both mapping and sequence formats
                if (yamlData.IsMap()) {
                    for (const auto& it : yamlData) {
                        ConfigValue configValue;
                        if (yamlToConfigValue(it.second, configValue)) {
                            set(it.first.Scalar(), configValue, ConfigSource::File);
                        }
                    }
                } else if (yamlData.IsSequence()) {
                    // For sequences, create keys like item0, item1, etc.
                    int index = 0;
                    for (const auto& item : yamlData) {
                        std::string key = "item" + std::to_string(index++);
                        ConfigValue configValue;
                        if (yamlToConfigValue(item, configValue)) {
                            set(key, configValue, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                return false;
            }
            break;
        }
        case FileFormat::Unknown:
        default:
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
    
    // Use JSON format by default
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        jsonData[entry.key] = configValueToJson(entry.value);
    }
    
    try {
        file << jsonData.dump(4); // Pretty print with indentation
    } catch (...) {
        return false;
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