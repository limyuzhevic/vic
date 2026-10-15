#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "Config.hpp"

namespace nlm {

class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    bool setFromString(const std::string& key, const std::string& valueStr);
    template<typename T>
    bool setTypedFromString(const std::string& key, const std::string& valueStr);
    
    bool validate(const std::vector<ConfigSchema>& schema) const;
    std::string getDocumentation() const;
    const std::vector<ConfigEntry>& getEntries() const { return pImpl->entries; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& value) const;
    std::type_index parseTypeFromString(const std::string& typeStr) const;
};

} // namespace nlm