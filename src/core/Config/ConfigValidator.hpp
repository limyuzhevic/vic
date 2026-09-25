#pragma once

#include <nlohmann/json.hpp>
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Configuration validation and schema
class ConfigValidator {
public:
    static bool validate(const std::string& key, const ConfigValue& value);
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
    static bool validateFileFormat(const std::string& filepath);
    static bool validateValueRange(const std::string& key, const ConfigValue& value);
    static bool validateDependencies(const std::string& key, const ConfigValue& value, const Config& config);
};

// JSON-based configuration loader
class JSONConfigLoader {
public:
    static bool loadFromJSON(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool saveToJSON(const std::string& filepath, const std::vector<ConfigEntry>& entries);
    static bool loadFromYAML(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool saveToYAML(const std::string& filepath, const std::vector<ConfigEntry>& entries);
};

} // namespace nlm
