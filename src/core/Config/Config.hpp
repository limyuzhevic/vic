#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <expected>
#include <compare>
#include <concepts>
#include <type_traits>
#include <span>
#include <functional>
#include <string_view>
#include <algorithm>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept StringConvertible = std::is_convertible_v<T, std::string>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: Enum class for explicit control
enum class ConfigSource : uint8_t {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

// C++20: Enhanced configuration value type using std::variant
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

// Configuration entry with C++20 improvements
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // C++20: Three-way comparison for sorting
    auto operator<=>(const ConfigEntry&) const = default;
    bool operator==(const ConfigEntry&) const = default;
    
    // C++20: Default constructor with member initializer list
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // C++20: Helper methods for validation and conversion
    [[nodiscard]] bool isValid() const noexcept {
        return !key.empty();
    }
    
    [[nodiscard]] std::string getTypeName() const;
};

// Main configuration class with C++20 features
class Config {
public:
    Config();
    ~Config();
    
    // C++20: Delete copy, enable moving for efficiency
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // C++20: Factory methods for creating configurations
    static Result<std::unique_ptr<Config>> create();
    static Result<std::unique_ptr<Config>> loadFromFile(const std::string& filepath);
    static Result<std::unique_ptr<Config>> loadFromArgs(int argc, char** argv);
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // C++20: Enhanced get methods with better error handling
    template<typename T>
    Result<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // C++20: Enhanced validation and utility methods
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Get configuration summary with C++20 features
    std::string summary() const;
    
    // C++20: Observer pattern for configuration changes
    template<typename Observer>
    void addObserver(Observer&& observer) {
        observers_.push_back(std::forward<Observer>(observer));
    }
    
    void notifyObservers() {
        for (auto& observer : observers_) {
            observer(*this);
        }
    }
    
    // C++20: Visitor pattern for configuration operations
    template<typename Visitor>
    void visit(Visitor&& visitor) & {
        visitor.visitEntries(*this);
        visitor.visitValidation(*this);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Observers for configuration changes
    std::vector<std::function<void(const Config&)>> observers_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // C++20: Constexpr helper functions
    static constexpr bool isKeyValid(const std::string& key) noexcept {
        return !key.empty() && key.find('=') == std::string::npos;
    }
    
    static constexpr bool isValueConvertible(const ConfigValue& value, std::type_index typeIndex);
};

} // namespace nlm
