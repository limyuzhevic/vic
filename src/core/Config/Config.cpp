#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <limits>
#include <stdexcept>

// Simple JSON parser implementation for NLM configuration
namespace nlm {
    // Helper functions for JSON parsing
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool isDigit(char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    }
    
    bool isAlpha(char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size() * 2);
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        result += "\\u";
                        result += "00";
                        char hex[4];
                        sprintf(hex, "%02x", static_cast<unsigned char>(c));
                        result += hex;
                    } else {
                        result += c;
                    }
            }
        }
        return result;
    }
    
    // Simple JSON value representation
    class JSONValue {
    public:
        enum class Type {
            NULL_TYPE,
            BOOL,
            NUMBER,
            STRING,
            ARRAY,
            OBJECT
        };
        
        JSONValue() : type_(Type::NULL_TYPE) {}
        JSONValue(bool value) : type_(Type::BOOL), bool_value_(value) {}
        JSONValue(double value) : type_(Type::NUMBER), number_value_(value) {}
        JSONValue(const std::string& value) : type_(Type::STRING), string_value_(value) {}
        JSONValue(const std::vector<JSONValue>& value) : type_(Type::ARRAY), array_value_(value) {}
        JSONValue(const std::map<std::string, JSONValue>& value) : type_(Type::OBJECT), object_value_(value) {}
        
        Type getType() const { return type_; }
        
        bool asBool() const { return bool_value_; }
        double asNumber() const { return number_value_; }
        const std::string& asString() const { return string_value_; }
        const std::vector<JSONValue>& asArray() const { return array_value_; }
        const std::map<std::string, JSONValue>& asObject() const { return object_value_; }
        
    private:
        Type type_;
        union {
            bool bool_value_;
            double number_value_;
            std::string string_value_;
            std::vector<JSONValue> array_value_;
            std::map<std::string, JSONValue> object_value_;
        };
    };
    
    // JSON parser class
    class JSONParser {
    public:
        bool parseFromFile(const std::string& filepath, std::map<std::string, ConfigValue>& out) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            return parseFromString(content, out);
        }
        
        bool parseFromString(const std::string& jsonString, std::map<std::string, ConfigValue>& out) {
            // Skip whitespace
            size_t pos = 0;
            skipWhitespace(jsonString, pos);
            
            if (pos >= jsonString.length() || jsonString[pos] != '{') {
                return false;  // Expected object at root
            }
            
            pos++;  // Skip {
            skipWhitespace(jsonString, pos);
            
            while (pos < jsonString.length() && jsonString[pos] != '}') {
                // Parse key
                size_t keyStart = pos;
                if (!parseString(jsonString, pos, keyStart)) {
                    return false;
                }
                std::string key = jsonString.substr(keyStart, pos - keyStart - 2);  // Remove quotes
                
                skipWhitespace(jsonString, pos);
                
                if (pos >= jsonString.length() || jsonString[pos] != ':') {
                    return false;  // Expected :
                }
                
                pos++;  // Skip :
                skipWhitespace(jsonString, pos);
                
                // Parse value
                JSONValue value;
                if (!parseValue(jsonString, pos, value)) {
                    return false;
                }
                
                // Convert JSONValue to ConfigValue
                out[key] = jsonValueToConfigValue(value);
                
                skipWhitespace(jsonString, pos);
                
                if (pos >= jsonString.length()) {
                    return false;  // Expected }
                }
                
                if (jsonString[pos] == '}') {
                    break;
                } else if (jsonString[pos] == ',') {
                    pos++;  // Skip ,
                    skipWhitespace(jsonString, pos);
                } else {
                    return false;  // Expected , or }
                }
            }
            
            if (pos >= jsonString.length() || jsonString[pos] != '}') {
                return false;  // Expected }
            }
            
            return true;
        }
        
        std::string serializeToJSON(const std::map<std::string, ConfigValue>& data) const {
            std::string result = "{\n";
            
            bool first = true;
            for (const auto& [key, value] : data) {
                if (!first) {
                    result += ",\n";
                }
                first = false;
                
                result += "  \"" + escapeString(key) + "\": ";
                result += configValueToJSON(value);
            }
            
            result += "\n}\n";
            return result;
        }
        
    private:
        void skipWhitespace(const std::string& str, size_t& pos) {
            while (pos < str.length() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                pos++;
            }
        }
        
        bool parseString(const std::string& str, size_t& pos, size_t& startPos) {
            if (pos >= str.length() || str[pos] != '\"') {
                return false;
            }
            
            startPos = ++pos;
            while (pos < str.length()) {
                if (str[pos] == '\\') {
                    pos += 2;  // Skip escaped character
                    continue;
                } else if (str[pos] == '\"') {
                    break;
                }
                pos++;
            }
            
            if (pos >= str.length() || str[pos] != '\"') {
                return false;  // Unterminated string
            }
            
            return true;
        }
        
        bool parseValue(const std::string& str, size_t& pos, JSONValue& out) {
            skipWhitespace(str, pos);
            
            if (pos >= str.length()) {
                return false;
            }
            
            char c = str[pos];
            
            if (c == 'n' && str.substr(pos, 4) == "null") {
                pos += 4;
                out = JSONValue();
                return true;
            } else if (c == 't' && str.substr(pos, 4) == "true") {
                pos += 4;
                out = JSONValue(true);
                return true;
            } else if (c == 'f' && str.substr(pos, 5) == "false") {
                pos += 5;
                out = JSONValue(false);
                return true;
            } else if (c == '\"') {
                size_t start = ++pos;
                if (!parseString(str, pos, start)) {
                    return false;
                }
                out = JSONValue(str.substr(start, pos - start - 1));
                return true;
            } else if (c == '[') {
                pos++;  // Skip [
                std::vector<JSONValue> array;
                skipWhitespace(str, pos);
                
                if (pos < str.length() && str[pos] == ']') {
                    pos++;  // Skip ]
                    out = JSONValue(array);
                    return true;
                }
                
                while (true) {
                    JSONValue element;
                    if (!parseValue(str, pos, element)) {
                        return false;
                    }
                    
                    array.push_back(element);
                    skipWhitespace(str, pos);
                    
                    if (pos >= str.length()) {
                        return false;
                    }
                    
                    if (str[pos] == ']') {
                        pos++;  // Skip ]
                        break;
                    } else if (str[pos] == ',') {
                        pos++;  // Skip ,
                        skipWhitespace(str, pos);
                        continue;
                    } else {
                        return false;
                    }
                }
                
                out = JSONValue(array);
                return true;
            } else if (c == '{') {
                pos++;  // Skip {
                std::map<std::string, JSONValue> object;
                skipWhitespace(str, pos);
                
                if (pos < str.length() && str[pos] == '}') {
                    pos++;  // Skip }
                    out = JSONValue(object);
                    return true;
                }
                
                while (true) {
                    // Parse key
                    size_t keyStart;
                    if (!parseString(str, pos, keyStart)) {
                        return false;
                    }
                    std::string key = str.substr(keyStart, pos - keyStart - 1);
                    
                    skipWhitespace(str, pos);
                    
                    if (pos >= str.length() || str[pos] != ':') {
                        return false;
                    }
                    
                    pos++;  // Skip :
                    skipWhitespace(str, pos);
                    
                    // Parse value
                    JSONValue value;
                    if (!parseValue(str, pos, value)) {
                        return false;
                    }
                    
                    object[key] = value;
                    skipWhitespace(str, pos);
                    
                    if (pos >= str.length()) {
                        return false;
                    }
                    
                    if (str[pos] == '}') {
                        pos++;  // Skip }
                        break;
                    } else if (str[pos] == ',') {
                        pos++;  // Skip ,
                        skipWhitespace(str, pos);
                        continue;
                    } else {
                        return false;
                    }
                }
                
                out = JSONValue(object);
                return true;
            } else if (isDigit(c) || c == '-') {
                size_t start = pos;
                bool hasDecimal = false;
                bool hasExponent = false;
                
                while (pos < str.length()) {
                    c = str[pos];
                    if (isDigit(c)) {
                        pos++;
                    } else if (c == '.' && !hasDecimal) {
                        hasDecimal = true;
                        pos++;
                    } else if ((c == 'e' || c == 'E') && !hasExponent) {
                        hasExponent = true;
                        pos++;
                        if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) {
                            pos++;
                        }
                    } else {
                        break;
                    }
                }
                
                std::string numStr = str.substr(start, pos - start);
                try {
                    double value = std::stod(numStr);
                    out = JSONValue(value);
                    return true;
                } catch (...) {
                    return false;
                }
            }
            
            return false;
        }
        
        ConfigValue jsonValueToConfigValue(const JSONValue& jsonValue) const {
            switch (jsonValue.getType()) {
                case JSONValue::Type::NULL_TYPE:
                    return std::string();
                case JSONValue::Type::BOOL:
                    return jsonValue.asBool();
                case JSONValue::Type::NUMBER:
                    return jsonValue.asNumber();
                case JSONValue::Type::STRING:
                    return jsonValue.asString();
                case JSONValue::Type::ARRAY:
                    // Convert JSON array to appropriate ConfigValue type
                    const auto& arr = jsonValue.asArray();
                    if (arr.empty()) {
                        return std::vector<int>{};
                    }
                    
                    // Try to determine array type from first element
                    bool allInts = true;
                    bool allDoubles = true;
                    bool allStrings = true;
                    
                    for (const auto& element : arr) {
                        if (element.getType() != JSONValue::Type::NUMBER) {
                            allInts = false;
                            allDoubles = false;
                            allStrings = false;
                        } else {
                            if (element.asNumber() != static_cast<int64_t>(element.asNumber())) {
                                allInts = false;
                            }
                            if (std::isnan(element.asNumber())) {
                                allDoubles = false;
                                allInts = false;
                                allStrings = false;
                            }
                        }
                    }
                    
                    if (allInts) {
                        std::vector<int> result;
                        result.reserve(arr.size());
                        for (const auto& element : arr) {
                            result.push_back(static_cast<int>(element.asNumber()));
                        }
                        return result;
                    } else if (allDoubles) {
                        std::vector<double> result;
                        result.reserve(arr.size());
                        for (const auto& element : arr) {
                            result.push_back(element.asNumber());
                        }
                        return result;
                    } else {
                        // Default to string array
                        std::vector<std::string> result;
                        result.reserve(arr.size());
                        for (const auto& element : arr) {
                            if (element.getType() == JSONValue::Type::STRING) {
                                result.push_back(element.asString());
                            } else {
                                result.push_back(std::to_string(element.asNumber()));
                            }
                        }
                        return result;
                    }
                case JSONValue::Type::OBJECT:
                    // JSON objects are not directly supported in ConfigValue,
                    // return empty string as a placeholder
                    return std::string();
            }
            
            return std::string();
        }
        
        std::string configValueToJSON(const ConfigValue& value) const {
            std::string result;
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    result = "\"" + escapeString(val) + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    result = val ? "true" : "false";
                } else if constexpr (std::is_same_v<T, double> ||
                                     std::is_same_v<T, float> ||
                                     std::is_same_v<T, int> ||
                                     std::is_same_v<T, int64_t> ||
                                     std::is_same_v<T, uint64_t>) {
                    result = std::to_string(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    result = "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) result += ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            result += "\"" + escapeString(val[i]) + "\"";
                        } else {
                            result += std::to_string(val[i]);
                        }
                    }
                    result += "]";
                }
            }, value);
            
            return result;
        }
    };
    
    // Global JSON parser instance
    JSONParser g_jsonParser;
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <limits>
#include <stdexcept>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// Use RapidJSON for JSON parsing
namespace nlm {
    // Simple JSON parser wrapper for NLM configuration
    class JSONParser {
    public:
        bool parseFromFile(const std::string& filepath, std::map<std::string, ConfigValue>& out) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            rapidjson::IStreamWrapper wrapper(file);
            rapidjson::Document doc;
            
            if (doc.ParseStream(wrapper).HasParseError()) {
                return false;
            }
            
            parseDocument(doc, out);
            return true;
        }
        
        bool parseFromString(const std::string& jsonString, std::map<std::string, ConfigValue>& out) {
            rapidjson::Document doc;
            if (doc.Parse(jsonString.c_str()).HasParseError()) {
                return false;
            }
            
            parseDocument(doc, out);
            return true;
        }
        
        std::string serializeToJSON(const std::map<std::string, ConfigValue>& data) const {
            rapidjson::Document doc;
            doc.SetObject();
            auto& allocator = doc.GetAllocator();
            
            for (const auto& [key, value] : data) {
                rapidjson::Value keyValue(key.c_str(), allocator);
                doc.AddMember(keyValue, toRapidJSONValue(value, allocator), allocator);
            }
            
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);
            
            return buffer.GetString();
        }
        
    private:
        rapidjson::Value toRapidJSONValue(const ConfigValue& value, rapidjson::Document::AllocatorType& allocator) const {
            rapidjson::Value jsonValue;
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue.SetString(val.c_str(), allocator);
                } else if constexpr (std::is_same_v<T, int>) {
                    jsonValue.SetInt(val);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue.SetInt64(val);
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    jsonValue.SetUint64(val);
                } else if constexpr (std::is_same_v<T, float>) {
                    jsonValue.SetFloat(val);
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue.SetDouble(val);
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue.SetBool(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonValue.SetArray();
                    for (const auto& item : val) {
                        jsonValue.PushBack(item, allocator);
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue.SetArray();
                    for (const auto& item : val) {
                        jsonValue.PushBack(static_cast<double>(item), allocator);
                    }
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue.SetArray();
                    for (const auto& item : val) {
                        jsonValue.PushBack(rapidjson::Value(item.c_str(), allocator), allocator);
                    }
                }
            }, value);
            
            return jsonValue;
        }
        
        void parseDocument(const rapidjson::Document& doc, std::map<std::string, ConfigValue>& out) {
            if (!doc.IsObject()) {
                return;
            }
            
            for (const auto& member : doc.GetObject()) {
                std::string key = member.name.GetString();
                ConfigValue value = toConfigValue(member.value);
                out[key] = std::move(value);
            }
        }
        
        ConfigValue toConfigValue(const rapidjson::Value& value) {
            if (value.IsString()) {
                return value.GetString();
            } else if (value.IsInt()) {
                return value.GetInt();
            } else if (value.IsInt64()) {
                return value.GetInt64();
            } else if (value.IsUint64()) {
                return value.GetUint64();
            } else if (value.IsFloat()) {
                return value.GetFloat();
            } else if (value.IsDouble()) {
                return value.GetDouble();
            } else if (value.IsBool()) {
                return value.GetBool();
            } else if (value.IsArray()) {
                // Determine array element type
                if (value.Empty()) {
                    return std::vector<int>{};
                }
                
                // Try to infer type from first element
                if (value[0].IsInt()) {
                    std::vector<int> result;
                    result.reserve(value.Size());
                    for (const auto& item : value.GetArray()) {
                        result.push_back(item.GetInt());
                    }
                    return result;
                } else if (value[0].IsDouble()) {
                    std::vector<double> result;
                    result.reserve(value.Size());
                    for (const auto& item : value.GetArray()) {
                        result.push_back(item.GetDouble());
                    }
                    return result;
                } else if (value[0].IsString()) {
                    std::vector<std::string> result;
                    result.reserve(value.Size());
                    for (const auto& item : value.GetArray()) {
                        result.push_back(item.GetString());
                    }
                    return result;
                }
                
                // Default to int array if type cannot be determined
                std::vector<int> result;
                result.reserve(value.Size());
                for (const auto& item : value.GetArray()) {
                    if (item.IsInt()) {
                        result.push_back(item.GetInt());
                    } else {
                        result.push_back(static_cast<int>(item.GetDouble()));
                    }
                }
                return result;
            }
            
            return std::string(); // Default empty string
        }
    };
    
    // Global JSON parser instance
    JSONParser g_jsonParser;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to load as JSON
    std::map<std::string, ConfigValue> jsonData;
    if (g_jsonParser.parseFromFile(filepath, jsonData)) {
        // Successfully parsed as JSON, merge with existing config
        for (const auto& [key, value] : jsonData) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
    // Check file extension to determine format
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < filepath.length()) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (extension == "json" || extension == "cfg") {
        return saveToJSON(filepath);
    } else {
        return saveToKeyValue(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Convert config entries to map format
    std::map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    // Serialize to JSON
    std::string jsonString = g_jsonParser.serializeToJSON(configMap);
    file << jsonString;
    
    return true;
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value appropriately
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << val[i] << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]";
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
