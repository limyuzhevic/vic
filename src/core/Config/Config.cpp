#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First, try to load as JSON
    if (loadFromJsonFile(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (jsonToConfigValue(*it, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJson(entry.value);
    }
    
    file << jsonData.dump(2);
    
    return true;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr) {
    return setTypedFromString<std::string>(key, valueStr);
}

template<typename T>
bool Config::setTypedFromString(const std::string& key, const std::string& valueStr) {
    try {
        size_t pos;
        T value;
        
        if constexpr (std::is_same_v<T, int>) {
            value = std::stoi(valueStr, &pos);
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            value = std::stoll(valueStr, &pos);
        }
        else if constexpr (std::is_same_v<T, double>) {
            value = std::stod(valueStr, &pos);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            if (valueStr == "true") value = true;
            else if (valueStr == "false") value = false;
            else return false;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            value = valueStr;
        }
        else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Parse comma-separated list of ints
            std::vector<int> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stoi(item));
                }
            }
            value = vec;
        }
        else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Parse comma-separated list of doubles
            std::vector<double> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stod(item));
                }
            }
            value = vec;
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Parse comma-separated list of strings
            std::vector<std::string> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    // Remove quotes if present
                    if (item.size() >= 2 && 
                        ((item.front() == '"' && item.back() == '"') ||
                         (item.front() == '\'' && item.back() == '\''))) {
                        item = item.substr(1, item.size() - 2);
                    }
                    vec.push_back(item);
                }
            }
            value = vec;
        }
        else {
            return false;
        }
        
        // Verify we consumed the entire string
        if constexpr (!std::is_same_v<T, std::string> && 
                      !std::is_same_v<T, std::vector<int>> && 
                      !std::is_same_v<T, std::vector<double>> && 
                      !std::is_same_v<T, std::vector<std::string>>) {
            if (pos != valueStr.size()) return false;
        }
        
        set(key, value, ConfigSource::Runtime);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) const {
    nlohmann::json jsonValue;
    std::visit([&jsonValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    return jsonValue;
}

bool Config::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& value) const {
    try {
        if (jsonValue.is_number_integer()) {
            value = jsonValue.get<int>();
            return true;
        }
        else if (jsonValue.is_number_float()) {
            value = jsonValue.get<double>();
            return true;
        }
        else if (jsonValue.is_boolean()) {
            value = jsonValue.get<bool>();
            return true;
        }
        else if (jsonValue.is_string()) {
            value = jsonValue.get<std::string>();
            return true;
        }
        else if (jsonValue.is_array()) {
            // Check array element types
            if (!jsonValue.empty()) {
                auto firstType = jsonValue[0].type();
                bool allSameType = true;
                for (const auto& elem : jsonValue) {
                    if (elem.type() != firstType) {
                        allSameType = false;
                        break;
                    }
                }
                
                if (allSameType) {
                    if (firstType == nlohmann::json::value_t::number_integer) {
                        std::vector<int> vec = jsonValue.get<std::vector<int>>();
                        value = vec;
                        return true;
                    }
                    else if (firstType == nlohmann::json::value_t::number_float) {
                        std::vector<double> vec = jsonValue.get<std::vector<double>>();
                        value = vec;
                        return true;
                    }
                    else if (firstType == nlohmann::json::value_t::string) {
                        std::vector<std::string> vec = jsonValue.get<std::vector<std::string>>();
                        value = vec;
                        return true;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
    }
    
    return false;
}

// Template instantiations for common types

// Template instantiations for get()
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Template instantiations for getOr()
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Template instantiations for setTypedFromString
template bool Config::setTypedFromString<int>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<int64_t>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<double>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<bool>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<std::string>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<std::vector<int>>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<std::vector<double>>(const std::string&, const std::string&) const;
template bool Config::setTypedFromString<std::vector<std::string>>(const std::string&, const std::string&) const;

} // namespace nlm