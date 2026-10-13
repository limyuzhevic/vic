#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <iomanip>
#include <cctype>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schemas;
    bool validationEnabled;
    std::string validationErrors;
    std::string baseDir;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validationEnabled = true;
    pImpl->baseDir = std::filesystem::current_path().string();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        pImpl->validationErrors = "Empty filepath provided";
        return false;
    }
    
    std::string fullPath = filepath;
    if (!std::filesystem::is_absolute(filepath)) {
        fullPath = pImpl->baseDir + "/" + filepath;
    }
    
    if (!std::filesystem::exists(fullPath)) {
        pImpl->validationErrors = "File does not exist: " + fullPath;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(fullPath)) {
        pImpl->validationErrors = "Path is not a regular file: " + fullPath;
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".jsonc" || ext == ".json5") {
        bool result = loadFromJsonFile(fullPath);
        if (!result) {
            pImpl->validationErrors = "Failed to parse JSON file: " + fullPath;
        }
        return result;
    } else if (ext == ".yaml" || ext == ".yml") {
        if constexpr (HAS_YAML) {
            bool result = loadFromYamlFile(fullPath);
            if (!result) {
                pImpl->validationErrors = "Failed to parse YAML file: " + fullPath;
            }
            return result;
        } else {
            pImpl->validationErrors = "YAML support not available for file: " + fullPath;
            return false; // YAML support not available
        }
    } else {
        // Try legacy format first
        bool result = loadFromLegacyFile(fullPath);
        if (result) {
            return true;
        }
        // Try JSON format if legacy fails
        result = loadFromJsonFile(fullPath);
        if (!result) {
            pImpl->validationErrors = "Failed to parse file " + fullPath + ". Unsupported format or parsing error.";
        }
        return result;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationErrors = "Failed to open file: " + filepath;
        return false;
    }
    
    // Check if file is empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        pImpl->validationErrors = "File is empty: " + filepath;
        return false;
    }
    file.seekg(0, std::ios::beg);
    
    try {
        json jsonData;
        file >> jsonData;
        clear(); // Clear existing entries before loading
        
        // Handle nested JSON structure (with sections)
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                if (it.key() == "__schema" || it.key() == "metadata") {
                    continue; // Skip metadata fields
                }
                
                // Parse as nested structure
                if (it.value().is_object()) {
                    // Handle nested section
                    for (auto nestedIt = it.value().begin(); nestedIt != it.value().end(); ++nestedIt) {
                        setNestedValueFromJson(it.key(), nestedIt.key(), nestedIt.value(), ConfigSource::File);
                    }
                } else {
                    // Handle top-level key
                    setNestedValueFromJson("", it.key(), it.value(), ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        pImpl->validationErrors = std::string("JSON parse error: ") + e.what() + " in file: " + filepath;
        return false;
    } catch (const std::exception& e) {
        pImpl->validationErrors = std::string("Unexpected error loading JSON: ") + e.what() + " in file: " + filepath;
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationErrors = "Failed to open file: " + filepath;
        return false;
    }
    
    // Check if file is empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        pImpl->validationErrors = "File is empty: " + filepath;
        return false;
    }
    file.seekg(0, std::ios::beg);
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        clear();
        
        if (yamlData.IsMap()) {
            for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
                if (it->first.IsScalar() && (it->first.as<std::string>() == "__schema" || 
                                             it->first.as<std::string>() == "metadata")) {
                    continue;
                }
                
                std::string section = "";
                if (it->second.IsMap()) {
                    for (auto nestedIt = it->second.begin(); nestedIt != it->second.end(); ++nestedIt) {
                        section = it->first.as<std::string>();
                        setNestedValueFromYaml(section, nestedIt->first.as<std::string>(), nestedIt->second, ConfigSource::File);
                    }
                } else {
                    setNestedValueFromYaml("", it->first.as<std::string>(), *it, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->validationErrors = std::string("YAML parse error: ") + e.what() + " in file: " + filepath;
        return false;
    } catch (const std::exception& e) {
        pImpl->validationErrors = std::string("Unexpected error loading YAML: ") + e.what() + " in file: " + filepath;
        return false;
    }
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing entries before loading
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
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
            
            // Parse value based on content
            ConfigValue configValue = parseLegacyValue(value);
            
            // Handle section key (e.g., "section.key")
            std::string section = "";
            std::string actualKey = key;
            if (key.find('.') != std::string::npos) {
                auto [sec, keyPart] = parseSectionKey(key);
                section = sec;
                actualKey = keyPart;
            }
            
            set(section, actualKey, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1) {
        return true; // Nothing to do
    }
    
    clear(); // Clear existing entries before parsing args
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            pImpl->validationErrors = "Null argument pointer at index " + std::to_string(i);
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --section.key=value format
        if (arg.find('=') != std::string::npos) {
            // Parse --section.key=value
            if (arg.find("=") != std::string::npos) {
                size_t pos = arg.find('=');
                std::string keyPart = arg.substr(2, pos - 2); // Skip "--"
                std::string value = arg.substr(pos + 1);
                
                std::string section, actualKey;
                try {
                    std::tie(section, actualKey) = parseSectionKey(keyPart);
                } catch (...) {
                    pImpl->validationErrors = "Invalid section.key format in argument: " + arg;
                    continue;
                }
                
                ConfigValue configValue = parseCommandLineValue(value);
                set(section, actualKey, configValue, ConfigSource::CommandLine);
            }
        }
        // Handle -section.key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string keyPart = arg.substr(1);
            std::string value = argv[++i];
            
            std::string section, actualKey;
            try {
                std::tie(section, actualKey) = parseSectionKey(keyPart);
            } catch (...) {
                pImpl->validationErrors = "Invalid section.key format in argument: " + arg;
                continue;
            }
            
            ConfigValue configValue = parseCommandLineValue(value);
            set(section, actualKey, configValue, ConfigSource::CommandLine);
        }
        // Handle -key=value or -key value for backward compatibility
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            
            // Check if next arg is a value (not another flag)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                ConfigValue configValue = parseCommandLineValue(value);
                set(key, configValue, ConfigSource::CommandLine);
            } else if (key.find('=') != std::string::npos) {
                // Handle -key=value
                size_t pos = key.find('=');
                std::string actualKey = key.substr(0, pos);
                std::string value = key.substr(pos + 1);
                ConfigValue configValue = parseCommandLineValue(value);
                set(actualKey, configValue, ConfigSource::CommandLine);
            } else {
                // Treat as boolean flag
                set(key, true, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        json jsonData = json::parse(jsonString);
        return fromJson(jsonData);
    } catch (const json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYamlString(const std::string& yamlString) {
#ifdef HAS_YAML
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        return fromYaml(yamlData);
    } catch (const YAML::Exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    if (filepath.empty()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json jsonData = toJson();
        
        if (prettyPrint) {
            file << jsonData.dump(2);
        } else {
            file << jsonData.dump();
        }
        
        // Check if write was successful
        if (!file.good()) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }

bool Config::saveToLegacyFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            // Ensure file is still writable
            if (!file.good()) {
                return false;
            }
            
            file << "# " << entry.description << "\n";
            file << entry.key;
            if (!entry.section.empty()) {
                file << "." << entry.section;
            }
            file << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        // Check if write was successful
        if (!file.good()) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return get<T>("", key);
}

template<typename T>
std::optional<T> Config::get(const std::string& section, const std::string& key) const {
    if (key.empty()) {
        return std::nullopt;
    }
    
    std::string fullKey = createSectionKey(section, key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
    
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
    return getOr<T>("", key, defaultValue);
}

template<typename T>
T Config::getOr(const std::string& section, const std::string& key, const T& defaultValue) const {
    auto val = get<T>(section, key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set("", key, value, source);
}

void Config::set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string fullKey = createSectionKey(section, key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->validated = validateValueInternal(fullKey, value);
    } else {
        ConfigEntry newEntry(fullKey, section, value, source);
        newEntry.validated = validateValueInternal(fullKey, value);
        pImpl->entries.push_back(newEntry);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set("", key, ConfigValue(value), source);
}

void Config::set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set("", key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set("", key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set("", key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return has("", key);
}

bool Config::has(const std::string& section, const std::string& key) const {
    std::string fullKey = createSectionKey(section, key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
}

void Config::remove(const std::string& key) {
    remove("", key);
}

void Config::remove(const std::string& section, const std::string& key) {
    std::string fullKey = createSectionKey(section, key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&fullKey](const ConfigEntry& e) { return e.key == fullKey; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.section.empty()) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::pair<std::string, std::string>> Config::getKeySectionPairs() const {
    std::vector<std::pair<std::string, std::string>> pairs;
    for (const auto& entry : pImpl->entries) {
        if (!entry.section.empty()) {
            pairs.push_back({entry.section, entry.key});
        }
    }
    return pairs;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  ";
        if (!entry.section.empty()) {
            oss << entry.section << ".";
        }
        oss << entry.key << " = [";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (!entry.section.empty()) {
            oss << " [section:" << entry.section << "]";
        }
        oss << "\n";
    }
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n" << pImpl->validationErrors;
    }
    
    return oss.str();
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                stats.fileEntries++;
                break;
            case ConfigSource::File:
                stats.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                stats.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                stats.runtimeEntries++;
                break;
            case ConfigSource::Inherited:
                stats.inheritedEntries++;
                break;
        }
    }
    
    int validEntries = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.validated) {
            validEntries++;
        }
    }
    
    stats.validationScore = stats.totalEntries > 0 ? 
        static_cast<double>(validEntries) / stats.totalEntries * 100.0 : 0.0;
    
    return stats;
}

bool Config::validate() const {
    if (!pImpl->validationEnabled) {
        return true;
    }
    
    std::string errors;
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValueInternal(entry.key, entry.value)) {
            errors += "Key: " + entry.key;
            if (!entry.section.empty()) {
                errors += " (section: " + entry.section + ")";
            }
            errors += " - " + entry.validationError + "\n";
            allValid = false;
        }
    }
    
    pImpl->validationErrors = errors;
    return allValid;
}

std::string Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::setValidationEnabled(bool enabled) {
    pImpl->validationEnabled = enabled;
    if (enabled) {
        validate();
    }
}

bool Config::loadInheritance(const std::string& baseConfigFile) {
    if (baseConfigFile.empty()) {
        pImpl->validationErrors = "Empty base config file path provided";
        return false;
    }
    
    std::string fullPath = baseConfigFile;
    if (!std::filesystem::is_absolute(baseConfigFile)) {
        fullPath = pImpl->baseDir + "/" + baseConfigFile;
    }
    
    if (!std::filesystem::exists(fullPath)) {
        pImpl->validationErrors = "Base config file does not exist: " + fullPath;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(fullPath)) {
        pImpl->validationErrors = "Base config path is not a regular file: " + fullPath;
        return false;
    }
    
    Config baseConfig;
    if (!baseConfig.loadFromFile(fullPath)) {
        pImpl->validationErrors = "Failed to load base config from " + fullPath + ": " + baseConfig.getValidationErrors();
        return false;
    }
    
    return mergeConfig(baseConfig);
}

bool Config::mergeConfig(const Config& otherConfig) {
    if (this == &otherConfig) {
        return false; // Cannot merge with itself
    }
    
    bool changed = false;
    
    for (const auto& entry : otherConfig.pImpl->entries) {
        if (!has(entry.section, entry.key)) {
            ConfigEntry newEntry(entry.key, entry.section, entry.value, ConfigSource::Inherited);
            newEntry.description = entry.description;
            newEntry.validated = entry.validated;
            pImpl->entries.push_back(newEntry);
            changed = true;
        }
    }
    
    return changed;
}

void Config::addSchemaEntry(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

void Config::clearSchemas() {
    pImpl->schemas.clear();
}

nlohmann::json Config::toJson() const {
    json jsonData;
    
    // Organize by section
    std::unordered_map<std::string, json> sections;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.section.empty()) {
            jsonData[entry.key] = configValueToJson(entry.value);
        } else {
            if (!sections.count(entry.section)) {
                sections[entry.section] = json::object();
            }
            sections[entry.section][entry.key] = configValueToJson(entry.value);
        }
    }
    
    // Add sections to JSON
    for (const auto& [section, sectionData] : sections) {
        jsonData[section] = sectionData;
    }
    
    return jsonData;
}

bool Config::fromJson(const json& jsonData) {
    clear();
    
    if (!jsonData.is_object()) {
        return false;
    }
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        if (it.key() == "__schema" || it.key() == "metadata") {
            continue;
        }
        
        if (it.value().is_object()) {
            // Handle section
            for (auto nestedIt = it.value().begin(); nestedIt != it.value().end(); ++nestedIt) {
                setNestedValueFromJson(it.key(), nestedIt.key(), nestedIt.value(), ConfigSource::File);
            }
        } else {
            setNestedValueFromJson("", it.key(), it.value(), ConfigSource::File);
        }
    }
    
    return true;
}

#ifdef HAS_YAML
bool Config::loadFromYaml(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        return fromYaml(yamlData);
    } catch (const YAML::Exception& e) {
        return false;
    }
}

bool Config::loadFromYaml(const YAML::Node& yamlData) {
    clear();
    
    if (yamlData.IsMap()) {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            if (it->first.IsScalar() && (it->first.as<std::string>() == "__schema" || 
                                         it->first.as<std::string>() == "metadata")) {
                continue;
            }
            
            std::string section = "";
            if (it->second.IsMap()) {
                for (auto nestedIt = it->second.begin(); nestedIt != it->second.end(); ++nestedIt) {
                    section = it->first.as<std::string>();
                    setNestedValueFromYaml(section, nestedIt->first.as<std::string>(), nestedIt->second, ConfigSource::File);
                }
            } else {
                setNestedValueFromYaml("", it->first.as<std::string>(), *it, ConfigSource::File);
            }
        }
    }
    
    return true;
}
#endif

ConfigValue Config::jsonToConfigValue(const json& value) {
    if (value.is_null()) {
        return std::string("");
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_array()) {
        // Determine array type
        if (!value.empty()) {
            if (value[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (value[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else {
                std::vector<std::string> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int>{}; // Empty int vector
    }
    
    return std::string("");
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json {
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
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(arg);
        }
        return json();
    }, value);
}

std::vector<int> Config::parseIntVector(const std::string& str) {
    std::vector<int> result;
    std::string content = str.substr(1, str.size() - 2); // Remove brackets
    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(std::stoi(item));
        }
    }
    return result;
}

std::vector<double> Config::parseDoubleVector(const std::string& str) {
    std::vector<double> result;
    std::string content = str.substr(1, str.size() - 2); // Remove brackets
    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(std::stod(item));
        }
    }
    return result;
}

std::vector<std::string> Config::parseStringVector(const std::string& str) {
    std::vector<std::string> result;
    std::string content = str.substr(1, str.size() - 2); // Remove brackets
    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        // Remove quotes
        if ((item.front() == '\"' && item.back() == '\"') ||
            (item.front() == '\'' && item.back() == '\'')) {
            item = item.substr(1, item.size() - 2);
        }
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

ConfigValue Config::parseLegacyValue(const std::string& value) {
    // Try different types in order
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            return int64Val;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Check if it's a vector
    if (value.find('[') != std::string::npos && value.find(']') != std::string::npos) {
        return parseVectorValue(value);
    }
    
    // Default to string
    return value;
}

ConfigValue Config::parseCommandLineValue(const std::string& value) {
    // Similar to legacy but also handle booleans
    if (value == "true" || value == "false" || 
        value == "yes" || value == "no" ||
        value == "y" || value == "n" ||
        value == "1" || value == "0") {
        
        std::string lowerValue = toLower(value);
        if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "y" || lowerValue == "1") {
            return true;
        } else {
            return false;
        }
    }
    
    return parseLegacyValue(value);
}

ConfigValue Config::parseVectorValue(const std::string& str) {
    if (str.front() == '[' && str.back() == ']') {
        // Try to parse as int vector
        try {
            return parseIntVector(str);
        } catch (...) {}
        
        try {
            return parseDoubleVector(str);
        } catch (...) {}
        
        try {
            return parseStringVector(str);
        } catch (...) {}
    }
    
    return str; // Fallback to string
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::validateValueInternal(const std::string& key, const ConfigValue& value) {
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt == pImpl->schemas.end()) {
        return true; // No schema defined, skip validation
    }
    
    const ConfigSchema& schema = schemaIt->second;
    return validateValue(value, schema);
}

bool Config::validateType(const ConfigValue& value, const std::string& type) const {
    std::string typeLower = toLower(type);
    
    if (typeLower == "int") {
        return std::holds_alternative<int>(value);
    } else if (typeLower == "int64") {
        return std::holds_alternative<int64_t>(value);
    } else if (typeLower == "double") {
        return std::holds_alternative<double>(value);
    } else if (typeLower == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (typeLower == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (typeLower == "int[]") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (typeLower == "double[]") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (typeLower == "string[]") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return false;
}

bool Config::validateRange(const ConfigValue& value, const ConfigSchema& schema) const {
    // Implementation for range validation
    return true;
}

bool Config::validatePattern(const ConfigValue& value, const std::string& pattern) const {
    std::string strValue;
    
    std::visit([&strValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            strValue = arg;
        } else {
            strValue = std::to_string(arg);
        }
    }, value);
    
    try {
        std::regex regex(pattern);
        return std::regex_match(strValue, regex);
    } catch (...) {
        return false;
    }
}

void Config::setNestedValueFromJson(const std::string& section, const std::string& key, 
                                   const json& jsonValue, ConfigSource source) {
    ConfigValue configValue = jsonToConfigValue(jsonValue);
    set(section, key, configValue, source);
}

#ifdef HAS_YAML
void Config::setNestedValueFromYaml(const std::string& section, const std::string& key,
                                   const YAML::Node& yamlValue, ConfigSource source) {
    ConfigValue configValue = yamlToConfigValue(yamlValue);
    set(section, key, configValue, source);
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& value) {
    if (!value) {
        return std::string("");
    } else if (value.IsScalar()) {
        const std::string& str = value.as<std::string>();
        if (str == "true" || str == "false") {
            return value.as<bool>();
        }
        
        try {
            return value.as<int>();
        } catch (...) {}
        
        try {
            return value.as<double>();
        } catch (...) {}
        
        return str;
    } else if (value.IsSequence()) {
        if (value.size() == 0) {
            return std::vector<int>{};
        }
        
        // Determine array type
        if (value[0].IsScalar()) {
            const std::string& firstStr = value[0].as<std::string>();
            if (firstStr == "true" || firstStr == "false") {
                std::vector<bool> vec;
                for (const auto& item : value) {
                    vec.push_back(item.as<bool>());
                }
                return vec;
            }
            
            try {
                std::vector<int> vec;
                for (const auto& item : value) {
                    vec.push_back(item.as<int>());
                }
                return vec;
            } catch (...) {}
            
            try {
                std::vector<double> vec;
                for (const auto& item : value) {
                    vec.push_back(item.as<double>());
                }
                return vec;
            } catch (...) {}
            
            std::vector<std::string> vec;
            for (const auto& item : value) {
                vec.push_back(item.as<std::string>());
            }
            return vec;
        }
        
        return std::vector<int>{};
    } else if (value.IsMap()) {
        // Convert YAML map to JSON string representation
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : value) {
            if (!first) ss << ", ";
            ss << "\"" << pair.first.as<std::string>() << "\": " << pair.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    return std::string("");
}
#endif

// Explicit template instantiations
namespace nlm {
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
    
    template std::optional<int> Config::get<int>(const std::string&, const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&, const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&, const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&, const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&, const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&, const std::string&) const;
}

// NOTE: This implementation requires nlohmann/json library (header-only)
// YAML support is optional and requires yaml-cpp library
// The implementation maintains backward compatibility with the legacy key=value format
// while adding support for modern configuration formats with sections, inheritance, and validation.