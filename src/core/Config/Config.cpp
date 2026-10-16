#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <map>
#include <cctype>
#include <ctime>

// JSON library support
#include "nlohmann/json.hpp"

// YAML library support
#include "yaml-cpp/yaml.h"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validationEnabled_;
};

Config::Config() : pImpl(std::make_unique<Impl>())
{
    // Initialize validation to enabled by default
    pImpl->validationEnabled_ = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first, then YAML, then fallback to simple format
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    if (ext == ".json") {
        return loadFromJSON(content);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(content);
    } else {
        // Backward compatibility with simple key=value format
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
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
                
                setFromString(key, value, ConfigSource::File);
            }
        }
        return true;
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        // Save as JSON format
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            file << "  \"" << entry.key << "\": ";
            
            // Convert value to JSON string
            file << valueToJSON(entry.value);
            
            file << "\n";
        }
        file << "}\n";
    } else if (ext == ".yaml" || ext == ".yml") {
        // Save as YAML format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": " << valueToYAML(entry.value) << "\n";
        }
    } else {
        // Save as simple key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToSimple(entry.value) << "\n";
        }
    }
    
    return true;
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

// Helper function to detect file type from content
static bool isJSON(const std::string& content) {
    // Quick check for JSON-like structure
    if (content.empty()) return false;
    
    char first = content[0];
    return (first == '{' || first == '[');
}

static bool isYAML(const std::string& content) {
    // Quick check for YAML-like structure
    if (content.empty()) return false;
    
    std::string trimmed = trim(content);
    return (trimmed.find(':') != std::string::npos || 
            trimmed.find('-') != std::string::npos);
}

// Configuration value serialization/deserialization
std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::fromString(const std::string& str, ConfigValue& value) {
    // Trim whitespace
    std::string trimmed = trim(str);
    
    // Try parsing as boolean
    if (trimmed == "true" || trimmed == "false") {
        value = (trimmed == "true");
        return true;
    }
    
    // Try parsing as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(trimmed, &pos);
        if (pos == trimmed.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try parsing as double
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    value = trimmed;
    return true;
}

std::string Config::toJSON(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j.dump(2);
}

bool Config::fromJSON(const nlohmann::json& j, ConfigValue& value) {
    try {
        if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_number_integer()) {
            // Try to fit into int64_t
            value = j.get<int64_t>();
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_array()) {
            // Try to detect array type
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;
            
            for (const auto& item : j) {
                if (!item.is_number_integer()) allInt = false;
                if (!item.is_number_float()) allDouble = false;
                if (!item.is_string()) allString = false;
            }
            
            if (allInt) {
                std::vector<int> intVec;
                for (const auto& item : j) {
                    intVec.push_back(item.get<int>());
                }
                value = intVec;
            } else if (allDouble) {
                std::vector<double> doubleVec;
                for (const auto& item : j) {
                    doubleVec.push_back(item.get<double>());
                }
                value = doubleVec;
            } else if (allString) {
                std::vector<std::string> stringVec;
                for (const auto& item : j) {
                    stringVec.push_back(item.get<std::string>());
                }
                value = stringVec;
            } else {
                // Mixed array - store as string representation
                value = j.dump();
            }
        } else {
            return false; // Unsupported type
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::toYAML(const ConfigValue& value) {
    YAML::Emitter emitter;
    std::visit([&emitter](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            emitter << YAML::DoubleQuotedString(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << YAML::DoubleQuotedString(item);
            }
            emitter << YAML::EndSeq;
        } else {
            emitter << "unknown";
        }
    }, value);
    return emitter.c_str();
}

bool Config::fromYAML(const YAML::Node& node, ConfigValue& value) {
    try {
        if (node.IsScalar()) {
            std::string str = node.Scalar();
            if (str == "true" || str == "false") {
                value = (str == "true");
            } else {
                // Try to parse as number
                try {
                    if (str.find('.') != std::string::npos) {
                        value = std::stod(str);
                    } else {
                        value = std::stoll(str);
                    }
                } catch (...) {
                    value = str;
                }
            }
        } else if (node.IsSequence()) {
            // Check array type
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;
            
            for (const auto& item : node) {
                if (!item.IsScalar()) {
                    allInt = allDouble = allString = false;
                    break;
                }
                std::string str = item.Scalar();
                if (str == "true" || str == "false") {
                    allInt = allDouble = allString = false;
                    break;
                }
                try {
                    if (str.find('.') == std::string::npos) {
                        std::stoll(str);
                    } else {
                        std::stod(str);
                    }
                } catch (...) {
                    allInt = allDouble = false;
                }
            }
            
            if (allInt) {
                std::vector<int> intVec;
                for (const auto& item : node) {
                    intVec.push_back(item.as<int>());
                }
                value = intVec;
            } else if (allDouble) {
                std::vector<double> doubleVec;
                for (const auto& item : node) {
                    doubleVec.push_back(item.as<double>());
                }
                value = doubleVec;
            } else if (allString) {
                std::vector<std::string> stringVec;
                for (const auto& item : node) {
                    stringVec.push_back(item.as<std::string>());
                }
                value = stringVec;
            } else {
                // Mixed array - store as string
                value = node.as<std::string>();
            }
        } else if (node.IsMap()) {
            // Map as string representation
            std::ostringstream oss;
            oss << node;
            value = oss.str();
        } else {
            return false; // Unsupported type
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto j = nlohmann::json::parse(jsonStr);
        clear();
        
        for (auto& [key, value] : j.items()) {
            ConfigValue configValue;
            if (fromJSON(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto node = YAML::Load(yamlStr);
        clear();
        
        for (const auto& key : node.keys()) {
            if (node[key]) {
                ConfigValue configValue;
                if (fromYAML(node[key], configValue)) {
                    set(key, configValue, ConfigSource::File);
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::saveToJSON() const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        nlohmann::json value;
        std::visit([&value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = arg;
            }
        }, entry.value);
        
        j[entry.key] = value;
    }
    
    return j.dump(2);
}

std::string Config::saveToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::DoubleQuotedString(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << YAML::DoubleQuotedString(item);
                }
                emitter << YAML::EndSeq;
            } else {
                emitter << "unknown";
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

bool Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue;
    if (fromString(value, configValue)) {
        set(key, configValue, source);
        return true;
    }
    return false;
}

bool Config::getAs(const std::string& key, T& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    try {
        value = std::get<T>(it->value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        file << saveToJSON();
    } else if (ext == ".yaml" || ext == ".yml") {
        file << saveToYAML();
    } else {
        // Backward compatibility with simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toString(entry.value) << "\n";
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        file << saveToJSON();
    } else if (ext == ".yaml" || ext == ".yml") {
        file << saveToYAML();
    } else {
        // Backward compatibility with simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toString(entry.value) << "\n";
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto j = nlohmann::json::parse(jsonStr);
        clear();
        
        for (auto& [key, value] : j.items()) {
            ConfigValue configValue;
            if (fromJSON(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto node = YAML::Load(yamlStr);
        clear();
        
        for (const auto& key : node.keys()) {
            if (node[key]) {
                ConfigValue configValue;
                if (fromYAML(node[key], configValue)) {
                    set(key, configValue, ConfigSource::File);
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::saveToJSON() const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        nlohmann::json value;
        std::visit([&value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = arg;
            }
        }, entry.value);
        
        j[entry.key] = value;
    }
    
    return j.dump(2);
}

std::string Config::saveToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::DoubleQuotedString(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::FlowSeq;
                for (const auto& item : arg) {
                    emitter << YAML::DoubleQuotedString(item);
                }
                emitter << YAML::EndSeq;
            } else {
                emitter << "unknown";
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

bool Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue;
    if (fromString(value, configValue)) {
        set(key, configValue, source);
        return true;
    }
    return false;
}

bool Config::getAs(const std::string& key, T& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    try {
        value = std::get<T>(it->value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

// Helper function to detect file type from content
static bool isJSON(const std::string& content) {
    // Quick check for JSON-like structure
    if (content.empty()) return false;
    
    char first = content[0];
    return (first == '{' || first == '[');
}

static bool isYAML(const std::string& content) {
    // Quick check for YAML-like structure
    if (content.empty()) return false;
    
    std::string trimmed = trim(content);
    return (trimmed.find(':') != std::string::npos || 
            trimmed.find('-') != std::string::npos);
}

// Configuration value serialization/deserialization
std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::fromString(const std::string& str, ConfigValue& value) {
    // Trim whitespace
    std::string trimmed = trim(str);
    
    // Try parsing as boolean
    if (trimmed == "true" || trimmed == "false") {
        value = (trimmed == "true");
        return true;
    }
    
    // Try parsing as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(trimmed, &pos);
        if (pos == trimmed.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try parsing as double
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    value = trimmed;
    return true;
}

std::string Config::toJSON(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j.dump(2);
}

bool Config::fromJSON(const nlohmann::json& j, ConfigValue& value) {
    try {
        if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_number_integer()) {
            // Try to fit into int64_t
            value = j.get<int64_t>();
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_array()) {
            // Try to detect array type
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;
            
            for (const auto& item : j) {
                if (!item.is_number_integer()) allInt = false;
                if (!item.is_number_float()) allDouble = false;
                if (!item.is_string()) allString = false;
            }
            
            if (allInt) {
                std::vector<int> intVec;
                for (const auto& item : j) {
                    intVec.push_back(item.get<int>());
                }
                value = intVec;
            } else if (allDouble) {
                std::vector<double> doubleVec;
                for (const auto& item : j) {
                    doubleVec.push_back(item.get<double>());
                }
                value = doubleVec;
            } else if (allString) {
                std::vector<std::string> stringVec;
                for (const auto& item : j) {
                    stringVec.push_back(item.get<std::string>());
                }
                value = stringVec;
            } else {
                // Mixed array - store as string representation
                value = j.dump();
            }
        } else {
            return false; // Unsupported type
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::toYAML(const ConfigValue& value) {
    YAML::Emitter emitter;
    std::visit([&emitter](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            emitter << YAML::DoubleQuotedString(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::FlowSeq;
            for (const auto& item : arg) {
                emitter << YAML::DoubleQuotedString(item);
            }
            emitter << YAML::EndSeq;
        } else {
            emitter << "unknown";
        }
    }, value);
    return emitter.c_str();
}

bool Config::fromYAML(const YAML::Node& node, ConfigValue& value) {
    try {
        if (node.IsScalar()) {
            std::string str = node.Scalar();
            if (str == "true" || str == "false") {
                value = (str == "true");
            } else {
                // Try to parse as number
                try {
                    if (str.find('.') != std::string::npos) {
                        value = std::stod(str);
                    } else {
                        value = std::stoll(str);
                    }
                } catch (...) {
                    value = str;
                }
            }
        } else if (node.IsSequence()) {
            // Check array type
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;
            
            for (const auto& item : node) {
                if (!item.IsScalar()) {
                    allInt = allDouble = allString = false;
                    break;
                }
                std::string str = item.Scalar();
                if (str == "true" || str == "false") {
                    allInt = allDouble = allString = false;
                    break;
                }
                try {
                    if (str.find('.') == std::string::npos) {
                        std::stoll(str);
                    } else {
                        std::stod(str);
                    }
                } catch (...) {
                    allInt = allDouble = false;
                }
            }
            
            if (allInt) {
                std::vector<int> intVec;
                for (const auto& item : node) {
                    intVec.push_back(item.as<int>());
                }
                value = intVec;
            } else if (allDouble) {
                std::vector<double> doubleVec;
                for (const auto& item : node) {
                    doubleVec.push_back(item.as<double>());
                }
                value = doubleVec;
            } else if (allString) {
                std::vector<std::string> stringVec;
                for (const auto& item : node) {
                    stringVec.push_back(item.as<std::string>());
                }
                value = stringVec;
            } else {
                // Mixed array - store as string
                value = node.as<std::string>();
            }
        } else if (node.IsMap()) {
            // Map as string representation
            std::ostringstream oss;
            oss << node;
            value = oss.str();
        } else {
            return false; // Unsupported type
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
void Config::setValidationEnabled(bool enabled) {
    pImpl->validationEnabled_ = enabled;
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled_;
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    oss << "Configuration validation errors:";
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            oss << "\n  Invalid key format: " << entry.key;
        }
        if (!validateValue(entry.key, entry.value)) {
            oss << "\n  Invalid value for key " << entry.key << ": ";
            oss << toString(entry.value);
        }
    }
    
    return oss.str();
}

bool Config::validate() const {
    // Validate required fields based on file type
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key) || !validateValue(entry.key, entry.value)) {
            NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key);
            return false;
        }
    }
    
    return true;
}

bool Config::validateKey(const std::string& key) const {
    // Check for valid key format
    if (key.empty() || key.length() > 100) {
        return false;
    }
    
    // Keys should only contain alphanumeric characters, underscores, and hyphens
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Validate based on expected type for known keys
    if (key == "random_seed") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        int64_t seed = std::get<int64_t>(value);
        if (seed < 0 || seed > 2147483647) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double timestep = std::get<double>(value);
        if (timestep <= 0.0 || timestep > 1.0) {
            return false;
        }
    } else if (key == "neuron_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        int64_t count = std::get<int64_t>(value);
        if (count < 1 || count > 1000000) {
            return false;
        }
    } else if (key == "region_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        int64_t count = std::get<int64_t>(value);
        if (count < 1 || count > 1000) {
            return false;
        }
    } else if (key == "connection_probability") {
        if (!std::holds_alternative<float>(value) && !std::holds_alternative<double>(value)) {
            return false;
        }
        double prob = std::get<double>(value);
        if (prob < 0.0 || prob > 1.0) {
            return false;
        }
    } else if (key == "log_level") {
        if (!std::holds_alternative<std::string>(value)) {
            return false;
        }
        std::string level = std::get<std::string>(value);
        std::string lowerLevel = toLower(level);
        if (lowerLevel != "debug" && lowerLevel != "info" && 
            lowerLevel != "warning" && lowerLevel != "error" && 
            lowerLevel != "critical") {
            return false;
        }
    }
    
    return true;
}
