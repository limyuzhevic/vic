#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Logger declaration for warning messages
#include "Logger.hpp"

using json = nlohmann::json;

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension = "." + std::string(filepath.substr(filepath.find_last_of(".")));
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == ".json") {
            json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                const json& value = it.value();
                
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle arrays
                    std::vector<double> arr = value.get<std::vector<double>>();
                    configValue = arr;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } else if (extension == ".yaml" || extension == ".yml") {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            for (const auto& it : doc) {
                std::string key = it.first.as<std::string>();
                const YAML::Node& value = it.second;
                
                ConfigValue configValue;
                
                if (value.IsScalar()) {
                    std::string type = value.Scalar();
                    if (type.find('.') != std::string::npos || type.find('e') != std::string::npos || type.find('E') != std::string::npos) {
                        // Could be a float or scientific notation
                        configValue = type;  // String to handle numbers as strings
                    } else if (type == "true" || type == "false") {
                        configValue = (type == "true");
                    } else {
                        try {
                            size_t pos;
                            int64_t intVal = std::stoll(type, &pos);
                            if (pos == type.length()) {
                                configValue = intVal;
                            } else {
                                double doubleVal = std::stod(type);
                                configValue = doubleVal;
                            }
                        } catch (...) {
                            configValue = type;
                        }
                    }
                } else if (value.IsSequence()) {
                    std::vector<double> arr;
                    for (const auto& item : value) {
                        try {
                            size_t pos;
                            double val = std::stod(item.as<std::string>());
                            arr.push_back(val);
                        } catch (...) {
                            // Skip non-numeric items
                        }
                    }
                    configValue = arr;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } else {
            // Fall back to simple key=value format
            return loadFromFileSimple(filepath);
        }
    } catch (const std::exception& e) {
        // If parsing fails, fall back to simple format
        NLM_LOG_WARNING(std::string("Failed to parse " + filepath + ": " + e.what() + ", falling back to simple format"));
        return loadFromFileSimple(filepath);
    }
}

// Helper method for simple key=value format
bool Config::loadFromFileSimple(const std::string& filepath) {
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