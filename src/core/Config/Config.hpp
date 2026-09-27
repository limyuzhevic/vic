#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

namespace nlm {

// Configuration source enumeration
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic
};

// ConfigValue variant type
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string
>;

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::string& key, const ConfigValue& value,
                ConfigSource source = ConfigSource::Default,
                const std::string& description = "")
        : key(key), value(value), source(source), description(description) {}
};

class Config {
public:
    Config();
    ~Config();
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;

    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;

    // Configuration access
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Configuration modification
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default);

    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
};

} // namespace nlm
