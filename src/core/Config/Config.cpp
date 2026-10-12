#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <filesystem>
#include <system_error>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigParserError> parserErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

std::string Config::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "none";
    }
    
    char firstChar;
    file.get(firstChar);
    
    if (firstChar == '{') {
        return "json";
    } else if (firstChar == '[' || std::isalpha(firstChar)) {
        // Could be JSON array or YAML
        // Try to parse as JSON first
        file.seekg(0);
        nlohmann::json jsonData;
        try {
            file >> jsonData;
            return "json";
        } catch (...) {
            file.seekg(0);
            try {
                YAML::Node yamlData = YAML::Load(file.str());
                return "yaml";
            } catch (...) {
                return "unknown";
            }
        }
    } else if (firstChar == '#') {
        return "simple";
    }
    
    return "unknown";
}

bool Config::isJSON(const std::string& data) {
    try {
        nlohmann::json::parse(data);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isYAML(const std::string& data) {
    try {
        YAML::Load(data);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isSimple(const std::string& data) {
    std::istringstream stream(data);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<ConfigParserError> Config::getParserErrors() const {
    return pImpl->parserErrors;
}

void Config::clearParserErrors() {
    pImpl->parserErrors.clear();
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::loadFromString(const std::string& data, const std::string& format) {
    clearParserErrors();
    
    std::string detectedFormat = format;
    if (format == "auto") {
        if (isJSON(data)) {
            detectedFormat = "json";
        } else if (isYAML(data)) {
            detectedFormat = "yaml";
        } else if (isSimple(data)) {
            detectedFormat = "simple";
        } else {
            pImpl->parserErrors.push_back({"Unable to detect format from data", ""});
            return false;
        }
    }
    
    if (detectedFormat == "json") {
        return loadFromJSONString(data);
    } else if (detectedFormat == "yaml") {
        return loadFromYAMLString(data);
    } else if (detectedFormat == "simple") {
        return parseSimpleFormat(data);
    }
    
    pImpl->parserErrors.push_back({"Unsupported format: " + detectedFormat, ""});
    return false;
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

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string detectedFormat = format;
    if (format == "auto") {
        detectedFormat = detectFormat(filepath);
        if (detectedFormat == "none" || detectedFormat == "unknown") {
            detectedFormat = "json";
        }
    }
    
    if (detectedFormat == "json") {
        return toJSONFile(filepath);
    } else if (detectedFormat == "yaml") {
        return toYAMLFile(filepath);
    } else if (detectedFormat == "simple") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << convertValueToString(entry.value) << "\n";
        }
        
        return true;
    }
    
    return false;
}

nlohmann::json Config::exportToJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Handle different types
        if (std::holds_alternative<int>(entry.value)) {
            jsonData[key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            jsonData[key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            jsonData[key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            jsonData[key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            jsonData[key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            jsonData[key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            jsonData[key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            jsonData[key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    return jsonData;
}

YAML::Node Config::exportToYAML() const {
    YAML::Node yamlData;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Handle different types
        if (std::holds_alternative<int>(entry.value)) {
            yamlData[key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            yamlData[key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            yamlData[key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            yamlData[key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            yamlData[key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            yamlData[key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            yamlData[key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            yamlData[key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    return yamlData;
}

std::string Config::toJSON() const {
    nlohmann::json jsonData = exportToJSON();
    return jsonData.dump(2);
}

bool Config::toJSONFile(const std::string& filepath) const {
    nlohmann::json jsonData = exportToJSON();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(2);
    return true;
}

bool Config::toYAMLFile(const std::string& filepath) const {
    YAML::Node yamlData = exportToYAML();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << yamlData;
    return true;
}

std::string Config::toYAML() const {
    YAML::Node yamlData = exportToYAML();
    return YAML::Dump(yamlData);
}

bool Config::parseSimpleFormat(const std::string& data) {
    std::istringstream stream(data);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
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
            
            // Convert to appropriate type based on content
            ConfigValue configValue = convertStringToValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseJSON(const nlohmann::json& jsonData) {
    try {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            nlohmann::json value = *it;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Try to determine array type
                bool allInts = std::all_of(value.begin(), value.end(), 
                    [](const nlohmann::json& v) { return v.is_number_integer(); });
                bool allFloats = std::all_of(value.begin(), value.end(), 
                    [](const nlohmann::json& v) { return v.is_number_float(); });
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    for (const auto& v : value) {
                        intArray.push_back(v.get<int64_t>());
                    }
                    // Convert to vector<int> if possible
                    configValue = std::vector<int>();
                    auto& intVec = std::get<std::vector<int>>(configValue);
                    for (int64_t val : intArray) {
                        if (val <= std::numeric_limits<int>::max() && val >= std::numeric_limits<int>::min()) {
                            intVec.push_back(static_cast<int>(val));
                        }
                    }
                } else if (allFloats) {
                    std::vector<double> doubleArray;
                    for (const auto& v : value) {
                        doubleArray.push_back(v.get<double>());
                    }
                    configValue = doubleArray;
                } else {
                    // String array
                    std::vector<std::string> stringArray;
                    for (const auto& v : value) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::JSON);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("JSON parsing error: ") + e.what(), ""});
        return false;
    }
}

bool Config::parseYAML(const YAML::Node& yamlData) {
    try {
        if (!yamlData.IsMap()) {
            pImpl->parserErrors.push_back({"YAML data is not a mapping", ""});
            return false;
        }
        
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            YAML::Node value = pair.second;
            
            // Convert YAML value to ConfigValue
            ConfigValue configValue;
            
            if (value.IsScalar()) {
                // Try to determine scalar type
                if (value.IsInt()) {
                    configValue = value.as<int64_t>();
                } else if (value.IsDouble()) {
                    configValue = value.as<double>();
                } else if (value.IsBoolean()) {
                    configValue = value.as<bool>();
                } else {
                    configValue = value.as<std::string>();
                }
            } else if (value.IsSequence()) {
                // Try to determine sequence type
                bool allInts = std::all_of(value.begin(), value.end(), 
                    [](const YAML::Node& v) { return v.IsInt(); });
                bool allFloats = std::all_of(value.begin(), value.end(), 
                    [](const YAML::Node& v) { return v.IsDouble(); });
                
                if (allInts) {
                    std::vector<int> intArray;
                    for (const auto& v : value) {
                        intArray.push_back(v.as<int>());
                    }
                    configValue = intArray;
                } else if (allFloats) {
                    std::vector<double> doubleArray;
                    for (const auto& v : value) {
                        doubleArray.push_back(v.as<double>());
                    }
                    configValue = doubleArray;
                } else {
                    // String array
                    std::vector<std::string> stringArray;
                    for (const auto& v : value) {
                        stringArray.push_back(v.as<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::YAML);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("YAML parsing error: ") + e.what(), ""});
        return false;
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    clearParserErrors();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->parserErrors.push_back({"Failed to open JSON file: " + filepath, filepath});
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        return parseJSON(jsonData);
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("JSON file parsing error: ") + e.what(), filepath});
        return false;
    }
}

bool Config::loadFromJSONString(const std::string& jsonString) {
    clearParserErrors();
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        return parseJSON(jsonData);
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("JSON string parsing error: ") + e.what(), ""});
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    clearParserErrors();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->parserErrors.push_back({"Failed to open YAML file: " + filepath, filepath});
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        return parseYAML(yamlData);
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("YAML file parsing error: ") + e.what(), filepath});
        return false;
    }
}

bool Config::loadFromYAMLString(const std::string& yamlString) {
    clearParserErrors();
    
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        return parseYAML(yamlData);
    } catch (const std::exception& e) {
        pImpl->parserErrors.push_back({std::string("YAML string parsing error: ") + e.what(), ""});
        return false;
    }
}

std::string Config::convertValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += "\"" + vec[i] + "\"";
        }
        result += "]";
        return result;
    }
    
    return "";
}

ConfigValue Config::convertStringToValue(const std::string& str, const std::string& typeHint) {
    // Try to parse as boolean
    if (typeHint == "bool" || str == "true" || str == "false") {
        if (str == "true") return true;
        if (str == "false") return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intValue;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleValue;
        }
    } catch (...) {}
    
    // Parse as string (default)
    return str;
}

bool Config::compareJSON(const std::string& jsonString) const {
    try {
        nlohmann::json thisJson = exportToJSON();
        nlohmann::json otherJson = nlohmann::json::parse(jsonString);
        
        return thisJson == otherJson;
    } catch (...) {
        return false;
    }
}

std::string Config::diffJSON(const std::string& jsonString) const {
    try {
        nlohmann::json thisJson = exportToJSON();
        nlohmann::json otherJson = nlohmann::json::parse(jsonString);
        
        std::ostringstream diff;
        
        // Check for differences in this.json vs other.json
        for (auto it = thisJson.begin(); it != thisJson.end(); ++it) {
            std::string key = it.key();
            if (!otherJson.contains(key)) {
                diff << "+ " << key << ": " << it.value() << "\n";
            } else if (it.value() != otherJson[key]) {
                diff << "~ " << key << ": " << it.value() << " != " << otherJson[key] << "\n";
            }
        }
        
        // Check for keys in other.json but not in this.json
        for (auto it = otherJson.begin(); it != otherJson.end(); ++it) {
            std::string key = it.key();
            if (!thisJson.contains(key)) {
                diff << "- " << key << ": " << it.value() << "\n";
            }
        }
        
        return diff.str();
    } catch (...) {
        return "Failed to compare JSON";
    }
}

std::string Config::getJSONSchema() const {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["description"] = "NLM Configuration Schema";
    
    nlohmann::json properties = nlohmann::json::object();
    for (const auto& entry : pImpl->entries) {
        nlohmann::json prop;
        
        // Determine property type
        if (std::holds_alternative<int>(entry.value)) {
            prop["type"] = "integer";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            prop["type"] = "integer";
        } else if (std::holds_alternative<double>(entry.value)) {
            prop["type"] = "number";
        } else if (std::holds_alternative<bool>(entry.value)) {
            prop["type"] = "boolean";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            prop["type"] = "string";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            prop["type"] = "array";
            prop["items"]["type"] = "integer";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            prop["type"] = "array";
            prop["items"]["type"] = "number";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            prop["type"] = "array";
            prop["items"]["type"] = "string";
        }
        
        prop["description"] = entry.description;
        prop["default"] = convertValueToString(entry.value);
        
        properties[entry.key] = prop;
    }
    
    schema["properties"] = properties;
    
    nlohmann::json required = nlohmann::json::array();
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            required.push_back(entry.key);
        }
    }
    schema["required"] = required;
    
    return schema.dump(2);
}

std::shared_ptr<Config> Config::fromJSON(const std::string& jsonString) {
    auto config = std::make_shared<Config>();
    if (config->loadFromJSONString(jsonString)) {
        return config;
    }
    return nullptr;
}

std::shared_ptr<Config> Config::fromYAML(const std::string& yamlString) {
    auto config = std::make_shared<Config>();
    if (config->loadFromYAMLString(yamlString)) {
        return config;
    }
    return nullptr;
}

std::shared_ptr<Config> Config::mergeJSON(const std::shared_ptr<Config>& base, 
                                         const std::string& jsonString) {
    if (!base) {
        return fromJSON(jsonString);
    }
    
    auto merged = std::make_shared<Config>();
    
    // Copy existing entries
    for (const auto& entry : base->getEntries()) {
        merged->set(entry.key, entry.value, entry.source);
    }
    
    // Parse and merge new entries
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            if (std::holds_alternative<int64_t>(it.value())) {
                merged->set(key, it.value().get<int64_t>(), ConfigSource::JSON);
            } else if (std::holds_alternative<double>(it.value())) {
                merged->set(key, it.value().get<double>(), ConfigSource::JSON);
            } else if (std::holds_alternative<bool>(it.value())) {
                merged->set(key, it.value().get<bool>(), ConfigSource::JSON);
            } else if (it.value().is_string()) {
                merged->set(key, it.value().get<std::string>(), ConfigSource::JSON);
            } else if (it.value().is_array()) {
                // Handle array types
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                bool allInts = std::all_of(it.value().begin(), it.value().end(), 
                    [](const nlohmann::json& v) { return v.is_number_integer(); });
                bool allStrings = std::all_of(it.value().begin(), it.value().end(), 
                    [](const nlohmann::json& v) { return v.is_string(); });
                
                if (allInts) {
                    for (const auto& v : it.value()) {
                        intArray.push_back(v.get<int>());
                    }
                    merged->set(key, intArray, ConfigSource::JSON);
                } else if (allStrings) {
                    for (const auto& v : it.value()) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    merged->set(key, stringArray, ConfigSource::JSON);
                } else {
                    // Default to string array
                    for (const auto& v : it.value()) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    merged->set(key, stringArray, ConfigSource::JSON);
                }
            }
        }
    } catch (...) {
        // If JSON parsing fails, return the original config
        return base;
    }
    
    return merged;
}

std::shared_ptr<Config> Config::mergeYAML(const std::shared_ptr<Config>& base,
                                         const std::string& yamlString) {
    if (!base) {
        return fromYAML(yamlString);
    }
    
    auto merged = std::make_shared<Config>();
    
    // Copy existing entries
    for (const auto& entry : base->getEntries()) {
        merged->set(entry.key, entry.value, entry.source);
    }
    
    // Parse and merge new entries
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            YAML::Node value = pair.second;
            
            if (value.IsInt()) {
                merged->set(key, value.as<int>(), ConfigSource::YAML);
            } else if (value.IsDouble()) {
                merged->set(key, value.as<double>(), ConfigSource::YAML);
            } else if (value.IsBoolean()) {
                merged->set(key, value.as<bool>(), ConfigSource::YAML);
            } else if (value.IsScalar()) {
                merged->set(key, value.as<std::string>(), ConfigSource::YAML);
            } else if (value.IsSequence()) {
                // Check sequence type
                bool allInts = std::all_of(value.begin(), value.end(), 
                    [](const YAML::Node& v) { return v.IsInt(); });
                bool allStrings = std::all_of(value.begin(), value.end(), 
                    [](const YAML::Node& v) { return v.IsScalar(); });
                
                if (allInts) {
                    std::vector<int> intArray;
                    for (const auto& v : value) {
                        intArray.push_back(v.as<int>());
                    }
                    merged->set(key, intArray, ConfigSource::YAML);
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    for (const auto& v : value) {
                        stringArray.push_back(v.as<std::string>());
                    }
                    merged->set(key, stringArray, ConfigSource::YAML);
                } else {
                    // Default to string array
                    std::vector<std::string> stringArray;
                    for (const auto& v : value) {
                        stringArray.push_back(v.as<std::string>());
                    }
                    merged->set(key, stringArray, ConfigSource::YAML);
                }
            }
        }
    } catch (...) {
        // If YAML parsing fails, return the original config
        return base;
    }
    
    return merged;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, "auto");
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath);
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

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::string Config::exportJSON() const {
    return toJSON();
}

std::string Config::exportYAML() const {
    return toYAML();
}

// Helper function implementations
nlohmann::json configEntryToJSON(const ConfigEntry& entry) {
    nlohmann::json jsonData;
    
    if (std::holds_alternative<int>(entry.value)) {
        jsonData[entry.key] = std::get<int>(entry.value);
    } else if (std::holds_alternative<int64_t>(entry.value)) {
        jsonData[entry.key] = std::get<int64_t>(entry.value);
    } else if (std::holds_alternative<double>(entry.value)) {
        jsonData[entry.key] = std::get<double>(entry.value);
    } else if (std::holds_alternative<bool>(entry.value)) {
        jsonData[entry.key] = std::get<bool>(entry.value);
    } else if (std::holds_alternative<std::string>(entry.value)) {
        jsonData[entry.key] = std::get<std::string>(entry.value);
    } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
        jsonData[entry.key] = std::get<std::vector<int>>(entry.value);
    } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
        jsonData[entry.key] = std::get<std::vector<double>>(entry.value);
    } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
        jsonData[entry.key] = std::get<std::vector<std::string>>(entry.value);
    }
    
    return jsonData;
}

ConfigEntry jsonToConfigEntry(const nlohmann::json& jsonData) {
    ConfigEntry entry;
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        entry.key = it.key();
        
        if (it.value().is_number_integer()) {
            entry.value = it.value().get<int64_t>();
        } else if (it.value().is_number_float()) {
            entry.value = it.value().get<double>();
        } else if (it.value().is_boolean()) {
            entry.value = it.value().get<bool>();
        } else if (it.value().is_string()) {
            entry.value = it.value().get<std::string>();
        } else if (it.value().is_array()) {
            // Try to determine array type
            bool allInts = std::all_of(it.value().begin(), it.value().end(), 
                [](const nlohmann::json& v) { return v.is_number_integer(); });
            bool allStrings = std::all_of(it.value().begin(), it.value().end(), 
                [](const nlohmann::json& v) { return v.is_string(); });
            
            if (allInts) {
                std::vector<int> intArray;
                for (const auto& v : it.value()) {
                    intArray.push_back(v.get<int>());
                }
                entry.value = intArray;
            } else if (allStrings) {
                std::vector<std::string> stringArray;
                for (const auto& v : it.value()) {
                    stringArray.push_back(v.get<std::string>());
                }
                entry.value = stringArray;
            }
        }
        
        entry.source = ConfigSource::JSON;
    }
    
    return entry;
}

YAML::Node configEntryToYAML(const ConfigEntry& entry) {
    YAML::Node yamlData;
    
    if (std::holds_alternative<int>(entry.value)) {
        yamlData[entry.key] = std::get<int>(entry.value);
    } else if (std::holds_alternative<int64_t>(entry.value)) {
        yamlData[entry.key] = std::get<int64_t>(entry.value);
    } else if (std::holds_alternative<double>(entry.value)) {
        yamlData[entry.key] = std::get<double>(entry.value);
    } else if (std::holds_alternative<bool>(entry.value)) {
        yamlData[entry.key] = std::get<bool>(entry.value);
    } else if (std::holds_alternative<std::string>(entry.value)) {
        yamlData[entry.key] = std::get<std::string>(entry.value);
    } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
        yamlData[entry.key] = std::get<std::vector<int>>(entry.value);
    } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
        yamlData[entry.key] = std::get<std::vector<double>>(entry.value);
    } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
        yamlData[entry.key] = std::get<std::vector<std::string>>(entry.value);
    }
    
    return yamlData;
}

ConfigEntry yamlToConfigEntry(const YAML::Node& yamlData) {
    ConfigEntry entry;
    
    if (!yamlData.IsMap()) {
        return entry;
    }
    
    for (const auto& pair : yamlData) {
        entry.key = pair.first.as<std::string>();
        YAML::Node value = pair.second;
        
        if (value.IsInt()) {
            entry.value = value.as<int>();
        } else if (value.IsDouble()) {
            entry.value = value.as<double>();
        } else if (value.IsBoolean()) {
            entry.value = value.as<bool>();
        } else if (value.IsScalar()) {
            entry.value = value.as<std::string>();
        } else if (value.IsSequence()) {
            // Check sequence type
            bool allInts = std::all_of(value.begin(), value.end(), 
                [](const YAML::Node& v) { return v.IsInt(); });
            bool allStrings = std::all_of(value.begin(), value.end(), 
                [](const YAML::Node& v) { return v.IsScalar(); });
            
            if (allInts) {
                std::vector<int> intArray;
                for (const auto& v : value) {
                    intArray.push_back(v.as<int>());
                }
                entry.value = intArray;
            } else if (allStrings) {
                std::vector<std::string> stringArray;
                for (const auto& v : value) {
                    stringArray.push_back(v.as<std::string>());
                }
                entry.value = stringArray;
            }
        }
        
        entry.source = ConfigSource::YAML;
    }
    
    return entry;
}

} // namespace nlm
