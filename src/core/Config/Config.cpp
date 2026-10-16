#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::vector<std::string> validationErrors;
    std::string filepath;
    std::string format; // "key=value", "json", or "yaml"
    bool strictValidation;
    std::map<std::string, std::function<bool(const std::string&)>> customValidators;
    
    Impl() : strictValidation(false), filepath(), format("key=value") {
        // Initialize with default config entries
        setDefaults();
    }
    
    void setDefaults() {
        // Set default configuration values
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        set("replay_interval", static_cast<size_t>(100), ConfigSource::Default);
        set("consolidation_interval", static_cast<size_t>(1000), ConfigSource::Default);
        set("checkpoint_dir", std::string("./checkpoints"), ConfigSource::Default);
    }
    
    void clearValidationErrors() {
        validationErrors.clear();
    }
    
    void addValidationError(const std::string& error) {
        validationErrors.push_back(error);
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// File loading with format detection
bool Config::loadFromFile(const std::string& filepath) {
    pImpl->clearValidationErrors();
    pImpl->filepath = filepath;
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        pImpl->format = "key=value";
        return loadFromFileLegacy(filepath);
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        pImpl->format = "yaml";
        
        // Convert YAML to flat key=value format for backward compatibility
        // Then merge with existing entries
        std::map<std::string, std::string> flatData;
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            // Handle nested structures recursively
            YAML::Node value = pair.second;
            flattenYAMLNode(key, value, flatData);
        }
        
        // Merge the data
        return mergeFromFlatData(flatData, ConfigSource::YAML);
        
    } catch (const YAML::Exception& e) {
        pImpl->addValidationError(std::string("YAML parse error: ") + e.what());
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->addValidationError("Cannot open JSON file: " + filepath);
        return false;
    }
    
    // Get file size to preallocate buffer
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read entire file content
    std::string content(fileSize, '\0');
    file.read(&content[0], fileSize);
    
    return loadFromJSONContent(content);
}

bool Config::loadFromYAMLContent(const std::string& yamlContent) {
    try {
        YAML::Node doc = YAML::Load(yamlContent);
        pImpl->format = "yaml";
        
        std::map<std::string, std::string> flatData;
        flattenYAMLNode("", doc, flatData);
        
        return mergeFromFlatData(flatData, ConfigSource::YAML);
        
    } catch (const YAML::Exception& e) {
        pImpl->addValidationError(std::string("YAML parse error: ") + e.what());
        return false;
    }
}

bool Config::loadFromJSONContent(const std::string& jsonContent) {
    try {
        rapidjson::Document doc;
        doc.Parse(jsonContent.c_str());
        
        if (doc.HasParseError()) {
            pImpl->addValidationError(std::string("JSON parse error: ") + 
                std::string(rapidjson::GetParseError_En(doc.GetParseError())) +
                " at position " + std::to_string(doc.GetErrorOffset()));
            return false;
        }
        
        pImpl->format = "json";
        
        // Convert JSON to flat key=value format
        std::map<std::string, std::string> flatData;
        flattenJSONNode("", doc, flatData);
        
        // Merge the data
        return mergeFromFlatData(flatData, ConfigSource::JSON);
        
    } catch (const std::exception& e) {
        pImpl->addValidationError(std::string("JSON processing error: ") + e.what());
        return false;
    }
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
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Default to key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write JSON content directly
    file << toJSON();
    
    return file.good();
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write YAML content directly
    file << toYAML();
    
    return file.good();
}

bool Config::loadSchema(const std::string& schemaFile) {
    try {
        pImpl->schema = std::make_unique<ConfigSchema>(ConfigSchema::fromFile(schemaFile));
        return true;
    } catch (const std::exception& e) {
        pImpl->addValidationError(std::string("Schema load error: ") + e.what());
        return false;
    }
}

bool Config::validate() const {
    pImpl->clearValidationErrors();
    
    if (pImpl->schema) {
        std::map<std::string, std::string> flatData;
        for (const auto& entry : pImpl->entries) {
            flatData[entry.key] = valueToString(entry.value);
        }
        
        return pImpl->schema->validate(flatData);
    }
    
    // Basic validation without schema
    for (const auto& entry : pImpl->entries) {
        if (entry.validationRegex.empty()) continue;
        
        std::string valueStr = valueToString(entry.value);
        if (!std::regex_match(valueStr, std::regex(entry.validationRegex))) {
            pImpl->addValidationError("Validation failed for key '" + entry.key + 
                "': value '" + valueStr + "' does not match pattern '" + 
                entry.validationRegex + "'");
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    validate(); // Ensure validation is up to date
    return pImpl->validationErrors;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->schema && pImpl->schema->validate({{key, valueToString(value)}}, ConfigSource::Runtime)) {
        // Validation passed
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "");
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

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->schema) {
        std::map<std::string, std::string> flatData = {{key, valueToString(value)}};
        if (!pImpl->schema->validate(flatData, source)) {
            return false;
        }
    }
    
    set(key, value, source);
    return true;
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    return oss.str();
}

bool Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = std::make_unique<ConfigSchema>(schema);
    return validate();
}

const ConfigSchema* Config::getSchema() const {
    return pImpl->schema.get();
}

std::string Config::toJSON() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    
    for (const auto& entry : pImpl->entries) {
        writer.Key(entry.key.c_str());
        
        std::visit([&writer](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                writer.String(arg.c_str());
            } else if constexpr (std::is_same_v<T, bool>) {
                writer.Bool(arg);
            } else if constexpr (std::is_same_v<T, int>) {
                writer.Int(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                writer.Int64(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                writer.Double(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.Int(item);
                }
                writer.EndArray();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.Double(item);
                }
                writer.EndArray();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                writer.StartArray();
                for (const auto& item : arg) {
                    writer.String(item.c_str());
                }
                writer.EndArray();
            }
        }, entry.value);
    }
    
    writer.EndObject();
    
    return buffer.GetString();
}

std::string Config::toYAML() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetMapFormat(YAML::E_MultiDoc);
    
    emitter << YAML::BeginDoc;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
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
    emitter << YAML::EndDoc;
    
    std::stringstream buffer;
    buffer << emitter;
    
    return buffer.str();
}

bool Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
    return true;
}

bool Config::mergeFromJSON(const std::string& jsonContent, ConfigSource source) {
    rapidjson::Document doc;
    doc.Parse(jsonContent.c_str());
    
    if (doc.HasParseError()) {
        return false;
    }
    
    std::map<std::string, std::string> flatData;
    flattenJSONNode("", doc, flatData);
    
    return mergeFromFlatData(flatData, source);
}

bool Config::mergeFromYAML(const std::string& yamlContent, ConfigSource source) {
    YAML::Node doc;
    try {
        doc = YAML::Load(yamlContent);
    } catch (const YAML::Exception& e) {
        return false;
    }
    
    std::map<std::string, std::string> flatData;
    flattenYAMLNode("", doc, flatData);
    
    return mergeFromFlatData(flatData, source);
}

// Helper functions
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

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

int Config::stringToInt(const std::string& str) {
    return std::stoi(str);
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

bool Config::isValidJSON(const std::string& str) {
    rapidjson::Document doc;
    doc.Parse(str.c_str());
    return !doc.HasParseError();
}

bool Config::isValidYAML(const std::string& str) {
    try {
        YAML::Load(str);
        return true;
    } catch (const YAML::Exception& e) {
        return false;
    }
}

// Legacy key=value file loading
bool Config::loadFromFileLegacy(const std::string& filepath) {
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

// Helper functions for JSON/YAML conversion
void Config::flattenJSONNode(const std::string& prefix, const rapidjson::Value& node, std::map<std::string, std::string>& flatData) {
    if (node.IsObject()) {
        for (auto it = node.MemberBegin(); it != node.MemberEnd(); ++it) {
            std::string key = prefix.empty() ? it->name.GetString() : prefix + "." + it->name.GetString();
            flattenJSONNode(key, it->value, flatData);
        }
    } else if (node.IsArray()) {
        for (SizeType i = 0; i < node.Size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            flattenJSONNode(key, node[i], flatData);
        }
    } else {
        std::string valueStr;
        
        if (node.IsString()) {
            valueStr = node.GetString();
        } else if (node.IsBool()) {
            valueStr = node.GetBool() ? "true" : "false";
        } else if (node.IsInt()) {
            valueStr = std::to_string(node.GetInt());
        } else if (node.IsInt64()) {
            valueStr = std::to_string(node.GetInt64());
        } else if (node.IsUint64()) {
            valueStr = std::to_string(node.GetUint64());
        } else if (node.IsDouble()) {
            valueStr = std::to_string(node.GetDouble());
        } else {
            valueStr = "";
        }
        
        flatData[prefix] = valueStr;
    }
}

void Config::flattenYAMLNode(const std::string& prefix, const YAML::Node& node, std::map<std::string, std::string>& flatData) {
    if (node.IsMap()) {
        for (const auto& pair : node) {
            std::string key = prefix.empty() ? pair.first.as<std::string>() : prefix + "." + pair.first.as<std::string>();
            flattenYAMLNode(key, pair.second, flatData);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            flattenYAMLNode(key, node[i], flatData);
        }
    } else {
        std::string valueStr;
        
        if (node.IsScalar()) {
            valueStr = node.as<std::string>();
        } else if (node.IsDouble()) {
            valueStr = std::to_string(node.as<double>());
        } else if (node.IsInt()) {
            valueStr = std::to_string(node.as<int>());
        } else if (node.IsBool()) {
            valueStr = node.as<bool>() ? "true" : "false";
        } else {
            valueStr = "";
        }
        
        flatData[prefix] = valueStr;
    }
}

bool Config::mergeFromFlatData(const std::map<std::string, std::string>& flatData, ConfigSource source) {
    bool success = true;
    
    for (const auto& pair : flatData) {
        const std::string& key = pair.first;
        const std::string& valueStr = pair.second;
        
        // Try to convert to appropriate type
        if (valueStr == "true" || valueStr == "false") {
            bool boolValue = stringToBool(valueStr);
            set(key, boolValue, source);
        } else {
            // Try to determine if it's an integer
            bool isInt = true;
            try {
                size_t pos;
                int intValue = std::stoi(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, intValue, source);
                    continue;
                }
            } catch (...) {
                isInt = false;
            }
            
            // Try to determine if it's a double
            bool isDouble = true;
            try {
                size_t pos;
                double doubleValue = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, doubleValue, source);
                    continue;
                }
            } catch (...) {
                isDouble = false;
            }
            
            // Default to string
            set(key, valueStr, source);
        }
    }
    
    return success;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

// Legacy helper functions
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

// Enhanced schema implementation with proper validation
bool ConfigSchema::validate(const std::map<std::string, std::string>& jsonData) const {
    std::vector<std::string> errors;
    
    for (const auto& entry : entries) {
        if (entry.required) {
            auto it = jsonData.find(entry.key);
            if (it == jsonData.end()) {
                errors.push_back("Required key '" + entry.key + "' is missing");
            }
        }
        
        auto it = jsonData.find(entry.key);
        if (it != jsonData.end()) {
            // Validate against regex if present
            if (!entry.validationRegex.empty()) {
                std::regex regex(entry.validationRegex);
                if (!std::regex_match(it->second, regex)) {
                    errors.push_back("Validation failed for key '" + entry.key + 
                        "': value '" + it->second + "' does not match pattern '" + 
                        entry.validationRegex + "'");
                }
            }
            
            // Type validation based on default value
            if (!entry.defaultValue.empty()) {
                // Simple type checking based on default value format
                if (entry.defaultValue == "true" || entry.defaultValue == "false") {
                    // Should be boolean
                    const std::string& value = it->second;
                    if (value != "true" && value != "false" && value != "0" && value != "1") {
                        errors.push_back("Invalid type for key '" + entry.key + 
                            "': expected boolean, got '" + value + "'");
                    }
                } else if (entry.defaultValue.find('.') != std::string::npos) {
                    // Should be numeric (double)
                    try {
                        double val = std::stod(it->second);
                    } catch (const std::exception&) {
                        errors.push_back("Invalid type for key '" + entry.key + 
                            "': expected numeric value, got '" + it->second + "'");
                    }
                } else {
                    // Try to determine type
                    try {
                        std::stoi(it->second);
                    } catch (...) {
                        try {
                            std::stoll(it->second);
                        } catch (...) {
                            // Not an integer, check if it could be a double
                            try {
                                std::stod(it->second);
                            } catch (...) {
                                // Could be string, which is valid
                            }
                        }
                    }
                }
            }
        }
    }
    
    return errors.empty();
}

std::map<std::string, std::string> ConfigSchema::toFlatFormat() const {
    std::map<std::string, std::string> flatData;
    
    for (const auto& entry : entries) {
        flatData[entry.key] = entry.defaultValue;
    }
    
    return flatData;
}

// Implement proper JSON schema parsing using rapidjson
ConfigSchema ConfigSchema::fromJSON(const std::string& jsonContent) {
    ConfigSchema schema;
    try {
        rapidjson::Document doc;
        doc.Parse(jsonContent.c_str());
        
        if (doc.HasParseError()) {
            throw ConfigException("JSON schema parse error: " + 
                std::string(rapidjson::GetParseError_En(doc.GetParseError())) +
                " at position " + std::to_string(doc.GetErrorOffset()));
        }
        
        // Parse schema entries
        if (doc.HasMember("entries") && doc["entries"].IsArray()) {
            for (const auto& entry : doc["entries"].GetArray()) {
                if (entry.IsObject()) {
                    ConfigEntry configEntry;
                    
                    if (entry.HasMember("key") && entry["key"].IsString()) {
                        configEntry.key = entry["key"].GetString();
                    }
                    
                    if (entry.HasMember("defaultValue") && entry["defaultValue"].IsString()) {
                        configEntry.defaultValue = entry["defaultValue"].GetString();
                    }
                    
                    if (entry.HasMember("required") && entry["required"].IsBool()) {
                        configEntry.required = entry["required"].GetBool();
                    }
                    
                    if (entry.HasMember("validationRegex") && entry["validationRegex"].IsString()) {
                        configEntry.validationRegex = entry["validationRegex"].GetString();
                    }
                    
                    if (entry.HasMember("description") && entry["description"].IsString()) {
                        configEntry.description = entry["description"].GetString();
                    }
                    
                    schema.entries[configEntry.key] = configEntry;
                }
            }
        }
        
        if (doc.HasMember("schemaVersion") && doc["schemaVersion"].IsString()) {
            schema.schemaVersion = doc["schemaVersion"].GetString();
        }
        
        schema.filename = ""; // Unknown for string-based schemas
        
    } catch (const std::exception& e) {
        throw ConfigException("JSON schema processing error: " + std::string(e.what()));
    }
    
    return schema;
}

// Implement proper YAML schema parsing using yaml-cpp
ConfigSchema ConfigSchema::fromYAML(const std::string& yamlContent) {
    ConfigSchema schema;
    try {
        YAML::Node doc = YAML::Load(yamlContent);
        
        // Parse schema entries
        if (doc["entries"] && doc["entries"].IsSequence()) {
            for (const auto& entry : doc["entries"].AsSequence()) {
                if (entry.IsMap()) {
                    ConfigEntry configEntry;
                    
                    if (entry["key"] && entry["key"].IsScalar()) {
                        configEntry.key = entry["key"].as<std::string>();
                    }
                    
                    if (entry["defaultValue"] && entry["defaultValue"].IsScalar()) {
                        configEntry.defaultValue = entry["defaultValue"].as<std::string>();
                    }
                    
                    if (entry["required"] && entry["required"].IsScalar()) {
                        configEntry.required = entry["required"].as<bool>();
                    }
                    
                    if (entry["validationRegex"] && entry["validationRegex"].IsScalar()) {
                        configEntry.validationRegex = entry["validationRegex"].as<std::string>();
                    }
                    
                    if (entry["description"] && entry["description"].IsScalar()) {
                        configEntry.description = entry["description"].as<std::string>();
                    }
                    
                    schema.entries[configEntry.key] = configEntry;
                }
            }
        }
        
        if (doc["schemaVersion"] && doc["schemaVersion"].IsScalar()) {
            schema.schemaVersion = doc["schemaVersion"].as<std::string>();
        }
        
        schema.filename = ""; // Unknown for string-based schemas
        
    } catch (const YAML::Exception& e) {
        throw ConfigException("YAML schema parsing error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigException("YAML schema processing error: " + std::string(e.what()));
    }
    
    return schema;
}

// Enhanced file-based schema parsing with format detection
ConfigSchema ConfigSchema::fromFile(const std::string& filepath) {
    ConfigSchema schema;
    schema.filename = filepath;
    
    try {
        // Determine file format based on extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Cannot open schema file: " + filepath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        if (ext == ".json") {
            return fromJSON(content);
        } else if (ext == ".yaml" || ext == ".yml") {
            return fromYAML(content);
        } else {
            throw ConfigException("Unsupported schema file format: " + ext);
        }
        
    } catch (const std::exception& e) {
        throw ConfigException("Schema file processing error: " + std::string(e.what()));
    }
}

// C-style API implementation
Config* ConfigCreate() {
    return new Config();
}

void ConfigDestroy(Config* config) {
    delete config;
}

bool ConfigLoadFromFile(Config* config, const char* filepath) {
    return config->loadFromFile(filepath);
}

bool ConfigLoadFromJSON(Config* config, const char* filepath) {
    return config->loadFromJSON(filepath);
}

bool ConfigLoadFromYAML(Config* config, const char* filepath) {
    return config->loadFromYAML(filepath);
}

bool ConfigSaveToFile(Config* config, const char* filepath) {
    return config->saveToFile(filepath);
}

bool ConfigSaveToJSON(Config* config, const char* filepath) {
    return config->saveToJSON(filepath);
}

bool ConfigSaveToYAML(Config* config, const char* filepath) {
    return config->saveToYAML(filepath);
}

bool ConfigHas(Config* config, const char* key) {
    return config->has(key);
}

const char* ConfigGetString(Config* config, const char* key, const char* defaultValue) {
    auto value = config->get<std::string>(key);
    if (value.has_value()) {
        return value.value().c_str();
    }
    return defaultValue;
}

int ConfigGetInt(Config* config, const char* key, int defaultValue) {
    auto value = config->get<int>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

double ConfigGetDouble(Config* config, const char* key, double defaultValue) {
    auto value = config->get<double>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

bool ConfigGetBool(Config* config, const char* key, bool defaultValue) {
    auto value = config->get<bool>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

void ConfigSetString(Config* config, const char* key, const char* value) {
    config->set(key, value);
}

void ConfigSetInt(Config* config, const char* key, int value) {
    config->set(key, value);
}

void ConfigSetDouble(Config* config, const char* key, double value) {
    config->set(key, value);
}

void ConfigSetBool(Config* config, const char* key, bool value) {
    config->set(key, value);
}

const char* ConfigSummary(Config* config) {
    static std::string summary;
    summary = config->summary();
    return summary.c_str();
}

} // namespace nlm