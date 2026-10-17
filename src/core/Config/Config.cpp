#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cstring>
#include <ctime>

namespace nlm {

// JSON parsing helpers (simplified)
static std::string escapeJSON(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\u";
                    result += "0000" + std::to_string(static_cast<unsigned char>(c));
                } else {
                    result += c;
                }
        }
    }
    return result;
}

static std::string unescapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[++i]) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u':
                    if (i + 5 < str.size()) {
                        int hex = 0;
                        for (int j = 0; j < 4; ++j) {
                            char c = str[++i];
                            if (c >= '0' && c <= '9') hex = hex * 16 + (c - '0');
                            else if (c >= 'a' && c <= 'f') hex = hex * 16 + (c - 'a' + 10);
                            else if (c >= 'A' && c <= 'F') hex = hex * 16 + (c - 'A' + 10);
                            else { hex = 0; break; }
                        }
                        result += static_cast<char>(hex);
                    }
                    break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    return std::all_of(str.begin() + start, str.end(), ::isdigit);
}

static bool isFloat(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExponent) return false;
            hasExponent = true;
            if (i + 1 >= str.size()) return false; // 'e' at end is invalid
        } else if (!(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }
    }
    return hasDecimal || hasExponent;
}

static bool isBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "yes" || lower == "no" ||
           lower == "on" || lower == "off" || lower == "1" || lower == "0";
}

// Type conversion implementations
bool ConfigTypeConverter::stringToBool(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "on" || lower == "1") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "off" || lower == "0") {
        result = false;
        return true;
    }
    return false;
}

bool ConfigTypeConverter::stringToInt(const std::string& str, int& result) {
    try {
        size_t pos;
        result = std::stoi(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigTypeConverter::stringToDouble(const std::string& str, double& result) {
    try {
        size_t pos;
        result = std::stod(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigTypeConverter::stringToInt64(const std::string& str, int64_t& result) {
    try {
        size_t pos;
        result = std::stoll(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigTypeConverter::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string ConfigTypeConverter::intToString(int value) {
    return std::to_string(value);
}

std::string ConfigTypeConverter::doubleToString(double value) {
    return std::to_string(value);
}

std::string ConfigTypeConverter::int64ToString(int64_t value) {
    return std::to_string(value);
}

// ConfigSchema implementation
void ConfigSchema::addField(const std::string& key, const ConfigValue& defaultValue,
                           ConfigSource source, const std::string& description,
                           std::function<ValidationResult(const ConfigValue&)> validator,
                           bool required) {
    FieldInfo field;
    field.key = key;
    field.defaultValue = defaultValue;
    field.defaultSource = source;
    field.description = description;
    field.validator = validator;
    field.required = required;
    fields_[key] = field;
}

const ConfigSchema::FieldInfo* ConfigSchema::getField(const std::string& key) const {
    auto it = fields_.find(key);
    return it != fields_.end() ? &it->second : nullptr;
}

std::vector<std::string> ConfigSchema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(fields_.size());
    for (const auto& pair : fields_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    auto it = fields_.find(key);
    return it != fields_.end() ? it->second.description : "";
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = fields_.find(key);
    if (it == fields_.end()) {
        return ValidationResult::error("Unknown configuration key: " + key);
    }
    
    const auto& field = it->second;
    
    // Check if value type matches expected type (simplified)
    if (!field.validator) {
        return ValidationResult::success();
    }
    
    return field.validator(value);
}

std::vector<std::string> ConfigSchema::getRequiredFields() const {
    std::vector<std::string> required;
    for (const auto& pair : fields_) {
        if (pair.second.required) {
            required.push_back(pair.first);
        }
    }
    return required;
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    std::unordered_map<std::string, std::function<ValidationResult(const ConfigValue&)>> validators;
    bool validateOnSet;
    uint64_t nextTimestamp;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnSet = true;
    pImpl->nextTimestamp = 0;
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeFromSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
    
    // Set default values from schema
    for (const auto& pair : schema.getFields()) {
        const auto& field = pair.second;
        if (!has(field.key)) {
            set(field.key, field.defaultValue, field.defaultSource, false);
        }
    }
}

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    return parseSimpleConfig(filepath, pImpl->entries);
}

bool Config::loadFromJSONFile(const std::string& filepath, bool validate) {
    return parseJSONConfig(filepath, pImpl->entries);
}

bool Config::loadFromYAMLFile(const std::string& filepath, bool validate) {
    return parseYAMLConfig(filepath, pImpl->entries);
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(arg.substr(2, pos - 2));
                std::string value = trim(arg.substr(pos + 1));
                set(key, value, ConfigSource::CommandLine, validate);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = trim(arg.substr(1));
            std::string value = trim(argv[++i]);
            set(key, value, ConfigSource::CommandLine, validate);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool includeDescriptions) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        if (includeDescriptions && !entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeJSON(arg[i]) << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << escapeJSON(entry.key) << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeJSON(arg[j]) << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeJSON(arg[i]) << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << escapeJSON(entry.key) << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << "\"" << escapeJSON(arg[j]) << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}\n";
    return oss.str();
}

std::string Config::toYAML() const {
    std::ostringstream oss;
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeJSON(arg[i]) << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "\n";
    }
    return oss.str();
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    if (validate && pImpl->validateOnSet) {
        ValidationResult validationResult;
        
        // Check schema validation first
        if (const auto* field = pImpl->schema.getField(key)) {
            validationResult = field->validator ? field->validator(value) : ValidationResult::success();
        }
        
        // Check custom validator
        auto validatorIt = pImpl->validators.find(key);
        if (validatorIt != pImpl->validators.end()) {
            auto customResult = validatorIt->second(value);
            if (!customResult.valid) {
                validationResult = customResult;
            }
        }
        
        if (!validationResult.valid) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + validationResult.errorMessage);
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = ++pImpl->nextTimestamp;
    } else {
        pImpl->entries.emplace_back(key, value, source, pImpl->schema.getDescription(key), ++pImpl->nextTimestamp);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    // Try to infer type from value
    bool boolVal;
    if (ConfigTypeConverter::stringToBool(value, boolVal)) {
        set(key, boolVal, source, validate);
    } else if (isInteger(value)) {
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.size()) {
                set(key, intVal, source, validate);
                return;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            int64_t int64Val = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, int64Val, source, validate);
                return;
            }
        } catch (...) {}
    } else if (isFloat(value)) {
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleVal, source, validate);
                return;
            }
        } catch (...) {}
    }
    
    // Default to string
    set(key, value, source, validate);
}

void Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
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

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSON(arg) << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (source=" << static_cast<int>(entry.source) << ", timestamp=" << entry.timestamp << ")\n";
        if (!entry.description.empty()) {
            oss << "      " << entry.description << "\n";
        }
    }
    return oss.str();
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->description : "";
}

bool Config::isValidated(const std::string& key) const {
    auto validatorIt = pImpl->validators.find(key);
    return validatorIt != pImpl->validators.end();
}

ValidationResult Config::validate() const {
    std::vector<std::string> errors;
    
    // Check required fields
    for (const auto& field : pImpl->schema.getRequiredFields()) {
        if (!has(field)) {
            errors.push_back("Required field missing: " + field);
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        if (const auto* field = pImpl->schema.getField(entry.key)) {
            if (field->validator) {
                ValidationResult result = field->validator(entry.value);
                if (!result.valid) {
                    errors.push_back("Field '" + entry.key + "': " + result.errorMessage);
                }
            }
        }
    }
    
    if (errors.empty()) {
        return ValidationResult::success();
    } else {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const auto& error : errors) {
            errorMsg += "  " + error + "\n";
        }
        return ValidationResult::error(errorMsg);
    }
}

bool Config::validate(const std::string& key) const {
    ValidationResult result = validate();
    return result.valid;
}

void Config::setValidator(const std::string& key, std::function<ValidationResult(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
}

bool Config::merge(const Config& other, bool overwrite) {
    bool changed = false;
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key) || overwrite) {
            pImpl->entries.push_back(entry);
            changed = true;
        }
    }
    return changed;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            return false;
        }
        
        if (it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::resetToDefaults() {
    clear();
    pImpl->schema = ConfigSchema();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->source : ConfigSource::Default;
}

uint64_t Config::getTimestamp(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    return it != pImpl->entries.end() ? it->timestamp : 0;
}

// File parsing implementations
bool Config::parseSimpleConfig(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    uint64_t timestamp = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            bool parsed = false;
            
            if (ConfigTypeConverter::stringToBool(value, std::get<bool>(configValue))) {
                parsed = true;
            } else if (ConfigTypeConverter::stringToInt(value, std::get<int>(configValue))) {
                parsed = true;
            } else if (ConfigTypeConverter::stringToInt64(value, std::get<int64_t>(configValue))) {
                parsed = true;
            } else if (ConfigTypeConverter::stringToDouble(value, std::get<double>(configValue))) {
                parsed = true;
            }
            
            if (!parsed) {
                configValue = value;
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File, "", ++timestamp);
        }
    }
    
    return true;
}

bool Config::parseJSONConfig(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified JSON parser
    size_t pos = 0;
    uint64_t timestamp = 0;
    
    while (pos < content.size()) {
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) {
            pos++;
        }
        
        if (pos >= content.size() || content[pos] != ':') {
            break;
        }
        
        pos++; // Skip ':'
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) {
            pos++;
        }
        
        // Parse value based on first character
        if (pos < content.size() && content[pos] == '"') {
            // String value
            size_t valueEnd = content.find('"', pos + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = content.substr(pos + 1, valueEnd - pos - 1);
            entries.emplace_back(key, unescapeJSON(value), ConfigSource::File, "", ++timestamp);
            pos = valueEnd + 1;
        } else if (pos < content.size() && (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9'))) {
            // Number value
            size_t valueEnd = content.find_first_of(",}", pos);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = content.substr(pos, valueEnd - pos);
            bool parsed = false;
            
            if (isInteger(valueStr)) {
                try {
                    size_t valPos;
                    int intVal = std::stoi(valueStr, &valPos);
                    if (valPos == valueStr.size()) {
                        entries.emplace_back(key, intVal, ConfigSource::File, "", ++timestamp);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed && isFloat(valueStr)) {
                try {
                    size_t valPos;
                    double doubleVal = std::stod(valueStr, &valPos);
                    if (valPos == valueStr.size()) {
                        entries.emplace_back(key, doubleVal, ConfigSource::File, "", ++timestamp);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                entries.emplace_back(key, valueStr, ConfigSource::File, "", ++timestamp);
            }
            
            pos = valueEnd;
        } else if (pos < content.size() && content[pos] == 't') {
            // Boolean value "true"
            if (content.compare(pos, 5, "true") == 0) {
                entries.emplace_back(key, true, ConfigSource::File, "", ++timestamp);
                pos += 4;
            }
        } else if (pos < content.size() && content[pos] == 'f') {
            // Boolean value "false"
            if (content.compare(pos, 6, "false") == 0) {
                entries.emplace_back(key, false, ConfigSource::File, "", ++timestamp);
                pos += 5;
            }
        } else if (pos < content.size() && content[pos] == '[') {
            // Array value (simplified - only handles strings for now)
            size_t valueEnd = content.find(']', pos);
            if (valueEnd == std::string::npos) break;
            
            std::string arrayStr = content.substr(pos + 1, valueEnd - pos - 1);
            std::vector<std::string> arrayValues;
            
            size_t start = 0;
            while (start < arrayStr.size()) {
                size_t quote = arrayStr.find('"', start);
                if (quote == std::string::npos) break;
                
                size_t quoteEnd = arrayStr.find('"', quote + 1);
                if (quoteEnd == std::string::npos) break;
                
                std::string item = arrayStr.substr(quote + 1, quoteEnd - quote - 1);
                arrayValues.push_back(unescapeJSON(item));
                
                start = quoteEnd + 1;
                if (start < arrayStr.size() && arrayStr[start] == ',') start++;
            }
            
            entries.emplace_back(key, arrayValues, ConfigSource::File, "", ++timestamp);
            pos = valueEnd + 1;
        }
        
        // Skip to next entry
        while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
            pos++;
        }
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        }
    }
    
    return !entries.empty();
}

bool Config::parseYAMLConfig(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified YAML parser
    std::istringstream stream(content);
    std::string line;
    uint64_t timestamp = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        // Remove quotes from YAML strings
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        ConfigValue configValue;
        bool parsed = false;
        
        if (ConfigTypeConverter::stringToBool(value, std::get<bool>(configValue))) {
            parsed = true;
        } else if (ConfigTypeConverter::stringToInt(value, std::get<int>(configValue))) {
            parsed = true;
        } else if (ConfigTypeConverter::stringToInt64(value, std::get<int64_t>(configValue))) {
            parsed = true;
        } else if (ConfigTypeConverter::stringToDouble(value, std::get<double>(configValue))) {
            parsed = true;
        }
        
        if (!parsed) {
            configValue = value;
        }
        
        entries.emplace_back(key, configValue, ConfigSource::File, "", ++timestamp);
    }
    
    return !entries.empty();
}

} // namespace nlm