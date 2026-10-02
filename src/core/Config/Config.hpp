#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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
    Runtime,
    Script  // For expert scripts
};

// Advanced configuration options for expert users
struct ExpertConfigOptions {
    // Performance tuning
    bool enableSIMD = false;           // Enable SIMD optimizations
    bool enableParallel = false;       // Enable parallel processing
    int threadCount = 0;               // Number of threads (0 = auto)
    
    // Memory management
    size_t memoryPoolSize = 0;         // Memory pool size in bytes (0 = auto)
    bool enableMemoryTracking = false; // Enable memory tracking
    
    // Simulation parameters
    double simulationTimestep = 0.001; // Default timestep
    bool enableTimeStepping = true;    // Enable adaptive time stepping
    
    // Learning parameters
    float defaultLTP = 0.01f;          // Default long-term potentiation
    float defaultLTD = 0.012f;         // Default long-term depression
    float dopamineBaseline = 0.05f;    // Baseline dopamine level
    
    // Debugging and analysis
    bool enableDebugLogging = false;   // Enable debug-level logging
    bool enableMetrics = false;        // Enable performance metrics
    std::string metricsOutputFile;     // File for metrics output
    
    // Neural network topology
    bool enableInterRegionConnections = true;  // Enable long-range connections
    float interRegionConnectionProb = 0.05f;   // Probability of inter-region connections
    
    // Plasticity controls
    bool enableStructuralPlasticity = true;    // Enable synapse creation/pruning
    float synaptogenesisRate = 0.0001f;        // Rate of new synapse formation
    float pruningRate = 0.00001f;             // Rate of synapse removal
    
    // Memory systems
    size_t workingMemoryCapacity = 0;    // Working memory capacity
    size_t episodicMemoryMaxEpisodes = 0; // Maximum episodes in episodic memory
    float memoryConsolidationRate = 0.3f; // Memory consolidation strength
    
    // Advanced neuromodulation
    bool enableAChModulation = false;   // Enable acetylcholine modulation
    bool enableNEModulation = false;    // Enable norepinephrine modulation
    bool enable5HTModulation = false;   // Enable serotonin modulation
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string expertNote;            // Expert-specific notes
    std::string validationRegex;       // Regex pattern for validation
    bool expertOnly = false;            // Hide from basic user interface
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description()
        , expertNote(), validationRegex(), expertOnly(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& note = "")
        : key(k), value(v), source(s), description(desc), expertNote(note), expertOnly(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc, const std::string& note, bool expert)
        : key(k), value(v), source(s), description(desc), expertNote(note), expertOnly(expert) {}
};

// Main configuration class with expert features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/TOML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from expert script file
    bool loadFromExpertScript(const std::string& scriptFile, const ExpertConfigOptions& expertOpts);
    
    // Save to file (pretty-printed for human readability)
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with enhanced metadata
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, 
             const std::string& description = "", const std::string& expertNote = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& expertNote = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& expertNote = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& expertNote = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& expertNote = "");
    
    // Validation and error handling
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Expert configuration access
    ExpertConfigOptions& getExpertOptions();
    const ExpertConfigOptions& getExpertOptions() const;
    void setExpertOptions(const ExpertConfigOptions& opts);
    
    // Check existence
    bool has(const std::string& key) const;
    bool isExpertOnly(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys with filtering
    std::vector<std::string> getKeys(bool includeExpertOnly = false) const;
    std::vector<std::string> getExpertKeys() const;
    
    // Configuration comparison and merging
    bool merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool isSubset(const Config& other) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with expert mode
    std::string summary(bool includeExpert = false, bool includeValidation = false) const;
    
    // Get configuration for specific mode
    std::string getUserFriendlySummary() const;
    std::string getExpertSummary() const;
    
    // Script execution for expert automation
    bool executeScript(const std::string& scriptContent, const ExpertConfigOptions& expertOpts);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesRegex(const std::string& value, const std::string& pattern);
    static ConfigValue parseValue(const std::string& str);
    
    // Expert configuration
    ExpertConfigOptions expertOptions_;
    
    // Validation errors
    mutable std::vector<std::string> validationErrors_;
    
    // Script execution for expert automation
    bool executeScriptImpl(const std::string& scriptContent, const ExpertConfigOptions& expertOpts);
};

} // namespace nlm

} // namespace nlm
