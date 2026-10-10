#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// JSON value types (simplified for configuration)
using JSONValue = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    uint64_t,
    double,
    std::string,
    std::vector<JSONValue>
>;

// YAML value types (simplified for configuration)
using YAMLValue = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    uint64_t,
    double,
    std::string,
    std::vector<YAMLValue>,
    std::unordered_map<std::string, YAMLValue>
>;

class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    
    // Parse from string
    virtual bool parse(const std::string& content) = 0;
    
    // Parse from file
    virtual bool parseFile(const std::string& filepath) = 0;
    
    // Get value by key (dot notation for nested access)
    virtual std::optional<JSONValue> get(const std::string& key) const = 0;
    
    // Check if key exists
    virtual bool has(const std::string& key) const = 0;
    
    // Get all keys
    virtual std::vector<std::string> getKeys() const = 0;
    
    // Convert to standard ConfigValue format
    virtual std::vector<ConfigEntry> convertToConfigFormat() const = 0;
};

// Simple JSON parser implementation (C++17 standard)
class SimpleJSONParser : public ConfigParser {
public:
    bool parse(const std::string& content) override {
        data_.clear();
        parseJSON(content, data_);
        return true;
    }
    
    bool parseFile(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }
    
    std::optional<JSONValue> get(const std::string& key) const override {
        return getNestedValue(data_, key);
    }
    
    bool has(const std::string& key) const override {
        return getNestedValue(data_, key).has_value();
    }
    
    std::vector<std::string> getKeys() const override {
        return getNestedKeys(data_);
    }
    
    std::vector<ConfigEntry> convertToConfigFormat() const override {
        std::vector<ConfigEntry> entries;
        convertJSONToConfig(data_, "", entries);
        return entries;
    }
    
private:
    JSONValue data_;
    
    void parseJSON(const std::string& content, JSONValue& result) {
        if (content.empty()) {
            return;
        }
        
        // Simple JSON parsing for configuration files
        // This handles basic objects, arrays, and primitive values
        skipWhitespace(content, 0);
        
        if (content[0] == '{') {
            parseObject(content, result);
        } else if (content[0] == '[') {
            parseArray(content, result);
        } else {
            result = parseValue(content, 0);
        }
    }
    
    void parseObject(const std::string& content, JSONValue& result) {
        std::unordered_map<std::string, JSONValue> obj;
        size_t pos = 1;  // Skip {
        
        while (pos < content.size() && content[pos] != '}') {
            skipWhitespace(content, pos);
            
            if (content[pos] == '}') break;
            
            // Parse key
            size_t keyStart = pos;
            while (pos < content.size() && content[pos] != ':' && content[pos] != '"') {
                pos++;
            }
            
            std::string key = content.substr(keyStart, pos - keyStart);
            trim(key);
            
            // Skip : or "
            if (pos < content.size() && content[pos] == ':') {
                pos++;
            } else if (pos < content.size() && content[pos] == '"') {
                pos++;
                skipWhitespace(content, pos);
            }
            
            // Parse value
            JSONValue value;
            parseValue(content, pos, value);
            
            obj[key] = value;
            
            skipWhitespace(content, pos);
            if (pos < content.size() && content[pos] == ',') {
                pos++;
                skipWhitespace(content, pos);
            }
        }
        
        result = std::move(obj);
    }
    
    void parseArray(const std::string& content, JSONValue& result) {
        std::vector<JSONValue> arr;
        size_t pos = 1;  // Skip [
        
        while (pos < content.size() && content[pos] != ']') {
            JSONValue value;
            parseValue(content, pos, value);
            arr.push_back(value);
            
            skipWhitespace(content, pos);
            if (pos < content.size() && content[pos] == ',') {
                pos++;
                skipWhitespace(content, pos);
            }
        }
        
        result = std::move(arr);
    }
    
    JSONValue parseValue(const std::string& content, size_t& pos, JSONValue value = {}) {
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) {
            return value;
        }
        
        char c = content[pos];
        
        if (c == '"') {
            return parseString(content, pos);
        } else if (c == 't' || c == 'f') {
            return parseBoolean(content, pos);
        } else if (c == 'n') {
            return parseNull(content, pos);
        } else if (c == '-' || (c >= '0' && c <= '9')) {
            return parseNumber(content, pos);
        } else if (c == '{') {
            JSONValue obj;
            parseObject(content, obj);
            return obj;
        } else if (c == '[') {
            JSONValue arr;
            parseArray(content, arr);
            return arr;
        }
        
        return value;
    }
    
    std::string parseString(const std::string& content, size_t& pos) {
        size_t start = ++pos;  // Skip opening "
        while (pos < content.size() && content[pos] != '"') {
            if (content[pos] == '\\' && pos + 1 < content.size()) {
                pos += 2;  // Skip escape sequence
            } else {
                pos++;
            }
        }
        
        if (pos >= content.size()) {
            return "";
        }
        
        std::string result = content.substr(start, pos - start);
        pos++;  // Skip closing "
        return result;
    }
    
    bool parseBoolean(const std::string& content, size_t& pos) {
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
        return false;
    }
    
    JSONValue parseNull(const std::string& content, size_t& pos) {
        if (content.substr(pos, 4) == "null") {
            pos += 4;
            return nullptr;
        }
        return nullptr;
    }
    
    JSONValue parseNumber(const std::string& content, size_t& pos) {
        size_t start = pos;
        if (content[pos] == '-') {
            pos++;
        }
        
        // Parse integer part
        while (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9')) {
            pos++;
        }
        
        // Parse decimal part
        if (pos < content.size() && content[pos] == '.') {
            pos++;
            while (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9')) {
                pos++;
            }
        }
        
        // Parse exponent
        if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
            pos++;
            if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
                pos++;
            }
            while (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9')) {
                pos++;
            }
        }
        
        std::string numStr = content.substr(start, pos - start);
        
        // Try to parse as integer first
        try {
            size_t endPos;
            uint64_t uintVal = std::stoull(numStr, &endPos);
            if (endPos == numStr.size()) {
                return uintVal;
            }
        } catch (...) {}
        
        try {
            size_t endPos;
            int64_t intVal = std::stoll(numStr, &endPos);
            if (endPos == numStr.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Parse as double
        try {
            size_t endPos;
            double doubleVal = std::stod(numStr, &endPos);
            if (endPos == numStr.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        return 0.0;
    }
    
    std::optional<JSONValue> getNestedValue(const JSONValue& data, const std::string& key) const {
        if (data.index() == 0) {  // nullptr
            return std::nullopt;
        }
        
        // Check if key contains dot notation for nested access
        size_t dotPos = key.find('.');
        if (dotPos == std::string::npos) {
            // Simple key
            if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(data)) {
                const auto& obj = std::get<std::unordered_map<std::string, JSONValue>>(data);
                auto it = obj.find(key);
                if (it != obj.end()) {
                    return it->second;
                }
            }
        } else {
            // Nested access
            std::string firstKey = key.substr(0, dotPos);
            std::string remainingKey = key.substr(dotPos + 1);
            
            if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(data)) {
                const auto& obj = std::get<std::unordered_map<std::string, JSONValue>>(data);
                auto it = obj.find(firstKey);
                if (it != obj.end()) {
                    return getNestedValue(it->second, remainingKey);
                }
            }
        }
        
        return std::nullopt;
    }
    
    std::vector<std::string> getNestedKeys(const JSONValue& data) const {
        std::vector<std::string> keys;
        
        if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(data)) {
            const auto& obj = std::get<std::unordered_map<std::string, JSONValue>>(data);
            for (const auto& pair : obj) {
                keys.push_back(pair.first);
                
                // Recursively get nested keys
                if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(pair.second)) {
                    std::vector<std::string> nestedKeys = getNestedKeys(pair.second);
                    for (const auto& nestedKey : nestedKeys) {
                        keys.push_back(pair.first + '.' + nestedKey);
                    }
                }
            }
        }
        
        return keys;
    }
    
    void convertJSONToConfig(const JSONValue& jsonData, const std::string& prefix, 
                             std::vector<ConfigEntry>& entries) const {
        if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(jsonData)) {
            const auto& obj = std::get<std::unordered_map<std::string, JSONValue>>(jsonData);
            for (const auto& pair : obj) {
                std::string fullKey = prefix.empty() ? pair.first : prefix + '.' + pair.first;
                
                if (std::holds_alternative<bool>(pair.second)) {
                    entries.emplace_back(fullKey, ConfigValue(std::get<bool>(pair.second)), ConfigSource::File);
                } else if (std::holds_alternative<int64_t>(pair.second)) {
                    entries.emplace_back(fullKey, ConfigValue(std::get<int64_t>(pair.second)), ConfigSource::File);
                } else if (std::holds_alternative<uint64_t>(pair.second)) {
                    entries.emplace_back(fullKey, ConfigValue(std::get<uint64_t>(pair.second)), ConfigSource::File);
                } else if (std::holds_alternative<double>(pair.second)) {
                    entries.emplace_back(fullKey, ConfigValue(std::get<double>(pair.second)), ConfigSource::File);
                } else if (std::holds_alternative<std::string>(pair.second)) {
                    entries.emplace_back(fullKey, ConfigValue(std::get<std::string>(pair.second)), ConfigSource::File);
                } else if (std::holds_alternative<std::vector<JSONValue>>(pair.second)) {
                    // For arrays, convert to string representation
                    const auto& arr = std::get<std::vector<JSONValue>>(pair.second);
                    std::string arrStr = "[";
                    for (size_t i = 0; i < arr.size(); ++i) {
                        if (i > 0) arrStr += ", ";
                        if (std::holds_alternative<std::string>(arr[i])) {
                            arrStr += "\"" + std::get<std::string>(arr[i]) + "\"";
                        } else {
                            arrStr += std::to_string(std::get<double>(arr[i]));
                        }
                    }
                    arrStr += "]";
                    entries.emplace_back(fullKey, ConfigValue(arrStr), ConfigSource::File);
                } else {
                    // Unknown type, store as string
                    entries.emplace_back(fullKey, ConfigValue("unknown"), ConfigSource::File);
                }
                
                // Recursively process nested objects
                convertJSONToConfig(pair.second, fullKey, entries);
            }
        }
    }
    
    void skipWhitespace(const std::string& content, size_t& pos) {
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }
    
    void trim(std::string& str) {
        str.erase(0, str.find_first_not_of(" \t\n\r"));
        str.erase(str.find_last_not_of(" \t\n\r") + 1);
    }
};

// Main Configuration class with enhanced JSON/YAML support
class Config {
public:
    Config() : pImpl(std::make_unique<Impl>()) {}
    
    ~Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath) {
        std::filesystem::path path(filepath);
        std::string extension = path.extension().string();
        
        // Convert extension to lowercase
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        
        if (extension == ".json" || extension == ".jsonc") {
            jsonParser_ = std::make_unique<SimpleJSONParser>();
            if (jsonParser_->parseFile(filepath)) {
                auto entries = jsonParser_->convertToConfigFormat();
                pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
                return true;
            }
        } else if (extension == ".yaml" || extension == ".yml") {
            // TODO: Implement YAML parser
            // For now, fall back to simple parser
            return loadFromSimpleFormat(filepath);
        } else if (extension == ".cfg" || extension == ".config") {
            // Simple key=value format (legacy support)
            return loadFromSimpleFormat(filepath);
        }
        
        return false;
    }
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv) {
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
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write as JSON
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) {
                file << ",\n";
            }
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](const auto& val) {
                if constexpr (std::is_same_v<decltype(val), std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<decltype(val), bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<decltype(val), int> || 
                                     std::is_same_v<decltype(val), int64_t> ||
                                     std::is_same_v<decltype(val), uint64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<decltype(val), double>) {
                    file << val;
                } else {
                    file << "\"unknown\"";
                }
            }, entry.value);
            
            first = false;
        }
        
        file << "\n}\n";
        return true;
    }
    
    // Get values (with enhanced error handling)
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        try {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (it == pImpl->entries.end()) {
                return std::nullopt;
            }
            
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        validateKey(key);
        validateValue(key, value);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    // Remove key
    void remove(const std::string& key) {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    // Clear all
    void clear() {
        pImpl->entries.clear();
    }
    
    // Get configuration summary with enhanced information
    std::string summary() const {
        std::ostringstream oss;
        oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
        oss << "Format: JSON/YAML with backward compatibility\n";
        oss << "Schema validation: Enabled\n";
        oss << "Sources: Default, File, CommandLine, Runtime\n\n";
        
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else {
                    oss << val;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        
        return oss.str();
    }
    
    // Enhanced schema validation
    bool validateSchema() const {
        // Check for required configuration sections
        std::unordered_set<std::string> requiredSections = {
            "random_seed",
            "simulation_timestep",
            "neuron_count"
        };
        
        for (const auto& section : requiredSections) {
            if (!has(section)) {
                return false;
            }
        }
        
        // Validate specific configuration values
        if (has("neuron_count")) {
            auto neuronCount = get<size_t>("neuron_count");
            if (!neuronCount || *neuronCount == 0 || *neuronCount > 1000000) {
                return false;
            }
        }
        
        if (has("simulation_timestep")) {
            auto timestep = get<double>("simulation_timestep");
            if (!timestep || *timestep <= 0.0 || *timestep > 1.0) {
                return false;
            }
        }
        
        return true;
    }
    
    // Get configuration statistics
    std::string getStatistics() const {
        std::ostringstream oss;
        oss << "Configuration Statistics:\n";
        oss << "Total entries: " << pImpl->entries.size() << "\n";
        
        std::unordered_map<ConfigSource, size_t> sourceCount;
        for (const auto& entry : pImpl->entries) {
            sourceCount[entry.source]++;
        }
        
        oss << "Sources:\n";
        for (const auto& pair : sourceCount) {
            oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
        
        return oss.str();
    }
    
    // Check if configuration is valid
    bool isValid() const {
        return validateSchema();
    }
    
    // Get configuration health status
    bool isHealthy() const {
        if (!isValid()) {
            return false;
        }
        
        // Check for duplicate keys
        std::unordered_set<std::string> seenKeys;
        for (const auto& entry : pImpl->entries) {
            if (seenKeys.find(entry.key) != seenKeys.end()) {
                return false;
            }
            seenKeys.insert(entry.key);
        }
        
        return true;
    }
    
private:
    struct Impl {
        std::vector<ConfigEntry> entries;
        std::unique_ptr<ConfigParser> jsonParser;
    };
    
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigParser> jsonParser_;
    
    // Legacy simple format parser
    bool loadFromSimpleFormat(const std::string& filepath) {
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
    
    void validateKey(const std::string& key) {
        if (key.empty()) {
            throw ConfigError("Configuration key cannot be empty");
        }
        
        // Check for valid characters in key
        for (char c : key) {
            if (!(std::isalnum(c) || c == '_' || c == '.' || c == '-')) {
                throw ConfigError("Invalid character in configuration key: '" + std::string(1, c) + "'");
            }
        }
    }
    
    void validateValue(const std::string& key, const ConfigValue& value) {
        // Basic validation - can be extended for specific key types
        if (std::holds_alternative<std::string>(value)) {
            const std::string& str = std::get<std::string>(value);
            if (str.empty() && key.find("path") != std::string::npos) {
                throw ConfigError("Configuration path cannot be empty");
            }
        }
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
};

} // namespace nlm
