#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <iomanip>
#include <map>

// Include logger for error reporting
#include "core/Logger/Logger.hpp"

using json = nlohmann::json;

namespace nlm {

// Forward declarations
class JsonConfigParser;
class YamlConfigParser;

// Configuration file parser interface
class IConfigParser {
public:
    virtual ~IConfigParser() = default;
    virtual bool parse(const std::string& content, Config& config) = 0;
    virtual bool serialize(const Config& config, std::string& output) = 0;
    virtual std::string getFormat() const = 0;
};

// JSON configuration parser
class JsonConfigParser : public IConfigParser {
public:
    bool parse(const std::string& content, Config& config) override {
        try {
            auto jsonConfig = json::parse(content);
            flattenJsonObject(jsonConfig, "", config);
            return true;
        } catch (const json::parse_error& e) {
            NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
            return false;
        }
    }

    bool serialize(const Config& config, std::string& output) override {
        try {
            json j;
            unflattenConfigToJson(config, j, "");
            output = j.dump(2); // Pretty print with 2-space indentation
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON serialization error: " + std::string(e.what()));
            return false;
        }
    }

    std::string getFormat() const override { return "JSON"; }

private:
    void flattenJsonObject(const json& j, const std::string& prefix, Config& config) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            if (!prefix.empty()) {
                key = prefix + "." + key;
            }

            if (it->is_object()) {
                flattenJsonObject(*it, key, config);
            } else if (it->is_array()) {
                // Convert array to string representation
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < it->size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << (*it)[i].dump();
                }
                oss << "]";
                config.set(key, oss.str(), ConfigSource::File);
            } else {
                setJsonValue(key, *it, config);
            }
        }
    }

    void setJsonValue(const std::string& key, const json& value, Config& config) {
        if (value.is_string()) {
            config.set(key, value.get<std::string>(), ConfigSource::File);
        } else if (value.is_number_integer()) {
            config.set(key, value.get<int64_t>(), ConfigSource::File);
        } else if (value.is_number_unsigned()) {
            config.set(key, value.get<uint64_t>(), ConfigSource::File);
        } else if (value.is_number_float()) {
            config.set(key, value.get<double>(), ConfigSource::File);
        } else if (value.is_boolean()) {
            config.set(key, value.get<bool>(), ConfigSource::File);
        } else {
            // Fallback to string representation
            config.set(key, value.dump(), ConfigSource::File);
        }
    }

    void unflattenConfigToJson(const Config& config, json& j, const std::string& prefix) {
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            std::string fullKey = (prefix.empty() ? key : prefix + "." + key);
            auto value = config.getAny(key);

            if (value.has_value()) {
                // Determine if this is a leaf value or should be nested
                if (fullKey.find(".") != std::string::npos && isNestedKey(config, key, prefix)) {
                    // Recursively build nested structure
                    size_t dotPos = fullKey.find(".");
                    std::string parentKey = fullKey.substr(0, dotPos);
                    std::string childKey = fullKey.substr(dotPos + 1);

                    if (!j.contains(parentKey)) {
                        j[parentKey] = json::object();
                    }
                    unflattenConfigToJson(config, j[parentKey], parentKey);
                } else {
                    // Set directly in the JSON object
                    setJsonValueFromConfig(j, fullKey, value.value());
                }
            }
        }
    }

    void setJsonValueFromConfig(json& j, const std::string& key, const ConfigValue& value) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int> || std::is_same_v<T, uint64_t>) {
                j[key] = static_cast<int64_t>(arg);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                j[key] = static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>) {
                // Convert vector to JSON array
                json array = json::array();
                for (const auto& item : arg) {
                    array.push_back(item);
                }
                j[key] = array;
            } else {
                // Unknown type, convert to string
                std::ostringstream oss;
                oss << arg;
                j[key] = oss.str();
            }
        }, value);
    }

    bool isNestedKey(const Config& config, const std::string& key, const std::string& prefix) const {
        // Check if there are other keys that start with the same prefix
        auto keys = config.getKeys();
        for (const auto& k : keys) {
            if (k != key && k.find(prefix + ".") == 0) {
                return true;
            }
        }
        return false;
    }
};

// YAML configuration parser
class YamlConfigParser : public IConfigParser {
public:
    bool parse(const std::string& content, Config& config) override {
        try {
            auto yamlConfig = YAML::Load(content);
            flattenYamlNode(yamlConfig, "", config);
            return true;
        } catch (const YAML::ParserException& e) {
            NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
            return false;
        }
    }

    bool serialize(const Config& config, std::string& output) override {
        try {
            YAML::Emitter emitter;
            emitter.SetIndent(2);
            unflattenConfigToYaml(config, emitter, "", false);
            output = emitter.c_str();
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("YAML serialization error: " + std::string(e.what()));
            return false;
        }
    }

    std::string getFormat() const override { return "YAML"; }

private:
    void flattenYamlNode(const YAML::Node& node, const std::string& prefix, Config& config) {
        if (node.IsMap()) {
            for (auto it : node) {
                std::string key = it.first.as<std::string>();
                if (!prefix.empty()) {
                    key = prefix + "." + key;
                }

                if (it.second.IsMap()) {
                    flattenYamlNode(it.second, key, config);
                } else if (it.second.IsSequence()) {
                    // Convert YAML sequence to string representation
                    std::ostringstream oss;
                    oss << "[";
                    for (size_t i = 0; i < it.second.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << it.second[i].as<std::string>();
                    }
                    oss << "]";
                    config.set(key, oss.str(), ConfigSource::File);
                } else {
                    setYamlValue(key, it.second, config);
                }
            }
        } else if (node.IsSequence()) {
            // This shouldn't happen at top level, but handle it
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < node.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << node[i].as<std::string>();
            }
            oss << "]";
            config.set("root", oss.str(), ConfigSource::File);
        } else {
            setYamlValue(prefix, node, config);
        }
    }

    void setYamlValue(const std::string& key, const YAML::Node& value, Config& config) {
        if (value.IsScalar()) {
            std::string strValue = value.as<std::string>();
            
            // Try to determine the type
            if (strValue == "true" || strValue == "false") {
                config.set(key, strValue == "true", ConfigSource::File);
            } else {
                try {
                    size_t pos;
                    int intValue = std::stoi(strValue, &pos);
                    if (pos == strValue.size()) {
                        config.set(key, intValue, ConfigSource::File);
                        return;
                    }
                } catch (...) {}

                try {
                    size_t pos;
                    long long intValue = std::stoll(strValue, &pos);
                    if (pos == strValue.size()) {
                        config.set(key, intValue, ConfigSource::File);
                        return;
                    }
                } catch (...) {}

                try {
                    size_t pos;
                    double floatValue = std::stod(strValue, &pos);
                    if (pos == strValue.size()) {
                        config.set(key, floatValue, ConfigSource::File);
                        return;
                    }
                } catch (...) {}

                // Fallback to string
                config.set(key, strValue, ConfigSource::File);
            }
        }
    }

    void unflattenConfigToYaml(const Config& config, YAML::Emitter& emitter, const std::string& prefix, bool isNested) {
        auto keys = config.getKeys();
        size_t i = 0;

        // Sort keys for deterministic output
        std::vector<std::string> sortedKeys(keys.begin(), keys.end());
        std::sort(sortedKeys.begin(), sortedKeys.end());

        for (const auto& key : sortedKeys) {
            std::string fullKey = (prefix.empty() ? key : prefix + "." + key);

            if (i++ > 0) {
                if (!isNested) emitter << YAML::Newline;
                else emitter << " ";
            }

            if (fullKey.find(".") != std::string::npos && isNestedKey(config, key, prefix)) {
                // This is a nested structure - we need to handle it differently in YAML
                // For simplicity, we'll output as a map
                size_t dotPos = fullKey.find(".");
                std::string parentKey = fullKey.substr(0, dotPos);
                std::string childKey = fullKey.substr(dotPos + 1);

                if (!isNested) {
                    emitter << YAML::BeginMap;
                    emitter << YAML::Key << parentKey;
                    emitter << YAML::Value;
                    unflattenConfigToYaml(config, emitter, parentKey, true);
                    emitter << YAML::EndMap;
                }
            } else {
                // Simple key-value pair
                if (!isNested) {
                    emitter << YAML::Key << key;
                    emitter << YAML::Value;
                }

                auto value = config.getAny(key);
                if (value.has_value()) {
                    setYamlValueFromConfig(emitter, value.value());
                }
            }
        }
    }

    void setYamlValueFromConfig(YAML::Emitter& emitter, const ConfigValue& value) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int> || std::is_same_v<T, uint64_t>) {
                emitter << static_cast<int64_t>(arg);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                // Check if it's an integer for cleaner YAML
                if (static_cast<double>(arg) == static_cast<int64_t>(arg)) {
                    emitter << static_cast<int64_t>(arg);
                } else {
                    emitter << static_cast<double>(arg);
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::FlowSeqStart;
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) emitter << YAML::Item;
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        emitter << arg[i];
                    } else {
                        emitter << arg[i];
                    }
                }
                emitter << YAML::FlowSeqEnd;
            } else {
                // Unknown type, convert to string
                std::ostringstream oss;
                oss << arg;
                emitter << oss.str();
            }
        }, value);
    }

    bool isNestedKey(const Config& config, const std::string& key, const std::string& prefix) const {
        // Check if there are other keys that start with the same prefix
        auto keys = config.getKeys();
        for (const auto& k : keys) {
            if (k != key && k.find(prefix + ".") == 0) {
                return true;
            }
        }
        return false;
    }
};

// Configuration file parser registry
class ConfigFileParser {
public:
    ConfigFileParser() {
        parsers["json"] = std::make_unique<JsonConfigParser>();
        parsers["yaml"] = std::make_unique<YamlConfigParser>();
        parsers["yml"] = std::make_unique<YamlConfigParser>();
        parsers["cfg"] = std::make_unique<SimpleConfigParser>();
    }

    std::unique_ptr<IConfigParser> getParser(const std::string& extension) {
        auto it = parsers.find(extension);
        if (it != parsers.end()) {
            return it->second->getFormat();
        }
        return "cfg"; // Default to simple format
    }

    bool parse(const std::string& content, const std::string& format, Config& config) {
        auto it = parsers.find(format);
        if (it != parsers.end()) {
            return it->second->parse(content, config);
        }
        return false;
    }

    bool serialize(const Config& config, const std::string& format, std::string& output) {
        auto it = parsers.find(format);
        if (it != parsers.end()) {
            return it->second->serialize(config, output);
        }
        return false;
    }

    std::vector<std::string> getSupportedFormats() const {
        std::vector<std::string> formats;
        for (const auto& pair : parsers) {
            formats.push_back(pair.first);
        }
        return formats;
    }

private:
    std::map<std::string, std::unique_ptr<IConfigParser>> parsers;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigFileParser> fileParser;

    Impl() : fileParser(std::make_unique<ConfigFileParser>()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile method with format detection
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Detect file format from extension
    std::string extension = "cfg"; // Default
    size_t dotPos = filepath.find_last_of(".");
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        std::string ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Check if we support this format
        auto supportedFormats = pImpl->fileParser->getSupportedFormats();
        if (std::find(supportedFormats.begin(), supportedFormats.end(), ext) != supportedFormats.end()) {
            extension = ext;
        }
    }

    // Parse based on detected format
    return pImpl->fileParser->parse(content, extension, *this);
}

// Enhanced saveToFile method with format support
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write config file: " + filepath);
        return false;
    }

    // Detect file format from extension
    std::string extension = "cfg"; // Default
    size_t dotPos = filepath.find_last_of(".");
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        std::string ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Check if we support this format
        auto supportedFormats = pImpl->fileParser->getSupportedFormats();
        if (std::find(supportedFormats.begin(), supportedFormats.end(), ext) != supportedFormats.end()) {
            extension = ext;
        }
    }

    // Serialize to the specified format
    std::string content;
    if (pImpl->fileParser->serialize(*this, extension, content)) {
        file << content;
        return true;
    } else {
        NLM_LOG_ERROR("Failed to serialize config to " + extension + " format");
        return false;
    }
}

// Backward compatibility methods (keep existing simple format)
bool Config::loadFromFileSimple(const std::string& filepath) {
    return Config::loadFromFile(filepath);
}

bool Config::saveToFileSimple(const std::string& filepath) const {
    return Config::saveToFile(filepath);
}

// Method to get supported file formats
std::vector<std::string> Config::getSupportedFormats() const {
    return pImpl->fileParser->getSupportedFormats();
}

// Enhanced loadFromArgs with better argument parsing
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

// New method to get configuration as JSON string
std::string Config::toJson() const {
    json j;
    for (const auto& entry : pImpl->entries) {
        setJsonValueFromConfig(j, entry.key, entry.value);
    }
    return j.dump(2);
}

// New method to get configuration as YAML string
std::string Config::toYaml() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    unflattenConfigToYaml(*this, emitter, "", false);
    return emitter.c_str();
}

// New method to validate configuration
bool Config::validate(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + key);
            return false;
        }
    }
    return true;
}

// New method to merge configurations
void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Don't override existing entries with same key unless from higher priority source
        if (!has(entry.key) || static_cast<int>(entry.source) > static_cast<int>(getEntrySource(entry.key))) {
            auto value = other.getAny(entry.key);
            if (value.has_value()) {
                // Create a copy with the new source
                ConfigEntry newEntry;
                newEntry.key = entry.key;
                newEntry.value = value.value();
                newEntry.source = source;
                newEntry.description = entry.description;
                
                // Replace or add
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key = entry.key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end()) {
                    *it = newEntry;
                } else {
                    pImpl->entries.push_back(newEntry);
                }
            }
        }
    }
}

// Helper method to get entry source
ConfigSource Config::getEntrySource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
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

// Template method to get any config value as a specific type
template<typename T>
T Config::getAny(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return std::get<T>(it->value);
    }
    throw std::runtime_error("Config key not found: " + key);
}

// Explicit template instantiations
template int64_t Config::getAny<int64_t>(const std::string& key) const;
template int Config::getAny<int>(const std::string& key) const;
template uint64_t Config::getAny<uint64_t>(const std::string& key) const;
template float Config::getAny<float>(const std::string& key) const;
template double Config::getAny<double>(const std::string& key) const;
template bool Config::getAny<bool>(const std::string& key) const;
template std::string Config::getAny<std::string>(const std::string& key) const;
template std::vector<int> Config::getAny<std::vector<int>>(const std::string& key) const;
template std::vector<double> Config::getAny<std::vector<double>>(const std::string& key) const;
template std::vector<std::string> Config::getAny<std::vector<std::string>>(const std::string& key) const;

} // namespace nlm
