#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cctype>
#include <unordered_map>

namespace nlm {

using json = nlohmann::json;

// Parser implementations
struct JsonConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override {
        if (content.empty()) return false;
        
        // Try to parse as JSON
        try {
            json::parse(content);
            return true;
        } catch (const json::parse_error&) {
            return false;
        }
    }
    
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override {
        try {
            auto jsonData = json::parse(content);
            entries.clear();
            
            // Recursive function to parse JSON objects
            std::function<void(const json&, const std::string&)> parseJsonObject = 
                [&](const json& j, const std::string& prefix) {
                    for (auto it = j.begin(); it != j.end(); ++it) {
                        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
                        
                        if (it.value().is_object()) {
                            // Nested object - recursively parse
                            parseJsonObject(it.value(), key);
                        } else {
                            // Leaf node - convert to ConfigValue
                            ConfigValue value;
                            std::string type;
                            
                            if (it.value().is_string()) {
                                value = it.value().get<std::string>();
                                type = "string";
                            } else if (it.value().is_number_integer()) {
                                value = it.value().get<int64_t>();
                                type = "int64";
                            } else if (it.value().is_number_float()) {
                                value = it.value().get<double>();
                                type = "double";
                            } else if (it.value().is_boolean()) {
                                value = it.value().get<bool>();
                                type = "bool";
                            } else if (it.value().is_array()) {
                                // Handle arrays
                                const auto& arr = it.value();
                                std::vector<ConfigValue> vec;
                                std::string arrayType = "string";
                                
                                for (const auto& item : arr) {
                                    if (item.is_string()) {
                                        vec.push_back(item.get<std::string>());
                                        arrayType = "string";
                                    } else if (item.is_number_integer()) {
                                        vec.push_back(item.get<int64_t>());
                                        arrayType = "int64";
                                    } else if (item.is_number_float()) {
                                        vec.push_back(item.get<double>());
                                        arrayType = "double";
                                    } else if (item.is_boolean()) {
                                        vec.push_back(item.get<bool>());
                                        arrayType = "bool";
                                    }
                                }
                                
                                // Convert to appropriate vector type
                                if (arrayType == "string") {
                                    std::vector<std::string> strVec;
                                    strVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        strVec.push_back(std::get<std::string>(v));
                                    }
                                    value = strVec;
                                } else if (arrayType == "int64") {
                                    std::vector<int64_t> intVec;
                                    intVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        intVec.push_back(std::get<int64_t>(v));
                                    }
                                    value = intVec;
                                } else if (arrayType == "double") {
                                    std::vector<double> doubleVec;
                                    doubleVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        doubleVec.push_back(std::get<double>(v));
                                    }
                                    value = doubleVec;
                                } else if (arrayType == "bool") {
                                    std::vector<bool> boolVec;
                                    boolVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        boolVec.push_back(std::get<bool>(v));
                                    }
                                    value = boolVec;
                                }
                                
                                type = arrayType + "_array";
                            }
                            
                            // Create ConfigEntry
                            entries.emplace_back(key, value, ConfigSource::File, 
                                              "", type);
                        }
                    }
                };
            
            parseJsonObject(jsonData, "");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::string serialize(const std::vector<ConfigEntry>& entries) const override {
        json jsonData;
        
        // Convert entries to JSON
        for (const auto& entry : entries) {
            std::string currentKey = entry.key;
            json* current = &jsonData;
            
            // Handle nested keys (e.g., "section.subsection.key")
            size_t dotPos = currentKey.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = currentKey.substr(0, dotPos);
                currentKey = currentKey.substr(dotPos + 1);
                
                if (!(*current).contains(segment)) {
                    (*current)[segment] = json::object();
                }
                current = &((*current)[segment]);
                dotPos = currentKey.find('.');
            }
            
            // Set the value
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    (*current)[currentKey] = val;
                }
            }, entry.value);
        }
        
        std::ostringstream oss;
        oss << std::setw(2) << jsonData;
        return oss.str();
    }
};

struct YamlConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override {
        if (content.empty()) return false;
        
        // Try to parse as YAML
        try {
            YAML::Load(content);
            return true;
        } catch (const YAML::ParserException&) {
            return false;
        }
    }
    
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override {
        try {
            auto yamlData = YAML::Load(content);
            entries.clear();
            
            // Recursive function to parse YAML nodes
            std::function<void(const YAML::Node&, const std::string&)> parseYamlNode =
                [&](const YAML::Node& node, const std::string& prefix) {
                    for (const auto& item : node) {
                        std::string key = item.first.IsScalar() ? 
                            (prefix.empty() ? item.first.as<std::string>() : 
                             prefix + "." + item.first.as<std::string>()) : 
                            prefix;
                        
                        if (item.second.IsMap()) {
                            // Nested object - recursively parse
                            parseYamlNode(item.second, key);
                        } else {
                            // Leaf node - convert to ConfigValue
                            ConfigValue value;
                            std::string type;
                            
                            if (item.second.IsScalar()) {
                                const std::string scalar = item.second.as<std::string>();
                                
                                // Try to determine type from string
                                try {
                                    // Try boolean first
                                    if (scalar == "true" || scalar == "false") {
                                        value = (scalar == "true");
                                        type = "bool";
                                    }
                                    // Try integer
                                    else if (std::regex_match(scalar, std::regex("^-?\d+$"))) {
                                        value = std::stoll(scalar);
                                        type = "int64";
                                    }
                                    // Try double
                                    else if (std::regex_match(scalar, std::regex("^-?\d+\.?\d*$"))) {
                                        value = std::stod(scalar);
                                        type = "double";
                                    }
                                    else {
                                        // Default to string
                                        value = scalar;
                                        type = "string";
                                    }
                                } catch (...) {
                                    value = scalar;
                                    type = "string";
                                }
                            } else if (item.second.IsSequence()) {
                                // Handle sequences
                                const auto& seq = item.second;
                                std::vector<ConfigValue> vec;
                                std::string arrayType = "string";
                                
                                for (const auto& item : seq) {
                                    if (item.IsScalar()) {
                                        const std::string scalar = item.as<std::string>();
                                        try {
                                            if (scalar == "true" || scalar == "false") {
                                                vec.push_back(scalar == "true");
                                                arrayType = "bool";
                                            } else if (std::regex_match(scalar, std::regex("^-?\d+$"))) {
                                                vec.push_back(std::stoll(scalar));
                                                arrayType = "int64";
                                            } else if (std::regex_match(scalar, std::regex("^-?\d+\.?\d*$"))) {
                                                vec.push_back(std::stod(scalar));
                                                arrayType = "double";
                                            } else {
                                                vec.push_back(scalar);
                                                arrayType = "string";
                                            }
                                        } catch (...) {
                                            vec.push_back(scalar);
                                            arrayType = "string";
                                        }
                                    }
                                }
                                
                                // Convert to appropriate vector type
                                if (arrayType == "string") {
                                    std::vector<std::string> strVec;
                                    strVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        strVec.push_back(std::get<std::string>(v));
                                    }
                                    value = strVec;
                                } else if (arrayType == "int64") {
                                    std::vector<int64_t> intVec;
                                    intVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        intVec.push_back(std::get<int64_t>(v));
                                    }
                                    value = intVec;
                                } else if (arrayType == "double") {
                                    std::vector<double> doubleVec;
                                    doubleVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        doubleVec.push_back(std::get<double>(v));
                                    }
                                    value = doubleVec;
                                } else if (arrayType == "bool") {
                                    std::vector<bool> boolVec;
                                    boolVec.reserve(vec.size());
                                    for (const auto& v : vec) {
                                        boolVec.push_back(std::get<bool>(v));
                                    }
                                    value = boolVec;
                                }
                                
                                type = arrayType + "_array";
                            }
                            
                            // Create ConfigEntry
                            entries.emplace_back(key, value, ConfigSource::File, 
                                              "", type);
                        }
                    }
                };
            
            parseYamlNode(yamlData, "");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::string serialize(const std::vector<ConfigEntry>& entries) const override {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Group entries by prefix to create nested structure
        std::map<std::string, YAML::Node> yamlMap;
        
        for (const auto& entry : entries) {
            std::string currentKey = entry.key;
            YAML::Node* current = &yamlMap;
            
            // Handle nested keys (e.g., "section.subsection.key")
            size_t dotPos = currentKey.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = currentKey.substr(0, dotPos);
                currentKey = currentKey.substr(dotPos + 1);
                
                if (!(*current)[segment]) {
                    (*current)[segment] = YAML::Node(YAML::NodeType::Map);
                }
                current = &((*current)[segment]);
                dotPos = currentKey.find('.');
            }
            
            // Set the value
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    (*current)[currentKey] = static_cast<long long>(val);
                } else if constexpr (std::is_same_v<T, double>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    (*current)[currentKey] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    YAML::Node seq;
                    for (const auto& item : val) {
                        seq.push_back(static_cast<long long>(item));
                    }
                    (*current)[currentKey] = seq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    YAML::Node seq;
                    for (const auto& item : val) {
                        seq.push_back(item);
                    }
                    (*current)[currentKey] = seq;
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    YAML::Node seq;
                    for (const auto& item : val) {
                        seq.push_back(item);
                    }
                    (*current)[currentKey] = seq;
                }
            }, entry.value);
        }
        
        // Emit the YAML map
        for (const auto& pair : yamlMap) {
            emitter << YAML::Key << pair.first;
            if (pair.second.IsMap()) {
                emitter << YAML::BeginMap;
                for (const auto& innerPair : pair.second) {
                    emitter << YAML::Key << innerPair.first.as<std::string>()
                            << YAML::Value << innerPair.second;
                }
                emitter << YAML::EndMap;
            } else {
                emitter << YAML::Value << pair.second;
            }
        }
        
        emitter << YAML::EndMap;
        return emitter.str();
    }
};

struct KeyValueConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override {
        // Simple heuristic: if it contains key=value pairs, treat as key-value format
        std::istringstream stream(content);
        std::string line;
        int keyValuePairs = 0;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/' || 
                line.find('=') == std::string::npos) {
                continue;
            }
            
            // Basic key=value validation
            size_t pos = line.find('=');
            if (pos > 0 && pos < line.size() - 1) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                if (!key.empty()) {
                    keyValuePairs++;
                }
            }
            
            // If we have enough key-value pairs, assume it's key-value format
            if (keyValuePairs >= 2) {
                return true;
            }
        }
        
        return false;
    }
    
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override {
        entries.clear();
        
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (key.empty()) {
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Determine type
                std::string type = "string";
                ConfigValue configValue;
                
                // Try to parse as boolean
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                    type = "bool";
                }
                // Try to parse as integer
                else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                    try {
                        configValue = std::stoll(value);
                        type = "int64";
                    } catch (...) {
                        configValue = value;
                    }
                }
                // Try to parse as double
                else if (std::regex_match(value, std::regex("^-?\d+\.?\d*$"))) {
                    try {
                        configValue = std::stod(value);
                        type = "double";
                    } catch (...) {
                        configValue = value;
                    }
                }
                // Try to parse as array
                else if (value.find('[') == 0 && value.back() == ']') {
                    std::string arrayContent = value.substr(1, value.size() - 2);
                    std::vector<std::string> tokens;
                    std::string token;
                    std::istringstream tokenStream(arrayContent);
                    
                    while (std::getline(tokenStream, token, ',')) {
                        token = trim(token);
                        if (!token.empty()) {
                            tokens.push_back(token);
                        }
                    }
                    
                    if (!tokens.empty()) {
                        // Try to determine array type
                        bool allInts = true;
                        bool allDoubles = true;
                        bool allBools = true;
                        
                        for (const auto& token : tokens) {
                            if (std::regex_match(token, std::regex("^-?\d+$"))) {
                                allBools = false;
                                allDoubles = false;
                            } else if (std::regex_match(token, std::regex("^-?\d+\.?\d*$"))) {
                                allInts = false;
                                allBools = false;
                            } else if (token == "true" || token == "false") {
                                allInts = false;
                                allDoubles = false;
                            } else {
                                allInts = false;
                                allDoubles = false;
                                allBools = false;
                            }
                        }
                        
                        if (allInts) {
                            std::vector<int64_t> intVec;
                            for (const auto& token : tokens) {
                                try {
                                    intVec.push_back(std::stoll(token));
                                } catch (...) {
                                    allInts = false;
                                    break;
                                }
                            }
                            if (allInts) {
                                configValue = intVec;
                                type = "int64_array";
                            }
                        } else if (allDoubles) {
                            std::vector<double> doubleVec;
                            for (const auto& token : tokens) {
                                try {
                                    doubleVec.push_back(std::stod(token));
                                } catch (...) {
                                    allDoubles = false;
                                    break;
                                }
                            }
                            if (allDoubles) {
                                configValue = doubleVec;
                                type = "double_array";
                            }
                        } else if (allBools) {
                            std::vector<bool> boolVec;
                            for (const auto& token : tokens) {
                                boolVec.push_back(token == "true");
                            }
                            configValue = boolVec;
                            type = "bool_array";
                        } else {
                            // Default to string array
                            std::vector<std::string> strVec;
                            for (const auto& token : tokens) {
                                // Remove quotes if present
                                std::string str = token;
                                if ((str.front() == '"' && str.back() == '"') ||
                                    (str.front() == '\'' && str.back() == '\'')) {
                                    str = str.substr(1, str.size() - 2);
                                }
                                strVec.push_back(str);
                            }
                            configValue = strVec;
                            type = "string_array";
                        }
                    }
                } else {
                    configValue = value;
                    type = "string";
                }
                
                entries.emplace_back(key, configValue, ConfigSource::File, "", type);
            }
        }
        
        return !entries.empty();
    }
    
    std::string serialize(const std::vector<ConfigEntry>& entries) const override {
        std::ostringstream oss;
        
        for (const auto& entry : entries) {
            oss << "# " << entry.description << "\n";
            oss << entry.key << " = " << configValueToString(entry.value) << "\n";
        }
        
        return oss.str();
    }
};

// Global parser instances
static std::vector<std::unique_ptr<IConfigParser>> getParsers() {
    static std::vector<std::unique_ptr<IConfigParser>> parsers;
    if (parsers.empty()) {
        parsers.push_back(std::make_unique<JsonConfigParser>());
        parsers.push_back(std::make_unique<YamlConfigParser>());
        parsers.push_back(std::make_unique<KeyValueConfigParser>());
    }
    return parsers;
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    std::string currentFormat;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::Config(const ConfigSchema& schema) : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = schema;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFormatFromExtension(filepath);
    return loadFromFile(filepath, format);
}

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    try {
        // Clear existing entries
        clear();
        
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            throw ConfigError(ConfigError::ErrorType::FileNotFound, 
                            "Configuration file not found: " + filepath);
        }
        
        // Get file modification time
        auto modTime = getFileModificationTime(filepath);
        
        // Read file content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError(ConfigError::ErrorType::IoError,
                            "Cannot open configuration file: " + filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        // Try to parse with specified format or auto-detect
        return loadFromString(content, format.empty() ? "auto" : format);
    } catch (const ConfigError& e) {
        // Re-throw ConfigError as-is
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorType::ParseError,
                        "Failed to load configuration from file: " + std::string(e.what()),
                        "", filepath);
    }
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    try {
        // Clear existing entries
        clear();
        
        if (content.empty()) {
            return true; // Empty content is valid
        }
        
        // Auto-detect format if specified as "auto"
        std::string actualFormat = format;
        if (format == "auto") {
            actualFormat = detectFormat(content);
        }
        
        pImpl->currentFormat = actualFormat;
        
        // Try to parse with available parsers
        auto parsers = getParsers();
        for (const auto& parser : parsers) {
            if (actualFormat != "auto" && parser->getFormatName() != actualFormat) {
                continue;
            }
            
            if (parser->canParse(content)) {
                if (parser->parse(content, pImpl->entries)) {
                    // Validate entries against schema if present
                    if (pImpl->schema.validate(*this)) {
                        return true;
                    } else {
                        throw ConfigError(ConfigError::ErrorType::ValidationError,
                                        "Configuration validation failed");
                    }
                } else {
                    throw ConfigError(ConfigError::ErrorType::ParseError,
                                    "Failed to parse configuration content");
                }
            }
        }
        
        throw ConfigError(ConfigError::ErrorType::UnsupportedFormat,
                        "Unsupported configuration format");
    } catch (const ConfigError& e) {
        // Re-throw ConfigError as-is
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorType::ParseError,
                        "Failed to parse configuration: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        // Clear existing entries except those from Default source
        auto defaultEntries = getEntriesBySource(ConfigSource::Default);
        clear();
        
        // Restore default entries
        for (const auto& entry : defaultEntries) {
            pImpl->entries.push_back(entry);
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    // Handle --key without value (treat as empty string)
                    set(arg.substr(2), std::string(""), ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
            // Handle --key (standalone boolean flag)
            else if (arg[0] == '-' && arg.size() > 1) {
                std::string key = arg.substr(1);
                set(key, "true", ConfigSource::CommandLine);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromEnvironment(const std::map<std::string, std::string>& envVars) {
    try {
        for (const auto& pair : envVars) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            // Convert environment variable name to key format
            std::string configKey = key;
            // Replace _ with . for nested keys
            std::replace(configKey.begin(), configKey.end(), '_', '.');
            
            set(configKey, value, ConfigSource::Environment);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError(ConfigError::ErrorType::IoError,
                            "Cannot save configuration to file: " + filepath);
        }
        
        std::string actualFormat = format;
        if (format == "auto") {
            actualFormat = pImpl->currentFormat.empty() ? "KEY_VALUE" : pImpl->currentFormat;
        }
        
        // Use existing format or default to KEY_VALUE
        if (!saveWithFormat(pImpl->entries, actualFormat.empty() ? "KEY_VALUE" : actualFormat, file)) {
            throw ConfigError(ConfigError::ErrorType::IoError,
                            "Failed to save configuration to file");
        }
        
        return true;
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorType::IoError,
                        "Failed to save configuration: " + std::string(e.what()));
    }
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
    // Validate value against schema if present
    if (!pImpl->schema.getFieldType(key).empty()) {
        std::string type = pImpl->schema.getFieldType(key);
        if (!validateValue(value, type)) {
            throw ConfigError(ConfigError::ErrorType::TypeError,
                            "Invalid value type for key: " + key,
                            key);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
    } else {
        ConfigEntry entry(key, value, source);
        // Set default description if not present
        if (pImpl->schema.getFieldDescription(key) != "" && entry.description.empty()) {
            entry.description = pImpl->schema.getFieldDescription(key);
        }
        // Set type from schema if present
        std::string type = pImpl->schema.getFieldType(key);
        if (!type.empty()) {
            entry.type = type;
        }
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Determine type based on string content
    std::string type = "string";
    if (value == "true" || value == "false") {
        type = "bool";
    } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
        type = "int64";
    } else if (std::regex_match(value, std::regex("^-?\d+\.?\d*$"))) {
        type = "double";
    } else if (value.find('[') == 0 && value.back() == ']') {
        type = "array";
    }
    
    ConfigValue configValue;
    
    // Parse value based on type
    if (type == "bool") {
        configValue = (value == "true");
    } else if (type == "int64") {
        try {
            configValue = std::stoll(value);
        } catch (...) {
            configValue = value;
        }
    } else if (type == "double") {
        try {
            configValue = std::stod(value);
        } catch (...) {
            configValue = value;
        }
    } else if (type == "array") {
        // Parse array content
        std::string arrayContent = value.substr(1, value.size() - 2);
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(arrayContent);
        
        while (std::getline(tokenStream, token, ',')) {
            token = trim(token);
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        std::vector<std::string> strVec;
        for (const auto& token : tokens) {
            // Remove quotes if present
            std::string str = token;
            if ((str.front() == '"' && str.back() == '"') ||
                (str.front() == '\'' && str.back() == '\'')) {
                str = str.substr(1, str.size() - 2);
            }
            strVec.push_back(str);
        }
        configValue = strVec;
    } else {
        configValue = value;
    }
    
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    std::vector<int64_t> intVec;
    intVec.reserve(value.size());
    for (const auto& item : value) {
        intVec.push_back(item);
    }
    set(key, intVec, source);
}

void Config::set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << (val[i] ? "true" : "false");
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << val[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

std::string Config::configValueToString(const ConfigValue& value) {
    return toString(value);
}

std::string Config::detectFormat(const std::string& content) {
    if (content.empty()) {
        return "KEY_VALUE";
    }
    
    auto parsers = getParsers();
    for (const auto& parser : parsers) {
        if (parser->canParse(content)) {
            return parser->getFormatName();
        }
    }
    
    return "KEY_VALUE";
}

std::string Config::detectFormatFromExtension(const std::string& filepath) {
    if (filepath.empty()) {
        return "KEY_VALUE";
    }
    
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return "JSON";
    } else if (ext == "yaml" || ext == "yml") {
        return "YAML";
    }
    
    return "KEY_VALUE";
}

bool Config::validateValue(const ConfigValue& value, const std::string& type) {
    if (type.empty()) {
        return true; // No type specified, validation passes
    }
    
    std::string normalizedType = toLower(type);
    
    auto validateVisitor = [normalizedType](const auto& val) -> bool {
        using T = std::decay_t<decltype(val)>;
        
        if (normalizedType == "int64") {
            return std::is_same_v<T, int64_t>;
        } else if (normalizedType == "double") {
            return std::is_same_v<T, double> || std::is_same_v<T, int64_t>;
        } else if (normalizedType == "bool") {
            return std::is_same_v<T, bool>;
        } else if (normalizedType == "string") {
            return std::is_same_v<T, std::string>;
        } else if (normalizedType == "int64_array") {
            return std::is_same_v<T, std::vector<int64_t>>;
        } else if (normalizedType == "double_array") {
            return std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<int64_t>>;
        } else if (normalizedType == "bool_array") {
            return std::is_same_v<T, std::vector<bool>>;
        } else if (normalizedType == "string_array") {
            return std::is_same_v<T, std::vector<std::string>>;
        }
        
        return false;
    };
    
    return std::visit(validateVisitor, value);
}

bool Config::validatePattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) {
        return true;
    }
    
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (...) {
        return false;
    }
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
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

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::chrono::system_clock::time_point Config::getFileModificationTime(const std::string& filepath) {
    auto ftime = std::filesystem::last_write_time(filepath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return sctp;
}

void ConfigSchema::addField(const std::string& key, const std::string& type,
                          const std::string& description,
                          const std::string& pattern,
                          bool required,
                          const std::string& defaultVal) {
    typeMappings[key] = type;
    validationPatterns[key] = pattern;
    defaultValues[key] = defaultVal;
    requiredFields[key] = required;
    descriptions[key] = description;
}

bool ConfigSchema::validate(const Config& config) const {
    for (const auto& pair : requiredFields) {
        const std::string& key = pair.first;
        bool isRequired = pair.second;
        
        if (isRequired && !config.has(key)) {
            return false;
        }
        
        if (config.has(key)) {
            auto value = config.get<std::string>(key);
            if (value) {
                // Validate pattern if specified
                auto patternIt = validationPatterns.find(key);
                if (patternIt != validationPatterns.end() && !patternIt->second.empty()) {
                    if (!Config::validatePattern(*value, patternIt->second)) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

std::string ConfigSchema::getFieldType(const std::string& key) const {
    auto it = typeMappings.find(key);
    return it != typeMappings.end() ? it->second : "";
}

std::string ConfigSchema::getFieldDescription(const std::string& key) const {
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->first : "";
}

std::string ConfigSchema::getFieldDefault(const std::string& key) const {
    auto it = defaultValues.find(key);
    return it != defaultValues.end() ? it->second : "";
}

bool ConfigSchema::isFieldRequired(const std::string& key) const {
    auto it = requiredFields.find(key);
    return it != requiredFields.end() ? it->second : false;
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

std::vector<std::string> Config::validateAgainstSchema() const {
    std::vector<std::string> errors;
    
    for (const auto& pair : pImpl->schema.requiredFields) {
        const std::string& key = pair.first;
        bool isRequired = pair.second;
        
        if (isRequired && !has(key)) {
            errors.push_back("Missing required field: " + key);
        }
    }
    
    for (const auto& entry : pImpl->entries) {
        // Check if field type is valid
        std::string type = pImpl->schema.getFieldType(entry.key);
        if (!type.empty() && !validateValue(entry.value, type)) {
            errors.push_back("Invalid type for field " + entry.key + ". Expected: " + type);
        }
        
        // Check pattern validation
        auto patternIt = pImpl->schema.validationPatterns.find(entry.key);
        if (patternIt != pImpl->schema.validationPatterns.end() && !patternIt->second.empty()) {
            std::string strValue = configValueToString(entry.value);
            if (!validatePattern(strValue, patternIt->second)) {
                errors.push_back("Invalid pattern for field " + entry.key);
            }
        }
    }
    
    return errors;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.getAllEntries()) {
        // Don't override existing entries unless source is higher priority
        if (!has(entry.key) || static_cast<int>(entry.source) > static_cast<int>(source)) {
            set(entry.key, entry.value, source);
        }
    }
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> filteredEntries;
    filteredEntries.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            filteredEntries.push_back(entry);
        }
    }
    
    return filteredEntries;
}

std::string Config::getCurrentFormat() const {
    return pImpl->currentFormat;
}

bool Config::loadWithParser(const std::string& content, const std::string& format) {
    auto parsers = getParsers();
    for (const auto& parser : parsers) {
        if (parser->getFormatName() == format && parser->canParse(content)) {
            return parser->parse(content, pImpl->entries);
        }
    }
    return false;
}

bool Config::saveWithFormat(const std::vector<ConfigEntry>& entries, const std::string& format,
                           std::ostream& output) const {
    auto parsers = getParsers();
    for (const auto& parser : parsers) {
        if (parser->getFormatName() == format) {
            output << parser->serialize(entries);
            return true;
        }
    }
    return false;
}

std::string Config::getFormatNameFromString(const std::string& format) const {
    std::string normalizedFormat = toLower(format);
    
    if (normalizedFormat == "json" || normalizedFormat == "application/json") {
        return "JSON";
    } else if (normalizedFormat == "yaml" || normalizedFormat == "yml" || normalizedFormat == "text/yaml") {
        return "YAML";
    } else if (normalizedFormat == "key_value" || normalizedFormat == "ini" || normalizedFormat == "cfg") {
        return "KEY_VALUE";
    }
    
    return "KEY_VALUE"; // Default
}

} // namespace nlm
