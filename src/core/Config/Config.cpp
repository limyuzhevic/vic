#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <optional>
#include <any>
#include <regex>
#include <stdexcept>
#include <variant>
#include <typeindex>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

namespace nlm {

    // JSON/YAML error classes
    class ConfigParseError : public std::runtime_error {
    public:
        ConfigParseError(const std::string& message, const std::string& filepath, int line = -1)
            : std::runtime_error(message), filepath_(filepath), line_(line) {}
        
        const std::string& getFilepath() const { return filepath_; }
        int getLine() const { return line_; }
        
    private:
        std::string filepath_;
        int line_;
    };

    class ConfigValidationError : public std::runtime_error {
    public:
        ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
    };

    // Configuration schema support
    enum class ParameterType {
        INT,
        INT64,
        DOUBLE,
        BOOL,
        STRING,
        STRING_LIST,
        INT_LIST,
        DOUBLE_LIST
    };

    struct ParameterMetadata {
        std::string description;
        std::string defaultValue;
        ParameterType type;
        std::vector<std::string> enumValues;
        std::optional<std::pair<double, double>> range;
        std::vector<std::string> dependencies;
        bool required = false;
        
        ParameterMetadata()
            : description(""), defaultValue(""), type(ParameterType::STRING),
              required(false) {}
    };

    class ConfigSchema {
    private:
        std::map<std::string, ParameterMetadata> parameters_;
        std::string name_;
        
    public:
        ConfigSchema(const std::string& name = "") : name_(name) {}
        
        void addParameter(const std::string& key, const ParameterMetadata& metadata) {
            parameters_[key] = metadata;
        }
        
        const ParameterMetadata* getParameter(const std::string& key) const {
            auto it = parameters_.find(key);
            return (it != parameters_.end()) ? &it->second : nullptr;
        }
        
        bool hasParameter(const std::string& key) const {
            return parameters_.find(key) != parameters_.end();
        }
        
        std::vector<std::string> getParameterKeys() const {
            std::vector<std::string> keys;
            for (const auto& pair : parameters_) {
                keys.push_back(pair.first);
            }
            return keys;
        }
        
        std::string getName() const { return name_; }
        size_t parameterCount() const { return parameters_.size(); }
        
        // Validation
        std::vector<std::string> validate(const Config& config) const {
            std::vector<std::string> errors;
            
            for (const auto& pair : parameters_) {
                const std::string& key = pair.first;
                const ParameterMetadata& metadata = pair.second;
                
                if (metadata.required && !config.has(key)) {
                    errors.push_back("Missing required parameter: " + key);
                }
                
                if (config.has(key)) {
                    validateParameter(key, metadata, config, errors);
                }
            }
            
            // Check dependencies
            for (const auto& pair : parameters_) {
                const std::string& key = pair.first;
                const ParameterMetadata& metadata = pair.second;
                
                for (const std::string& dep : metadata.dependencies) {
                    if (!config.has(dep)) {
                        errors.push_back("Parameter " + key + " requires dependency: " + dep);
                    }
                }
            }
            
            return errors;
        }
        
    private:
        void validateParameter(const std::string& key, const ParameterMetadata& metadata,
                              const Config& config, std::vector<std::string>& errors) const {
            auto value = config.getAny(key);
            if (!value) {
                return; // Should not happen if config.has(key)
            }
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                // Type checking
                switch (metadata.type) {
                    case ParameterType::INT:
                        if constexpr (std::is_same_v<T, int>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type int");
                        }
                        break;
                    case ParameterType::INT64:
                        if constexpr (std::is_same_v<T, int64_t>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type int64_t");
                        }
                        break;
                    case ParameterType::DOUBLE:
                        if constexpr (std::is_same_v<T, double>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type double");
                        }
                        break;
                    case ParameterType::BOOL:
                        if constexpr (std::is_same_v<T, bool>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type bool");
                        }
                        break;
                    case ParameterType::STRING:
                        if constexpr (std::is_same_v<T, std::string>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type string");
                        }
                        break;
                    case ParameterType::STRING_LIST:
                        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type string list");
                        }
                        break;
                    case ParameterType::INT_LIST:
                        if constexpr (std::is_same_v<T, std::vector<int>>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type int list");
                        }
                        break;
                    case ParameterType::DOUBLE_LIST:
                        if constexpr (std::is_same_v<T, std::vector<double>>) {
                            // OK
                        } else {
                            errors.push_back("Parameter '" + key + "' must be of type double list");
                        }
                        break;
                }
                
                // Enum validation
                if (!metadata.enumValues.empty()) {
                    std::string strValue;
                    if constexpr (std::is_same_v<T, std::string>) {
                        strValue = arg;
                    } else if constexpr (std::is_same_v<T, int>) {
                        strValue = std::to_string(arg);
                    } else if constexpr (std::is_same_v<T, double>) {
                        strValue = std::to_string(arg);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        strValue = arg ? "true" : "false";
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        strValue = std::to_string(arg);
                    }
                    
                    bool valid = false;
                    for (const auto& enumVal : metadata.enumValues) {
                        if (strValue == enumVal) {
                            valid = true;
                            break;
                        }
                    }
                    
                    if (!valid) {
                        errors.push_back("Parameter '" + key + "' value '" + strValue + "' is not in allowed values");
                    }
                }
                
                // Range validation
                if (metadata.range) {
                    double min = metadata.range->first;
                    double max = metadata.range->second;
                    
                    if constexpr (std::is_same_v<T, int>) {
                        if (arg < min || arg > max) {
                            errors.push_back("Parameter '" + key + "' value " + std::to_string(arg) + " is outside allowed range [" + 
                                           std::to_string(min) + ", " + std::to_string(max) + "]");
                        }
                    } else if constexpr (std::is_same_v<T, double>) {
                        if (arg < min || arg > max) {
                            errors.push_back("Parameter '" + key + "' value " + std::to_string(arg) + " is outside allowed range [" + 
                                           std::to_string(min) + ", " + std::to_string(max) + "]");
                        }
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        if (static_cast<double>(arg) < min || static_cast<double>(arg) > max) {
                            errors.push_back("Parameter '" + key + "' value " + std::to_string(arg) + " is outside allowed range [" + 
                                           std::to_string(min) + ", " + std::to_string(max) + "]");
                        }
                    }
                }
            }, value.value());
        }
    };

    // Format detection
    static std::string detectFormat(const std::string& filepath) {
        std::filesystem::path path(filepath);
        std::string ext = path.extension().string();
        
        // Convert to lowercase
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return "json";
        } else if (ext == ".yaml" || ext == ".yml") {
            return "yaml";
        } else {
            return "simple"; // default for backward compatibility
        }
    }

    // JSON parsing implementation
    static nlohmann::json parseJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigParseError("Cannot open file: " + filepath, filepath);
        }
        
        try {
            return nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& e) {
            throw ConfigParseError(std::string("JSON parse error: ") + e.what(), filepath);
        } catch (const std::exception& e) {
            throw ConfigParseError(std::string("Error reading JSON file: ") + e.what(), filepath);
        }
    }

    // YAML parsing implementation
    static YAML::Node parseYAML(const std::string& filepath) {
        try {
            return YAML::LoadFile(filepath);
        } catch (const YAML::Exception& e) {
            throw ConfigParseError(std::string("YAML parse error: ") + e.msg, filepath);
        } catch (const std::exception& e) {
            throw ConfigParseError(std::string("Error reading YAML file: ") + e.what(), filepath);
        }
    }

    // JSON to ConfigValue conversion
    static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
        if (jsonValue.is_string()) {
            return jsonValue.get<std::string>();
        } else if (jsonValue.is_number_integer()) {
            return jsonValue.get<int>();
        } else if (jsonValue.is_number_unsigned()) {
            return jsonValue.get<int64_t>();
        } else if (jsonValue.is_number()) {
            return jsonValue.get<double>();
        } else if (jsonValue.is_boolean()) {
            return jsonValue.get<bool>();
        } else if (jsonValue.is_array()) {
            if (!jsonValue.empty()) {
                if (jsonValue[0].is_string()) {
                    std::vector<std::string> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<std::string>());
                    }
                    return result;
                } else if (jsonValue[0].is_number_integer()) {
                    std::vector<int> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<int>());
                    }
                    return result;
                } else if (jsonValue[0].is_number()) {
                    std::vector<double> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<double>());
                    }
                    return result;
                }
            }
            return std::vector<std::string>(); // empty list
        }
        throw ConfigParseError("Unsupported JSON value type", "");
    }

    // YAML to ConfigValue conversion
    static ConfigValue yamlToConfigValue(const YAML::Node& yamlValue) {
        if (yamlValue.IsScalar()) {
            if (yamlValue.IsSequence()) {
                // Empty scalar sequence
                if (yamlValue.size() == 0) return std::vector<std::string>();
                
                // Check if all elements are strings
                bool allStrings = true;
                for (const auto& item : yamlValue) {
                    if (!item.IsScalar() || !item.as<std::string>().empty()) {
                        // Empty strings are also strings
                    } else {
                        allStrings = false;
                        break;
                    }
                }
                
                if (allStrings) {
                    std::vector<std::string> result;
                    for (const auto& item : yamlValue) {
                        result.push_back(item.as<std::string>());
                    }
                    return result;
                }
                
                // Check if all elements are integers
                bool allInts = true;
                for (const auto& item : yamlValue) {
                    if (!item.IsScalar() || !item.IsInteger()) {
                        allInts = false;
                        break;
                    }
                }
                
                if (allInts) {
                    std::vector<int> result;
                    for (const auto& item : yamlValue) {
                        result.push_back(item.as<int>());
                    }
                    return result;
                }
                
                // Check if all elements are doubles
                bool allDoubles = true;
                for (const auto& item : yamlValue) {
                    if (!item.IsScalar() || !item.IsDouble()) {
                        allDoubles = false;
                        break;
                    }
                }
                
                if (allDoubles) {
                    std::vector<double> result;
                    for (const auto& item : yamlValue) {
                        result.push_back(item.as<double>());
                    }
                    return result;
                }
                
                // Mixed or unknown type - treat as string list
                std::vector<std::string> result;
                for (const auto& item : yamlValue) {
                    result.push_back(item.as<std::string>());
                }
                return result;
            } else {
                // Scalar
                if (yamlValue.IsBoolean()) {
                    return yamlValue.as<bool>();
                } else if (yamlValue.IsInteger()) {
                    return yamlValue.as<int64_t>();
                } else if (yamlValue.IsDouble()) {
                    return yamlValue.as<double>();
                } else {
                    return yamlValue.as<std::string>();
                }
            }
        } else if (yamlValue.IsSequence()) {
            // Empty sequence
            if (yamlValue.size() == 0) return std::vector<std::string>();
            
            // Check types and convert
            std::vector<std::string> stringList;
            std::vector<int> intList;
            std::vector<double> doubleList;
            bool isString = true, isInt = true, isDouble = true;
            
            for (const auto& item : yamlValue) {
                if (item.IsScalar()) {
                    if (item.IsBoolean() || item.IsDouble() || item.IsInteger()) {
                        isString = false;
                    }
                } else {
                    isString = false;
                    isInt = false;
                    isDouble = false;
                }
            }
            
            if (isString) {
                for (const auto& item : yamlValue) {
                    stringList.push_back(item.as<std::string>());
                }
                return stringList;
            } else if (isInt) {
                for (const auto& item : yamlValue) {
                    intList.push_back(item.as<int>());
                }
                return intList;
            } else if (isDouble) {
                for (const auto& item : yamlValue) {
                    doubleList.push_back(item.as<double>());
                }
                return doubleList;
            }
            
            // Mixed or unknown - return as string list
            for (const auto& item : yamlValue) {
                stringList.push_back(item.as<std::string>());
            }
            return stringList;
        }
        throw ConfigParseError("Unsupported YAML value type", "");
    }

    // ConfigValue to JSON conversion
    static nlohmann::json configValueToJSON(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            }
            return nlohmann::json();
        }, value);
    }

    // ConfigValue to YAML conversion
    static YAML::Node configValueToYAML(const ConfigValue& value) {
        YAML::Node node;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            }
        }, value);
        
        return node;
    }

    // Format-agnostic load implementation
    bool Config::loadFromFile(const std::string& filepath) {
        clear();
        
        try {
            std::string format = detectFormat(filepath);
            
            if (format == "json") {
                auto jsonData = parseJSON(filepath);
                
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    std::string key = it.key();
                    ConfigValue value = jsonToConfigValue(it.value());
                    set(key, value, ConfigSource::File);
                }
            } else if (format == "yaml") {
                auto yamlData = parseYAML(filepath);
                
                for (const auto& pair : yamlData) {
                    std::string key = pair.first.as<std::string>();
                    ConfigValue value = yamlToConfigValue(pair.second);
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Simple key=value format for backward compatibility
                std::ifstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                std::string line;
                int lineNum = 0;
                while (std::getline(file, line)) {
                    lineNum++;
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
                        
                        set(key, value, ConfigSource::File);
                    } else {
                        throw ConfigParseError("Invalid format: expected key=value", filepath, lineNum);
                    }
                }
            }
            
            return true;
        } catch (const ConfigParseError& e) {
            std::cerr << "Parse error in " << filepath << ": " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error loading config from " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    }

    bool Config::loadFromArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    try {
                        setFromString(key, value);
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing argument --" << key << "=" << value << ": " << e.what() << std::endl;
                    }
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                try {
                    setFromString(key, value);
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing argument -" << key << " " << value << ": " << e.what() << std::endl;
                }
            }
        }
        return true;
    }

    // Improved saveToFile with format support
    bool Config::saveToFile(const std::string& filepath) const {
        std::filesystem::path path(filepath);
        std::string ext = path.extension().string();
        
        // Convert to lowercase
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (ext == ".json") {
            nlohmann::json jsonData;
            
            for (const auto& entry : pImpl->entries) {
                jsonData[entry.key] = std::visit([](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        return nlohmann::json(arg);
                    } else if constexpr (std::is_same_v<T, int>) {
                        return nlohmann::json(arg);
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        return nlohmann::json(arg);
                    } else if constexpr (std::is_same_v<T, double>) {
                        return nlohmann::json(arg);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return nlohmann::json(arg);
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        nlohmann::json arr = nlohmann::json::array();
                        for (const auto& item : arg) {
                            arr.push_back(item);
                        }
                        return arr;
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        nlohmann::json arr = nlohmann::json::array();
                        for (const auto& item : arg) {
                            arr.push_back(item);
                        }
                        return arr;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        nlohmann::json arr = nlohmann::json::array();
                        for (const auto& item : arg) {
                            arr.push_back(item);
                        }
                        return arr;
                    }
                    return nlohmann::json();
                }, entry.value);
                
                // Add comment as description if available
                if (!entry.description.empty()) {
                    jsonData[entry.key].description = entry.description;
                }
            }
            
            file << jsonData.dump(2) << std::endl;
        } else if (ext == ".yaml" || ext == ".yml") {
            YAML::Emitter yamlData;
            yamlData << YAML::BeginMap;
            
            for (const auto& entry : pImpl->entries) {
                yamlData << YAML::Key << entry.key;
                yamlData << YAML::Value << configValueToYAML(entry.value);
                
                if (!entry.description.empty()) {
                    yamlData << YAML::Comment << entry.description;
                }
            }
            
            yamlData << YAML::EndMap;
            file << yamlData.c_str() << std::endl;
        } else {
            // Simple key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << arg << "\"\n";
                    } else if constexpr (std::is_same_v<T, int>) {
                        file << entry.key << " = " << arg << "\n";
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        file << entry.key << " = " << arg << "\n";
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << entry.key << " = " << arg << "\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << entry.key << " = \"";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "\"\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]\n";
                    }
                }, entry.value);
            }
        }
        
        return true;
    }

    // Additional methods
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

    void Config::setFromString(const std::string& key, const std::string& valueStr) {
        // Try to determine type from string
        std::string lowerValue = toLower(valueStr);
        
        // Boolean
        if (lowerValue == "true" || lowerValue == "false") {
            bool val = (lowerValue == "true");
            set(key, val, ConfigSource::CommandLine);
            return;
        }
        
        // Integer
        try {
            if (valueStr.find('.') == std::string::npos) {
                size_t pos;
                int val = std::stoi(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, val, ConfigSource::CommandLine);
                    return;
                }
            }
        } catch (...) {}
        
        // Double
        try {
            size_t pos;
            double val = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, val, ConfigSource::CommandLine);
                return;
            }
        } catch (...) {}
        
        // List (comma-separated)
        if (valueStr.find(',') != std::string::npos) {
            std::vector<std::string> list;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    list.push_back(item);
                }
            }
            if (!list.empty()) {
                set(key, list, ConfigSource::CommandLine);
                return;
            }
        }
        
        // Default to string
        set(key, valueStr, ConfigSource::CommandLine);
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
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        return oss.str();
    }

    // Utility functions for configuration
    class ConfigUtils {
    public:
        // Compare two configurations
        static bool compare(const Config& config1, const Config& config2) {
            if (config1.getKeys().size() != config2.getKeys().size()) {
                return false;
            }
            
            auto keys1 = config1.getKeys();
            auto keys2 = config2.getKeys();
            
            // Sort keys for comparison
            std::sort(keys1.begin(), keys1.end());
            std::sort(keys2.begin(), keys2.end());
            
            for (size_t i = 0; i < keys1.size(); ++i) {
                if (keys1[i] != keys2[i]) {
                    return false;
                }
                
                // Compare values
                auto val1 = config1.getAny(keys1[i]);
                auto val2 = config2.getAny(keys2[i]);
                
                if (!val1.has_value() && !val2.has_value()) {
                    continue;
                }
                
                if (!val1.has_value() || !val2.has_value()) {
                    return false;
                }
                
                if (!valuesEqual(val1.value(), val2.value())) {
                    return false;
                }
            }
            
            return true;
        }
        
        // Merge two configurations
        static Config merge(const Config& config1, const Config& config2, bool overwrite = true) {
            Config result;
            
            // Add all entries from config1
            auto keys1 = config1.getKeys();
            for (const auto& key : keys1) {
                auto val = config1.getAny(key);
                if (val.has_value()) {
                    result.set(key, val.value(), config1.getSource(key));
                }
            }
            
            // Add entries from config2
            auto keys2 = config2.getKeys();
            for (const auto& key : keys2) {
                auto val = config2.getAny(key);
                if (val.has_value()) {
                    if (overwrite) {
                        result.set(key, val.value(), config2.getSource(key));
                    } else {
                        // Only add if not already present
                        if (!result.has(key)) {
                            result.set(key, val.value(), config2.getSource(key));
                        }
                    }
                }
            }
            
            return result;
        }
        
        // Get configuration differences
        static std::vector<std::string> diff(const Config& config1, const Config& config2) {
            std::vector<std::string> differences;
            
            auto keys1 = config1.getKeys();
            auto keys2 = config2.getKeys();
            
            std::set<std::string> allKeys;
            for (const auto& key : keys1) allKeys.insert(key);
            for (const auto& key : keys2) allKeys.insert(key);
            
            for (const auto& key : allKeys) {
                auto val1 = config1.getAny(key);
                auto val2 = config2.getAny(key);
                
                if (!val1.has_value() && !val2.has_value()) {
                    continue;
                }
                
                if (!val1.has_value()) {
                    differences.push_back("Added: " + key + " = " + configValueToString(val2.value()));
                } else if (!val2.has_value()) {
                    differences.push_back("Removed: " + key + " = " + configValueToString(val1.value()));
                } else if (!valuesEqual(val1.value(), val2.value())) {
                    differences.push_back("Changed: " + key + " from " + configValueToString(val1.value()) + 
                                        " to " + configValueToString(val2.value()));
                }
            }
            
            return differences;
        }
        
        // Convert ConfigValue to string for display
        static std::string configValueToString(const ConfigValue& value) {
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, value);
            return oss.str();
        }
        
    private:
        static bool valuesEqual(const ConfigValue& val1, const ConfigValue& val2) {
            return std::visit([](auto&& arg1, auto&& arg2) {
                using T1 = std::decay_t<decltype(arg1)>;
                using T2 = std::decay_t<decltype(arg2)>;
                
                if constexpr (std::is_same_v<T1, T2>) {
                    if constexpr (std::is_same_v<T1, std::vector<std::string>>) {
                        return arg1 == arg2;
                    } else if constexpr (std::is_same_v<T1, std::vector<int>>) {
                        return arg1 == arg2;
                    } else if constexpr (std::is_same_v<T1, std::vector<double>>) {
                        return arg1 == arg2;
                    } else {
                        return arg1 == arg2;
                    }
                } else {
                    return false;
                }
            }, val1, val2);
        }
    };

    // Get value as any type (utility)
    std::optional<ConfigValue> Config::getAny(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        return it->value;
    }
    
    // Get source of a key
    ConfigSource Config::getSource(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return ConfigSource::Default;
        }
        
        return it->source;
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

    // Update Config.hpp with new methods
    void Config::updateSchema(const ConfigSchema& schema) {
        schema_ = schema;
    }
    
    std::vector<std::string> Config::validate() const {
        if (schema_) {
            return schema_->validate(*this);
        }
        return std::vector<std::string>();
    }

    std::string Config::saveToFile(const std::string& filepath, const std::string& format) const {
        std::filesystem::path path(filepath);
        std::string ext = path.extension().string();
        
        // Override format if extension is provided
        if (!ext.empty()) {
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".json") format = "json";
            else if (ext == ".yaml" || ext == ".yml") format = "yaml";
            else if (ext == ".cfg" || ext == ".txt") format = "simple";
        }
        
        std::string actualFilepath = filepath;
        
        // If format is specified but extension doesn't match, adjust filename
        if (!format.empty() && format != "auto") {
            if ((format == "json" && ext != ".json") ||
                (format == "yaml" && ext != ".yaml" && ext != ".yml") ||
                (format == "simple" && ext != ".cfg" && ext != ".txt")) {
                // Remove extension from filepath and add appropriate one
                actualFilepath = path.replace_extension(
                    (format == "json" ? ".json" : (format == "yaml" ? ".yaml" : ".cfg"))
                ).string();
            }
        }
        
        bool success = saveToFile(actualFilepath);
        return success ? "Configuration saved successfully to " + actualFilepath :
               "Failed to save configuration to " + actualFilepath;
    }

} // namespace nlm

// Global schema instances (for backward compatibility)
nlm::ConfigSchema globalConfigSchema("global");

// Convenience functions for backward compatibility
nlm::Config& getGlobalConfig() {
    static nlm::Config config;
    return config;
}

void loadGlobalConfig(const std::string& filepath) {
    getGlobalConfig().loadFromFile(filepath);
}

void saveGlobalConfig(const std::string& filepath) {
    getGlobalConfig().saveToFile(filepath);
}