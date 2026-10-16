#pragma once

namespace nlohmann {
    template<typename T, typename... Args>
    T json_get(const nlohmann::json& j, const std::string& key, const T& default_value);
}

// Simple JSON helper functions
namespace nlohmann {
    template<typename T>
    T json_get(const nlohmann::json& j, const std::string& key, const T& default_value) {
        if (j.contains(key)) {
            try {
                return j.at(key).get<T>();
            } catch (const std::exception&) {
                return default_value;
            }
        }
        return default_value;
    }
}