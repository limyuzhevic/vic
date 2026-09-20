#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <variant>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include "ConfigError.hpp"
#include "ConfigSchema.hpp"

namespace nlm {

// Configuration value types (matching ConfigValue)
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

class ConfigManager {
private:
    Schema schema;
    std::map<std::string, ConfigValue> configData;
    std::map<std::string, ConfigSource> configSources;
    bool initialized;
    
public:
    ConfigManager() : initialized(false) {}
    
    void initialize() {
        // Initialize with default values from schema
        const auto& parameters = schema.getAllParameters();
        for (const auto& param : parameters) {
            configData[param.key] = param.defaultValue;
            configSources[param.key] = ConfigSource::Default;
        }
        initialized = true;
    }
    
    void setSchema(const Schema& s) {
        schema = s;
        schema.lock();
        initialize();
    }
    
    const Schema& getSchema() const { return schema; }
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    std::string summary() const;
    bool validate() const;
    void printHelp(std::ostream& os = std::cout) const;
    
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
private:
    static std::string trim(const std::string& str);
    static ConfigValue stringToConfigValue(const std::string& str, ParamType type);
    static std::string configValueToString(const ConfigValue& value) const;
    static std::string paramTypeToString(ParamType type);
    static ParamType stringToParamType(const std::string& typeStr);
    
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm