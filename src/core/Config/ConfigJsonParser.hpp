#pragma once

#include "Config.hpp"
#include <nlohmann/json.hpp>

namespace nlm {

class Config;

class ConfigJsonParser {
public:
    explicit ConfigJsonParser(Config& config);
    
    bool parseFile(const std::string& filepath);
    bool parseString(const std::string& jsonStr, ConfigSource source);
    
    static bool supportsExtension(const std::string& filename);
    
private:
    void parseJsonObject(const nlohmann::json& jsonData, 
                         const std::string& prefix, 
                         ConfigSource source);
    void parseJsonValue(const nlohmann::json& value, 
                       const std::string& key, 
                       ConfigSource source);
    
    Config& config_;
};

} // namespace nlm
