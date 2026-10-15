#pragma once

#include "Validator.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace nlm {

enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Environment = 3,
    Runtime = 4
};

using ConfigValue = std::variant<int, int64_t, double, float, bool, std::string>;

class ConfigValidator; // Forward declaration

class Config {
public:
    struct ConfigEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
        std::string description;
    };

    Config();
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;

    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    bool saveToString(std::string& output) const;
    
    // New validation methods
    void setValidator(std::shared_ptr<ConfigValidator> validator) { validator_ = validator; }
    bool validate() const;
    std::string getValidationErrors() const;
    bool hasErrors() const;

    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    void set(const std::string& key, const std::string& value, ConfigSource source);
    void set(const std::string& key, int value, ConfigSource source);
    void set(const std::string& key, int64_t value, ConfigSource source);
    void set(const std::string& key, double value, ConfigSource source);
    void set(const std::string& key, float value, ConfigSource source);
    void set(const std::string& key, bool value, ConfigSource source);

    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();

    std::vector<ConfigEntry> getEntries() const;
    std::vector<std::string> getKeys() const;
    std::string summary() const;

    // Validator access for external registration
    static std::shared_ptr<ConfigValidator> getDefaultValidator();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigValidator> validator_;
};

} // namespace nlm

