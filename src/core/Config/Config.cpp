#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to convert string to lowercase for format detection
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to detect file format based on extension and content
Config::FileFormat Config::detectFileFormat(const std::string& filepath) const {
    std::string lower = toLower(filepath);
    
    // Check file extension
    if (lower.find(".json") != std::string::npos) {
        return FileFormat::JSON;
    } else if (lower.find(".yaml") != std::string::npos || 
               lower.find(".yml") != std::string::npos) {
        return FileFormat::YAML;
    } else if (lower.find(".cfg") != std::string::npos) {
        return FileFormat::CFG;
    }
    
    // Check file content for JSON/YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return FileFormat::UNKNOWN;
    }
    
    // Read first 1024 bytes to analyze content
    std::string firstChunk(1024, '\0');
    file.read(&firstChunk[0], 1024);
    file.close();
    
    // Trim whitespace
    size_t start = firstChunk.find_first_not_of(" \t\r\n");
    size_t end = firstChunk.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        return FileFormat::UNKNOWN;
    }
    
    std::string content = firstChunk.substr(start, end - start + 1);
    
    // Check for JSON format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Additional check for JSON object structure
        if (content.find('"') != std::string::npos) {
            return FileFormat::JSON;
        }
    }
    
    // Check for YAML format
    if (content.find(":") != std::string::npos && 
        (content.find("- ") != std::string::npos || content.find("* ") != std::string::npos)) {
        return FileFormat::YAML;
    }
    
    return FileFormat::CFG; // Default to cfg format
}

bool Config::loadFromFile(const std::string& filepath) {
    FileFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case FileFormat::JSON:
            return loadFromJSON(filepath);
        case FileFormat::YAML:
            return loadFromYAML(filepath);
        case FileFormat::CFG:
            return loadFromFileFormat(filepath); // Original cfg parser
        default:
            NLM_LOG_ERROR("Unsupported file format for: " + filepath);
            return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file: " + filepath);
            return false;
        }
        
        json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Convert JSON to config entries
        convertJSONToConfig(jsonData, "");
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in file " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Loader loader;
        YAML::Node yamlData = loader.LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        // Convert YAML to config entries
        convertYAMLToConfig(yamlData, "");
        
        NLM_LOG_INFO("Successfully loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error in file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromFileFormat(const std::string& filepath) {
    // Original implementation for backward compatibility
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
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!setFromString(key, value)) {
                    NLM_LOG_ERROR("Invalid command line value for key: " + key);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument format (missing '='): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!setFromString(key, value)) {
                NLM_LOG_ERROR("Invalid command line value for key: " + key);
                success = false;
            }
        }
        // Handle boolean flags (e.g., --enable_feature)
        else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            
            // For boolean flags, default to true
            set(key, true, ConfigSource::CommandLine);
            NLM_LOG_INFO(std::string("Command line flag enabled: ") + key);
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from command line arguments");
    }
    
    return success;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr) {
    // Try to parse value as different types in order of preference
    try {
        // Try boolean first (case-insensitive)
        if (valueStr == "true" || valueStr == "yes" || valueStr == "1" || 
            valueStr == "on" || valueStr == "enable") {
            set(key, true, ConfigSource::CommandLine);
            return true;
        } else if (valueStr == "false" || valueStr == "no" || valueStr == "0" || 
                   valueStr == "off" || valueStr == "disable") {
            set(key, false, ConfigSource::CommandLine);
            return true;
        }
        
        // Try integer
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intValue, ConfigSource::CommandLine);
            return true;
        }
        
        // Try floating point
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleValue, ConfigSource::CommandLine);
            return true;
        }
        
        // Fall back to string
        set(key, valueStr, ConfigSource::CommandLine);
        return true;
        
    } catch (const std::exception&) {
        // Failed to parse, treat as string
        set(key, valueStr, ConfigSource::CommandLine);
        return true;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    FileFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case FileFormat::JSON:
            return saveToJSON(filepath);
        case FileFormat::YAML:
            return saveToYAML(filepath);
        case FileFormat::CFG:
            return saveToFileFormat(filepath);
        default:
            NLM_LOG_ERROR("Unsupported file format for saving: " + filepath);
            return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json jsonData;
        
        // Convert config entries to JSON
        convertConfigToJSON(jsonData, "");
        
        // Write formatted JSON with indentation
        file << jsonData.dump(2);
        
        NLM_LOG_INFO("Successfully saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        
        emitter << YAML::BeginMap;
        
        // Convert config entries to YAML
        convertConfigToYAML(emitter, "");
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        
        NLM_LOG_INFO("Successfully saved YAML configuration to: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML error saving file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToFileFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

void Config::convertJSONToConfig(const json& jsonData, const std::string& prefix) {
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = prefix + std::string(it.key());
        
        if (it->is_object()) {
            // Nested object - recurse
            convertJSONToConfig(*it, key + ".");
        } else if (it->is_array()) {
            // Array - convert based on type
            std::vector<double> doubleArray;
            std::vector<int> intArray;
            std::vector<std::string> stringArray;
            
            for (const auto& item : *it) {
                if (item.is_number_integer()) {
                    intArray.push_back(item.get<int>());
                } else if (item.is_number_float()) {
                    doubleArray.push_back(item.get<double>());
                } else {
                    stringArray.push_back(item.get<std::string>());
                }
            }
            
            // Use appropriate vector type based on first element
            if (!doubleArray.empty()) {
                set(key, doubleArray, ConfigSource::File);
            } else if (!intArray.empty()) {
                set(key, intArray, ConfigSource::File);
            } else if (!stringArray.empty()) {
                set(key, stringArray, ConfigSource::File);
            } else {
                // Empty array
                std::vector<double> empty;
                set(key, empty, ConfigSource::File);
            }
        } else if (it->is_number_integer()) {
            set(key, it->get<int>(), ConfigSource::File);
        } else if (it->is_number_float()) {
            set(key, it->get<double>(), ConfigSource::File);
        } else if (it->is_boolean()) {
            set(key, it->get<bool>(), ConfigSource::File);
        } else if (it->is_string()) {
            set(key, it->get<std::string>(), ConfigSource::File);
        }
    }
}

void Config::convertYAMLToConfig(const YAML::Node& yamlData, const std::string& prefix) {
    for (const auto& it : yamlData) {
        std::string key = prefix + std::string(it.first);
        
        if (it.second.IsMap()) {
            // Nested map - recurse
            convertYAMLToConfig(it.second, key + ".");
        } else if (it.second.IsSequence()) {
            // Sequence - convert based on type
            std::vector<double> doubleArray;
            std::vector<int> intArray;
            std::vector<std::string> stringArray;
            
            for (const auto& item : it.second) {
                if (item.IsScalar()) {
                    std::string scalar = item.Scalar();
                    try {
                        doubleArray.push_back(std::stod(scalar));
                    } catch (...) {
                        try {
                            intArray.push_back(std::stoi(scalar));
                        } catch (...) {
                            stringArray.push_back(scalar);
                        }
                    }
                }
            }
            
            // Use appropriate vector type based on first element
            if (!doubleArray.empty()) {
                set(key, doubleArray, ConfigSource::File);
            } else if (!intArray.empty()) {
                set(key, intArray, ConfigSource::File);
            } else if (!stringArray.empty()) {
                set(key, stringArray, ConfigSource::File);
            } else {
                // Empty sequence
                std::vector<double> empty;
                set(key, empty, ConfigSource::File);
            }
        } else if (it.second.IsScalar()) {
            std::string scalar = it.second.Scalar();
            
            // Try to convert to numeric type
            try {
                size_t pos;
                int intValue = std::stoi(scalar, &pos);
                if (pos == scalar.size()) {
                    set(key, intValue, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                double doubleValue = std::stod(scalar, &pos);
                if (pos == scalar.size()) {
                    set(key, doubleValue, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Try to convert to boolean
            if (scalar == "true" || scalar == "yes" || scalar == "1") {
                set(key, true, ConfigSource::File);
            } else if (scalar == "false" || scalar == "no" || scalar == "0") {
                set(key, false, ConfigSource::File);
            } else {
                // String value
                set(key, scalar, ConfigSource::File);
            }
        }
    }
}

void Config::convertConfigToJSON(json& jsonData, const std::string& prefix) const {
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        if (!prefix.empty()) {
            key = prefix + key;
        }
        
        jsonData[key] = getConfigValue(entry.value);
        
        // Add description as a comment if needed
        if (!entry.description.empty()) {
            // JSON doesn't have a direct comment field, so we'll use a special key
            jsonData[key + "_description"] = entry.description;
        }
    }
}

void Config::convertConfigToYAML(YAML::Emitter& emitter, const std::string& prefix) const {
    // Convert config entries to YAML format
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        if (!prefix.empty()) {
            key = prefix + key;
        }
        
        emitter << YAML::Key << key;
        emitter << YAML::Value << getConfigValue(entry.value);
        
        // Add description as a comment if needed
        if (!entry.description.empty()) {
            emitter << "# " << entry.description;
        }
    }
    
    emitter << YAML::EndMap;
}

ConfigValue Config::getConfigValue(const ConfigValue& value) {
    // Convert ConfigValue to a format suitable for JSON/YAML
    // For complex types like vectors, we keep them as is
    return value;
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

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

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

} // namespace nlm