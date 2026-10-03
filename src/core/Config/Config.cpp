// Missing implementations that were truncated

// Helper method for template instantiation
const ConfigValue& Config::getRawValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    if (parentConfig && parentConfig->has(key)) {
        return parentConfig->getRawValue(key);
    }
    
    static const ConfigValue empty;
    return empty;
}

// Include necessary headers for implementations
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cctype>

// ConfigSchema implementation
namespace nlm {
    bool ConfigSchema::loadFromFile(const std::string& schemaPath) {
        // Simplified schema loading - in practice would parse JSON/YAML schema
        if (fileExists(schemaPath)) {
            // For now, just set a default schema if file exists
            name = "Default Schema";
            version = "1.0.0";
            description = "Default configuration schema";
            return true;
        }
        return false;
    }
}

// Global helper functions
namespace nlm {
    bool Config::parseSimpleValue(const std::string& str, ConfigValue& value, ConfigDataType& type) {
        if (str.empty()) {
            return false;
        }
        
        // Boolean
        if (str == "true" || str == "false") {
            bool boolVal = (str == "true");
            value = boolVal;
            type = ConfigDataType::Bool;
            return true;
        }
        
        // Integer
        try {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                value = intVal;
                type = ConfigDataType::Int64;
                return true;
            }
        } catch (...) {}
        
        // Double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                type = ConfigDataType::Double;
                return true;
            }
        } catch (...) {}
        
        // String
        value = str;
        type = ConfigDataType::String;
        return true;
    }

    // Additional helper functions for better type conversion
    bool Config::isIntegerString(const std::string& str) {
        if (str.empty() || (str[0] == '-' && str.size() == 1)) return false;
        
        char* end;
        strtoll(str.c_str(), &end, 10);
        return (*end == '\0');
    }

    bool Config::isDoubleString(const std::string& str) {
        if (str.empty()) return false;
        
        char* end;
        strtod(str.c_str(), &end);
        return (*end == '\0');
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

    std::string Config::extractVariableName(const std::string& str) {
        if (str.find('${') != 0 || str.find('}') == std::string::npos) {
            return "";
        }
        
        size_t start = 2; // Skip ${
        size_t end = str.find('}', start);
        if (end == std::string::npos) {
            return "";
        }
        
        return str.substr(start, end - start);
    }

    bool Config::validateRegex(const std::string& value, const std::string& pattern) {
        if (pattern.empty()) return true;
        try {
            std::regex regex(pattern);
            return std::regex_match(value, regex);
        } catch (const std::regex_error&) {
            return false;
        }
    }

    std::string Config::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
        return oss.str();
    }
}

// Template instantiations
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

// ConfigSchema implementations
const ConfigSchema& Config::getSchema() const { return schema; }

bool Config::loadSchemaFromFile(const std::string& schemaPath) {
    try {
        return schema.loadFromFile(schemaPath);
    } catch (const std::exception& e) {
        lastError = "Error loading schema from file " + schemaPath + ": " + e.what();
        return false;
    }
}

// Error handling methods
std::string Config::getLastError() const { return lastError; }
void Config::clearLastError() { lastError.clear(); }

// File utility methods
bool Config::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string Config::getFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(pos + 1);
}

// Type conversion utility methods
std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, double>) {
            oss << arg;
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
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << "; ";
                oss << "[";
                for (size_t j = 0; j < arg[i].size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << arg[i][j];
                }
                oss << "]";
            }
            oss << "]";
        } else {
            oss << "\"" << "unknown" << "\"";
        }
    }, value);
    return oss.str();
}

bool Config::stringToConfigValue(const std::string& str, ConfigValue& value, ConfigDataType targetType) {
    std::istringstream iss(str);
    
    switch (targetType) {
        case ConfigDataType::Int: {
            int val;
            if (iss >> val) {
                value = val;
                return true;
            }
            break;
        }
        case ConfigDataType::Int64: {
            int64_t val;
            if (iss >> val) {
                value = val;
                return true;
            }
            break;
        }
        case ConfigDataType::Double: {
            double val;
            if (iss >> val) {
                value = val;
                return true;
            }
            break;
        }
        case ConfigDataType::Bool: {
            if (str == "true" || str == "1" || str == "yes" || str == "TRUE" || str == "YES") {
                value = true;
                return true;
            } else if (str == "false" || str == "0" || str == "no" || str == "FALSE" || str == "NO") {
                value = false;
                return true;
            }
            break;
        }
        case ConfigDataType::String: {
            value = str;
            return true;
        }
        case ConfigDataType::IntVector: {
            std::vector<int> vec;
            int val;
            while (iss >> val) {
                vec.push_back(val);
                if (iss.peek() == ',') iss.ignore();
                else if (iss.peek() == ' ') iss.ignore();
            }
            if (!vec.empty()) {
                value = vec;
                return true;
            }
            break;
        }
        case ConfigDataType::DoubleVector: {
            std::vector<double> vec;
            double val;
            while (iss >> val) {
                vec.push_back(val);
                if (iss.peek() == ',') iss.ignore();
                else if (iss.peek() == ' ') iss.ignore();
            }
            if (!vec.empty()) {
                value = vec;
                return true;
            }
            break;
        }
        case ConfigDataType::StringVector: {
            std::vector<std::string> vec;
            std::string item;
            std::istringstream itemStream(str);
            while (std::getline(itemStream, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(item);
                }
            }
            if (!vec.empty()) {
                value = vec;
                return true;
            }
            break;
        }
        case ConfigDataType::Matrix: {
            std::vector<std::vector<double>> matrix;
            std::vector<double> row;
            double val;
            
            std::istringstream rowStream(str);
            std::string token;
            
            while (std::getline(rowStream, token, ';')) {
                std::istringstream colStream(token);
                row.clear();
                while (colStream >> val) {
                    row.push_back(val);
                    if (colStream.peek() == ',') colStream.ignore();
                    else if (colStream.peek() == ' ') colStream.ignore();
                }
                if (!row.empty()) {
                    matrix.push_back(row);
                }
            }
            
            if (!matrix.empty()) {
                value = matrix;
                return true;
            }
            break;
        }
        default:
            break;
    }
    
    // Default to string
    value = str;
    return true;
}

template<typename T>
bool Config::convertToType(const ConfigValue& from, T& to) {
    try {
        to = std::get<T>(from);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

ConfigDataType Config::getConfigDataType(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return ConfigDataType::Int;
    if (std::holds_alternative<int64_t>(value)) return ConfigDataType::Int64;
    if (std::holds_alternative<double>(value)) return ConfigDataType::Double;
    if (std::holds_alternative<bool>(value)) return ConfigDataType::Bool;
    if (std::holds_alternative<std::string>(value)) return ConfigDataType::String;
    if (std::holds_alternative<std::vector<int>>(value)) return ConfigDataType::IntVector;
    if (std::holds_alternative<std::vector<double>>(value)) return ConfigDataType::DoubleVector;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigDataType::StringVector;
    if (std::holds_alternative<std::vector<std::vector<double>>>(value)) return ConfigDataType::Matrix;
    return ConfigDataType::String;
}

std::string Config::configDataTypeToString(ConfigDataType type) {
    switch (type) {
        case ConfigDataType::Int: return "int";
        case ConfigDataType::Int64: return "int64_t";
        case ConfigDataType::Double: return "double";
        case ConfigDataType::Bool: return "bool";
        case ConfigDataType::String: return "string";
        case ConfigDataType::IntVector: return "int[]";
        case ConfigDataType::DoubleVector: return "double[]";
        case ConfigDataType::StringVector: return "string[]";
        case ConfigDataType::Matrix: return "matrix";
        default: return "unknown";
    }
}

void Config::clearCommandLineEntries() {
    // Remove all entries with CommandLine source
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [](const ConfigEntry& e) { return e.source == ConfigSource::CommandLine; }),
        pImpl->entries.end()
    );
}

std::string Config::expandVariable(const std::string& varName) const {
    // Try environment variable first
    const char* envValue = std::getenv(varName.c_str());
    if (envValue != nullptr && std::string(envValue) != "") {
        return std::string(envValue);
    }
    
    // Try parent config
    if (parentConfig && parentConfig->has("env_" + varName)) {
        ConfigSource source;
        auto valueOpt = parentConfig->get<std::string>("env_" + varName, source);
        if (valueOpt) {
            return *valueOpt;
        }
    }
    
    // Check version-specific prefix
    std::string fullPrefix = envPrefix + varName;
    const char* fullEnvValue = std::getenv(fullPrefix.c_str());
    if (fullEnvValue != nullptr && std::string(fullEnvValue) != "") {
        return std::string(fullEnvValue);
    }
    
    return "";
}

bool Config::isEnvironmentVariable(const std::string& str) const {
    return (str.find('$') == 0 && str.find('}') != std::string::npos) ||
           (str.find('${') == 0 && str.find('}') != std::string::npos);
}

bool Config::validateRegex(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) return true;
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (const std::regex_error&) {
        lastError = "Invalid regex pattern: '" + pattern + "'";
        return false;
    }
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
    return oss.str();
}

// Forward declarations for implementation
bool Config::loadFromFileInternal(const std::string& filepath, bool inheritFromDefaults);
bool Config::loadFromJSONInternal(const std::string& content, bool inheritFromDefaults);
bool Config::loadFromYAMLInternal(const std::string& content, bool inheritFromDefaults);

bool Config::loadFromFileInternal(const std::string& filepath, bool inheritFromDefaults) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Cannot open file: " + filepath;
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    file.close();
    
    // Determine file type based on extension
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJSONInternal(content, inheritFromDefaults);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAMLInternal(content, inheritFromDefaults);
    } else {
        // Default to simple key=value format for backward compatibility
        // Clear existing entries
        clear();
        
        std::istringstream iss(content);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                lastError = "Invalid line in config file " + filepath + " at line " + std::to_string(lineNum) + ": " + line;
                return false;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Determine type
            ConfigDataType type = ConfigDataType::String;
            ConfigValue configValue;
            
            if (parseSimpleValue(value, configValue, type)) {
                set(key, configValue, ConfigSource::File, "Loaded from config file: " + filepath, type);
            } else {
                // Try environment variable expansion
                if (isEnvironmentVariable(value)) {
                    value = expandVariable(value.substr(2, value.size() - 3)); // Remove ${ and }
                }
                
                // Set as string if parsing failed
                set(key, value, ConfigSource::File, "Loaded from config file: " + filepath, ConfigDataType::String);
            }
        }
        
        return true;
    }
}

bool Config::loadFromJSONInternal(const std::string& content, bool inheritFromDefaults) {
    // Simple JSON-like parser for demonstration
    SimpleJSONParser parser(content);
    std::unordered_map<std::string, ConfigValue> parsed;
    
    if (!parser.parseObject(parsed)) {
        lastError = "Failed to parse JSON content";
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Add parsed entries
    for (const auto& pair : parsed) {
        set(pair.first, pair.second, ConfigSource::File, 
            "Parsed from JSON configuration", getConfigDataType(pair.second));
    }
    
    return true;
}

bool Config::loadFromYAMLInternal(const std::string& content, bool inheritFromDefaults) {
    // Simple YAML-like parser for demonstration
    SimpleYAMLParser parser(content);
    std::unordered_map<std::string, ConfigValue> parsed;
    
    if (!parser.parseDocument(parsed)) {
        lastError = "Failed to parse YAML content";
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Add parsed entries
    for (const auto& pair : parsed) {
        set(pair.first, pair.second, ConfigSource::File,
            "Parsed from YAML configuration", getConfigDataType(pair.second));
    }
    
    return true;
}

std::string Config::serializeToJSON(const ConfigEntry& entry) const {
    std::ostringstream oss;
    oss << "\"" << entry.key << "\": ";
    
    switch (entry.dataType) {
        case ConfigDataType::Int:
        case ConfigDataType::Int64:
        case ConfigDataType::Double: {
            if (std::holds_alternative<int>(entry.value)) {
                oss << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                oss << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                oss << std::fixed << std::setprecision(6) << std::get<double>(entry.value);
            }
            break;
        }
        case ConfigDataType::Bool:
            oss << (std::get<bool>(entry.value) ? "true" : "false");
            break;
        case ConfigDataType::String:
            oss << "\"" << std::get<std::string>(entry.value) << "\"";
            break;
        case ConfigDataType::IntVector:
        case ConfigDataType::DoubleVector:
        case ConfigDataType::StringVector:
        case ConfigDataType::Matrix:
            oss << configValueToString(entry.value);
            break;
        default:
            oss << configValueToString(entry.value);
            break;
    }
    
    return oss.str();
}

bool Config::parseJSONValue(const std::string& jsonStr, ConfigValue& value, ConfigDataType& type) {
    // Remove surrounding quotes
    std::string str = trim(jsonStr);
    if (str.empty()) return false;
    
    if (str.front() == '"' && str.back() == '"') {
        // String value
        type = ConfigDataType::String;
        value = str.substr(1, str.size() - 2);
        return true;
    } else if (str == "true" || str == "false") {
        // Boolean value
        type = ConfigDataType::Bool;
        value = (str == "true");
        return true;
    } else if (std::isdigit(str.front()) || (str.front() == '-' && str.size() > 1 && std::isdigit(str[1]))) {
        // Number value - try to determine type
        size_t pos;
        try {
            // Try int64 first (larger range)
            size_t pos64;
            if (str.find('.') == std::string::npos) {
                // Integer
                size_t endpos;
                int64_t intVal = std::stoll(str, &endpos);
                if (endpos == str.size()) {
                    value = intVal;
                    type = ConfigDataType::Int64;
                    return true;
                }
            } else {
                // Double
                double doubleVal = std::stod(str, &pos);
                if (pos == str.size()) {
                    value = doubleVal;
                    type = ConfigDataType::Double;
                    return true;
                }
            }
        } catch (const std::exception&) {
            // Fall through
        }
    } else if (str.front() == '[' && str.back() == ']') {
        // Array - try to determine type
        std::string arrayStr = str.substr(1, str.size() - 2);
        
        // Try to parse as numbers or strings
        std::vector<std::string> elements;
        size_t start = 0;
        for (size_t i = 0; i <= arrayStr.size(); ++i) {
            if (i == arrayStr.size() || arrayStr[i] == ',') {
                std::string element = trim(arrayStr.substr(start, i - start));
                if (!element.empty()) {
                    elements.push_back(element);
                }
                start = i + 1;
            }
        }
        
        if (!elements.empty()) {
            // Determine element type
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& elem : elements) {
                if (allInts) {
                    try {
                        size_t endpos;
                        std::stoll(elem, &endpos);
                        if (endpos != elem.size()) allInts = false;
                    } catch (...) {
                        allInts = false;
                    }
                }
                
                if (allDoubles) {
                    try {
                        size_t endpos;
                        std::stod(elem, &endpos);
                        if (endpos != elem.size()) allDoubles = false;
                    } catch (...) {
                        allDoubles = false;
                    }
                }
                
                if (allStrings && (elem.front() == '"' && elem.back() == '"')) {
                    // Quoted string
                    allStrings = true;
                } else if (allStrings) {
                    allStrings = false;
                }
            }
            
            if (allInts && !elements.empty()) {
                std::vector<int64_t> intVec;
                for (const auto& elem : elements) {
                    int64_t val = std::stoll(elem);
                    intVec.push_back(val);
                }
                value = intVec;
                type = ConfigDataType::IntVector;
                return true;
            } else if (allDoubles && !elements.empty()) {
                std::vector<double> doubleVec;
                for (const auto& elem : elements) {
                    double val = std::stod(elem);
                    doubleVec.push_back(val);
                }
                value = doubleVec;
                type = ConfigDataType::DoubleVector;
                return true;
            } else if (allStrings && !elements.empty()) {
                std::vector<std::string> strVec;
                for (const auto& elem : elements) {
                    std::string str = elem;
                    if ((str.front() == '"' && str.back() == '"') ||
                        (str.front() == '\'' && str.back() == '\'')) {
                        str = str.substr(1, str.size() - 2);
                    }
                    strVec.push_back(str);
                }
                value = strVec;
                type = ConfigDataType::StringVector;
                return true;
            }
        }
    }
    
    // Default to string
    type = ConfigDataType::String;
    value = jsonStr;
    return true;
}

} // namespace nlm