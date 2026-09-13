#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include "Config.hpp"

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Extract key-value pairs recursively
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Store as string initially, will be converted on demand
            std::string valueStr;
            
            if (it.value().is_string()) {
                valueStr = it.value().get<std::string>();
                set(key, valueStr, ConfigSource::File);
            } else if (it.value().is_number_integer()) {
                int64_t val = it.value().get<int64_t>();
                set(key, val, ConfigSource::File);
            } else if (it.value().is_number_unsigned()) {
                uint64_t val = it.value().get<uint64_t>();
                set(key, val, ConfigSource::File);
            } else if (it.value().is_number_float()) {
                double val = it.value().get<double>();
                set(key, val, ConfigSource::File);
            } else if (it.value().is_boolean()) {
                bool val = it.value().get<bool>();
                set(key, val, ConfigSource::File);
            } else if (it.value().is_array()) {
                // Convert array to string representation
                std::vector<double> arr = it.value().get<std::vector<double>>();
                std::string arrStr = "[";
                for (size_t i = 0; i < arr.size(); ++i) {
                    arrStr += std::to_string(arr[i]);
                    if (i < arr.size() - 1) arrStr += ", ";
                }
                arrStr += "]";
                set(key, arrStr, ConfigSource::File);
            } else if (it.value().is_object()) {
                // For nested objects, convert to JSON string
                std::string objStr = it.value().dump();
                set(key, objStr, ConfigSource::File);
            } else {
                // Handle null or other types
                set(key, "", ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Try to get the actual value based on type
            if (entry.value.index() == 0) {  // int
                jsonData[entry.key] = std::get<int>(entry.value);
            } else if (entry.value.index() == 1) {  // int64_t
                jsonData[entry.key] = std::get<int64_t>(entry.value);
            } else if (entry.value.index() == 2) {  // double
                jsonData[entry.key] = std::get<double>(entry.value);
            } else if (entry.value.index() == 3) {  // bool
                jsonData[entry.key] = std::get<bool>(entry.value);
            } else if (entry.value.index() == 4) {  // std::string
                jsonData[entry.key] = std::get<std::string>(entry.value);
            } else if (entry.value.index() == 5) {  // std::vector<int>
                jsonData[entry.key] = std::get<std::vector<int>>(entry.value);
            } else if (entry.value.index() == 6) {  // std::vector<double>
                jsonData[entry.key] = std::get<std::vector<double>>(entry.value);
            } else if (entry.value.index() == 7) {  // std::vector<std::string>
                jsonData[entry.key] = std::get<std::vector<std::string>>(entry.value);
            }
        }
        
        // Write formatted JSON with indentation
        file << jsonData.dump(2);
        file << "\n";
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nlm
