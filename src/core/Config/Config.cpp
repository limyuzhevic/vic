#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json json_cache;  // Cache for JSON parsing
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    // Try JSON first
    if (parseJSONFile(filepath)) {
        return true;
    }
    
    // Try YAML if JSON failed
    if (parseYAMLFile(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    // TODO PHASE 2: Remove this fallback after full JSON/YAML support
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

bool Config::parseJSONFile(const std::string& filepath) {
    // Try to parse JSON file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        pImpl->json_cache = nlohmann::json::parse(file);
        
        // Convert JSON to config entries
        if (pImpl->json_cache.is_object()) {
            for (const auto& element : pImpl->json_cache.items()) {
                std::string key = element.key();
                nlohmann::json value = element.value();
                
                // Convert JSON value to string for storage
                std::string value_str;
                if (value.is_string()) {
                    value_str = value.get<std::string>();
                } else if (value.is_boolean()) {
                    value_str = value.get<bool>() ? "true" : "false";
                } else if (value.is_number_integer()) {
                    value_str = std::to_string(value.get<int64_t>());
                } else if (value.is_number_float()) {
                    value_str = std::to_string(value.get<double>());
                } else if (value.is_array()) {
                    // Convert array to string representation
                    value_str = "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) value_str += ", ";
                        if (value[i].is_string()) {
                            value_str += "\"" + value[i].get<std::string>() + "\"";
                        } else {
                            value_str += std::to_string(value[i].get<double>());
                        }
                    }
                    value_str += "]";
                } else {
                    value_str = value.dump();
                }
                
                set(key, value_str, ConfigSource::File);
            }
            return true;
        }
    } catch (const nlohmann::json::parse_error& e) {
        // JSON parsing failed, try YAML
        return false;
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Config::parseYAMLFile(const std::string& filepath) {
    // Try to parse YAML file if available
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Check if libyaml-cpp is available
        // For now, just read as text and parse simple YAML-like format
        std::string line;
        std::unordered_map<std::string, std::string> yaml_map;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse YAML key: value format
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = trim(line.substr(0, colon_pos));
                std::string value = trim(line.substr(colon_pos + 1));
                
                // Remove quotes from value
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                yaml_map[key] = value;
            }
        }
        
        // Convert to config entries
        for (const auto& element : yaml_map) {
            set(element.first, element.second, ConfigSource::File);
        }
        return true;
    } catch (...) {
        return false;
    }
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
        return false;
    }
    
    // Try to save as JSON
    if (saveAsJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string
        std::string value_str;
        std::visit([&value_str](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value_str = '"' + arg + '"';
            } else {
                value_str = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << value_str << "\n";
    }
    
    return true;
}

bool Config::saveAsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json_data = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        std::string value_str;
        std::visit([&value_str](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value_str = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value_str = arg ? "true" : "false";
            } else {
                value_str = std::to_string(arg);
            }
        }, entry.value);
        
        // Remove quotes from string values for JSON
        if (value_str.size() >= 2 && value_str.front() == '"' && value_str.back() == '"') {
            // This is a string value, keep it as JSON string
            json_data[entry.key] = value_str.substr(1, value_str.size() - 2);
        } else if (value_str == "true" || value_str == "false") {
            // Boolean value
            json_data[entry.key] = (value_str == "true");
        } else {
            // Try to parse as number
            try {
                if (value_str.find('.') != std::string::npos) {
                    double num = std::stod(value_str);
                    json_data[entry.key] = num;
                } else {
                    int64_t num = std::stoll(value_str);
                    json_data[entry.key] = num;
                }
            } catch (...) {
                // Keep as string if not a number
                json_data[entry.key] = value_str;
            }
        }
    }
    
    file << json_data.dump(2);
    return true;
}

// Explicit template instantiations

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

// Other method implementations continue...
// The rest of the method implementations are in the original file
// This is a partial implementation focusing on the JSON/YAML parsing improvements

} // namespace nlm
