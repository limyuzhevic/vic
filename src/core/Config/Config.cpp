#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool jsonSupported = true;  // Check if nlohmann::json is available
    bool yamlSupported = true;  // Check if yaml-cpp is available
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Check if parsing libraries are available
    pImpl->jsonSupported = checkJsonSupport();
    pImpl->yamlSupported = checkYamlSupport();
}

bool Config::checkJsonSupport() {
    try {
        // Test if nlohmann::json is available
        json::parse("{}");
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::checkYamlSupport() {
    try {
        // Test if yaml-cpp is available
        YAML::Load("{}");
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try different formats in order of preference
    if (filepath.substr(filepath.find_last_of(".") + 1) == "json") {
        return loadFromJson(filepath);
    } else if (filepath.substr(filepath.find_last_of(".") + 1) == "yaml" ||
               filepath.substr(filepath.find_last_of(".") + 1) == "yml") {
        return loadFromYaml(filepath);
    } else {
        // Try to auto-detect format
        return loadFromAutoDetected(filepath);
    }
}

bool Config::loadFromJson(const std::string& filepath) {
    if (!pImpl->jsonSupported) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j = json::parse(file);
        pImpl->entries.clear();
        
        // Parse JSON recursively
        parseJsonObject(j, pImpl->entries, ConfigSource::File);
        
        return true;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    if (!pImpl->yamlSupported) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::Load(file);
        pImpl->entries.clear();
        
        // Parse YAML recursively
        parseYamlNode(doc, pImpl->entries, ConfigSource::File);
        
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parse error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromAutoDetected(const std::string& filepath) {
    // Try JSON first, then YAML, then fall back to simple format
    if (loadFromJson(filepath)) {
        return true;
    }
    if (loadFromYaml(filepath)) {
        return true;
    }
    // Fall back to simple key=value format
    return loadFromSimple(filepath);
}

void Config::parseJsonObject(const json& j, std::vector<ConfigEntry>& entries, ConfigSource source) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        
        // Handle different value types
        if (it->is_number_integer()) {
            entries.emplace_back(key, ConfigValue(it->get<int64_t>()), source, "Integer parameter");
        } else if (it->is_number_unsigned()) {
            entries.emplace_back(key, ConfigValue(static_cast<int64_t>(it->get<uint64_t>())), source, "Unsigned integer parameter");
        } else if (it->is_number_float()) {
            entries.emplace_back(key, ConfigValue(it->get<double>()), source, "Floating point parameter");
        } else if (it->is_boolean()) {
            entries.emplace_back(key, ConfigValue(it->get<bool>()), source, "Boolean parameter");
        } else if (it->is_string()) {
            // Check if it looks like a vector (comma-separated)
            std::string value = it->get<std::string>();
            if (value.find(',') != std::string::npos) {
                // Parse as vector
                std::vector<std::string> vec;
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    vec.push_back(trim(item));
                }
                entries.emplace_back(key, ConfigValue(vec), source, "Vector of strings");
            } else {
                entries.emplace_back(key, ConfigValue(value), source, "String parameter");
            }
        } else if (it->is_array()) {
            // Handle arrays of numbers
            json arr = *it;
            if (!arr.empty() && arr[0].is_number()) {
                std::vector<double> vec;
                for (const auto& item : arr) {
                    vec.push_back(item.get<double>());
                }
                entries.emplace_back(key, ConfigValue(vec), source, "Vector of doubles");
            } else if (!arr.empty() && arr[0].is_number_integer()) {
                std::vector<int64_t> vec;
                for (const auto& item : arr) {
                    vec.push_back(item.get<int64_t>());
                }
                entries.emplace_back(key, ConfigValue(vec), source, "Vector of integers");
            } else {
                // Default to string representation
                std::string vecStr = it->dump();
                entries.emplace_back(key, ConfigValue(vecStr), source, "Vector of strings (JSON)");
            }
        } else if (it->is_object()) {
            // Nested object - serialize to JSON string
            entries.emplace_back(key, ConfigValue(it->dump()), source, "JSON object");
        } else if (it->is_null()) {
            entries.emplace_back(key, ConfigValue("null"), source, "Null value");
        }
    }
}

void Config::parseYamlNode(const YAML::Node& node, std::vector<ConfigEntry>& entries, ConfigSource source) {
    if (node.IsMap()) {
        for (const auto& pair : node) {
            std::string key = pair.first.as<std::string>();
            
            // Handle different value types
            if (pair.second.IsScalar()) {
                // Check if scalar can be parsed as different types
                std::string valueStr = pair.second.as<std::string>();
                
                // Try to parse as boolean
                if (valueStr == "true" || valueStr == "false") {
                    entries.emplace_back(key, ConfigValue(valueStr == "true"), source, "Boolean parameter");
                    continue;
                }
                
                // Try to parse as integer
                try {
                    int64_t intVal = std::stoll(valueStr);
                    entries.emplace_back(key, ConfigValue(intVal), source, "Integer parameter");
                    continue;
                } catch (...) {}
                
                // Try to parse as float
                try {
                    double floatVal = std::stod(valueStr);
                    entries.emplace_back(key, ConfigValue(floatVal), source, "Floating point parameter");
                    continue;
                } catch (...) {}
                
                // Check if it looks like a comma-separated vector
                if (valueStr.find(',') != std::string::npos) {
                    std::vector<std::string> vec;
                    std::stringstream ss(valueStr);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        vec.push_back(trim(item));
                    }
                    entries.emplace_back(key, ConfigValue(vec), source, "Vector of strings");
                    continue;
                }
                
                // Default to string
                entries.emplace_back(key, ConfigValue(valueStr), source, "String parameter");
                
            } else if (pair.second.IsSequence()) {
                // Handle arrays
                YAML::Node seq = pair.second;
                if (!seq.empty() && seq[0].IsScalar()) {
                    std::vector<std::string> vec;
                    for (const auto& item : seq) {
                        vec.push_back(item.as<std::string>());
                    }
                    entries.emplace_back(key, ConfigValue(vec), source, "Vector of strings");
                } else {
                    // For complex sequences, serialize to YAML string
                    entries.emplace_back(key, ConfigValue(pair.second.as<std::string>()), source, "YAML sequence");
                }
            } else if (pair.second.IsMap()) {
                // Nested object - serialize to YAML string
                entries.emplace_back(key, ConfigValue(pair.second.as<std::string>()), source, "YAML object");
            }
        }
    }
}

bool Config::loadFromSimple(const std::string& filepath) {
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
    // Determine file format from extension
    std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        if (extension == "json") {
            saveToJson(file);
        } else if (extension == "yaml" || extension == "yml") {
            saveToYaml(file);
        } else {
            // Default to simple format for backward compatibility
            saveToSimple(file);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

void Config::saveToJson(std::ostream& out) const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                // Convert to JSON array of numbers
                json arr = arg;
                j[entry.key] = arr;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Convert to JSON array of numbers
                json arr = arg;
                j[entry.key] = arr;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Convert to JSON array of strings
                json arr = arg;
                j[entry.key] = arr;
            }
        }, entry.value);
    }
    
    out << std::setw(2) << j << std::endl;
}

void Config::saveToYaml(std::ostream& out) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                emitter << YAML::BeginSeq;
                for (auto val : arg) {
                    emitter << static_cast<int>(val);
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::BeginSeq;
                for (auto val : arg) {
                    emitter << val;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::BeginSeq;
                for (auto& val : arg) {
                    emitter << val;
                }
                emitter << YAML::EndSeq;
            } else {
                emitter << "UNKNOWN_TYPE";
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    out << emitter.c_str();
}

void Config::saveToSimple(std::ostream& out) const {
    for (const auto& entry : pImpl->entries) {
        out << "# " << entry.description << "\n";
        out << entry.key << " = ";
        
        std::visit([&out](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Special handling for vector values
                out << "\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) out << ",";
                    out << arg[i];
                }
                out << "\"";
            } else {
                out << arg;
            }
        }, entry.value);
        
        out << "\n\n";
    }
}

// Other methods remain the same but use the improved structure
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
    oss << "  JSON Support: " << (pImpl->jsonSupported ? "Yes" : "No") << "\n";
    oss << "  YAML Support: " << (pImpl->yamlSupported ? "Yes" : "No") << "\n";
    oss << "  Available formats: ";
    
    if (pImpl->jsonSupported && pImpl->yamlSupported) {
        oss << "JSON, YAML, SIMPLE\n";
    } else if (pImpl->jsonSupported) {
        oss << "JSON, SIMPLE\n";
    } else if (pImpl->yamlSupported) {
        oss << "YAML, SIMPLE\n";
    } else {
        oss << "SIMPLE\n";
    }
    
    oss << "  Entries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "    " << entry.key << " = [";
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

// Explicit template instantiations remain the same
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