#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>

namespace nlm {

class ConfigFormatDetector {
public:
    enum class Format {
        Unknown,
        KeyValue,  // key=value format (.cfg, .txt)
        JSON,      // JSON format (.json)
        YAML,      // YAML format (.yaml, .yml)
        TOML       // TOML format (.toml)
    };

    static Format detectFormat(const std::string& filepath);
    static Format detectFormatFromExtension(const std::string& filepath);
    static bool isTextFormat(const Format format);
    static std::string formatToString(const Format format);
    static Format stringToFormat(const std::string& formatStr);
};

class ConfigParser {
public:
    using ConfigValue = std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    >;

    using ConfigMap = std::unordered_map<std::string, ConfigValue>;

    static bool parseKeyValue(const std::string& filepath, ConfigMap& config);
    static bool parseJSON(const std::string& filepath, ConfigMap& config);
    static bool parseYAML(const std::string& filepath, ConfigMap& config);
    static bool parse(const std::string& filepath, ConfigMap& config);
    
    static std::string serializeToKeyValue(const ConfigMap& config);
    static std::string serializeToJSON(const ConfigMap& config);
    static std::string serializeToYAML(const ConfigMap& config);
    
private:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValueFromStream(std::ifstream& file, ConfigMap& config);
    static bool parseJSONArray(const std::string& content, ConfigMap& config);
    static bool parseJSONObject(const std::string& content, ConfigMap& config);
};

} // namespace nlm
