#ifndef NLM_CORE_CONFIG_CONFIG_CPP
#define NLM_CORE_CONFIG_CONFIG_CPP

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declaration for YAML parsing
namespace yaml {
    template<typename T>
    T get(const YAML::Node& node, const std::string& key, const T& defaultValue);
}

// Load from JSON file
bool Config::loadFromJSON(std::ifstream& file) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        
        auto doc = nlohmann::json::parse(jsonContent);
        
        // Process JSON entries
        if (doc.is_object()) {
            for (auto it = doc.begin(); it != doc.end(); ++it) {
                std::string key = it.key();
                
                // Handle different value types
                if (it->is_string()) {
                    set(key, it->get<std::string>(), ConfigSource::File);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), ConfigSource::File);
                } else if (it->is_number_unsigned()) {
                    set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), ConfigSource::File);
                } else if (it->is_boolean()) {
                    set(key, it->get<bool>(), ConfigSource::File);
                } else if (it->is_array()) {
                    // Try to infer array type
                    const auto& array = *it;
                    if (!array.empty()) {
                        if (std::all_of(array.begin(), array.end(), [](const auto& val) { return val.is_number_integer(); })) {
                            std::vector<int64_t> intArray;
                            intArray.reserve(array.size());
                            for (const auto& val : array) {
                                intArray.push_back(val.get<int64_t>());
                            }
                            set(key, intArray, ConfigSource::File);
                        } else if (std::all_of(array.begin(), array.end(), [](const auto& val) { return val.is_number_float(); })) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(array.size());
                            for (const auto& val : array) {
                                doubleArray.push_back(val.get<double>());
                            }
                            set(key, doubleArray, ConfigSource::File);
                        } else if (std::all_of(array.begin(), array.end(), [](const auto& val) { return val.is_string(); })) {
                            std::vector<std::string> stringArray;
                            stringArray.reserve(array.size());
                            for (const auto& val : array) {
                                stringArray.push_back(val.get<std::string>());
                            }
                            set(key, stringArray, ConfigSource::File);
                        }
                    }
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        // JSON parsing failed, fall back to simple format
    }
    return false;
}

// Load from YAML file
bool Config::loadFromYAML(std::ifstream& file) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string yamlContent = buffer.str();
        
        auto doc = YAML::Load(yamlContent);
        
        // Process YAML entries
        if (doc.IsMap()) {
            for (const auto& item : doc) {
                std::string key = item.first.as<std::string>();
                
                // Handle different value types
                if (item.second.IsScalar()) {
                    if (item.second.IsString()) {
                        set(key, item.second.as<std::string>(), ConfigSource::File);
                    } else if (item.second.IsInteger()) {
                        set(key, item.second.as<int64_t>(), ConfigSource::File);
                    } else if (item.second.IsDouble()) {
                        set(key, item.second.as<double>(), ConfigSource::File);
                    } else if (item.second.IsBoolean()) {
                        set(key, item.second.as<bool>(), ConfigSource::File);
                    }
                } else if (item.second.IsSequence()) {
                    // Try to infer sequence type
                    const auto& sequence = item.second;
                    if (sequence.size() > 0) {
                        if (std::all_of(sequence.begin(), sequence.end(), [](const auto& val) { return val.IsInteger(); })) {
                            std::vector<int64_t> intArray;
                            intArray.reserve(sequence.size());
                            for (const auto& val : sequence) {
                                intArray.push_back(val.as<int64_t>());
                            }
                            set(key, intArray, ConfigSource::File);
                        } else if (std::all_of(sequence.begin(), sequence.end(), [](const auto& val) { return val.IsDouble(); })) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(sequence.size());
                            for (const auto& val : sequence) {
                                doubleArray.push_back(val.as<double>());
                            }
                            set(key, doubleArray, ConfigSource::File);
                        } else if (std::all_of(sequence.begin(), sequence.end(), [](const auto& val) { return val.IsScalar() && val.as<std::string>().find('.') == std::string::npos && !std::isdigit(val.as<std::string>()[0]); })) {
                            std::vector<std::string> stringArray;
                            stringArray.reserve(sequence.size());
                            for (const auto& val : sequence) {
                                stringArray.push_back(val.as<std::string>());
                            }
                            set(key, stringArray, ConfigSource::File);
                        }
                    }
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        // YAML parsing failed, fall back to simple format
    }
    return false;
}

// Convert to JSON string
std::string Config::toJSON() const {
    nlohmann::json doc;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&doc, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                doc[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                doc[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return doc.dump(2); // Pretty print with 2-space indentation
}

// Convert to YAML string
std::string Config::toYAML() const {
    YAML::Emitter doc;
    doc << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        doc << YAML::Key << entry.key;
        std::visit([&doc](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                doc << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                doc << YAML::Value << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                doc << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                doc << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                doc << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                doc << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                doc << YAML::Value << arg;
            }
        }, entry.value);
    }
    
    doc << YAML::EndMap;
    return std::string(doc.c_str());
}

// Detect file format
Config::FileFormat Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return FileFormat::Unknown;
    }
    
    std::string line;
    bool hasKeyValue = false;
    bool hasJsonBraces = false;
    bool hasYamlKeyColon = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            hasKeyValue = true;
        }
        
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            hasJsonBraces = true;
        }
        
        if (line.find(":") != std::string::npos && 
            (line.find("  ") != std::string::npos || 
             (line.find(' ') == std::string::npos && line.find('\t') != std::string::npos))) {
            hasYamlKeyColon = true;
        }
    }
    
    if (hasJsonBraces && !hasKeyValue) {
        return FileFormat::JSON;
    } else if (hasYamlKeyColon) {
        return FileFormat::YAML;
    } else if (hasKeyValue) {
        return FileFormat::KeyValue;
    }
    
    return FileFormat::Unknown;
}

// Detect content format
Config::FileFormat Config::detectContentFormat(const std::string& content) {
    bool hasKeyValue = false;
    bool hasJsonBraces = false;
    bool hasYamlKeyColon = false;
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            hasKeyValue = true;
        }
        
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            hasJsonBraces = true;
        }
        
        if (line.find(":") != std::string::npos && 
            (line.find("  ") != std::string::npos || 
             (line.find(' ') == std::string::npos && line.find('\t') != std::string::npos))) {
            hasYamlKeyColon = true;
        }
    }
    
    if (hasJsonBraces && !hasKeyValue) {
        return FileFormat::JSON;
    } else if (hasYamlKeyColon) {
        return FileFormat::YAML;
    } else if (hasKeyValue) {
        return FileFormat::KeyValue;
    }
    
    return FileFormat::Unknown;
}

// Override loadFromFile to use enhanced parsers
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format
    FileFormat format = detectFileFormat(filepath);
    
    // Try JSON parser first
    if (format == FileFormat::JSON || format == FileFormat::Unknown) {
        file.clear();
        file.seekg(0);
        if (loadFromJSON(file)) {
            return true;
        }
    }
    
    // Try YAML parser
    if (format == FileFormat::YAML || format == FileFormat::Unknown) {
        file.clear();
        file.seekg(0);
        if (loadFromYAML(file)) {
            return true;
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    file.clear();
    file.seekg(0);
    
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

} // namespace nlm

#endif // NLM_CORE_CONFIG_CONFIG_CPP