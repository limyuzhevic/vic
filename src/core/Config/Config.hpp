#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// User level for configuration
enum class UserLevel {
    Beginner,
    Intermediate,
    Expert
};

// Configuration settings that vary by user level
struct LevelConfigSettings {
    // Beginner mode settings
    struct Beginner {
        bool enableLearningHelp = true;
        bool showGuidance = true;
        bool disableAdvancedPlacing = true;
        bool showTutorial = true;
        bool simpleMode = true;
        std::string helpTopic = "getting_started";
    } beginner;
    
    // Intermediate mode settings
    struct Intermediate {
        bool enableLearningHelp = true;
        bool showGuidance = true;
        bool disableAdvancedPlacing = false;
        bool showTutorial = false;
        bool simpleMode = false;
        bool advancedMode = true;
        std::string helpTopic = "intermediate";
    } intermediate;
    
    // Expert mode settings
    struct Expert {
        bool enableLearningHelp = false;
        bool showGuidance = false;
        bool disableAdvancedPlacing = false;
        bool showTutorial = false;
        bool simpleMode = false;
        bool advancedMode = true;
        bool debugMode = true;
        bool rawOutput = true;
        std::string helpTopic = "advanced";
    } expert;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// User level configuration for accessing settings by level
struct UserLevelConfigSettings {
    UserLevel userLevel;
    
    const LevelConfigSettings::Beginner& getBeginner() const { return levelSettings.beginner; }
    const LevelConfigSettings::Intermediate& getIntermediate() const { return levelSettings.intermediate; }
    const LevelConfigSettings::Expert& getExpert() const { return levelSettings.expert; }
    
    // Accessors for individual settings based on user level
    bool getShowGuidance() const {
        switch (userLevel) {
            case UserLevel::Beginner: return levelSettings.beginner.showGuidance;
            case UserLevel::Intermediate: return levelSettings.intermediate.showGuidance;
            case UserLevel::Expert: return levelSettings.expert.showGuidance;
            default: return false;
        }
    }
    
    bool getSimpleMode() const {
        switch (userLevel) {
            case UserLevel::Beginner: return levelSettings.beginner.simpleMode;
            case UserLevel::Intermediate: return levelSettings.intermediate.simpleMode;
            case UserLevel::Expert: return levelSettings.expert.simpleMode;
            default: return false;
        }
    }
    
    bool getAdvancedMode() const {
        switch (userLevel) {
            case UserLevel::Beginner: return levelSettings.beginner.simpleMode ? false : levelSettings.intermediate.advancedMode;
            case UserLevel::Intermediate: return levelSettings.intermediate.advancedMode;
            case UserLevel::Expert: return levelSettings.expert.advancedMode;
            default: return false;
        }
    }
    
    bool getEnableLearningHelp() const {
        switch (userLevel) {
            case UserLevel::Beginner: return levelSettings.beginner.enableLearningHelp;
            case UserLevel::Intermediate: return levelSettings.intermediate.enableLearningHelp;
            case UserLevel::Expert: return levelSettings.expert.enableLearningHelp;
            default: return false;
        }
    }
    
    bool getDebugMode() const {
        switch (userLevel) {
            case UserLevel::Beginner: return false;
            case UserLevel::Intermediate: return false;
            case UserLevel::Expert: return levelSettings.expert.debugMode;
            default: return false;
        }
    }
    
    bool getRawOutput() const {
        switch (userLevel) {
            case UserLevel::Beginner: return false;
            case UserLevel::Intermediate: return false;
            case UserLevel::Expert: return levelSettings.expert.rawOutput;
            default: return false;
        }
    }
    
    const std::string& getHelpTopic() const {
        switch (userLevel) {
            case UserLevel::Beginner: return levelSettings.beginner.helpTopic;
            case UserLevel::Intermediate: return levelSettings.intermediate.helpTopic;
            case UserLevel::Expert: return levelSettings.expert.helpTopic;
            default: static const std::string empty = ""; return empty;
        }
    }
    
private:
    LevelConfigSettings levelSettings;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // User level management
    void setUserLevel(UserLevel level);
    UserLevel getUserLevel() const;
    
    // User-level specific features
    bool isTutorialMode() const;
    bool isDebugMode() const;
    bool shouldShowGuidance() const;
    bool isAdvancedMode() const;
    
    // Help system
    void showHelp() const;
    void showTutorial(const std::string& topic = "") const;
    
    // Configuration by user level
    void setBeginnerMode(bool enabled = true);
    void setIntermediateMode(bool enabled = true);
    void setExpertMode(bool enabled = true);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
