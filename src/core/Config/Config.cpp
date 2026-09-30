#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), lastError("") {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = getFileFormat(filepath);
    
    try {
        if (format == "json") {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                lastError = "Cannot open JSON file for reading: " + filepath;
                return false;
            }
            
            try {
                nlohmann::json jsonData = nlohmann::json::parse(file);
                return loadFromJSONData(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                lastError = std::string("Invalid JSON format in file ") + filepath + ": " + e.what();
                return false;
            }
        } else if (format == "yaml" || format == "yml") {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                lastError = "Cannot open YAML file for reading: " + filepath;
                return false;
            }
            
            try {
                YAML::Node yamlData = YAML::LoadFile(filepath);
                return loadFromYAMLData(yamlData);
            } catch (const YAML::Exception& e) {
                lastError = std::string("Invalid YAML format in file ") + filepath + ": " + e.what();
                return false;
            }
        } else {
            return loadFromKeyValue(filepath);
        }
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("Unexpected error loading config: ") + e.what();
        return false;
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            lastError = "Cannot open key=value file for reading: " + filepath;
            return false;
        }
        
        nlohmann::json jsonData;
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
                
                jsonData[key] = value;
            }
        }
        
        return loadFromJSONData(jsonData);
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("Key-value parsing error: ") + e.what();
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string format = getFileFormat(filepath);
    
    try {
        if (format == "json") {
            return saveToJSON(filepath, true);
        } else if (format == "yaml" || format == "yml") {
            return saveToYAML(filepath, true);
        } else {
            return saveToKeyValue(filepath);
        }
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("Save error: ") + e.what();
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJson(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            lastError = "Cannot open file for writing: " + filepath;
            return false;
        }
        
        if (pretty) {
            file << jsonData.dump(4);
        } else {
            file << jsonData.dump();
        }
        
        return true;
    } catch (const std::exception& e) {
        lastError = std::string("JSON save error: ") + e.what();
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath, bool pretty) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJson(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            lastError = "Cannot open file for writing: " + filepath;
            return false;
        }
        
        if (pretty) {
            file << "# NLM Configuration File\n# Generated by NLM Configuration System\n\n";
            for (const auto& [key, value] : jsonData.items()) {
                file << key << " = " << value.dump() << "\n";
            }
        } else {
            file << "# NLM Configuration File\n# Generated by NLM Configuration System\n\n";
            for (const auto& [key, value] : jsonData.items()) {
                if (value.is_string()) {
                    file << key << " = \"" << value.get<std::string>() << "\"\n";
                } else {
                    file << key << " = " << value.dump() << "\n";
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        lastError = std::string("YAML save error: ") + e.what();
        return false;
    }
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        lastError = "Cannot open file for writing: " + filepath;
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setAuto(key, value, ConfigSource::CommandLine);
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                setAuto(key, value, ConfigSource::CommandLine);
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("Command line parsing error: ") + e.what();
        return false;
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

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setAuto(const std::string& key, const std::string& value, ConfigSource source) {
    std::string type = detectType(value);
    auto configValue = stringToConfigValue(value, type);
    
    if (configValue.has_value()) {
        set(key, configValue.value(), source);
    } else {
        lastError = "Cannot convert string to config value for key: " + key;
        set(key, ConfigValue(value), source);
    }
}

std::string Config::getString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return configValueToString(it->value);
}

std::string Config::getType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    return it->type;
}

void Config::addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, 
                      const std::string& description, const std::string& type) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description;
        it->type = type;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, type);
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

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::getFileFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension.empty()) {
        return "keyvalue";
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return "json";
    } else if (extension == ".yaml" || extension == ".yml") {
        return "yaml";
    } else if (extension == ".cfg" || extension == ".conf" || extension.empty()) {
        return "keyvalue";
    }
    
    return "auto";
}

std::string Config::getLastError() const {
    return lastError;
}

std::string Config::configValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << arg;
            std::string result = oss.str();
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
        return "";
    }, value);
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        }
        return nullptr;
    }, value);
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str, const std::string& type) {
    if (type == "auto") {
        return stringToConfigValue(str);
    } else if (type == "int" || type == "integer") {
        try {
            return std::stoll(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (type == "double" || type == "float") {
        try {
            return std::stod(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (type == "bool" || type == "boolean") {
        std::string lowerStr = toLower(str);
        if (lowerStr == "true" || lowerStr == "yes" || lowerStr == "1") {
            return true;
        } else if (lowerStr == "false" || lowerStr == "no" || lowerStr == "0") {
            return false;
        }
        return std::nullopt;
    } else if (type == "string") {
        return str;
    }
    
    return stringToConfigValue(str);
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str) {
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    std::string lowerStr = toLower(str);
    if (lowerStr == "true" || lowerStr == "yes" || lowerStr == "1") {
        return true;
    } else if (lowerStr == "false" || lowerStr == "no" || lowerStr == "0") {
        return false;
    }
    
    return str;
}

std::string Config::detectType(const std::string& str) {
    if (str.empty()) {
        return "string";
    }
    
    std::string lowerStr = toLower(str);
    if (lowerStr == "true" || lowerStr == "false" || lowerStr == "yes" || lowerStr == "no" || 
        lowerStr == "1" || lowerStr == "0") {
        return "bool";
    }
    
    size_t pos;
    try {
        std::stoll(str, &pos);
        if (pos == str.size()) {
            return "int";
        }
    } catch (...) {}
    
    try {
        std::stod(str, &pos);
        if (pos == str.size()) {
            return "double";
        }
    } catch (...) {}
    
    if (str.front() == '[' && str.back() == ']') {
        std::string content = str.substr(1, str.size() - 2);
        if (content.empty()) {
            return "string";
        }
        
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        size_t start = 0;
        size_t end;
        while ((end = content.find(',', start)) != std::string::npos) {
            std::string element = content.substr(start, end - start);
            trim(element);
            
            if (isInteger(element)) allStrings = allDoubles = false;
            else if (isDouble(element)) allInts = allStrings = false;
            else allInts = allDoubles = false;
            
            start = end + 1;
        }
        
        std::string lastElement = content.substr(start);
        trim(lastElement);
        if (isInteger(lastElement)) allStrings = allDoubles = false;
        else if (isDouble(lastElement)) allInts = allStrings = false;
        else allInts = allDoubles = false;
        
        if (allInts) return "vector<int>";
        if (allDoubles) return "vector<double>";
        if (allStrings) return "vector<string>";
        return "vector<string>";
    }
    
    return "string";
}

bool Config::isInteger(const std::string& str) {
    try {
        size_t pos;
        std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::isDouble(const std::string& str) {
    try {
        size_t pos;
        std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::loadFromJSONData(const nlohmann::json& jsonData) {
    try {
        clear();
        
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                auto configValue = jsonValueToConfigValue(value);
                if (configValue.has_value()) {
                    set(key, configValue.value(), ConfigSource::File);
                } else {
                    set(key, value.get<std::string>(), ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("JSON data loading error: ") + e.what();
        return false;
    }
}

bool Config::loadFromYAMLData(const YAML::Node& yamlData) {
    try {
        clear();
        
        nlohmann::json jsonData;
        yamlToJSON(yamlData, jsonData);
        
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                auto configValue = jsonValueToConfigValue(value);
                if (configValue.has_value()) {
                    set(key, configValue.value(), ConfigSource::File);
                } else {
                    set(key, value.get<std::string>(), ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        lastError = e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = std::string("YAML data loading error: ") + e.what();
        return false;
    }
}

std::optional<ConfigValue> Config::jsonValueToConfigValue(const nlohmann::json& j) {
    if (j.is_null()) {
        return std::nullopt;
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& item : j) {
            if (item.is_number_integer()) {
                allStrings = allDoubles = false;
            } else if (item.is_number_float()) {
                allInts = allStrings = false;
            } else if (item.is_string()) {
                allInts = allDoubles = false;
            } else {
                allInts = allDoubles = allStrings = false;
                break;
            }
        }
        
        if (allInts) {
            std::vector<int> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<int64_t>());
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (allStrings) {
            std::vector<std::string> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        } else {
            std::vector<std::string> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.dump());
            }
            return vec;
        }
    }
    
    return std::nullopt;
}

void Config::yamlToJSON(const YAML::Node& node, nlohmann::json& json) {
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        try {
            if (value.find('.') != std::string::npos) {
                double d = std::stod(value);
                json = d;
            } else {
                int64_t i = std::stoll(value);
                json = i;
            }
        } catch (...) {
            if (value == "true" || value == "false") {
                json = (value == "true");
            } else {
                json = value;
            }
        }
    } else if (node.IsSequence()) {
        nlohmann::json array;
        for (const auto& item : node) {
            nlohmann::json itemJson;
            yamlToJSON(item, itemJson);
            array.push_back(itemJson);
        }
        json = array;
    } else if (node.IsMap()) {
        nlohmann::json object;
        for (const auto& pair : node) {
            nlohmann::json valueJson;
            yamlToJSON(pair.second, valueJson);
            object[pair.first.as<std::string>()] = valueJson;
        }
        json = object;
    } else if (node.IsNull()) {
        json = nullptr;
    }
}

bool Config::validate() const {
    return true;
}