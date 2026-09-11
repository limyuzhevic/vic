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

// Helper function to detect file format by extension
static std::string getFileFormat(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Helper function to merge two configs
static void mergeConfigs(std::vector<ConfigEntry>& target, const std::vector<ConfigEntry>& source) {
    for (const auto& entry : source) {
        auto it = std::find_if(target.begin(), target.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != target.end()) {
            it->value = entry.value;
            it->source = entry.source;
            it->description = entry.description;
        } else {
            target.push_back(entry);
        }
    }
}

// Helper function to get file size
static bool getFileSize(const std::string& filepath, uintmax_t& size) {
    std::error_code ec;
    size = std::filesystem::file_size(filepath, ec);
    return !ec;
}

// Helper function to check if file is readable
static bool isFileReadable(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

// Helper function to check if file is writable
static bool isFileWritable(const std::string& filepath) {
    std::ofstream file(filepath, std::ios::app);
    return file.good();
}

// Helper function to backup a file
static bool backupFile(const std::string& filepath) {
    std::ifstream src(filepath, std::ios::binary);
    if (!src) return false;
    
    std::string backupPath = filepath + ".bak";
    std::ofstream dst(backupPath, std::ios::binary);
    if (!dst) return false;
    
    dst << src.rdbuf();
    return true;
}

// Helper function to parse YAML with line numbers
static YAML::Node parseYAMLWithLines(const std::string& yamlString) {
    YAML::Scanner scanner(yamlString);
    YAML::Parser parser;
    return YAML::Load(yamlString);
}

// Helper function to convert YAML to string with formatting
static std::string formatYAML(const YAML::Node& node, int indent = 0) {
    std::stringstream ss;
    std::string indentStr(indent, ' ');
    
    if (node.IsScalar()) {
        ss << node.as<std::string>();
    } else if (node.IsSequence()) {
        ss << "[";
        for (size_t i = 0; i < node.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << formatYAML(node[i], indent + 2);
        }
        ss << "]";
    } else if (node.IsMap()) {
        ss << "{\n";
        for (const auto& pair : node) {
            ss << indentStr << "  " << pair.first.as<std::string>() << ": ";
            ss << formatYAML(pair.second, indent + 4) << ",\n";
        }
        ss << indentStr << "}";
    }
    
    return ss.str();
}

// Helper function to get human-readable file size
static std::string getHumanReadableSize(uintmax_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

// Helper function to generate config hash
static std::string generateConfigHash(const Config& config) {
    std::stringstream ss;
    for (const auto& entry : config.pImpl->entries) {
        ss << entry.key << "=";
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << arg;
            } else {
                ss << std::to_string(arg);
            }
        }, entry.value);
        ss << "\n";
    }
    
    std::hash<std::string> hasher;
    return std::to_string(hasher(ss.str()));
}

// Helper function to validate config entry
static bool validateConfigEntry(const ConfigEntry& entry) {
    if (entry.key.empty()) {
        return false;
    }
    
    // Validate value based on type
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string> && arg.empty()) {
            // Allow empty strings
        } else if constexpr (std::is_same_v<T, bool>) {
            // Always valid
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            // Always valid
        } else if constexpr (std::is_same_v<T, double>) {
            // Always valid
        } else if constexpr (std::is_same_v<T, std::vector<int>> && !arg.empty()) {
            for (const auto& val : arg) {
                // Each value must be valid
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>> && !arg.empty()) {
            for (const auto& val : arg) {
                // Each value must be valid
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& val : arg) {
                // Each string must be valid
            }
        }
    }, entry.value);
    
    return true;
}

// Helper function to convert Config to JSON
static nlohmann::json configToJSON(const Config& config) {
    nlohmann::json jsonConfig = nlohmann::json::object();
    
    for (const auto& entry : config.pImpl->entries) {
        nlohmann::json jsonValue;
        
        std::visit([&jsonValue](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            }
        }, entry.value);
        
        jsonConfig[entry.key] = jsonValue;
    }
    
    return jsonConfig;
}

// Helper function to convert Config to YAML
static YAML::Node configToYAML(const Config& config) {
    YAML::Node yamlConfig;
    
    for (const auto& entry : config.pImpl->entries) {
        YAML::Node yamlValue;
        
        std::visit([&yamlValue](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                yamlValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                yamlValue = arg;
            }
        }, entry.value);
        
        yamlConfig[entry.key] = yamlValue;
    }
    
    return yamlConfig;
}

// Helper function to convert Config to YAML string
static std::string configToYAMLString(const Config& config) {
    YAML::Node yaml = configToYAML(config);
    std::stringstream ss;
    ss << yaml;
    return ss.str();
}

// Helper function to convert JSON to Config string
static std::string configToJSONString(const Config& config) {
    nlohmann::json json = config.convertToJSON();
    return json.dump(4);
}

// Helper function to validate against schema
static bool validateConfigAgainstSchema(const Config& config, const ConfigSchema& schema) {
    for (const auto& entry : config.pImpl->entries) {
        auto it = schema.children.find(entry.key);
        if (it != schema.children.end()) {
            const auto& fieldSchema = it->second;
            
            if (fieldSchema.required && entry.source == ConfigSource::Default) {
                return false;
            }
            
            if (fieldSchema.min.has_value()) {
                std::visit([&fieldSchema](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        if (static_cast<int64_t>(arg) < fieldSchema.min.value()) {
                            throw std::runtime_error("Value below minimum");
                        }
                    } else if constexpr (std::is_same_v<T, double>) {
                        if (arg < fieldSchema.max.value()) {
                            throw std::runtime_error("Value below minimum");
                        }
                    }
                }, entry.value);
            }
            
            if (fieldSchema.max.has_value()) {
                std::visit([&fieldSchema](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        if (static_cast<int64_t>(arg) > fieldSchema.max.value()) {
                            throw std::runtime_error("Value above maximum");
                        }
                    } else if constexpr (std::is_same_v<T, double>) {
                        if (arg > fieldSchema.max.value()) {
                            throw std::runtime_error("Value above maximum");
                        }
                    }
                }, entry.value);
            }
            
            if (!fieldSchema.allowedValues.empty()) {
                std::visit([&fieldSchema](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    std::string valueStr;
                    if constexpr (std::is_same_v<T, std::string>) {
                        valueStr = arg;
                    } else {
                        std::stringstream ss;
                        ss << arg;
                        valueStr = ss.str();
                    }
                    
                    if (std::find(fieldSchema.allowedValues.begin(), fieldSchema.allowedValues.end(), valueStr) == fieldSchema.allowedValues.end()) {
                        throw std::runtime_error("Value not in allowed values");
                    }
                }, entry.value);
            }
        }
    }
    return true;
}

// Helper function to convert JSON to Config
static bool jsonToConfig(const nlohmann::json& json, std::vector<ConfigEntry>& entries, ConfigSource source) {
    try {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                if (!it->empty()) {
                    if (it->front().is_string()) {
                        value = it->get<std::vector<std::string>>();
                    } else if (it->front().is_number_integer()) {
                        value = it->get<std::vector<int>>();
                    } else if (it->front().is_number_float()) {
                        value = it->get<std::vector<double>>();
                    }
                }
            }
            
            if (validateConfigEntry(ConfigEntry(key, value, ConfigSource::Default))) {
                entries.emplace_back(key, value, source);
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper function to convert YAML to Config
static bool yamlToConfig(const YAML::Node& yaml, std::vector<ConfigEntry>& entries, ConfigSource source) {
    try {
        for (const auto& node : yaml) {
            std::string key = node.first.as<std::string>();
            ConfigValue value;
            
            if (node.second.IsScalar()) {
                if (node.second.IsInt()) {
                    value = node.second.as<int>();
                } else if (node.second.IsFloat()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBool()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else if (node.second.IsSequence()) {
                if (!node.second.empty()) {
                    if (node.second[0].IsScalar()) {
                        value = node.second.as<std::vector<std::string>>();
                    } else if (node.second[0].IsInt()) {
                        value = node.second.as<std::vector<int>>();
                    } else if (node.second[0].IsFloat()) {
                        value = node.second.as<std::vector<double>>();
                    }
                }
            }
            
            if (validateConfigEntry(ConfigEntry(key, value, ConfigSource::Default))) {
                entries.emplace_back(key, value, source);
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string format = getFileFormat(filepath);
    
    try {
        if (format == "json" || format == "jsonc") {
            nlohmann::json json;
            file >> json;
            return jsonToConfig(json, pImpl->entries, ConfigSource::File);
        } else if (format == "yaml" || format == "yml") {
            YAML::Node yaml;
            file >> yaml;
            return yamlToConfig(yaml, pImpl->entries, ConfigSource::File);
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return false;
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
    
    // Determine format from extension
    std::string format = getFileFormat(filepath);
    
    try {
        if (format == "json" || format == "jsonc") {
            nlohmann::json json = configToJSON(*this);
            file << json.dump(4);
        } else if (format == "yaml" || format == "yml") {
            YAML::Node yaml = configToYAML(*this);
            file << yaml;
        } else {
            // Fallback to simple format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
            }
        }
    } catch (const std::exception& e) {
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
