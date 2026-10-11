#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <regex>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ValidationConstraint> validations;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[CONFIG] Config initialized" << std::endl;
}

Config::~Config() {
    std::cout << "[CONFIG] Config destroyed" << std::endl;
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::cout << "[CONFIG] Loading configuration from: " << filepath << std::endl;
        
        if (!isFileExists(filepath)) {
            std::cout << "[CONFIG] WARNING: Configuration file not found: " << filepath << std::endl;
            return false;
        }
        
        std::string format = detectFileFormat(filepath);
        std::cout << "[CONFIG] Detected file format: " << format << std::endl;
        
        bool success = false;
        if (format == "json") {
            success = loadFromJsonFile(filepath);
        } else if (format == "yaml" || format == "yml") {
            success = loadFromYamlFile(filepath);
        } else {
            std::cout << "[CONFIG] Using legacy key=value format" << std::endl;
            success = loadFromLegacyFile(filepath);
        }
        
        if (success) {
            std::cout << "[CONFIG] Successfully loaded " << pImpl->entries.size() << " configuration entries" << std::endl;
            validateAll();
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        std::cout << "[CONFIG] ERROR loading configuration from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    try {
        rapidjson::Document doc = parseJsonFile(filepath);
        
        if (!doc.IsObject()) {
            throw ConfigParseError(filepath, "JSON file root is not an object");
        }
        
        pImpl->entries.clear();
        
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            const std::string& key = it->name.GetString();
            ConfigValue value = jsonValueToConfigValue(it->value, key);
            
            std::cout << "[CONFIG] Loaded JSON entry: " << key << " = " << valueToString(value) << std::endl;
            pImpl->entries.emplace_back(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cout << "[CONFIG] JSON parsing error: " << e.what() << std::endl;
        throw;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    try {
        YAML::Node root = parseYamlFile(filepath);
        
        pImpl->entries.clear();
        
        extractFromYamlNode(root, "");
        
        std::cout << "[CONFIG] Loaded YAML configuration with " << pImpl->entries.size() << " entries" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "[CONFIG] YAML parsing error: " << e.what() << std::endl;
        throw;
    }
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[CONFIG] ERROR: Cannot open configuration file: " << filepath << std::endl;
        return false;
    }
    
    pImpl->entries.clear();
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        ParseResult result = parseLine(line);
        
        if (result.valid) {
            ConfigValue value = parseValue(result.rawValue, result.key);
            std::cout << "[CONFIG] Loaded legacy entry: " << result.key << " = " << result.rawValue << std::endl;
            pImpl->entries.emplace_back(result.key, value, ConfigSource::File);
        } else {
            std::cout << "[CONFIG] Skipping invalid line " << lineNumber << " in " << filepath 
                     << ": " << result.errorMessage << std::endl;
        }
    }
    
    std::cout << "[CONFIG] Loaded legacy configuration with " << pImpl->entries.size() << " entries" << std::endl;
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        std::cout << "[CONFIG] Loading configuration from command line arguments" << std::endl;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(arg.substr(2, pos - 2));
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                    std::cout << "[CONFIG] Set from command line --" << key << "=" << value << std::endl;
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = trim(arg.substr(1));
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
                std::cout << "[CONFIG] Set from command line -" << key << "=" << value << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cout << "[CONFIG] ERROR loading command line arguments: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::cout << "[CONFIG] Saving configuration to: " << filepath << std::endl;
        
        std::string format = getFileExtension(filepath);
        std::cout << "[CONFIG] Using " << format << " format for saving" << std::endl;
        
        bool success = false;
        if (format == "json") {
            success = saveToJsonFile(filepath);
        } else if (format == "yaml" || format == "yml") {
            success = saveToYamlFile(filepath);
        } else {
            std::cout << "[CONFIG] Using legacy key=value format for saving" << std::endl;
            success = saveToLegacyFile(filepath);
        }
        
        if (success) {
            std::cout << "[CONFIG] Successfully saved configuration" << std::endl;
        } else {
            std::cout << "[CONFIG] ERROR: Failed to save configuration" << std::endl;
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cout << "[CONFIG] ERROR saving configuration: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    rapidjson::Document doc;
    doc.SetObject();
    
    auto& allocator = doc.GetAllocator();
    
    for (const auto& entry : pImpl->entries) {
        rapidjson::Value key(key.c_str(), allocator);
        rapidjson::Value value = configValueToJson(entry.value, allocator);
        doc.AddMember(key, value, allocator);
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadError(filepath, "Cannot open file for writing");
    }
    
    file << buffer.GetString();
    std::cout << "[CONFIG] Saved JSON configuration to: " << filepath << std::endl;
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYaml(entry.value);
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadError(filepath, "Cannot open file for writing");
    }
    
    file << emitter.c_str();
    std::cout << "[CONFIG] Saved YAML configuration to: " << filepath << std::endl;
    return true;
}

bool Config::saveToLegacyFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadError(filepath, "Cannot open file for writing");
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    std::cout << "[CONFIG] Saved legacy configuration to: " << filepath << std::endl;
    return true;
}

void Config::extractFromYamlNode(const YAML::Node& node, const std::string& prefix) {
    if (node.IsMap()) {
        for (const auto& it : node) {
            std::string key = prefix.empty() ? it.first.as<std::string>() : 
                prefix + "." + it.first.as<std::string>();
            extractFromYamlNode(it.second, key);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            extractFromYamlNode(node[i], key);
        }
    } else {
        ConfigValue value = yamlValueToConfigValue(node, prefix);
        if (!prefix.empty()) {
            std::cout << "[CONFIG] Loaded YAML entry: " << prefix << " = " << valueToString(value) << std::endl;
            pImpl->entries.emplace_back(prefix, value, ConfigSource::File);
        }
    }
}

rapidjson::Document Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadError(filepath, "Cannot open JSON file");
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    rapidjson::Document doc;
    doc.Parse(buffer.str().c_str());
    
    if (doc.HasParseError()) {
        throw ConfigParseError(filepath, "JSON parse error at offset " + 
            std::to_string(doc.GetParseErrorOffset()) + ". " +
            std::string(rapidjson::GetParseError_En(doc.GetParseError())));
    }
    
    return doc;
}

YAML::Node Config::parseYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadError(filepath, "Cannot open YAML file");
    }
    
    try {
        return YAML::LoadFile(filepath);
    } catch (const YAML::Exception& e) {
        throw ConfigParseError(filepath, std::string("YAML parse error: ") + e.what());
    }
}

ParseResult Config::parseLine(const std::string& line) {
    ParseResult result;
    result.valid = false;
    
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
        result.valid = false;
        return result;
    }
    
    size_t pos = trimmed.find('=');
    if (pos == std::string::npos) {
        result.valid = false;
        result.errorMessage = "No '=' found in line";
        return result;
    }
    
    result.key = trim(trimmed.substr(0, pos));
    result.rawValue = trim(trimmed.substr(pos + 1));
    result.source = "file";
    
    if (result.key.empty() || result.rawValue.empty()) {
        result.valid = false;
        result.errorMessage = "Empty key or value";
        return result;
    }
    
    // Remove quotes from value
    if (result.rawValue.size() >= 2 && 
        ((result.rawValue.front() == '"' && result.rawValue.back() == '"') ||
         (result.rawValue.front() == '\'' && result.rawValue.back() == '\''))) {
        result.rawValue = result.rawValue.substr(1, result.rawValue.size() - 2);
    }
    
    result.valid = true;
    return result;
}

ConfigValue Config::parseValue(const std::string& valueStr, const std::string& key) {
    if (valueStr.empty()) {
        return std::string();
    }
    
    // Try to parse as boolean
    if (isBoolean(valueStr)) {
        return valueStr == "true" || valueStr == "1";
    }
    
    // Try to parse as integer
    if (isInteger(valueStr)) {
        return std::stoll(valueStr);
    }
    
    // Try to parse as double
    if (isFloat(valueStr)) {
        return std::stod(valueStr);
    }
    
    // Check for list format [item1,item2,item3]
    if (valueStr.front() == '[' && valueStr.back() == ']') {
        std::string inner = valueStr.substr(1, valueStr.size() - 2);
        if (inner.empty()) {
            return std::vector<std::string>();
        }
        
        // Determine list type based on first non-empty element
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream stream(inner);
        while (std::getline(stream, token, ',')) {
            token = trim(token);
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        if (tokens.empty()) {
            return std::vector<std::string>();
        }
        
        // Try to parse as integer list
        bool allInts = true;
        for (const auto& t : tokens) {
            if (!isInteger(t)) {
                allInts = false;
                break;
            }
        }
        if (allInts) {
            std::vector<int> intList;
            for (const auto& t : tokens) {
                intList.push_back(std::stoi(t));
            }
            return intList;
        }
        
        // Try to parse as double list
        bool allFloats = true;
        for (const auto& t : tokens) {
            if (!isFloat(t)) {
                allFloats = false;
                break;
            }
        }
        if (allFloats) {
            std::vector<double> doubleList;
            for (const auto& t : tokens) {
                doubleList.push_back(std::stod(t));
            }
            return doubleList;
        }
        
        // Fall back to string list
        return tokens;
    }
    
    // Default to string
    return valueStr;
}

ConfigValue Config::jsonValueToConfigValue(const rapidjson::Value& jsonValue, const std::string& key) {
    if (jsonValue.IsString()) {
        return jsonValue.GetString();
    } else if (jsonValue.IsBool()) {
        return jsonValue.GetBool();
    } else if (jsonValue.IsInt()) {
        return jsonValue.GetInt();
    } else if (jsonValue.IsInt64()) {
        return jsonValue.GetInt64();
    } else if (jsonValue.IsUint64()) {
        return static_cast<int64_t>(jsonValue.GetUint64());
    } else if (jsonValue.IsDouble()) {
        return jsonValue.GetDouble();
    } else if (jsonValue.IsArray()) {
        std::vector<ConfigValue> vectorValues;
        for (const auto& element : jsonValue.GetArray()) {
            vectorValues.push_back(jsonValueToConfigValue(element, key));
        }
        
        // Try to convert to specific type
        if (vectorValues.empty()) {
            return std::vector<std::string>();
        }
        
        // Check if all are integers
        bool allInts = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<int>(v); });
        if (allInts) {
            std::vector<int> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<int>(v));
            }
            return result;
        }
        
        // Check if all are doubles
        bool allDoubles = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<double>(v); });
        if (allDoubles) {
            std::vector<double> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<double>(v));
            }
            return result;
        }
        
        // Check if all are strings
        bool allStrings = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); });
        if (allStrings) {
            std::vector<std::string> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<std::string>(v));
            }
            return result;
        }
        
        // Mixed types - fall back to string representation
        std::string combined;
        for (size_t i = 0; i < vectorValues.size(); ++i) {
            if (i > 0) combined += ",";
            combined += valueToString(vectorValues[i]);
        }
        return combined;
    }
    
    return std::string(); // Default for null/unknown values
}

rapidjson::Value Config::configValueToJson(const ConfigValue& configValue, rapidjson::Document::AllocatorType& allocator) {
    if (std::holds_alternative<int>(configValue)) {
        return rapidjson::Value(std::get<int>(configValue));
    } else if (std::holds_alternative<int64_t>(configValue)) {
        return rapidjson::Value(std::get<int64_t>(configValue));
    } else if (std::holds_alternative<double>(configValue)) {
        return rapidjson::Value(std::get<double>(configValue));
    } else if (std::holds_alternative<bool>(configValue)) {
        return rapidjson::Value(std::get<bool>(configValue));
    } else if (std::holds_alternative<std::string>(configValue)) {
        return rapidjson::Value(std::get<std::string>(configValue).c_str(), allocator);
    } else if (std::holds_alternative<std::vector<int>>(configValue)) {
        rapidjson::Value array(rapidjson::kArrayType);
        for (const auto& item : std::get<std::vector<int>>(configValue)) {
            array.PushBack(item, allocator);
        }
        return array;
    } else if (std::holds_alternative<std::vector<double>>(configValue)) {
        rapidjson::Value array(rapidjson::kArrayType);
        for (const auto& item : std::get<std::vector<double>>(configValue)) {
            array.PushBack(item, allocator);
        }
        return array;
    } else if (std::holds_alternative<std::vector<std::string>>(configValue)) {
        rapidjson::Value array(rapidjson::kArrayType);
        for (const auto& item : std::get<std::vector<std::string>>(configValue)) {
            array.PushBack(item.c_str(), allocator);
        }
        return array;
    }
    
    return rapidjson::Value(); // Default for null
}

ConfigValue Config::yamlValueToConfigValue(const YAML::Node& yamlValue, const std::string& key) {
    if (yamlValue.IsScalar()) {
        std::string value = yamlValue.as<std::string>();
        return parseValue(value, key);
    } else if (yamlValue.IsSequence()) {
        std::vector<ConfigValue> vectorValues;
        for (const auto& item : yamlValue) {
            vectorValues.push_back(yamlValueToConfigValue(item, key));
        }
        
        // Try to convert to specific type
        if (vectorValues.empty()) {
            return std::vector<std::string>();
        }
        
        // Check if all are integers
        bool allInts = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<int>(v); });
        if (allInts) {
            std::vector<int> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<int>(v));
            }
            return result;
        }
        
        // Check if all are doubles
        bool allDoubles = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<double>(v); });
        if (allDoubles) {
            std::vector<double> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<double>(v));
            }
            return result;
        }
        
        // Check if all are strings
        bool allStrings = std::all_of(vectorValues.begin(), vectorValues.end(),
            [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); });
        if (allStrings) {
            std::vector<std::string> result;
            for (const auto& v : vectorValues) {
                result.push_back(std::get<std::string>(v));
            }
            return result;
        }
        
        // Mixed types - fall back to string representation
        std::string combined;
        for (size_t i = 0; i < vectorValues.size(); ++i) {
            if (i > 0) combined += ",";
            combined += valueToString(vectorValues[i]);
        }
        return combined;
    }
    
    return std::string(); // Default for null
}

YAML::Node Config::configValueToYaml(const ConfigValue& configValue) {
    if (std::holds_alternative<int>(configValue)) {
        return YAML::Node(std::get<int>(configValue));
    } else if (std::holds_alternative<int64_t>(configValue)) {
        return YAML::Node(std::get<int64_t>(configValue));
    } else if (std::holds_alternative<double>(configValue)) {
        return YAML::Node(std::get<double>(configValue));
    } else if (std::holds_alternative<bool>(configValue)) {
        return YAML::Node(std::get<bool>(configValue));
    } else if (std::holds_alternative<std::string>(configValue)) {
        return YAML::Node(std::get<std::string>(configValue));
    } else if (std::holds_alternative<std::vector<int>>(configValue)) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<int>>(configValue)) {
            node.push_back(item);
        }
        return node;
    } else if (std::holds_alternative<std::vector<double>>(configValue)) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<double>>(configValue)) {
            node.push_back(item);
        }
        return node;
    } else if (std::holds_alternative<std::vector<std::string>>(configValue)) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<std::string>>(configValue)) {
            node.push_back(item);
        }
        return node;
    }
    
    return YAML::Node(); // Default for null
}

std::string Config::valueToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            std::ostringstream voss;
            voss << std::fixed << std::setprecision(6) << vec[i];
            oss << voss.str();
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    
    return "";
}

bool Config::isBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

bool Config::isInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool Config::isFloat(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    size_t start = 0;
    
    if (str[0] == '-') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            continue;
        } else if (str[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if ((str[i] == 'e' || str[i] == 'E') && i > start) {
            if (hasExponent) return false;
            hasExponent = true;
        } else {
            return false;
        }
    }
    
    return true;
}

bool Config::isNumeric(const std::string& str) {
    return isInteger(str) || isFloat(str);
}

std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::string Config::detectFileFormat(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json") {
        return "json";
    } else if (ext == "yaml" || ext == "yml") {
        return "yaml";
    } else if (ext == "cfg" || ext == "txt" || ext.empty()) {
        return "legacy";
    }
    
    return "legacy";
}

bool Config::isFileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

bool Config::isJsonFile(const std::string& filepath) {
    return getFileExtension(filepath) == "json";
}

bool Config::isYamlFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return ext == "yaml" || ext == "yml";
}

bool Config::isLegacyConfigFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return ext == "cfg" || ext == "txt" || ext.empty();
}

bool ValidationConstraint::validate(const ConfigValue& value) const {
    switch (type) {
        case ValidationType::Range:
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                return val >= minValue && val <= maxValue;
            } else if (std::holds_alternative<int64_t>(value)) {
                int64_t val = std::get<int64_t>(value);
                return static_cast<double>(val) >= minValue && static_cast<double>(val) <= maxValue;
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val >= minValue && val <= maxValue;
            }
            break;
        
        case ValidationType::Enum:
            if (std::holds_alternative<std::string>(value)) {
                const std::string& val = std::get<std::string>(value);
                return std::find(enumValues.begin(), enumValues.end(), val) != enumValues.end();
            }
            break;
        
        case ValidationType::Pattern:
            if (std::holds_alternative<std::string>(value)) {
                const std::string& val = std::get<std::string>(value);
                return std::regex_match(val, std::regex(patternRegex));
            }
            break;
        
        default:
            return true;
    }
    
    return false;
}

std::string ValidationConstraint::getTypeName() const {
    switch (type) {
        case ValidationType::Range: return "Range";
        case ValidationType::Enum: return "Enum";
        case ValidationType::Pattern: return "Pattern";
        case ValidationType::Custom: return "Custom";
        default: return "None";
    }
}

void Config::addValidation(const std::string& key, const ValidationConstraint& constraint) {
    pImpl->validations[key] = constraint;
    std::cout << "[CONFIG] Added validation for key '" << key << "': " << constraint.getTypeName() 
              << " (" << constraint.minValue << " to " << constraint.maxValue << ")" << std::endl;
}

bool Config::validate(const std::string& key) const {
    auto it = pImpl->validations.find(key);
    if (it == pImpl->validations.end()) {
        return true; // No validation defined for this key
    }
    
    const ValidationConstraint& constraint = it->second;
    if (!constraint.enabled) {
        return true;
    }
    
    // Find the value in entries
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return true; // Key doesn't exist yet
    }
    
    if (constraint.validate(entryIt->value)) {
        return true;
    }
    
    std::string error = constraint.errorMessage.empty() ? 
        "Validation failed" : constraint.errorMessage;
    
    std::cout << "[CONFIG] ERROR: Validation failed for key '" << key << "': " << error << std::endl;
    return false;
}

bool Config::validateAll() const {
    pImpl->validationErrors.clear();
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            allValid = false;
        }
    }
    
    return allValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
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
        std::cout << "[CONFIG] ERROR: Type mismatch when getting key '" << key << "': expected " 
                  << typeid(T).name() << std::endl;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    std::cout << "[CONFIG] Using default value for key '" << key << "': " 
              << valueToString(ConfigValue(defaultValue)) << std::endl;
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::cout << "[CONFIG] Updating existing config key '" << key << "': " 
                  << valueToString(it->value) << " -> " << valueToString(value) << std::endl;
        it->value = value;
        it->source = source;
        
        // Re-validate if needed
        validate(key);
    } else {
        std::cout << "[CONFIG] Adding new config key '" << key << "': " 
                  << valueToString(value) << " (source: " << static_cast<int>(source) << ")" << std::endl;
        pImpl->entries.emplace_back(key, value, source);
        
        // Validate if needed
        if (!validate(key)) {
            std::cout << "[CONFIG] WARNING: Config key '" << key << "' added with potential validation issues" << std::endl;
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = parseValue(value, key);
    set(key, configValue, source);
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
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::cout << "[CONFIG] Removed config key '" << key << "'" << std::endl;
        pImpl->entries.erase(it, pImpl->entries.end());
    }
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
    std::cout << "[CONFIG] Clearing all configuration entries" << std::endl;
    pImpl->entries.clear();
    pImpl->validations.clear();
    pImpl->validationErrors.clear();
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