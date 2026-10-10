#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> envVarMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to replace environment variables in a string
std::string Config::replaceEnvVars(const std::string& str, const std::unordered_map<std::string, std::string>& envVars) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find("}", pos + 2);
        if (endPos != std::string::npos) {
            std::string varName = result.substr(pos + 2, endPos - pos - 2);
            std::string replacement = "";
            
            // First try custom env map, then system env
            if (envVars.find(varName) != envVars.end()) {
                replacement = envVars.at(varName);
            } else {
                const char* envValue = std::getenv(varName.c_str());
                if (envValue) replacement = envValue;
            }
            
            result.replace(pos, endPos - pos + 1, replacement);
            pos += replacement.length();
        } else {
            break;
        }
    }
    return result;
}

// Helper function to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to convert to uppercase
std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Helper function to convert ConfigType to string
std::string Config::configTypeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Int: return "int";
        case ConfigType::Int64: return "int64_t";
        case ConfigType::Double: return "double";
        case ConfigType::Bool: return "bool";
        case ConfigType::String: return "string";
        case ConfigType::VectorInt: return "vector<int>";
        case ConfigType::VectorDouble: return "vector<double>";
        case ConfigType::VectorString: return "vector<string>";
        default: return "unknown";
    }
}

// Helper function to convert string to ConfigType
ConfigType Config::getConfigType(const std::string& typeStr) {
    std::string lower = toLower(typeStr);
    if (lower == "int" || lower == "integer") return ConfigType::Int;
    if (lower == "int64" || lower == "int64_t") return ConfigType::Int64;
    if (lower == "double" || lower == "float") return ConfigType::Double;
    if (lower == "bool" || lower == "boolean") return ConfigType::Bool;
    if (lower == "string") return ConfigType::String;
    if (lower == "vector<int>" || lower == "vector integer") return ConfigType::VectorInt;
    if (lower == "vector<double>" || lower == "vector float") return ConfigType::VectorDouble;
    if (lower == "vector<string>" || lower == "vector string") return ConfigType::VectorString;
    return ConfigType::String; // Default to string
}

// Helper function to parse string value to ConfigValue
bool Config::parseValue(const std::string& valueStr, ConfigValue& outValue) {
    // Remove surrounding quotes if present
    std::string value = valueStr;
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Try parsing as boolean
    if (value == "true") {
        outValue = true;
        return true;
    } else if (value == "false") {
        outValue = false;
        return true;
    }
    
    // Try parsing as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            outValue = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try parsing as int64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            outValue = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try parsing as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Treat as string
    outValue = value;
    return true;
}

// Helper function to convert ConfigValue to string
std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Try JSON first, then YAML, then fallback to simple format
        if (loadFromJSON(filepath)) return true;
        if (loadFromYAML(filepath)) return true;
        return loadFromFileSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::Document doc;
    rapidjson::IStreamWrapper isw(file);
    
    if (doc.ParseStream(isw).HasParseError()) {
        return false;
    }
    
    if (!doc.IsObject()) {
        return false;
    }
    
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        std::string valueStr;
        
        const rapidjson::Value& value = it->value;
        
        if (value.IsString()) {
            valueStr = value.GetString();
        } else if (value.IsBool()) {
            valueStr = value.GetBool() ? "true" : "false";
        } else if (value.IsInt()) {
            valueStr = std::to_string(value.GetInt());
        } else if (value.IsInt64()) {
            valueStr = std::to_string(value.GetInt64());
        } else if (value.IsUint()) {
            valueStr = std::to_string(value.GetUint());
        } else if (value.IsDouble()) {
            valueStr = std::to_string(value.GetDouble());
        } else if (value.IsArray()) {
            // For arrays, we need to handle them specially
            // For now, just skip arrays
            continue;
        }
        
        ConfigValue configValue;
        if (parseValue(valueStr, configValue)) {
            set(key, configValue, ConfigSource::File, "Loaded from JSON file");
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    YAML::Node doc;
    try {
        doc = YAML::LoadFile(filepath);
    } catch (const YAML::Exception& e) {
        return false;
    }
    
    if (!doc.IsMap()) {
        return false;
    }
    
    for (const auto& node : doc) {
        std::string key = node.first.as<std::string>();
        std::string valueStr;
        
        const YAML::Node& value = node.second;
        
        if (value.IsScalar()) {
            valueStr = value.as<std::string>();
        } else if (value.IsSequence()) {
            // For sequences, we need to handle them specially
            // For now, just skip sequences
            continue;
        }
        
        ConfigValue configValue;
        if (parseValue(valueStr, configValue)) {
            set(key, configValue, ConfigSource::File, "Loaded from YAML file");
        }
    }
    
    return true;
}

bool Config::loadFromFileSimple(const std::string& filepath) {
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
            
            // Replace environment variables
            value = replaceEnvVars(value, pImpl->envVarMap);
            
            ConfigValue configValue;
            if (parseValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple format
        return saveToFileSimple(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();
    
    for (const auto& entry : pImpl->entries) {
        rapidjson::Value key(key.c_str(), allocator);
        
        // Convert ConfigValue to JSON value
        rapidjson::Value jsonValue;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                jsonValue.SetInt(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonValue.SetInt64(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue.SetDouble(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue.SetBool(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                rapidjson::Value str(arg.c_str(), allocator);
                jsonValue = std::move(str);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                rapidjson::Value arr(rapidjson::kArrayType);
                for (const auto& item : arg) {
                    arr.PushBack(item, allocator);
                }
                jsonValue = std::move(arr);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                rapidjson::Value arr(rapidjson::kArrayType);
                for (const auto& item : arg) {
                    arr.PushBack(item, allocator);
                }
                jsonValue = std::move(arr);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                rapidjson::Value arr(rapidjson::kArrayType);
                for (const auto& item : arg) {
                    rapidjson::Value str(item.c_str(), allocator);
                    arr.PushBack(std::move(str), allocator);
                }
                jsonValue = std::move(arr);
            }
        }, entry.value);
        
        doc.AddMember(key, jsonValue, allocator);
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    file << buffer.GetString();
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    return true;
}

bool Config::saveToFileSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
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
                
                // Replace environment variables in value
                value = replaceEnvVars(value, pImpl->envVarMap);
                
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    set(key, configValue, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Replace environment variables in value
            value = replaceEnvVars(value, pImpl->envVarMap);
            
            ConfigValue configValue;
            if (parseValue(value, configValue)) {
                set(key, configValue, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
    pImpl->envVarMap = envVars;
    
    // Prefix for environment variable configuration
    std::string prefix = "NLM_";
    
    for (const auto& pair : envVars) {
        if (pair.first.size() > prefix.size() && pair.first.substr(0, prefix.size()) == prefix) {
            std::string key = pair.first.substr(prefix.size());
            std::string value = pair.second;
            
            ConfigValue configValue;
            if (parseValue(value, configValue)) {
                set(key, configValue, ConfigSource::Environment, "Loaded from environment variable " + pair.first);
            }
        }
    }
    
    return true;
}

bool Config::loadFromSchema(const std::vector<ConfigValidationRule>& schema) {
    setDefaults(schema);
    return validate(schema);
}

bool Config::validate(const std::vector<ConfigValidationRule>& schema) const {
    bool allValid = true;
    
    for (const auto& rule : schema) {
        if (!has(rule.key)) {
            if (rule.required) {
                NLM_LOG_ERROR("Missing required configuration parameter: " + rule.key);
                allValid = false;
            }
            continue;
        }
        
        auto value = get(rule.key);
        if (!value.has_value()) {
            NLM_LOG_ERROR("Invalid configuration value for parameter: " + rule.key);
            allValid = false;
            continue;
        }
        
        // Check type
        if (rule.type != ConfigType::String) {
            bool typeMatches = false;
            
            if (rule.type == ConfigType::Int && std::holds_alternative<int>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::Int64 && std::holds_alternative<int64_t>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::Double && std::holds_alternative<double>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::Bool && std::holds_alternative<bool>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::String && std::holds_alternative<std::string>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::VectorInt && std::holds_alternative<std::vector<int>>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::VectorDouble && std::holds_alternative<std::vector<double>>(*value)) typeMatches = true;
            else if (rule.type == ConfigType::VectorString && std::holds_alternative<std::vector<std::string>>(*value)) typeMatches = true;
            
            if (!typeMatches) {
                NLM_LOG_ERROR("Configuration parameter " + rule.key + " has wrong type. Expected " + 
                            configTypeToString(rule.type) + ", got unknown type");
                allValid = false;
            }
        }
        
        // Run custom validator if provided
        if (rule.validator && value.has_value()) {
            try {
                if (!rule.validator(value.value())) {
                    NLM_LOG_ERROR("Configuration validation failed for parameter: " + rule.key);
                    allValid = false;
                }
            } catch (...) {
                NLM_LOG_ERROR("Configuration validator exception for parameter: " + rule.key);
                allValid = false;
            }
        }
    }
    
    return allValid;
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) it->description = description;
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::setDefaults(const std::vector<ConfigValidationRule>& schema) {
    for (const auto& rule : schema) {
        if (!has(rule.key) && rule.defaultValue.has_value()) {
            ConfigValue defaultVal;
            
            // Convert any_type to ConfigValue
            try {
                if (rule.type == ConfigType::Int) defaultVal = std::any_cast<int>(rule.defaultValue);
                else if (rule.type == ConfigType::Int64) defaultVal = std::any_cast<int64_t>(rule.defaultValue);
                else if (rule.type == ConfigType::Double) defaultVal = std::any_cast<double>(rule.defaultValue);
                else if (rule.type == ConfigType::Bool) defaultVal = std::any_cast<bool>(rule.defaultValue);
                else if (rule.type == ConfigType::String) defaultVal = std::any_cast<std::string>(rule.defaultValue);
                else if (rule.type == ConfigType::VectorInt) defaultVal = std::any_cast<std::vector<int>>(rule.defaultValue);
                else if (rule.type == ConfigType::VectorDouble) defaultVal = std::any_cast<std::vector<double>>(rule.defaultValue);
                else if (rule.type == ConfigType::VectorString) defaultVal = std::any_cast<std::vector<std::string>>(rule.defaultValue);
                else defaultVal = rule.defaultValue;
                
                set(rule.key, defaultVal, ConfigSource::Default, rule.description);
            } catch (const std::bad_any_cast&) {
                // If we can't cast, just skip
                continue;
            }
        }
    }
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->envVarMap.clear();
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

std::string Config::getSummaryBySource(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration from " << static_cast<int>(source) << " (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "]\n";
        }
    }
    return oss.str();
}

// Explicit template instantiations
namespace {
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
