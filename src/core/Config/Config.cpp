#if defined(__linux__) && __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#endif

#if defined(__linux__) && __has_include(<yaml-cpp/yaml.h>)
#include <yaml-cpp/yaml.h>
#endif

#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
#ifdef __linux__
    if (__has_include(<nlohmann/json.hpp>)) {
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                nlohmann::json jsonData = nlohmann::json::parse(file);
                return parseFromJson(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
            }
        }
    }
    
    // Try YAML
    if (__has_include(<yaml-cpp/yaml.h>)) {
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                YAML::Node yamlData = YAML::Load(file);
                return parseFromYaml(yamlData);
            } catch (const YAML::ParserException& e) {
                NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
            }
        }
    }
#endif
    
    // Fall back to simple key=value format
    NLM_LOG_INFO("Attempting simple key=value format for " + filepath);
    return Config::loadFromFileSimple(filepath);
}

bool Config::parseFromJson(const nlohmann::json& json) {
    clear();
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        
        try {
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Convert JSON array to vector
                std::vector<double> vec;
                for (const auto& item : *it) {
                    vec.push_back(item.get<double>());
                }
                value = vec;
            } else {
                NLM_LOG_WARN("Skipping JSON key \"" + key + "\" - unsupported type");
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing JSON key \"" + key + "\": " + std::string(e.what()));
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::parseFromYaml(const YAML::Node& yaml) {
    clear();
    
    if (!yaml.IsMap()) {
        NLM_LOG_ERROR("YAML root is not a map (key-value pairs)");
        return false;
    }
    
    for (const auto& it : yaml) {
        std::string key = it.first.as<std::string>();
        ConfigValue value;
        
        try {
            if (it.second.IsScalar()) {
                const std::string& scalar = it.second.as<std::string>();
                
                // Try to infer type from string content
                if (scalar.find('.') != std::string::npos) {
                    try {
                        value = std::stod(scalar);
                    } catch (...) {
                        value = scalar;
                    }
                } else if (scalar == "true" || scalar == "false") {
                    value = (scalar == "true");
                } else {
                    try {
                        value = std::stoll(scalar);
                    } catch (...) {
                        value = scalar;
                    }
                }
            } else if (it.second.IsSequence()) {
                // Convert YAML sequence to vector
                std::vector<double> vec;
                for (const auto& item : it.second) {
                    vec.push_back(item.as<double>());
                }
                value = vec;
            } else {
                NLM_LOG_WARN("Skipping YAML key \"" + key + "\" - unsupported type");
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing YAML key \"" + key + "\": " + std::string(e.what()));
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    // Original simple key=value parsing logic
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARN("Skipping line " + std::to_string(lineNum) + " in " + filepath + ": no '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        success = true;
    }
    
    if (!success) {
        NLM_LOG_ERROR("No valid key=value pairs found in " + filepath);
        return false;
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Save as JSON for better readability and compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, uint64_t> || std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int> > ||
                                std::is_same_v<T, std::vector<double> > ||
                                std::is_same_v<T, std::vector<std::string> >) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    // Write pretty-printed JSON
    file << std::setw(2) << jsonData.dump(2);
    file << std::endl;
    
    NLM_LOG_INFO("Saved configuration to " + filepath + " (" + 
                 std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

// Helper to get file extension and determine format
static std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}

// Override loadFromFile to auto-detect format
bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Auto-detect format based on extension or content
    if (ext == "json") {
#ifdef __linux__
        if (__has_include(<nlohmann/json.hpp>)) {
            try {
                nlohmann::json jsonData = nlohmann::json::parse(file);
                return parseFromJson(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
            }
        } else {
            NLM_LOG_ERROR("JSON support not available (nlohmann/json not found)");
        }
#endif
        // If JSON parsing fails or not available, fall through
    } else if (ext == "yaml" || ext == "yml") {
#ifdef __linux__
        if (__has_include(<yaml-cpp/yaml.h>)) {
            file.clear();
            file.seekg(0, std::ios::beg);
            try {
                YAML::Node yamlData = YAML::Load(file);
                return parseFromYaml(yamlData);
            } catch (const YAML::ParserException& e) {
                NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
            }
        } else {
            NLM_LOG_ERROR("YAML support not available (yaml-cpp not found)");
        }
#endif
        // If YAML parsing fails or not available, fall through
    }
    
    // Default to simple key=value format
    file.clear();
    file.seekg(0, std::ios::beg);
    return loadFromFileSimple(filepath);
}

} // namespace nlm
