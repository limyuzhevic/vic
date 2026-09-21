#pragma once

#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

class ConfigBuilder {
public:
    ConfigBuilder& set(const std::string& key, const nlohmann::json& value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    ConfigBuilder& setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    ConfigBuilder& setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    ConfigBuilder& setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    ConfigBuilder& setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, value, source);
    }
    
    ConfigBuilder& addCallback(std::function<void(const std::string&, const ConfigValue&, ConfigSource)> callback) {
        callbacks.push_back(callback);
        return *this;
    }
    
    std::unique_ptr<Config> build() {
        auto config = std::make_unique<Config>();
        for (const auto& entry : entries) {
            config->set(entry.key, entry.value, entry.source);
        }
        config->setCallbacks(std::move(callbacks));
        return config;
    }
    
private:
    struct BuilderEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
    };
    
    std::vector<BuilderEntry> entries;
    std::vector<std::function<void(const std::string&, const ConfigValue&, ConfigSource)>> callbacks;
    
    ConfigBuilder& set(const std::string& key, const nlohmann::json& value, ConfigSource source) {
        ConfigValue configValue;
        if (value.is_string()) configValue = value.get<std::string>();
        else if (value.is_number_integer()) configValue = value.get<int>();
        else if (value.is_number_float()) configValue = value.get<double>();
        else if (value.is_boolean()) configValue = value.get<bool>();
        else if (value.is_array()) {
            // Convert JSON array to vector
            auto arr = value.get<std::vector<nlohmann::json>>();
            std::vector<int> intArray;
            bool allInt = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_number_integer(); });
            if (allInt) {
                intArray = arr.get<std::vector<int>>();
                configValue = intArray;
            } else {
                std::vector<double> doubleArray;
                bool allDouble = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_number_float(); });
                if (allDouble) {
                    doubleArray = arr.get<std::vector<double>>();
                    configValue = doubleArray;
                } else {
                    std::vector<std::string> stringArray;
                    bool allString = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_string(); });
                    if (allString) {
                        stringArray = arr.get<std::vector<std::string>>();
                        configValue = stringArray;
                    } else {
                        throw std::runtime_error("Unsupported JSON array type");
                    }
                }
            }
        }
        entries.push_back({key, configValue, source});
        return *this;
    }
    
    friend class Config;
};

} // namespace nlm