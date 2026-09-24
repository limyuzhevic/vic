// Simple JSON support for config files
// This file provides basic JSON serialization/deserialization for config values

#include "Config.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Helper functions for JSON conversion
json ConfigValueToJson(const ConfigValue& value) {
    json j;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

ConfigValue JsonToConfigValue(const json& j) {
    ConfigValue result;
    
    if (j.is_number_integer()) {
        result = j.get<int64_t>();
    } else if (j.is_number_float()) {
        result = j.get<double>();
    } else if (j.is_boolean()) {
        result = j.get<bool>();
    } else if (j.is_string()) {
        result = j.get<std::string>();
    } else if (j.is_array()) {
        // Check array element type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                std::vector<int64_t> vec = j.get<std::vector<int64_t>>();
                result = std::vector<int>(vec.begin(), vec.end());
            } else if (j[0].is_number_float()) {
                result = j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                result = j.get<std::vector<std::string>>();
            }
        }
    }
    
    return result;
}

void to_json(json& j, const ConfigEntry& entry) {
    j[entry.key] = ConfigValueToJson(entry.value);
    j[entry.key]["__source__"] = static_cast<int>(entry.source);
    if (!entry.description.empty()) {
        j[entry.key]["__description__"] = entry.description;
    }
}

void from_json(const json& j, ConfigEntry& entry) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.key() == "__source__" || it.key() == "__description__") {
            continue;
        }
        
        entry.key = it.key();
        entry.value = JsonToConfigValue(*it);
        
        // Extract metadata if present
        if (it->contains("__source__")) {
            entry.source = static_cast<ConfigSource>(it->at("__source__").get<int>());
        }
        if (it->contains("__description__")) {
            entry.description = it->at("__description__").get<std::string>();
        }
    }
}

} // namespace nlm
