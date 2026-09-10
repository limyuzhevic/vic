#pragma once

#include <string>
#include <map>
#include <variant>
#include <optional>

namespace nlm {

// Define ConfigValue as a variant type supporting basic types
using ConfigValue = std::variant<std::string, int, double, bool>;

enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

struct ConfigEntry {
    std::string key;
    std::string description;
    ConfigValue value;
    ConfigSource source;
};

class Config {
public:
    Config();
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;

    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;

    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    void set(const std::string& key, const std::string& value, ConfigSource source);
    void set(const std::string& key, int value, ConfigSource source);
    void set(const std::string& key, double value, ConfigSource source);
    void set(const std::string& key, bool value, ConfigSource source);

    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;

private:
    struct Impl;
    Impl* pImpl;

    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
