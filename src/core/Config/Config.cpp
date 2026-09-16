// JSON/YAML parser implementation
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <variant>
#include <algorithm>

namespace nlm {
namespace {

// Helper to convert ConfigValue to JSON
nlohmann::json toJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        }
        return nullptr; // Should not happen
    }, value);
}

// Helper to convert JSON to ConfigValue
ConfigValue fromJson(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number_unsigned()) {
        return json.get<int64_t>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        // Try to infer array type
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec = json.get<std::vector<int>>();
                return vec;
            } else if (json[0].is_number()) {
                std::vector<double> vec = json.get<std::vector<double>>();
                return vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec = json.get<std::vector<std::string>>();
                return vec;
            }
        }
        // Fallback to string array
        std::vector<std::string> vec;
        for (const auto& item : json) {
            vec.push_back(item.get<std::string>());
        }
        return vec;
    }
    throw std::runtime_error("Unsupported JSON type");
}

} // anonymous namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Use nlohmann/json for proper JSON parsing
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                std::string description = "";
                
                // Check for description field (can be at top level or in value)
                if (it.value().is_object() && it.value().contains("_description")) {
                    description = it.value()["_description"].get<std::string>();
                    // Remove _description from value
                    nlohmann::json value = it.value();
                    value.erase("_description");
                    set(key, fromJson(value), ConfigSource::File);
                } else {
                    set(key, fromJson(it.value()), ConfigSource::File);
                }
                
                if (!description.empty()) {
                    set(key, description); // Store description separately
                }
            }
        } else {
            // If not an object, try as simple key=value pairs
            clear();
            std::string line;
            std::ifstream file2(filepath);
            while (std::getline(file2, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Config::loadFromFile error: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write JSON with comments for descriptions
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json value;
            
            // Convert ConfigValue to JSON
            value = toJson(entry.value);
            
            // Add description as a comment (JSON doesn't support comments in standard)
            // We'll add it as a special field that can be stripped later
            jsonData[entry.key] = value;
        }
        
        // Write formatted JSON
        file << jsonData.dump(2);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Config::saveToFile error: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm