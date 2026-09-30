#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    try {
        if (ext == "json") {
            return loadFromJSON(filepath);
        } else if (ext == "yaml" || ext == "yml") {
            return loadFromYAML(filepath);
        } else {
            // Try to auto-detect format
            std::string line;
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::getline(file, line);
            file.close();
            
            if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
                return loadFromJSON(filepath);
            } else if (line.find("---") != std::string::npos || line.find("key:") != std::string::npos) {
                return loadFromYAML(filepath);
            } else {
                // Fallback to simple key=value format
                return loadFromSimpleFormat(filepath);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON data
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_number_integer()) {
                configValue = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                configValue = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_array()) {
                // Handle arrays based on element type
                auto arr = it->get<std::vector<nlohmann::json>>();
                if (!arr.empty() && arr[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& elem : arr) {
                        intArray.push_back(elem.get<int>());
                    }
                    configValue = intArray;
                } else if (!arr.empty() && arr[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& elem : arr) {
                        doubleArray.push_back(elem.get<double>());
                    }
                    configValue = doubleArray;
                } else {
                    std::vector<std::string> stringArray;
                    for (const auto& elem : arr) {
                        stringArray.push_back(elem.get<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse YAML data recursively
        parseYAMLNode(doc, "");
        
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseYAMLNode(const YAML::Node& node, const std::string& prefix) {
    if (node.IsMap()) {
        for (const auto& it : node) {
            std::string key = prefix.empty() ? it.first.as<std::string>() : prefix + "." + it.first.as<std::string>();
            parseYAMLNode(it.second, key);
        }
    } else if (node.IsScalar()) {
        // Convert YAML scalar to ConfigValue
        ConfigValue configValue;
        std::string value = node.as<std::string>();
        
        // Try to determine type
        if (value == "true" || value == "false") {
            configValue = (value == "true");
        } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
            configValue = std::stoll(value);
        } else if (std::regex_match(value, std::regex("^-?\d+\.\d*$") ) || 
                   std::regex_match(value, std::regex("^\d+\.\d*$") ) ||
                   std::regex_match(value, std::regex("^-?\d*\.\d+$") )) {
            configValue = std::stod(value);
        } else {
            configValue = value;
        }
        
        set(prefix, configValue, ConfigSource::File);
    } else if (node.IsSequence()) {
        // Handle sequences - convert to string array for now
        std::vector<std::string> stringArray;
        for (const auto& elem : node) {
            stringArray.push_back(elem.as<std::string>());
        }
        set(prefix, stringArray, ConfigSource::File);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    try {
        if (ext == "json") {
            saveToJSON(file);
        } else if (ext == "yaml" || ext == "yml") {
            saveToYAML(file);
        } else {
            // Default to simple format
            saveToSimpleFormat(file);
        }
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::saveToJSON(std::ostream& os) const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        setJSONValue(j, entry.key, entry.value);
    }
    
    os << j.dump(2);
}

void Config::saveToYAML(std::ostream& os) const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    emitter << YAML::BeginDoc;
    emitYAMLValue(emitter, pImpl->entries, "");
    emitter << YAML::EndDoc;
    
    os << emitter.c_str();
}

void Config::setJSONValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) {
    std::string currentKey = key;
    size_t dotPos = key.find('.');
    
    if (dotPos != std::string::npos) {
        // Nested key - create nested objects
        std::string parentKey = key.substr(0, dotPos);
        std::string childKey = key.substr(dotPos + 1);
        
        if (!j.contains(parentKey)) {
            j[parentKey] = nlohmann::json::object();
        }
        
        setJSONValue(j[parentKey], childKey, value);
    } else {
        // Simple key
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[key] = val;
            }
        }, value);
    }
}

void Config::emitYAMLValue(YAML::Emitter& emitter, const std::vector<ConfigEntry>& entries, const std::string& prefix) {
    bool first = true;
    
    for (const auto& entry : entries) {
        if (entry.key.empty() || (prefix.empty() && entry.key.find('.') != std::string::npos)) {
            // Handle nested keys
            size_t dotPos = entry.key.find('.');
            if (dotPos != std::string::npos) {
                std::string parentKey = entry.key.substr(0, dotPos);
                std::string childKey = entry.key.substr(dotPos + 1);
                
                if (first) {
                    emitter << YAML::BeginMap;
                    first = false;
                }
                
                if (!hasNestedKey(entries, parentKey)) {
                    emitter << YAML::Key << parentKey;
                    emitter << YAML::Value;
                    emitYAMLValue(emitter, entries, parentKey);
                }
            }
        } else if (prefix.empty() && entry.key.find('.') == std::string::npos) {
            // Top-level key
            emitter << YAML::Key << entry.key;
            emitYAMLConfigValue(emitter, entry.value);
        } else if (!prefix.empty() && entry.key == prefix) {
            // Current prefix
            emitter << YAML::Key << entry.key;
            emitYAMLConfigValue(emitter, entry.value);
        }
    }
    
    if (!first) {
        emitter << YAML::EndMap;
    }
}

bool Config::hasNestedKey(const std::vector<ConfigEntry>& entries, const std::string& key) const {
    for (const auto& entry : entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string parentKey = entry.key.substr(0, dotPos);
            if (parentKey == key) {
                return true;
            }
        }
    }
    return false;
}

void Config::emitYAMLConfigValue(YAML::Emitter& emitter, const ConfigValue& value) {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            emitter << val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            emitter << static_cast<int>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            emitter << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            emitter << YAML::FlowSeq;
            for (const auto& elem : val) {
                emitter << elem;
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            emitter << YAML::FlowSeq;
            for (const auto& elem : val) {
                emitter << elem;
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::FlowSeq;
            for (const auto& elem : val) {
                emitter << elem;
            }
        }
    }, value);
}

void Config::saveToSimpleFormat(std::ostream& os) const {
    for (const auto& entry : pImpl->entries) {
        os << "# " << entry.description << "\n";
        os << entry.key << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                os << val;
            } else if constexpr (std::is_same_v<T, double>) {
                os << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                os << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                os << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                os << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) os << ", ";
                    os << val[i];
                }
                os << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                os << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) os << ", ";
                    os << std::fixed << std::setprecision(6) << val[i];
                }
                os << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                os << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) os << ", ";
                    os << "\"" << val[i] << "\"";
                }
                os << "]";
            }
        }, entry.value);
        
        os << "\n\n";
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

