// Config.cpp implementation
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Simple key=value format for now
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
            
            setString(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setString(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setString(key, value, ConfigSource::CommandLine);
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
        file << entry.key << " = ";
        
        if (entry.value.type() == typeid(std::string)) {
            file << "\"" << std::any_cast<const std::string&>(entry.value) << "\"";
        } else if (entry.value.type() == typeid(int)) {
            file << std::any_cast<int>(entry.value);
        } else if (entry.value.type() == typeid(double)) {
            file << std::any_cast<double>(entry.value);
        } else if (entry.value.type() == typeid(bool)) {
            file << (std::any_cast<bool>(entry.value) ? "true" : "false");
        } else {
            file << "UNKNOWN_TYPE";
        }
        file << "\n";
    }
    
    return true;
}

void Config::initializeWithSchema(const std::unordered_map<std::string, ConfigSchema>& schemaMap) {
    schema = schemaMap;
}

void Config::validateAndSet(const std::string& key, const std::any& value, ConfigSource source) {
    if (!validateValue(key, value)) {
        std::string errorMsg = "Validation failed for key '" + key + "' with value of type '" + 
                              (value.has_value() ? typeid(value).name() : "null") + "'";
        throw ConfigError(ConfigError::ErrorType::ValidationFailed, errorMsg);
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

bool Config::validateValue(const std::string& key, const std::any& value) const {
    auto it = schema.find(key);
    if (it == schema.end()) {
        return true; // No schema validation if not in schema
    }
    
    const auto& schemaEntry = it->second;
    return ConfigValidator::validateValue(schemaEntry, value);
}

void Config::set(const std::string& key, const std::any& value, ConfigSource source) {
    validateAndSet(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    set(key, value, source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, value, source);
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<int>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<int64_t> Config::getInt64(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<int64_t>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<double>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<bool>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<std::string>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<std::vector<int>> Config::getIntVector(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<std::vector<int>>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<std::vector<double>> Config::getDoubleVector(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<std::vector<double>>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>> Config::getStringVector(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<std::vector<std::string>>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
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
        return std::any_cast<T>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

int Config::getOrInt(const std::string& key, int defaultValue) const {
    auto val = getInt(key);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t Config::getOrInt64(const std::string& key, int64_t defaultValue) const {
    auto val = getInt64(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getOrDouble(const std::string& key, double defaultValue) const {
    auto val = getDouble(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getOrBool(const std::string& key, bool defaultValue) const {
    auto val = getBool(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string Config::getOrString(const std::string& key, const std::string& defaultValue) const {
    auto val = getString(key);
    return val.has_value() ? val.value() : defaultValue;
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
        if (entry.value.type() == typeid(std::string)) {
            oss << "\"" << std::any_cast<const std::string&>(entry.value) << "\"";
        } else if (entry.value.type() == typeid(int)) {
            oss << std::any_cast<int>(entry.value);
        } else if (entry.value.type() == typeid(double)) {
            oss << std::any_cast<double>(entry.value);
        } else if (entry.value.type() == typeid(bool)) {
            oss << (std::any_cast<bool>(entry.value) ? "true" : "false");
        } else if (entry.value.type() == typeid(std::vector<int>)) {
            const auto& vec = std::any_cast<const std::vector<int>&>(entry.value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (entry.value.type() == typeid(std::vector<double>)) {
            const auto& vec = std::any_cast<const std::vector<double>&>(entry.value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (entry.value.type() == typeid(std::vector<std::string>)) {
            const auto& vec = std::any_cast<const std::vector<std::string>&>(entry.value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "UNKNOWN_TYPE";
        }
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::generateJSONSchema(const std::unordered_map<std::string, ConfigSchema>& schema) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"definitions\": {\n";
    
    bool first = true;
    for (const auto& pair : schema) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "    \"" << pair.first << "\": {\n";
        
        switch (pair.second.type) {
            case ConfigSchema::Type::Integer:
                oss << "      \"type\": \"integer\"";
                break;
            case ConfigSchema::Type::Integer64:
                oss << "      \"type\": \"integer\"";
                break;
            case ConfigSchema::Type::Double:
                oss << "      \"type\": \"number\"";
                break;
            case ConfigSchema::Type::Boolean:
                oss << "      \"type\": \"boolean\"";
                break;
            case ConfigSchema::Type::String:
                oss << "      \"type\": \"string\"";
                break;
            case ConfigSchema::Type::IntVector:
                oss << "      \"type\": \"array\",\n      \"items\": {\"type\": \"integer\"}";
                break;
            case ConfigSchema::Type::DoubleVector:
                oss << "      \"type\": \"array\",\n      \"items\": {\"type\": \"number\"}";
                break;
            case ConfigSchema::Type::StringVector:
                oss << "      \"type\": \"array\",\n      \"items\": {\"type\": \"string\"}";
                break;
            default:
                oss << "      \"type\": \"string\"";
                break;
        }
        
        if (!pair.second.description.empty()) {
            oss << ",\n      \"description\": \"" << escapeJSONString(pair.second.description) << "\"";
        }
        
        if (pair.second.required) {
            oss << ",\n      \"required\": true";
        }
        
        oss << "\n    }";
    }
    
    oss << "\n  }\n}\n";
    return oss.str();
}

std::string Config::escapeJSONString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

bool Config::validateAgainstSchema(const std::unordered_map<std::string, ConfigSchema>& schema) const {
    std::unordered_map<std::string, std::any> values;
    for (const auto& entry : pImpl->entries) {
        values[entry.key] = entry.value;
    }
    
    return ConfigValidator::validateAll(schema, values);
}

ConfigBuilder Config::createBuilder() {
    return ConfigBuilder();
}

ConfigFactory Config::createFactory() {
    return ConfigFactory();
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

// Explicit template instantiations for vector types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

 template int Config::getOr<int>(const std::string&, const int&) const;
 template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
 template double Config::getOr<double>(const std::string&, const double&) const;
 template bool Config::getOr<bool>(const std::string&, const bool&) const;
 template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
 template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
 template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
 template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm