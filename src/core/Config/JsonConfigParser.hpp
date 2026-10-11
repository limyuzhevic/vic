#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <nlohmann/json.hpp>

namespace nlm {

class JsonConfigParser {
public:
    static bool parseFile(const std::string& filepath, Config& config);
    static bool parseString(const std::string& content, Config& config);
    static std::string serializeToJson(const Config& config);
    static std::string serializeToYaml(const Config& config);

private:
    static void convertJsonToConfig(const nlohmann::json& json, Config& config, ConfigSource source);
    static nlohmann::json convertConfigToJson(const Config& config);
    static std::vector<std::string> splitPath(const std::string& path);
};

} // namespace nlm
