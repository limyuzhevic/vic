#include "AdvancedConfigManager.hpp"
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

struct AdvancedConfigManager::Impl {
    std::vector<ConfigEntry> entries;
};

AdvancedConfigManager::AdvancedConfigManager() : pImpl(std::make_unique<Impl>()) {}

AdvancedConfigManager::~AdvancedConfigManager() = default;

bool AdvancedConfigManager::loadFromFile(const std::string& filepath) {
    // First, try to load as JSON
    if (loadFromJsonFile(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    return loadFromSimpleFile(filepath);
}

bool AdvancedConfigManager::loadFromJsonFile(const std::string& filepath) {
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
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfigManager::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            pImpl->entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

bool AdvancedConfigManager::saveToFile(const std::string& filepath) const {
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

bool AdvancedConfigManager::setFromString(const std::string& key, const std::string& valueStr) {
    return setTypedFromString<std::string>(key, valueStr);
}

template<typename T>
bool AdvancedConfigManager::setTypedFromString(const std::string& key, const std::string& valueStr) {
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
        
        if constexpr (!std::is_same_v<T, std::string>) {
            if (pos != valueStr.size()) return false;
        }
        
        // Check if key already exists and update it
        bool found = false;
        for (auto& entry : pImpl->entries) {
            if (entry.key == key) {
                entry.value = ConfigValue(value);
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->entries.emplace_back(key, ConfigValue(value), ConfigSource::Runtime);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json AdvancedConfigManager::configValueToJson(const ConfigValue& value) const {
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
    }, value);
    return jsonValue;
}

bool AdvancedConfigManager::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& value) const {
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
    } catch (const std::exception& e) {
    }
    
    return false;
}

std::type_index AdvancedConfigManager::parseTypeFromString(const std::string& typeStr) const {
    if (typeStr == "int") return typeid(int);
    else if (typeStr == "int64_t") return typeid(int64_t);
    else if (typeStr == "double") return typeid(double);
    else if (typeStr == "bool") return typeid(bool);
    else if (typeStr == "string") return typeid(std::string);
    else return typeid(void);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm