#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include "Config.hpp"

using json = nlohmann::json;

namespace nlm {

// Helper function for simple key=value parsing
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

bool Config::loadFromFile(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    // Try to parse as JSON first
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            // Not valid JSON, try the old key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
            return loadFromSimpleFormat(filepath);
        }
        
        // JSON parsing successful - convert to key=value format
        clear();
        
        // Handle different JSON value types
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Skip comment keys
            if (key.rfind("#", 0) == 0) {
                continue; // This is a comment, skip it
            }
            
            // Convert JSON value to ConfigValue
            if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Handle arrays
                json array = *it;
                if (array.empty()) {
                    continue;
                }
                
                // Check if array contains all integers
                bool allInts = true;
                for (const auto& elem : array) {
                    if (!elem.is_number_integer()) {
                        allInts = false;
                        break;
                    }
                }
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    for (const auto& elem : array) {
                        intArray.push_back(elem.get<int64_t>());
                    }
                    set(key, intArray, ConfigSource::File);
                } else {
                    // Check if array contains all doubles
                    bool allDoubles = true;
                    for (const auto& elem : array) {
                        if (!elem.is_number_float() && !elem.is_number_integer()) {
                            allDoubles = false;
                            break;
                        }
                    }
                    
                    if (allDoubles) {
                        std::vector<double> doubleArray;
                        for (const auto& elem : array) {
                            doubleArray.push_back(elem.get<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    } else {
                        // Try string array
                        bool allStrings = true;
                        for (const auto& elem : array) {
                            if (!elem.is_string()) {
                                allStrings = false;
                                break;
                            }
                        }
                        
                        if (allStrings) {
                            std::vector<std::string> stringArray;
                            for (const auto& elem : array) {
                                stringArray.push_back(elem.get<std::string>());
                            }
                            set(key, stringArray, ConfigSource::File);
                        } else {
                            // Fallback to string representation
                            std::string value = array.dump();
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            } else if (it->is_object()) {
                // Convert nested objects to JSON string
                std::string value = it->dump();
                set(key, value, ConfigSource::File);
            } else {
                // Null value or other types - treat as string
                std::string value = it->dump();
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (...) {
        // Fallback to simple key=value parsing
        return loadFromSimpleFormat(filepath);
    }
#else
    // nlohmann/json not available, use simple parsing
    return loadFromSimpleFormat(filepath);
#endif
}

bool Config::saveToFile(const std::string& filepath) const {
#ifdef HAS_NLOHMANN_JSON
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Skip description keys when saving
        if (entry.key.rfind("#", 0) == 0) {
            continue;
        }
        
        // Add description as a comment in the JSON
        if (!entry.description.empty()) {
            // Store description in a separate field
            std::string descKey = "#desc_" + entry.key;
            j[descKey] = entry.description;
        }
        
        // Convert ConfigValue to JSON
        std::visit([&](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), int>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<decltype(val), double>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<decltype(val), std::vector<int64_t>>) {
                json array = val;
                j[entry.key] = array;
            } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
                json array = val;
                j[entry.key] = array;
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                json array = val;
                j[entry.key] = array;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write JSON with pretty formatting
    file << j.dump(2);
    
    return true;
#else
    // Fallback to simple format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        // Skip description keys
        if (entry.key.rfind("#", 0) == 0) {
            continue;
        }
        
        file << entry.key << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
#endif
}

} // namespace nlm