#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <map>

// Include JSON and YAML libraries if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from JSON file
    bool loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json j;
            file >> j;
            
            // Convert JSON to config entries
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                const auto& value = it.value();
                
                if (value.is_number_integer()) {
                    configValue = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Convert array to vector of appropriate type
                    if (!value.empty()) {
                        if (value[0].is_number_integer()) {
                            std::vector<int> intArray;
                            for (const auto& item : value) {
                                intArray.push_back(item.get<int>());
                            }
                            configValue = intArray;
                        } else if (value[0].is_number_float()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : value) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        } else {
                            std::vector<std::string> stringArray;
                            for (const auto& item : value) {
                                stringArray.push_back(item.get<std::string>());
                            }
                            configValue = stringArray;
                        }
                    }
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception&) {
            return false; // Not valid JSON
        }
    }
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            // Convert YAML to config entries
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                
                // Convert YAML value to ConfigValue
                ConfigValue configValue;
                const auto& value = pair.second;
                
                if (value.IsScalar()) {
                    // Try to determine type
                    if (value.IsSequence()) {
                        // Array - check element types
                        if (!value.size()) {
                            configValue = std::vector<int>{};
                        } else {
                            bool allInts = true, allDoubles = true, allStrings = true;
                            for (const auto& item : value) {
                                if (!item.IsScalar()) {
                                    allInts = allDoubles = allStrings = false;
                                    break;
                                }
                                if (item.IsInteger()) allStrings = false;
                                else if (item.IsFloat()) allInts = false;
                                else allInts = allDoubles = false;
                            }
                            
                            if (allInts) {
                                std::vector<int> intArray;
                                for (const auto& item : value) {
                                    intArray.push_back(item.as<int>());
                                }
                                configValue = intArray;
                            } else if (allDoubles) {
                                std::vector<double> doubleArray;
                                for (const auto& item : value) {
                                    doubleArray.push_back(item.as<double>());
                                }
                                configValue = doubleArray;
                            } else if (allStrings) {
                                std::vector<std::string> stringArray;
                                for (const auto& item : value) {
                                    stringArray.push_back(item.as<std::string>());
                                }
                                configValue = stringArray;
                            } else {
                                // Mixed types - store as string
                                configValue = value.as<std::string>();
                            }
                        }
                    } else if (value.IsInteger()) {
                        configValue = value.as<int>();
                    } else if (value.IsFloat()) {
                        configValue = value.as<double>();
                    } else if (value.IsBoolean()) {
                        configValue = value.as<bool>();
                    } else {
                        configValue = value.as<std::string>();
                    }
                } else {
                    // Array/scalar YAML node
                    configValue = value.as<std::string>();
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception&) {
            return false; // Not valid YAML
        }
    }
    
    // Load from simple key=value format
    bool loadFromSimpleFormat(const std::string& filepath) {
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
