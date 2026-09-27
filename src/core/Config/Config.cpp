#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Forward declarations for helper functions
void Config::loadFromJSON(const nlohmann::json& json);
void Config::loadFromYAML(const YAML::Node& yaml);
void Config::saveToJSON(const std::string& filepath) const;
void Config::saveToYAML(const std::string& filepath) const;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 2: Use proper JSON/YAML parser
    try {
        // Determine file type based on extension
        std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Clear existing entries
        clear();
        
        // Parse based on file type
        if (ext == "json" || nlm::ConfigParser::isJSONFile(filepath)) {
            // Parse JSON file
            return nlm::ConfigParser::parseJSONFromFile(filepath, jsonConfig);
        } else if (ext == "yml" || ext == "yaml" || nlm::ConfigParser::isYAMLFile(filepath)) {
            // Parse YAML file
            return nlm::ConfigParser::parseYAMLFromFile(filepath, yamlConfig);
        } else {
            // Fallback to simple key=value format for backward compatibility
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
        
    } catch (const std::exception& e) {
        // Log error (would use Logger in full implementation)
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

void Config::loadFromJSON(const nlohmann::json& json) {
    // Parse JSON and convert to Config entries
    // Implementation would go here
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        
        // Convert JSON value to ConfigValue
        if (it->is_number_integer()) {
            value = it->get<int>();
        } else if (it->is_number_float()) {
            value = it->get<double>();
        } else if (it->is_boolean()) {
            value = it->get<bool>();
        } else if (it->is_string()) {
            value = it->get<std::string>();
        } else if (it->is_array()) {
            // Handle arrays
            if (!it->empty()) {
                if (it->front().is_number_integer()) {
                    std::vector<int> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<int>());
                    }
                    value = vec;
                } else if (it->front().is_number_float()) {
                    std::vector<double> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                } else if (it->front().is_string()) {
                    std::vector<std::string> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<std::string>());
                    }
                    value = vec;
                }
            }
        } else if (it->is_object()) {
            // Handle nested objects
            std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
            for (auto obj_it = it->begin(); obj_it != it->end(); ++obj_it) {
                nestedMap[obj_it.key()] = jsonValueFromJSON(*obj_it);
            }
            value = nestedMap;
        }
        
        set(key, value, ConfigSource::File);
    }
}

void Config::loadFromYAML(const YAML::Node& yaml) {
    // Parse YAML and convert to Config entries
    // Implementation would go here
    if (yaml.IsMap()) {
        for (auto it = yaml.begin(); it != yaml.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value;
            
            // Convert YAML node to ConfigValue
            if (it->IsScalar()) {
                if (it->Tag() == "!!int" || (it->IsScalar() && it->as<std::string>().find('.') == std::string::npos)) {
                    value = it->as<int>();
                } else if (it->Tag() == "!!float" || it->Tag() == "!!double" || (it->IsScalar() && it->as<std::string>().find('.') != std::string::npos)) {
                    value = it->as<double>();
                } else if (it->Tag() == "!!bool" || it->as<std::string>() == "true" || it->as<std::string>() == "false") {
                    value = it->as<bool>();
                } else {
                    value = it->as<std::string>();
                }
            } else if (it->IsSequence()) {
                // Handle sequences
                if (!it->empty()) {
                    if (it->front().IsScalar()) {
                        if (it->front().Tag() == "!!int" || (it->front().IsScalar() && it->front().as<std::string>().find('.') == std::string::npos)) {
                            std::vector<int> vec;
                            vec.reserve(it->size());
                            for (const auto& item : *it) {
                                vec.push_back(item.as<int>());
                            }
                            value = vec;
                        } else if (it->front().Tag() == "!!float" || it->front().Tag() == "!!double" || (it->front().IsScalar() && it->front().as<std::string>().find('.') != std::string::npos)) {
                            std::vector<double> vec;
                            vec.reserve(it->size());
                            for (const auto& item : *it) {
                                vec.push_back(item.as<double>());
                            }
                            value = vec;
                        } else {
                            std::vector<std::string> vec;
                            vec.reserve(it->size());
                            for (const auto& item : *it) {
                                vec.push_back(item.as<std::string>());
                            }
                            value = vec;
                        }
                    }
                }
            } else if (it->IsMap()) {
                // Handle nested maps
                std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
                for (auto map_it = it->begin(); map_it != it->end(); ++map_it) {
                    nestedMap[map_it->first.as<std::string>()] = yamlValueToVariant(*map_it);
                }
                value = nestedMap;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
}

void Config::saveToJSON(const std::string& filepath) const {
    // Save configuration to JSON file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = variantToJSON(entry.value);
    }
    
    file << json.dump(2); // Pretty print with 2-space indentation
}

void Config::saveToYAML(const std::string& filepath) const {
    // Save configuration to YAML file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << variantToYAML(entry.value);
    }
    
    emitter << YAML::EndMap;
    
    file << emitter.c_str();
}

// Helper functions for JSON/YAML conversion
std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> Config::jsonValueFromJSON(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (json[0].is_number_float()) {
                std::vector<double> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int>{};
    } else if (json.is_object()) {
        std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
        for (auto it = json.begin(); it != json.end(); ++it) {
            nestedMap[it.key()] = jsonValueFromJSON(*it);
        }
        return nestedMap;
    }
    
    throw ConfigParser::ParseError("Unsupported JSON value type");
}

std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> Config::yamlValueToVariant(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.Tag() == "!!int" || (node.IsScalar() && node.as<std::string>().find('.') == std::string::npos)) {
            return node.as<int>();
        } else if (node.Tag() == "!!float" || node.Tag() == "!!double" || (node.IsScalar() && node.as<std::string>().find('.') != std::string::npos)) {
            return node.as<double>();
        } else if (node.Tag() == "!!bool" || node.as<std::string>() == "true" || node.as<std::string>() == "false") {
            return node.as<bool>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (node.empty()) {
            return std::vector<int>{};
        }
        
        if (node[0].IsScalar()) {
            if (node[0].Tag() == "!!int" || (node[0].IsScalar() && node[0].as<std::string>().find('.') == std::string::npos)) {
                std::vector<int> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<int>());
                }
                return vec;
            } else if (node[0].Tag() == "!!float" || node[0].Tag() == "!!double" || (node[0].IsScalar() && node[0].as<std::string>().find('.') != std::string::npos)) {
                std::vector<double> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<double>());
                }
                return vec;
            } else {
                std::vector<std::string> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.as<std::string>());
                }
                return vec;
            }
        }
    } else if (node.IsMap()) {
        std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedMap;
        for (auto it = node.begin(); it != node.end(); ++it) {
            nestedMap[it->first.as<std::string>()] = yamlValueToVariant(*it);
        }
        return nestedMap;
    }
    
    throw ConfigParser::ParseError("Unsupported YAML value type");
}

nlohmann::json Config::variantToJSON(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<int>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<double>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        nlohmann::json jsonArray = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            jsonArray.push_back(item);
        }
        return jsonArray;
    } else if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (value)) {
        nlohmann::json jsonObject;
        for (const auto& pair : std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>(value)) {
            jsonObject[pair.first] = variantToJSON(pair.second);
        }
        return jsonObject;
    }
    
    throw ConfigParser::ParseError("Unsupported variant type");
}

YAML::Node Config::variantToYAML(const ConfigValue& value) {
    YAML::Node node;
    if (std::holds_alternative<int>(value)) {
        node = std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        node = std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        node = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        node = std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        node = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        YAML::Node seqNode = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<int>>(value)) {
            seqNode.push_back(item);
        }
        node = seqNode;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        YAML::Node seqNode = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<double>>(value)) {
            seqNode.push_back(item);
        }
        node = seqNode;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        YAML::Node seqNode = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            seqNode.push_back(item);
        }
        node = seqNode;
    } else if (std::holds_alternative<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> (value)) {
        YAML::Node mapNode = YAML::Node(YAML::NodeType::Map);
        for (const auto& pair : std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>(value)) {
            mapNode[pair.first] = variantToYAML(pair.second);
        }
        node = mapNode;
    }
    
    throw ConfigParser::ParseError("Unsupported variant type");
}

// Update saveToFile to use JSON/YAML format based on file extension
bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        saveToJSON(filepath);
        return true;
    } else if (ext == "yml" || ext == "yaml") {
        saveToYAML(filepath);
        return true;
    } else {
        // Fallback to simple key=value format for backward compatibility
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toString(entry.value) << "\n";
        }
        
        return true;
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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
