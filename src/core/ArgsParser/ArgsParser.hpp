#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "ArgsCategory.hpp"
#include "ArgsValidator.hpp"
#include "PresetManager.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class ArgsParser {
public:
    ArgsParser();
    ~ArgsParser();
    
    // Disable copying, enable moving
    ArgsParser(const ArgsParser&) = delete;
    ArgsParser& operator=(const ArgsParser&) = delete;
    ArgsParser(ArgsParser&&) noexcept;
    ArgsParser& operator=(ArgsParser&&) noexcept;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Parse configuration file
    bool parseConfigFile(const std::string& filepath);
    
    // Load configuration with auto-detection of format
    bool loadConfig(const std::string& source, bool isFile = true);
    
    // Get parsed value by key
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get parsed value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get help text
    std::string getHelp() const;
    
    // Get error messages
    std::vector<std::string> getErrors() const;
    
    // Check if parsing was successful
    bool isValid() const { return parseSuccess && errors.empty(); }
    
    // Get all parsed configuration
    const std::map<std::string, ArgsValue>& getConfig() const { return parsedConfig; }
    
    // Set custom validation
    void setValidator(std::shared_ptr<ArgsValidator> validator) { this->validator = validator; }
    
    // Set custom preset manager
    void setPresetManager(std::shared_ptr<PresetManager> presetManager) { 
        this->presetManager = presetManager; 
    }
    
    // Add preset programmatically
    void addPreset(const Preset& preset);
    
    // Register argument definitions
    void registerArgument(const ArgMetadata& arg);
    
    // Register preset factory
    void registerPresetFactory(const std::string& name,
                               const std::function<ArgsValue()>& factory);
    
    // Programmatically set configuration
    void set(const std::string& key, const ArgsValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Remove key from configuration
    void remove(const std::string& key);
    
    // Get configuration in external format
    std::shared_ptr<Config> getConfigObject() const;
    
    // Validate configuration
    bool validate(std::vector<std::string>& errors) const;
    
    // Apply preset and merge configurations
    bool applyPreset(const std::string& presetName);
    
    // Get available categories
    std::vector<std::string> getCategories() const;
    
    // Get arguments in a category
    std::vector<ArgMetadata> getArgumentsInCategory(ArgsCategory category) const;
    
    // Detect file format
    FileFormatInfo detectFileFormat(const std::string& filepath) const;
    
    // Generate command completion
    std::vector<std::string> generateCompletion(const std::string& prefix) const;
    
    // Log configuration
    void logConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    bool parseArgs(int argc, char** argv);
    bool parseFile(const std::string& filepath);
    void detectAndLoadConfig(const std::string& source);
    void applyDefaults();
    void updateConfigFromArgs(const std::vector<CmdArg>& cmdArgs);
    std::string formatHelp(ArgsCategory category) const;
    std::string formatArgument(const ArgMetadata& arg) const;
    ArgsValue parseValue(const std::string& str) const;
    bool detectFileFormatAndLoad(const std::string& filepath);
    void processCompletionRequest(const std::vector<CmdArg>& cmdArgs);
    void initializeDefaults();
    void setupPresets();
    void setupValidation();
    
    // Member variables
    std::map<std::string, ArgsValue> parsedConfig;
    std::vector<std::string> errors;
    bool parseSuccess;
    std::shared_ptr<ArgsValidator> validator;
    std::shared_ptr<PresetManager> presetManager;
    std::map<std::string, ArgMetadata> argumentRegistry;
    std::map<std::string, Preset> presets;
    std::map<std::string, std::function<ArgsValue()>> presetFactories;
    std::vector<ConfigSchema> configSchema;
    std::chrono::system_clock::time_point lastModified;
    std::string lastLoadedFile;
};

// File format detector
class FileFormatDetector {
public:
    static FileFormatInfo detectFormat(const std::string& filepath);
    
private:
    static bool tryDetectJSON(const std::string& filepath);
    static bool tryDetectYAML(const std::string& filepath);
    static bool tryDetectTOML(const std::string& filepath);
    static bool tryDetectINI(const std::string& filepath);
    static bool tryDetectProperties(const std::string& filepath);
    static bool tryDetectPlain(const std::string& filepath);
    
    static double calculateConfidence(bool detected, const std::string& format);
};

// Command line parser
class CmdLineParser {
public:
    static std::vector<CmdArg> parse(int argc, char** argv);
    static std::vector<CmdArg> parseFile(const std::string& filepath);
    static std::string extractOption(const std::string& arg, bool& hasValue);
    static std::string extractValue(const std::string& arg);
    static bool isOption(const std::string& arg);
    static bool hasLongOption(const std::string& arg);
    static bool hasShortOption(const std::string& arg);
    static std::string normalizeOption(const std::string& opt);
};

// Configuration summary formatter
class SummaryFormatter {
public:
    static std::string format(const std::map<std::string, ArgsValue>& config);
    static std::string formatCategory(const std::string& category,
                                     const std::map<std::string, ArgsValue>& config);
    static std::string formatEntry(const std::string& key, 
                                  const ArgsValue& value,
                                  const std::string& source = "");
};

// Help formatter
class HelpFormatter {
public:
    static std::string format(const std::map<std::string, ArgMetadata>& args,
                             const std::vector<std::string>& categories);
    static std::string formatCategoryHelp(const std::string& category,
                                         const std::map<std::string, ArgMetadata>& args);
    static std::string formatArgumentHelp(const ArgMetadata& arg);
};

// Argument value formatter
class ValueFormatter {
public:
    static std::string format(const ArgsValue& value);
    static std::string format(const ArgsValue& value, const std::string& type);
    static ArgsValue parse(const std::string& str, const std::string& type);
};