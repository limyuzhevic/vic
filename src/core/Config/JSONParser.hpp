#include "../core/Config/Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

class ConfigJSONParser {
public:
    static bool parse(const std::string& filepath, Config& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json j;
            file >> j;
            
            for (auto& [key, value] : j.items()) {
                if (value.is_string()) {
                    config.set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_number_integer()) {
                    config.set(key, value.get<int64_t>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    config.set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_number()) {
                    config.set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    config.set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // For arrays, convert to string representation
                    std::stringstream ss;
                    ss << value;
                    config.set(key, ss.str(), ConfigSource::File);
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            // Fall back to simple format if JSON parsing fails
            file.clear();
            file.seekg(0);
            return config.loadFromFile(filepath);
        }
    }
};

class ConfigYAMLParser {
public:
    static bool parse(const std::string& filepath, Config& config) {
        try {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                const YAML::Node& value = pair.second;
                
                if (value.IsScalar()) {
                    std::string strValue = value.as<std::string>();
                    config.set(key, strValue, ConfigSource::File);
                } else if (value.IsInteger()) {
                    config.set(key, value.as<int64_t>(), ConfigSource::File);
                } else if (value.IsFloat()) {
                    config.set(key, value.as<double>(), ConfigSource::File);
                } else if (value.IsBoolean()) {
                    config.set(key, value.as<bool>(), ConfigSource::File);
                } else if (value.IsSequence()) {
                    // Convert array to string representation
                    std::stringstream ss;
                    ss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << value[i].as<std::string>();
                    }
                    ss << "]";
                    config.set(key, ss.str(), ConfigSource::File);
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

} // namespace nlm
