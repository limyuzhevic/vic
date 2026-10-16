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
    Runtime
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
    
    const std::string& getKey() const { return key; }
    const ConfigValue& getValue() const { return value; }
    ConfigSource getSource() const { return source; }
    const std::string& getDescription() const { return description; }
    void setDescription(const std::string& desc) { description = desc; }
};

// Forward declaration for SpikeSystem (used in Brain::logStatus)
namespace SpikeSystem {
    class SpikeSystem;
}

// Forward declaration for PredictionSystem
namespace PredictionSystem {
    class PredictionSystem;
}

// Forward declaration for NeuralPlanner
namespace NeuralPlanner {
    class NeuralPlanner;
}

// Forward declaration for ConceptFormation
namespace ConceptFormation {
    class ConceptFormation;
}

// Forward declaration for AttentionalSelection
namespace AttentionalSelection {
    class AttentionalSelection;
}

// Forward declaration for DevelopmentSystem
namespace DevelopmentSystem {
    class DevelopmentSystem;
}

// Forward declaration for Dopamine
namespace Dopamine {
    class Dopamine;
}

// Forward declaration for Curiosity
namespace Curiosity {
    class Curiosity;
}

// Forward declaration for Novelty
namespace Novelty {
    class Novelty;
}

// Forward declaration for PredictionError
namespace PredictionError {
    class PredictionError;
}

// Forward declaration for NeuralWorkingMemory
namespace NeuralWorkingMemory {
    class NeuralWorkingMemory;
}

// Forward declaration for NeuralEpisodicMemory
namespace NeuralEpisodicMemory {
    class NeuralEpisodicMemory;
}

// Forward declaration for NeuralAssociativeMemory
namespace NeuralAssociativeMemory {
    class NeuralAssociativeMemory;
}

// Forward declaration for STDP
namespace STDP {
    class STDP;
}

// Forward declaration for Hebbian
namespace Hebbian {
    class Hebbian;
}

// Forward declaration for StructuralPlasticity
namespace StructuralPlasticity {
    class StructuralPlasticity;
}

// Basic types for API consistency
using SimulationStep = size_t;
using Timestamp = double;
using MembranePotential = double;
using Delay = double;
using SynapseId = size_t;
using NeuronId = size_t;
using RegionId = size_t;

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
    
    // Load from file (JSON/YAML format)
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
    
    // Enhanced methods
    void setDefaultValues();
    std::string getDescription() const;
    std::string getFilename() const;
    void setFilename(const std::string& filename);
    std::string getLastModified() const;
    
    void addMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    
    void addValidationRule(const std::string& key, const std::string& rule);
    bool validate() const;
    
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonStr);
    
    // Factory method
    static std::shared_ptr<Config> createDefaultConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::string getTimestamp() const;
    void updateLastModified();
};

// Configuration source enum helper
inline const char* toString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

// Configuration file format
enum class ConfigFormat {
    INI,
    JSON,
    YAML,
    XML
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error, const std::vector<std::string>& warnings = std::vector<std::string>())
        : valid(v), errorMessage(error), warnings(warnings) {}
};

} // namespace nlm