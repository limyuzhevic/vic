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

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser using nlohmann/json and yaml-cpp libraries
    // First, determine file type based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    
    // Try to open the file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Handle different file formats
    if (extension == ".json" || extension == ".JSON") {
        return loadFromJSONFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
        return loadFromYAMLFile(filepath);
    } else {
        // Auto-detect format by checking first few lines
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0);
        
        if (firstLine.find('{') != std::string::npos || firstLine.find('"') != std::string::npos) {
            // Looks like JSON
            return loadFromJSONFile(filepath);
        } else if (firstLine.find("---") != std::string::npos || firstLine.find("key:") != std::string::npos) {
            // Looks like YAML
            return loadFromYAMLFile(filepath);
        } else {
            // Fall back to simple key=value format
            return loadFromSimpleFile(filepath);
        }
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Load using nlohmann/json library
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON file
        json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue value;
                const json& val = it.value();
                
                if (val.is_string()) {
                    value = val.get<std::string>();
                } else if (val.is_number_integer()) {
                    // Use int64_t for large integers
                    value = val.get<int64_t>();
                } else if (val.is_number_float()) {
                    value = val.get<double>();
                } else if (val.is_boolean()) {
                    value = val.get<bool>();
                } else if (val.is_array()) {
                    value = convertJSONArray(val);
                }
                
                // Extract description from comment if available (JSON doesn't have native comments)
                std::string description = "";
                
                // Set config entry
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded config from JSON file: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // Load using yaml-cpp library
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        if (doc.IsMap()) {
            for (const auto& it : doc) {
                std::string key = it.first.as<std::string>();
                
                // Convert YAML value to ConfigValue
                ConfigValue value;
                const YAML::Node& val = it.second;
                
                if (val.IsScalar()) {
                    std::string scalar = val.as<std::string>();
                    
                    // Try to detect type
                    if (scalar == "true" || scalar == "false") {
                        value = scalar == "true";
                    } else {
                        // Try to convert to number
                        try {
                            size_t pos;
                            double doubleVal = std::stod(scalar, &pos);
                            if (pos == scalar.length()) {
                                value = doubleVal;
                            } else {
                                // Could be integer
                                try {
                                    int64_t intVal = std::stoll(scalar, &pos);
                                    if (pos == scalar.length()) {
                                        value = intVal;
                                    } else {
                                        value = scalar;  // Keep as string
                                    }
                                } catch (...) {
                                    value = scalar;  // Keep as string
                                }
                            }
                        } catch (...) {
                            value = scalar;  // Keep as string
                        }
                    }
                } else if (val.IsSequence()) {
                    value = convertYAMLSequence(val);
                } else if (val.IsMap()) {
                    // For nested objects, convert to JSON string representation
                    std::stringstream ss;
                    ss << val;
                    value = ss.str();
                }
                
                // Set config entry
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded config from YAML file: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Load simple key=value format (legacy format)
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    try {
        // Determine format from file extension
        std::string extension = "." + std::filesystem::path(filepath).extension().string();
        
        if (extension == ".json" || extension == ".JSON") {
            return saveToJSONFile(filepath);
        } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
            return saveToYAMLFile(filepath);
        } else {
            // Default to JSON format
            return saveToJSONFile(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Create JSON object
        json j;
        
        // Convert entries to JSON
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            // Convert ConfigValue to JSON
            if (std::holds_alternative<int>(entry.value)) {
                j[key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                j[key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                j[key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                j[key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                j[key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                j[key] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                j[key] = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                j[key] = std::get<std::vector<std::string>>(entry.value);
            }
        }
        
        // Write JSON to file with pretty formatting
        file << j.dump(2);
        
        NLM_LOG_INFO("Successfully saved config to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    // Create YAML document
    YAML::Emitter emitter;
    emitter << YAML::BeginDoc;
    
    // Add config entries to YAML
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Convert ConfigValue to YAML
        if (std::holds_alternative<int>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    emitter << YAML::EndDoc;
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    
    NLM_LOG_INFO("Successfully saved config to YAML file: " + filepath);
    return true;
}

ConfigValue Config::convertJSONArray(const json& j) const {
    // Try to determine array type
    if (j.empty()) {
        return std::vector<std::string>{}; // Default empty string array
    }
    
    // Check first element to determine type
    if (j[0].is_number_integer()) {
        std::vector<int64_t> intArray;
        for (const auto& item : j) {
            intArray.push_back(item.get<int64_t>());
        }
        return intArray;
    } else if (j[0].is_number_float()) {
        std::vector<double> doubleArray;
        for (const auto& item : j) {
            doubleArray.push_back(item.get<double>());
        }
        return doubleArray;
    } else if (j[0].is_string()) {
        std::vector<std::string> stringArray;
        for (const auto& item : j) {
            stringArray.push_back(item.get<std::string>());
        }
        return stringArray;
    } else if (j[0].is_boolean()) {
        // Use string array for booleans since we don't have bool vector in ConfigValue
        std::vector<std::string> boolArray;
        for (const auto& item : j) {
            boolArray.push_back(item.get<bool>() ? "true" : "false");
        }
        return boolArray;
    }
    
    // Default to string array
    std::vector<std::string> stringArray;
    for (const auto& item : j) {
        stringArray.push_back(item.get<std::string>());
    }
    return stringArray;
}

ConfigValue Config::convertYAMLSequence(const YAML::Node& node) const {
    // Try to determine sequence type
    if (node.size() == 0) {
        return std::vector<std::string>{}; // Default empty string array
    }
    
    // Check first element to determine type
    YAML::Node first = node[0];
    if (first.IsScalar()) {
        std::string scalar = first.as<std::string>();
        
        // Try to detect type
        if (scalar == "true" || scalar == "false") {
            // For YAML booleans, we'll use string representation
            std::vector<std::string> stringArray;
            for (const auto& item : node) {
                stringArray.push_back(item.as<std::string>());
            }
            return stringArray;
        } else {
            // Try to convert to number
            try {
                size_t pos;
                double doubleVal = std::stod(scalar, &pos);
                if (pos == scalar.length()) {
                    // All elements are likely numeric
                    std::vector<double> doubleArray;
                    for (const auto& item : node) {
                        if (item.IsScalar()) {
                            try {
                                doubleArray.push_back(std::stod(item.as<std::string>()));
                            } catch (...) {
                                // If conversion fails, use string
                                std::vector<std::string> mixedArray;
                                for (const auto& item : node) {
                                    mixedArray.push_back(item.as<std::string>());
                                }
                                return mixedArray;
                            }
                        } else {
                            // Non-scalar element, use string array
                            std::vector<std::string> stringArray;
                            for (const auto& item : node) {
                                stringArray.push_back(item.as<std::string>());
                            }
                            return stringArray;
                        }
                    }
                    return doubleArray;
                }
            } catch (...) {
                // Keep as strings
                std::vector<std::string> stringArray;
                for (const auto& item : node) {
                    stringArray.push_back(item.as<std::string>());
                }
                return stringArray;
            }
        }
    }
    
    // Default to string array
    std::vector<std::string> stringArray;
    for (const auto& item : node) {
        if (item.IsScalar()) {
            stringArray.push_back(item.as<std::string>());
        } else {
            // Convert non-scalar to string representation
            std::stringstream ss;
            ss << item;
            stringArray.push_back(ss.str());
        }
    }
    return stringArray;
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