// nlohmann/json.hpp - Simple include wrapper for nlohmann/json library
// This file provides a minimal interface to the nlohmann/json library if available
// For full functionality, use nlohmann/json directly or include this wrapper

#ifndef NLM_NLOHMANN_JSON_HPP
#define NLM_NLOHMANN_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>

// Main nlohmann/json types for use in NLM
namespace nlohmann {
    template<typename T> class basic_json;
    using json = basic_json<void>;
}

// Helper function to safely access JSON values
inline std::string getStringValue(const nlohmann::json& j, const std::string& key, const std::string& defaultValue = "") {
    return j.contains(key) ? j[key].get<std::string>() : defaultValue;
}

inline int getIntValue(const nlohmann::json& j, const std::string& key, int defaultValue = 0) {
    return j.contains(key) ? j[key].get<int>() : defaultValue;
}

inline double getDoubleValue(const nlohmann::json& j, const std::string& key, double defaultValue = 0.0) {
    return j.contains(key) ? j[key].get<double>() : defaultValue;
}

inline bool getBoolValue(const nlohmann::json& j, const std::string& key, bool defaultValue = false) {
    return j.contains(key) ? j[key].get<bool>() : defaultValue;
}

inline std::vector<int> getIntArrayValue(const nlohmann::json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_array()) {
        return j[key].get<std::vector<int>>();
    }
    return std::vector<int>();
}

inline std::vector<double> getDoubleArrayValue(const nlohmann::json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_array()) {
        return j[key].get<std::vector<double>>();
    }
    return std::vector<double>();
}

inline std::vector<std::string> getStringArrayValue(const nlohmann::json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_array()) {
        return j[key].get<std::vector<std::string>>();
    }
    return std::vector<std::string>();
}

#endif // NLM_NLOHMANN_JSON_HPP