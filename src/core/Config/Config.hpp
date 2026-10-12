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

// Developmental configuration parameters
namespace nlm {

// Default developmental configuration
const std::vector<ConfigEntry> DefaultDevelopmentalConfig = {
    // Developmental stage timing thresholds (in simulation seconds)
    ConfigEntry("developmental_initial_duration", 60.0, ConfigSource::Default, "Duration of Initial developmental stage"),
    ConfigEntry("developmental_critical_period_duration", 300.0, ConfigSource::Default, "Duration of CriticalPeriod developmental stage"),
    ConfigEntry("developmental_maturation_duration", 600.0, ConfigSource::Default, "Duration of Maturation developmental stage"),
    
    // Plasticity modifiers for each developmental stage
    ConfigEntry("developmental_plasticity_initial", 1.0f, ConfigSource::Default, "Plasticity modifier for Initial stage"),
    ConfigEntry("developmental_plasticity_critical_period", 0.8f, ConfigSource::Default, "Plasticity modifier for CriticalPeriod stage"),
    ConfigEntry("developmental_plasticity_maturation", 0.5f, ConfigSource::Default, "Plasticity modifier for Maturation stage"),
    ConfigEntry("developmental_plasticity_adult", 0.2f, ConfigSource::Default, "Plasticity modifier for Adult stage"),
    ConfigEntry("developmental_plasticity_aging", 0.1f, ConfigSource::Default, "Plasticity modifier for Aging stage"),
    
    // Working memory parameters
    ConfigEntry("developmental_working_memory_capacity_initial", 0.5f, ConfigSource::Default, "Working memory capacity multiplier for Initial stage"),
    ConfigEntry("developmental_working_memory_capacity_critical_period", 0.8f, ConfigSource::Default, "Working memory capacity multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_working_memory_capacity_maturation", 1.2f, ConfigSource::Default, "Working memory capacity multiplier for Maturation stage"),
    ConfigEntry("developmental_working_memory_capacity_adult", 1.0f, ConfigSource::Default, "Working memory capacity multiplier for Adult stage"),
    ConfigEntry("developmental_working_memory_capacity_aging", 0.7f, ConfigSource::Default, "Working memory capacity multiplier for Aging stage"),
    
    ConfigEntry("developmental_working_memory_decay_initial", 2.0f, ConfigSource::Default, "Working memory decay multiplier for Initial stage"),
    ConfigEntry("developmental_working_memory_decay_critical_period", 1.5f, ConfigSource::Default, "Working memory decay multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_working_memory_decay_maturation", 0.8f, ConfigSource::Default, "Working memory decay multiplier for Maturation stage"),
    ConfigEntry("developmental_working_memory_decay_adult", 0.7f, ConfigSource::Default, "Working memory decay multiplier for Adult stage"),
    ConfigEntry("developmental_working_memory_decay_aging", 1.3f, ConfigSource::Default, "Working memory decay multiplier for Aging stage"),
    
    // Episodic memory parameters
    ConfigEntry("developmental_episodic_memory_capacity_initial", 0.3f, ConfigSource::Default, "Episodic memory capacity multiplier for Initial stage"),
    ConfigEntry("developmental_episodic_memory_capacity_critical_period", 1.5f, ConfigSource::Default, "Episodic memory capacity multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_episodic_memory_capacity_maturation", 1.2f, ConfigSource::Default, "Episodic memory capacity multiplier for Maturation stage"),
    ConfigEntry("developmental_episodic_memory_capacity_adult", 1.0f, ConfigSource::Default, "Episodic memory capacity multiplier for Adult stage"),
    ConfigEntry("developmental_episodic_memory_capacity_aging", 0.6f, ConfigSource::Default, "Episodic memory capacity multiplier for Aging stage"),
    
    // Planning parameters
    ConfigEntry("developmental_planning_depth_initial", 2.0f, ConfigSource::Default, "Planning depth multiplier for Initial stage"),
    ConfigEntry("developmental_planning_depth_critical_period", 5.0f, ConfigSource::Default, "Planning depth multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_planning_depth_maturation", 10.0f, ConfigSource::Default, "Planning depth multiplier for Maturation stage"),
    ConfigEntry("developmental_planning_depth_adult", 15.0f, ConfigSource::Default, "Planning depth multiplier for Adult stage"),
    ConfigEntry("developmental_planning_depth_aging", 0.6f, ConfigSource::Default, "Planning depth multiplier for Aging stage"),
    
    // Concept formation parameters
    ConfigEntry("developmental_concept_formation_threshold_initial", 0.9f, ConfigSource::Default, "Concept formation threshold multiplier for Initial stage"),
    ConfigEntry("developmental_concept_formation_threshold_critical_period", 0.7f, ConfigSource::Default, "Concept formation threshold multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_concept_formation_threshold_maturation", 0.5f, ConfigSource::Default, "Concept formation threshold multiplier for Maturation stage"),
    ConfigEntry("developmental_concept_formation_threshold_adult", 0.3f, ConfigSource::Default, "Concept formation threshold multiplier for Adult stage"),
    ConfigEntry("developmental_concept_formation_threshold_aging", 0.6f, ConfigSource::Default, "Concept formation threshold multiplier for Aging stage"),
    
    ConfigEntry("developmental_concept_stability_threshold_initial", 0.8f, ConfigSource::Default, "Concept stability threshold multiplier for Initial stage"),
    ConfigEntry("developmental_concept_stability_threshold_critical_period", 0.6f, ConfigSource::Default, "Concept stability threshold multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_concept_stability_threshold_maturation", 0.4f, ConfigSource::Default, "Concept stability threshold multiplier for Maturation stage"),
    ConfigEntry("developmental_concept_stability_threshold_adult", 0.2f, ConfigSource::Default, "Concept stability threshold multiplier for Adult stage"),
    ConfigEntry("developmental_concept_stability_threshold_aging", 0.5f, ConfigSource::Default, "Concept stability threshold multiplier for Aging stage"),
    
    // Attention parameters
    ConfigEntry("developmental_attention_inhibition_initial", 0.7f, ConfigSource::Default, "Attention inhibition multiplier for Initial stage"),
    ConfigEntry("developmental_attention_inhibition_critical_period", 0.5f, ConfigSource::Default, "Attention inhibition multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_attention_inhibition_maturation", 0.3f, ConfigSource::Default, "Attention inhibition multiplier for Maturation stage"),
    ConfigEntry("developmental_attention_inhibition_adult", 0.2f, ConfigSource::Default, "Attention inhibition multiplier for Adult stage"),
    ConfigEntry("developmental_attention_inhibition_aging", 0.4f, ConfigSource::Default, "Attention inhibition multiplier for Aging stage"),
    
    ConfigEntry("developmental_attention_excitation_initial", 1.0f, ConfigSource::Default, "Attention excitation multiplier for Initial stage"),
    ConfigEntry("developmental_attention_excitation_critical_period", 1.5f, ConfigSource::Default, "Attention excitation multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_attention_excitation_maturation", 2.0f, ConfigSource::Default, "Attention excitation multiplier for Maturation stage"),
    ConfigEntry("developmental_attention_excitation_adult", 2.2f, ConfigSource::Default, "Attention excitation multiplier for Adult stage"),
    ConfigEntry("developmental_attention_excitation_aging", 1.5f, ConfigSource::Default, "Attention excitation multiplier for Aging stage"),
    
    ConfigEntry("developmental_attention_competition_threshold_initial", 0.8f, ConfigSource::Default, "Attention competition threshold multiplier for Initial stage"),
    ConfigEntry("developmental_attention_competition_threshold_critical_period", 0.6f, ConfigSource::Default, "Attention competition threshold multiplier for CriticalPeriod stage"),
    ConfigEntry("developmental_attention_competition_threshold_maturation", 0.4f, ConfigSource::Default, "Attention competition threshold multiplier for Maturation stage"),
    ConfigEntry("developmental_attention_competition_threshold_adult", 0.3f, ConfigSource::Default, "Attention competition threshold multiplier for Adult stage"),
    ConfigEntry("developmental_attention_competition_threshold_aging", 0.5f, ConfigSource::Default, "Attention competition threshold multiplier for Aging stage"),
};

} // namespace nlm
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
