//=============================================================================
// ConfigImpl.hpp - Implementation Details for Enhanced Config
//=============================================================================

#pragma once

#include "Config.hpp"
#include <limits>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> groups;  // For grouping configuration parameters
    std::vector<std::string> requiredGroups;
    nlohmann::json schema;  // JSON schema for validation
    
    Impl() : schema(createDefaultSchema()) {}
    
    // Internal helper functions
    static nlohmann::json createDefaultSchema();
    static bool validateAgainstSchema(const std::vector<ConfigEntry>& entries,
                                     const nlohmann::json& schema);
    static std::string getTypeNameFromValue(const ConfigValue& value);
    static std::string normalizeKey(const std::string& key);
    static ConfigValue parseValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static bool validateEntry(const ConfigEntry& entry);
};

} // namespace nlm

// Include implementation headers
#include "ConfigImpl.hpp"

// Implementation file
#include "ConfigImpl.cpp"

#endif // CONFIG_IMPL_HPP