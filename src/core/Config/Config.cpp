#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <filesystem>

// Add nlohmann/json dependency to pyproject.toml or include it directly
// Since NLM is a research project, we'll include the single header version

namespace nlm {

// Helper function for simple key=value file format (backward compatibility)
bool Config::loadFromFileSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (more common and structured)
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".JSON") {
        // Try to parse as JSON
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Clear existing entries
            clear();
            
            // Parse JSON object
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    std::string key = it.key();
                    ConfigValue value = JsonUtils::jsonValueToConfigValue(it.value());
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Single value - wrap it in object with "value" key
                ConfigValue value = JsonUtils::jsonValueToConfigValue(jsonData);
                set("value", value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            // JSON parsing failed, fall back to simple format
        }
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".cfg" || extension == ".properties") {
        // Try simple key=value format (for backward compatibility)
        return loadFromFileSimpleFormat(filepath);
    } else {
        // Try JSON first, then simple format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read first line to guess format
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0);
        
        bool looksLikeJson = false;
        if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
            // Might be JSON
            try {
                nlohmann::json testJson;
                file >> testJson;
                looksLikeJson = true;
            } catch (...) {}
        }
        
        if (looksLikeJson) {
            try {
                nlohmann::json jsonData;
                file >> jsonData;
                
                clear();
                
                if (jsonData.is_object()) {
                    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                        std::string key = it.key();
                        ConfigValue value = JsonUtils::jsonValueToConfigValue(it.value());
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const std::exception& e) {
                file.clear();
                file.seekg(0);
            }
        }
        
        // Fall back to simple format
        return loadFromFileSimpleFormat(filepath);
    }
    
    return false;
}

bool Config::loadFromFileSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect if user wants JSON format
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".JSON") {
        // Save as JSON
        nlohmann::json jsonData = nlohmann::json::object();
        
        for (const auto& entry : pImpl->entries) {
            std::string jsonKey = entry.key;
            
            // Convert ConfigValue to JSON
            nlohmann::json jsonValue;
            std::visit([&jsonValue](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Convert to double array for JSON
                    jsonValue = nlohmann::json::array();
                    for (const auto& item : arg) {
                        jsonValue.push_back(static_cast<double>(item));
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = arg;
                }
            }, entry.value);
            
            jsonData[jsonKey] = jsonValue;
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indent
        
    } else {
        // Save in simple format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    // Check if it's an integer value
                    if (static_cast<int>(arg) == arg) {
                        file << static_cast<int>(arg);
                    } else {
                        file << arg;
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Convert to comma-separated list
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        // Check if it's an integer value
                        if (static_cast<int>(arg[i]) == arg[i]) {
                            file << static_cast<int>(arg[i]);
                        } else {
                            file << arg[i];
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                } else {
                    file << "UNKNOWN_TYPE";
                }
            }, entry.value);
            
            file << "\n\n";
        }
    }
    
    return true;
}

} // namespace nlm