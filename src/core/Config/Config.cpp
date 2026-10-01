#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Helper functions for JSON serialization/deserialization
static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    return json;
}

static ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue value;
    
    if (json.is_number_integer()) {
        if (json.get<int64_t>() >= INT_MIN && json.get<int64_t>() <= INT_MAX) {
            value = json.get<int>();
        } else {
            value = json.get<int64_t>();
        }
    } else if (json.is_number_float()) {
        value = json.get<double>();
    } else if (json.is_boolean()) {
        value = json.get<bool>();
    } else if (json.is_string()) {
        value = json.get<std::string>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            const auto& first = json[0];
            if (first.is_number_integer() && first.get<int64_t>() >= INT_MIN && first.get<int64_t>() <= INT_MAX) {
                std::vector<int> intArray;
                for (const auto& item : json) {
                    intArray.push_back(item.get<int>());
                }
                value = intArray;
            } else if (first.is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : json) {
                    doubleArray.push_back(item.get<double>());
                }
                value = doubleArray;
            } else if (first.is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : json) {
                    stringArray.push_back(item.get<std::string>());
                }
                value = stringArray;
            }
        }
    }
    
    return value;
}

static bool tryLoadJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
