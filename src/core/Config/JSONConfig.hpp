#ifndef JSONCONFIG_HPP
#define JSONCONFIG_HPP

#include "Config.hpp"
#include <nlohmann/json.hpp>

namespace nlm {

class JSONConfig {
public:
    // Parse JSON string into Config
    static bool parseJSON(const std::string& jsonStr, Config& config);
    
    // Parse JSON file into Config
    static bool parseJSONFile(const std::string& filepath, Config& config);
    
    // Generate JSON string from Config
    static std::string toJSON(const Config& config);
    
    // Generate JSON string from Config with pretty printing
    static std::string toJSONPretty(const Config& config);
    
private:
    static ConfigSource jsonSourceToConfigSource(int source);
    static std::string configSourceToJSONString(ConfigSource source);
};

} // namespace nlm

#endif // JSONCONFIG_HPP