#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {
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

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

class Config {
public:
    Config() {}
    ~Config() {}
    
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    bool loadFromFile(const std::string& filepath) { return false; }
    bool loadFromArgs(int argc, char** argv) { return false; }
    bool saveToFile(const std::string& filepath) const { return false; }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const { return std::nullopt; }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const { return defaultValue; }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {}
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {}
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {}
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {}
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {}
    
    bool has(const std::string& key) const { return false; }
    void remove(const std::string& key) {}
    
    std::vector<std::string> getKeys() const { return {}; }
    void clear() {}
    std::string summary() const { return ""; }
    
    std::vector<ConfigEntry> getAllEntries() const { return {}; }
    
    bool hasKey(const std::string& key) const { return false; }
    std::optional<ConfigValue> getRaw(const std::string& key) const { return std::nullopt; }
    void update(const ConfigEntry& entry) {}
    void removeSource(ConfigSource source) {}
    std::string toString(const ConfigValue& value) const { return ""; }
};
}

int main() {
    return 0;
}
