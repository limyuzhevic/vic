#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> validators;
    std::unordered_map<std::string, std::string> envVariableMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return parseConfigString(filepath, "json");
    } else if (ext == ".yaml" || ext == ".yml") {
        return parseConfigString(filepath, "yaml");
    } else {
        return parseConfigString(filepath, "keyvalue");
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
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (format == "json" || ext == ".json") {
        nlohmann::json j = toJSON();
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError("Failed to open file for writing: " + filepath, ConfigError::ErrorType::FileIOError);
        }
        file << std::setw(2) << j;
        return true;
    } else if (format == "yaml" || ext == ".yaml" || ext == ".yml") {
        YAML::Emitter emitter;
        toYAML(emitter);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError("Failed to open file for writing: " + filepath, ConfigError::ErrorType::FileIOError);
        }
        file << emitter.c_str();
        return true;
    } else {
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
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    }
}

void Config::setFromEnvironment(const std::string& key, const std::string& envVarName) {
    const char* envValue = std::getenv(envVarName.c_str());
    if (envValue) {
        set(key, std::string(envValue), ConfigSource::EnvironmentVariable);
        pImpl->envVariableMap[key] = envVarName;
    }
}

bool Config::isFromEnvironment(const std::string& key) const {
    return pImpl->envVariableMap.find(key) != pImpl->envVariableMap.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->envVariableMap.erase(key);
    pImpl->validators.erase(key);
}

bool Config::validate(const std::string& key) const {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (entryIt != pImpl->entries.end()) {
            for (const auto& validator : it->second) {
                if (!validator(entryIt->value)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Config::addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key].push_back(validator);
}

bool Config::parseFromFile(const std::string& filepath, const std::string& format) {
    return parseConfigString(filepath, format);
}

bool Config::parseConfigString(const std::string& configString, const std::string& format) {
    if (format == "json") {
        try {
            std::ifstream file(configString);
            if (!file.is_open()) {
                throw ConfigError("Failed to open file: " + configString, ConfigError::ErrorType::FileIOError);
            }
            
            nlohmann::json j;
            file >> j;
            parseJSON(j);
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("JSON parsing error: ") + e.what(), ConfigError::ErrorType::InvalidFormat);
        }
    } else if (format == "yaml") {
        try {
            YAML::Node doc = YAML::LoadFile(configString);
            parseYAML(doc);
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("YAML parsing error: ") + e.what(), ConfigError::ErrorType::InvalidFormat);
        }
    } else {
        std::ifstream file(configString);
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
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

ConfigValue Config::getRawValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    throw ConfigError("Key not found: " + key, ConfigError::ErrorType::MissingKey);
}

nlohmann::json Config::toJSON() const {
    nlohmann::json j = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json value;
        
        std::visit([&value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
                value = arg;
            }
        }, entry.value);
        
        j[entry.key] = value;
    }
    
    return j;
}

void Config::toYAML(YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
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
            } else if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
                emitter << YAML::BeginMap;
                for (const auto& [k, v] : arg) {
                    emitter << YAML::Key << k;
                    emitter << YAML::Value << v;
                }
                emitter << YAML::EndMap;
            }
        }, entry.value);
        
        if (!entry.description.empty()) {
            emitter << YAML::Key << "_description";
            emitter << YAML::Value << entry.description;
        }
        
        emitter << YAML::Key << "_source";
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default: sourceStr = "default"; break;
            case ConfigSource::File: sourceStr = "file"; break;
            case ConfigSource::CommandLine: sourceStr = "commandline"; break;
            case ConfigSource::Runtime: sourceStr = "runtime"; break;
            case ConfigSource::EnvironmentVariable: sourceStr = "environment"; break;
        }
        emitter << sourceStr;
    }
    
    emitter << YAML::EndMap;
}

void Config::parseJSON(const nlohmann::json& j) {
    if (!j.is_object()) {
        throw ConfigError("JSON root must be an object", ConfigError::ErrorType::InvalidFormat);
    }
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        ConfigSource source = ConfigSource::File;
        
        if (it.value().contains("_source")) {
            std::string sourceStr = it.value()["_source"].get<std::string>();
            if (sourceStr == "file") source = ConfigSource::File;
            else if (sourceStr == "commandline") source = ConfigSource::CommandLine;
            else if (sourceStr == "runtime") source = ConfigSource::Runtime;
            else if (sourceStr == "environment") source = ConfigSource::EnvironmentVariable;
        }
        
        std::string description = it.value().contains("_description") ? 
            it.value()["_description"].get<std::string>() : "";
        
        ConfigValue value = jsonToConfigValue(it.value());
        set(key, value, source);
        
        if (!description.empty()) {
            auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (entryIt != pImpl->entries.end()) {
                entryIt->description = description;
            }
        }
    }
}

void Config::parseYAML(const YAML::Node& node) {
    if (!node.IsMap()) {
        throw ConfigError("YAML root must be a mapping", ConfigError::ErrorType::InvalidFormat);
    }
    
    for (auto it = node.begin(); it != node.end(); ++it) {
        std::string key = it->first.as<std::string>();
        ConfigSource source = ConfigSource::File;
        
        if (it->second["_source"]) {
            std::string sourceStr = it->second["_source"].as<std::string>();
            if (sourceStr == "file") source = ConfigSource::File;
            else if (sourceStr == "commandline") source = ConfigSource::CommandLine;
            else if (sourceStr == "runtime") source = ConfigSource::Runtime;
            else if (sourceStr == "environment") source = ConfigSource::EnvironmentVariable;
        }
        
        std::string description = it->second["_description"] ? 
            it->second["_description"].as<std::string>() : "";
        
        ConfigValue value = yamlToConfigValue(it->second);
        set(key, value, source);
        
        if (!description.empty()) {
            auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (entryIt != pImpl->entries.end()) {
                entryIt->description = description;
            }
        }
    }
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        if (j.empty()) {
            return std::vector<int>{};
        }
        
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& elem : j) {
            if (!elem.is_number_integer()) allInts = false;
            if (!elem.is_number()) allDoubles = false;
            if (!elem.is_string()) allStrings = false;
        }
        
        if (allInts) {
            return j.get<std::vector<int>>();
        } else if (allDoubles) {
            return j.get<std::vector<double>>();
        } else if (allStrings) {
            return j.get<std::vector<std::string>>();
        } else {
            std::map<std::string, nlohmann::json> nested;
            for (auto it = j.begin(); it != j.end(); ++it) {
                nested[it.key()] = *it;
            }
            return nested;
        }
    } else if (j.is_object()) {
        if (j.contains("_source") || j.contains("_description")) {
            throw std::runtime_error("Metadata field encountered");
        }
        
        std::map<std::string, nlohmann::json> nested;
        for (auto it = j.begin(); it != j.end(); ++it) {
            nested[it.key()] = *it;
        }
        return nested;
    }
    
    throw ConfigError("Unsupported JSON type for config value", ConfigError::ErrorType::TypeMismatch);
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.IsBoolean()) {
            return node.as<bool>();
        } else if (node.IsInteger()) {
            return node.as<int>();
        } else if (node.IsFloat()) {
            return node.as<double>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (node.empty()) {
            return std::vector<int>{};
        }
        
        bool allInts = true;
        bool allFloats = true;
        bool allStrings = true;
        
        for (const auto& elem : node) {
            if (!elem.IsInteger()) allInts = false;
            if (!elem.IsFloat()) allFloats = false;
            if (!elem.IsScalar() || elem.Scalar().find_first_not_of("0123456789.") != std::string::npos) allStrings = false;
        }
        
        if (allInts) {
            return node.as<std::vector<int>>();
        } else if (allFloats) {
            return node.as<std::vector<double>>();
        } else if (allStrings) {
            return node.as<std::vector<std::string>>();
        } else {
            std::map<std::string, nlohmann::json> nested;
            for (auto it = node.begin(); it != node.end(); ++it) {
                nested[it->first.as<std::string>()] = nlohmann::json(*it);
            }
            return nested;
        }
    } else if (node.IsMap()) {
        std::map<std::string, nlohmann::json> nested;
        for (auto it = node.begin(); it != node.end(); ++it) {
            nested[it->first.as<std::string>()] = nlohmann::json(*it);
        }
        return nested;
    }
    
    throw ConfigError("Unsupported YAML type for config value", ConfigError::ErrorType::TypeMismatch);
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

} // namespace nlm

// YAML-CPP library header includes need to be added to CMakeLists.txt
// Add find_package(yaml-cpp REQUIRED) to CMakeLists.txt

// nlohmann/json is header-only and should be available via CMake or package manager

// Missing includes for YAML-CPP:
// #include <yaml-cpp/yaml.h>
