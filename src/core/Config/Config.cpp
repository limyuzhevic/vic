#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;
namespace YAML = YAML;

namespace nlm {

class Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string currentFormat;  // "text", "json", or "yaml"
public:
    Impl() : currentFormat("text") {}
    
    const std::vector<ConfigEntry>& getEntries() const { return entries; }
    void setEntries(const std::vector<ConfigEntry>& entries) { this->entries = entries; }
    void setFormat(const std::string& format) { currentFormat = format; }
    const std::string& getFormat() const { return currentFormat; }
};

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Detect file format based on extension and content
std::string detectFileFormat(const std::string& filepath) {
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase for comparison
    ext = toLower(ext);
    
    if (ext == "json") return "json";
    if (ext == "yaml" || ext == "yml") return "yaml";
    if (ext == "cfg" || ext == "conf" || ext == "ini" || ext == "txt") return "text";
    
    // Try to detect from content
    std::ifstream file(filepath);
    if (!file.is_open()) return "text";  // Default to text if file can't be opened
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        std::string firstLineLower = toLower(firstLine);
        if (firstLineLower.find("{") != std::string::npos && firstLineLower.find("}") != std::string::npos) {
            return "json";
        }
        if (firstLineLower.find("---") != std::string::npos || 
            (firstLineLower.find("-") != std::string::npos && 
             (firstLineLower.find("key:") != std::string::npos || firstLineLower.find("value:") != std::string::npos))) {
            return "yaml";
        }
    }
    
    return "text";
}

// Load configuration from JSON file
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        pImpl->entries.clear();
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            const auto& jsonValue = it.value();
            
            if (jsonValue.is_string()) {
                value = jsonValue.get<std::string>();
            } else if (jsonValue.is_number_integer()) {
                value = jsonValue.get<int64_t>();
            } else if (jsonValue.is_number_float()) {
                value = jsonValue.get<double>();
            } else if (jsonValue.is_boolean()) {
                value = jsonValue.get<bool>();
            } else if (jsonValue.is_array()) {
                // Handle arrays
                std::vector<double> doubleArray;
                for (const auto& elem : jsonValue) {
                    if (elem.is_number_integer()) {
                        doubleArray.push_back(elem.get<int64_t>());
                    } else if (elem.is_number_float()) {
                        doubleArray.push_back(elem.get<double>());
                    }
                }
                value = doubleArray;
            }
            
            // Try to get description if present
            std::string description = "";
            if (jsonValue.contains("_description")) {
                description = jsonValue["_description"].get<std::string>();
            }
            
            set(key, value, ConfigSource::File);
            
            if (!description.empty()) {
                // Update description
                auto& entry = pImpl->entries.back();
                entry.description = description;
            }
        }
        
        pImpl->setFormat("json");
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML file
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open YAML configuration file: " + filepath);
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        pImpl->entries.clear();
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            // Convert YAML value to ConfigValue
            ConfigValue value;
            const YAML::Node& yamlValue = pair.second;
            
            if (yamlValue.IsScalar()) {
                // Check type
                if (yamlValue.IsBoolean()) {
                    value = yamlValue.as<bool>();
                } else if (yamlValue.IsInteger()) {
                    value = yamlValue.as<int64_t>();
                } else if (yamlValue.IsFloat()) {
                    value = yamlValue.as<double>();
                } else {
                    value = yamlValue.as<std::string>();
                }
            } else if (yamlValue.IsSequence()) {
                // Handle sequences (arrays)
                std::vector<double> doubleArray;
                for (const auto& elem : yamlValue) {
                    if (elem.IsBoolean()) {
                        doubleArray.push_back(elem.as<bool>() ? 1.0 : 0.0);
                    } else if (elem.IsInteger()) {
                        doubleArray.push_back(elem.as<int64_t>());
                    } else if (elem.IsFloat()) {
                        doubleArray.push_back(elem.as<double>());
                    } else if (elem.IsScalar()) {
                        try {
                            doubleArray.push_back(std::stod(elem.as<std::string>()));
                        } catch (...) {
                            doubleArray.push_back(0.0);
                        }
                    }
                }
                value = doubleArray;
            } else if (yamlValue.IsMap()) {
                // For nested maps, convert to JSON string representation
                json j = yamlValue;
                value = j.dump();
            }
            
            // Try to get description if present
            std::string description = "";
            if (yamlValue["_description"]) {
                description = yamlValue["_description"].as<std::string>();
            }
            
            set(key, value, ConfigSource::File);
            
            if (!description.empty()) {
                // Update description
                auto& entry = pImpl->entries.back();
                entry.description = description;
            }
        }
        
        pImpl->setFormat("yaml");
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Override existing loadFromFile to support multiple formats
bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    if (format == "json") {
        return loadFromJSON(filepath);
    } else if (format == "yaml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to original text-based parser
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
            return false;
        }
        
        pImpl->entries.clear();
        pImpl->setFormat("text");
        
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
        
        NLM_LOG_INFO("Loaded text-based configuration from: " + filepath);
        return true;
    }
}

// Save configuration to file in current format
bool Config::saveToFile(const std::string& filepath) const {
    std::string format = pImpl->getFormat();
    
    if (format == "json") {
        return saveToJSON(filepath);
    } else if (format == "yaml") {
        return saveToYAML(filepath);
    } else {
        return saveToText(filepath);
    }
}

// Save configuration to JSON file
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON configuration file for writing: " + filepath);
        return false;
    }
    
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Add to JSON object
        if (std::holds_alternative<std::string>(entry.value)) {
            j[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            j[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            j[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            j[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            json array = json::array();
            for (int val : std::get<std::vector<int>>(entry.value)) {
                array.push_back(val);
            }
            j[entry.key] = array;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            json array = json::array();
            for (double val : std::get<std::vector<double>>(entry.value)) {
                array.push_back(val);
            }
            j[entry.key] = array;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            json array = json::array();
            for (const std::string& val : std::get<std::vector<std::string>>(entry.value)) {
                array.push_back(val);
            }
            j[entry.key] = array;
        }
    }
    
    try {
        file << j.dump(4);  // Pretty print with 4-space indentation
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON configuration: " + std::string(e.what()));
        return false;
    }
}

// Save configuration to YAML file
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open YAML configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        YAML::Emitter emitter;
        emitter.SetIndent(2);
        emitter.SetMapFormat(YAML::EMitterStyle::Block);
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key << YAML::Value;
            
            if (std::holds_alternative<std::string>(entry.value)) {
                emitter << std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                emitter << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                emitter << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                emitter << std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                emitter << YAML::BeginSeq;
                for (int val : std::get<std::vector<int>>(entry.value)) {
                    emitter << val;
                }
                emitter << YAML::EndSeq;
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                emitter << YAML::BeginSeq;
                for (double val : std::get<std::vector<double>>(entry.value)) {
                    emitter << val;
                }
                emitter << YAML::EndSeq;
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                emitter << YAML::BeginSeq;
                for (const std::string& val : std::get<std::vector<std::string>>(entry.value)) {
                    emitter << val;
                }
                emitter << YAML::EndSeq;
            }
        }
        
        file << emitter.c_str();
        NLM_LOG_INFO("Saved YAML configuration to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML configuration: " + std::string(e.what()));
        return false;
    }
}

// Save configuration to text file (original format)
bool Config::saveToText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
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
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                bool first = true;
                for (int val : arg) {
                    if (!first) file << ", ";
                    file << val;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool first = true;
                for (double val : arg) {
                    if (!first) file << ", ";
                    file << val;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const std::string& val : arg) {
                    if (!first) file << ", ";
                    file << "\"" << val << "\"";
                    first = false;
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved text-based configuration to: " + filepath);
    return true;
}

} // namespace nlm
