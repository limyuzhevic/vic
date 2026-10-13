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

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Detect format from file extension
        ConfigFormat format = detectFormatFromExtension(filepath);
        
        // Read file content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Try to detect actual format if unknown
        if (format == ConfigFormat::Unknown) {
            format = detectFormatFromContent(content);
        }
        
        // Load based on detected format
        switch (format) {
            case ConfigFormat::JSON:
                return loadFromJSON(content);
            case ConfigFormat::YAML:
                return loadFromYAML(content);
            case ConfigFormat::KeyValue:
                return loadLegacyKeyValueFormat(content);
            case ConfigFormat::Unknown:
            default:
                return false;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromString(const std::string& content, ConfigFormat format) {
    if (format == ConfigFormat::Unknown) {
        format = detectFormatFromContent(content);
    }
    
    clear();  // Clear existing entries
    
    switch (format) {
        case ConfigFormat::JSON:
            return loadFromJSON(content);
        case ConfigFormat::YAML:
            return loadFromYAML(content);
        case ConfigFormat::KeyValue:
            return loadLegacyKeyValueFormat(content);
        case ConfigFormat::Unknown:
        default:
            return false;
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        clear();  // Clear existing entries
        
        auto json = nlohmann::json::parse(jsonString);
        
        // Handle both object and array formats
        if (json.is_object()) {
            jsonToConfig(json, *this);
        } else if (json.is_array()) {
            // For array format, use index-based keys
            for (size_t i = 0; i < json.size(); ++i) {
                std::string key = "item_" + std::to_string(i);
                ConfigValue value = jsonToVariant(json[i]);
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Try legacy format as fallback
        return loadLegacyKeyValueFormat(jsonString);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        clear();  // Clear existing entries
        
        auto yaml = YAML::Load(yamlString);
        
        // Convert YAML to Config
        yamlToConfig(yaml, *this);
        
        return true;
    } catch (const YAML::ParserException& e) {
        // Try legacy format as fallback
        return loadLegacyKeyValueFormat(yamlString);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadLegacyKeyValueFormat(const std::string& content) {
    clear();  // Clear existing entries
    
    std::stringstream stream(content);
    std::string line;
    bool hasEntries = false;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse legacy key=value pairs
        std::string key, value;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            key = trim(line.substr(0, pos));
            value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            if (tryConvertValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
                hasEntries = true;
            }
        }
    }
    
    return hasEntries;
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
    // Detect format from file extension
    ConfigFormat format = detectFormatFromExtension(filepath);
    
    if (format == ConfigFormat::JSON) {
        return saveToJSON(filepath);
    } else if (format == ConfigFormat::YAML || format == ConfigFormat::YML) {
        return saveToYAML(filepath);
    } else {
        return saveToKeyValue(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto json = configToJson(*this);
        
        // Write with pretty formatting
        file << json.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto yaml = configToYaml(*this);
        
        // Write YAML
        file << yaml;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "# NLM Configuration File\n";
        file << "# Format: key = value\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toString(entry.value) << "\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = validateConfigEntry(entry);
        if (!entryResult.isValid) {
            result.isValid = false;
            result.errorMessage = "Invalid config entry: " + entry.key + " - " + entryResult.errorMessage;
        }
        
        // Add warnings
        for (const auto& warning : entryResult.warnings) {
            result.warnings.push_back(entry.key + ": " + warning);
        }
    }
    
    return result;
}

ValidationResult Config::validateConfigEntry(const ConfigEntry& entry) const {
    ValidationResult result;
    
    // Check if key is valid
    if (entry.key.empty()) {
        return ValidationResult::Error("Key cannot be empty");
    }
    
    // Check for duplicate keys (only within same source)
    int sameSourceCount = 0;
    for (const auto& e : pImpl->entries) {
        if (e.key == entry.key && e.source == entry.source) {
            sameSourceCount++;
        }
    }
    
    if (sameSourceCount > 1) {
        return ValidationResult::Error("Duplicate key \"" + entry.key + "\" in same source");
    }
    
    // Validate type compatibility
    ValidationResult typeResult = validateTypeCompatibility(entry.key, entry.value);
    if (!typeResult.isValid) {
        return typeResult;
    }
    
    return ValidationResult::Success();
}

ValidationResult Config::validateTypeCompatibility(const std::string& key, const ConfigValue& value) const {
    // Simple validation - could be extended with more sophisticated rules
    std::string lowerKey = toLower(key);
    
    // Example: keys containing "seed" should be integers
    if (lowerKey.find("seed") != std::string::npos) {
        if (!std::holds_alternative<int64_t>(value)) {
            return ValidationResult::Error("Key \"" + key + "\" should be an integer (seed)");
        }
    }
    
    // Example: keys containing "rate" or "ratio" should be doubles
    if (lowerKey.find("rate") != std::string::npos || lowerKey.find("ratio") != std::string::npos) {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
            return ValidationResult::Error("Key \"" + key + "\" should be a floating point number");
        }
    }
    
    // Example: keys containing "count" or "number" should be integers
    if (lowerKey.find("count") != std::string::npos || lowerKey.find("number") != std::string::npos) {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            return ValidationResult::Error("Key \"" + key + "\" should be an integer");
        }
    }
    
    // Example: keys containing "enabled" or "active" should be boolean
    if (lowerKey.find("enabled") != std::string::npos || lowerKey.find("active") != std::string::npos) {
        if (!std::holds_alternative<bool>(value)) {
            return ValidationResult::Error("Key \"" + key + "\" should be a boolean");
        }
    }
    
    return ValidationResult::Success();
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
    oss << "Configuration (" << pImpl->entries.size() << " entries, format: ";
    
    switch (pImpl->format) {
        case ConfigFormat::JSON:
            oss << "JSON";
            break;
        case ConfigFormat::YAML:
            oss << "YAML";
            break;
        case ConfigFormat::KeyValue:
            oss << "key=value (legacy)";
            break;
        default:
            oss << "unknown";
    }
    oss << "):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << std::boolalpha << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
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
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

// Helper implementations
ConfigValue Config::fromString(const std::string& str, ConfigFormat format) {
    ConfigValue value;
    if (tryConvertValue(str, value)) {
        return value;
    }
    return str;  // Return as string if conversion fails
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
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
        }
    }, value);
    return oss.str();
}

bool Config::tryConvertValue(const std::string& str, ConfigValue& value) {
    // Trim the string
    std::string trimmed = trim(str);
    
    // Try boolean
    if (trimmed == "true" || trimmed == "false") {
        value = (trimmed == "true");
        return true;
    }
    
    // Try integer (including int64_t)
    try {
        size_t pos;
        int64_t intVal = std::stoll(trimmed, &pos);
        if (pos == trimmed.length()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try unsigned integer
    try {
        size_t pos;
        uint64_t uintVal = std::stoull(trimmed, &pos);
        if (pos == trimmed.length()) {
            value = static_cast<int64_t>(uintVal); // Use int64_t since that's in ConfigValue
            return true;
        }
    } catch (...) {}
    
    // Try floating point
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.length()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try vectors
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        // Parse vector-like string
        std::string content = trimmed.substr(1, trimmed.length() - 2);
        std::stringstream vectorStream(content);
        std::string item;
        std::vector<std::string> items;
        
        while (std::getline(vectorStream, item, ',')) {
            items.push_back(trim(item));
        }
        
        if (!items.empty()) {
            // Try to determine vector type
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& item : items) {
                try { std::stoll(item); } catch (...) { allInts = false; }
                try { std::stod(item); } catch (...) { allDoubles = false; }
                // Strings are always valid
            }
            
            if (allInts) {
                std::vector<int64_t> intVector;
                for (const auto& item : items) {
                    intVector.push_back(std::stoll(item));
                }
                value = intVector;
                return true;
            } else if (allDoubles) {
                std::vector<double> doubleVector;
                for (const auto& item : items) {
                    doubleVector.push_back(std::stod(item));
                }
                value = doubleVector;
                return true;
            } else {
                value = items;
                return true;
            }
        }
    }
    
    // Default to string
    value = trimmed;
    return true;
}

std::string Config::getDefaultDescription(const std::string& key) {
    std::string desc = "Configuration value for key: " + key;
    
    // Add common descriptions
    std::string lowerKey = toLower(key);
    if (lowerKey.find("seed") != std::string::npos) {
        return "Random seed for reproducible experiments";
    } else if (lowerKey.find("timestep") != std::string::npos) {
        return "Simulation timestep in seconds";
    } else if (lowerKey.find("count") != std::string::npos || lowerKey.find("number") != std::string::npos) {
        return "Number of items";
    } else if (lowerKey.find("probability") != std::string::npos) {
        return "Connection probability";
    } else if (lowerKey.find("rate") != std::string::npos) {
        return "Rate parameter";
    } else if (lowerKey.find("enabled") != std::string::npos || lowerKey.find("active") != std::string::npos) {
        return "Whether the feature is enabled";
    }
    
    return desc;
}

std::string Config::getFormatErrorMessage(ConfigFormat format, const std::string& filepath) {
    switch (format) {
        case ConfigFormat::JSON:
            return "Failed to parse JSON configuration file: " + filepath;
        case ConfigFormat::YAML:
            return "Failed to parse YAML configuration file: " + filepath;
        case ConfigFormat::KeyValue:
            return "Failed to parse key=value configuration file: " + filepath;
        default:
            return "Failed to parse configuration file: " + filepath;
    }
}

nlohmann::json Config::configToJson(const Config& config) {
    nlohmann::json json = nlohmann::json::object();
    
    for (const auto& entry : config.pImpl->entries) {
        json[entry.key] = Config::variantToJson(entry.value);
        // Add metadata if needed
        if (!entry.description.empty()) {
            json[entry.key]["__description"] = entry.description;
        }
    }
    
    return json;
}

YAML::Node Config::configToYaml(const Config& config) {
    YAML::Node yaml = YAML::Node(YAML::NodeType::Map);
    
    for (const auto& entry : config.pImpl->entries) {
        yaml[entry.key] = Config::variantToYaml(entry.value);
        // Add metadata if needed
        if (!entry.description.empty()) {
            yaml[entry.key]["__description"] = entry.description;
        }
    }
    
    return yaml;
}

void Config::jsonToConfig(const nlohmann::json& json, Config& config) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        
        // Skip internal metadata fields
        if (key.substr(0, 2) == "__") {
            continue;
        }
        
        ConfigValue value = jsonToVariant(*it);
        std::string description = it->contains("__description") ? (*it)["__description"] : getDefaultDescription(key);
        
        config.set(key, value, ConfigSource::File);
    }
}

void Config::yamlToConfig(const YAML::Node& yaml, Config& config) {
    for (const auto& it : yaml) {
        std::string key = it.first.as<std::string>();
        
        // Skip internal metadata fields
        if (key.substr(0, 2) == "__") {
            continue;\n        }
        
        ConfigValue value = yamlToVariant(it.second);
        std::string description = it.second["__description"].IsDefined() ? 
            it.second["__description"].as<std::string>() : getDefaultDescription(key);
        
        config.set(key, value, ConfigSource::File);
    }
}

nlohmann::json Config::variantToJson(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

YAML::Node Config::variantToYaml(const ConfigValue& value) {
    YAML::Node node;
    
    std::visit([&node](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
    
    return node;
}

ConfigValue Config::jsonToVariant(const nlohmann::json& json) {
    ConfigValue value;
    
    if (json.is_string()) {
        value = json.get<std::string>();
    } else if (json.is_number_integer()) {
        value = json.get<int64_t>();
    } else if (json.is_number()) {
        value = json.get<double>();
    } else if (json.is_boolean()) {
        value = json.get<bool>();
    } else if (json.is_array()) {
        // Determine vector type
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int64_t> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<int64_t>());
                }
                value = vec;
            } else if (json[0].is_number()) {
                std::vector<double> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                value = vec;
            } else {
                std::vector<std::string> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                value = vec;
            }
        } else {
            value = std::vector<std::string>();  // Empty vector
        }
    }
    
    return value;
}

ConfigValue Config::yamlToVariant(const YAML::Node& yaml) {
    ConfigValue value;
    
    if (yaml.IsScalar()) {
        std::string strValue = yaml.as<std::string>();
        
        // Try to convert to appropriate type
        if (strValue == "true" || strValue == "false") {
            value = (strValue == "true");
        } else {
            try {
                size_t pos;
                int64_t intVal = std::stoll(strValue, &pos);
                if (pos == strValue.length()) {
                    value = intVal;
                } else {
                    double doubleVal = std::stod(strValue, &pos);
                    if (pos == strValue.length()) {
                        value = doubleVal;
                    } else {
                        value = strValue;  // Keep as string
                    }
                }
            } catch (...) {
                value = strValue;  // Keep as string
            }
        }
    } else if (yaml.IsSequence()) {
        // Vector type - determine based on first element
        if (yaml.size() > 0) {
            if (yaml[0].IsScalar()) {
                std::string firstVal = yaml[0].as<std::string>();
                
                // Try to determine vector type
                bool allInts = true;
                bool allDoubles = true;
                
                for (size_t i = 0; i < yaml.size(); ++i) {
                    std::string val = yaml[i].as<std::string>();
                    try { std::stoll(val); } catch (...) { allInts = false; }
                    try { std::stod(val); } catch (...) { allDoubles = false; }
                }
                
                if (allInts) {
                    std::vector<int64_t> vec;
                    for (size_t i = 0; i < yaml.size(); ++i) {
                        vec.push_back(yaml[i].as<int64_t>());
                    }
                    value = vec;
                } else if (allDoubles) {
                    std::vector<double> vec;
                    for (size_t i = 0; i < yaml.size(); ++i) {
                        vec.push_back(yaml[i].as<double>());
                    }
                    value = vec;
                } else {
                    std::vector<std::string> vec;
                    for (size_t i = 0; i < yaml.size(); ++i) {
                        vec.push_back(yaml[i].as<std::string>());
                    }
                    value = vec;
                }
            }
        } else {
            value = std::vector<std::string>();  // Empty vector
        }
    }
    
    return value;
}

void Config::merge(const Config& other, bool overwriteExisting) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwriteExisting || !has(entry.key)) {
            // Note: We don't preserve the source of the other config's entries
            // They should be marked as File since they're being merged
            set(entry.key, entry.value, ConfigSource::File);
            
            // Try to preserve description if available
            // This would need a modify function in a full implementation
        }
    }
}

std::string Config::getConfigPath(const std::string& filename) {
    std::string configDir = "configs";
    return configDir + "/" + filename;
}

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
}

} // namespace nlm
