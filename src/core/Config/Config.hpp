#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

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
// Configuration fields for neuromodulation systems
struct NeuromodulationConfig {
    // Dopamine parameters
    float dopamine_baseline_level;
    float dopamine_phasic_strength;
    float dopamine_decay_rate;
    float dopamine_receptor_sensitivity;
    
    // Acetylcholine parameters
    float acetylcholine_baseline_level;
    float acetylcholine_phasic_strength;
    float acetylcholine_receptor_sensitivity;
    float acetylcholine_working_memory_influence;
    
    // Norepinephrine parameters
    float norepinephrine_baseline_level;
    float norepinephrine_phasic_strength;
    float norepinephrine_receptor_sensitivity;
    float norepinephrine_arousal_level;
    
    // Serotonin parameters
    float serotonin_baseline_level;
    float serotonin_phasic_strength;
    float serotonin_receptor_sensitivity;
    float serotonin_mood_inhibition;
    
    // Cross-neurotransmitter modulation
    float dopamine_acetylcholine_interaction;
    float norepinephrine_dopamine_interaction;
    float serotonin_neuropeptide_interaction;
    
    // Activity-dependent release parameters
    float neuromodulator_activity_sensitivity;
    float neuromodulator_history_window;
    
    NeuromodulationConfig() : 
        // Dopamine defaults
        dopamine_baseline_level(0.02f),
        dopamine_phasic_strength(1.0f),
        dopamine_decay_rate(0.05f),
        dopamine_receptor_sensitivity(0.9f),
        
        // Acetylcholine defaults
        acetylcholine_baseline_level(0.05f),
        acetylcholine_phasic_strength(0.8f),
        acetylcholine_receptor_sensitivity(0.8f),
        acetylcholine_working_memory_influence(0.7f),
        
        // Norepinephrine defaults
        norepinephrine_baseline_level(0.02f),
        norepinephrine_phasic_strength(0.6f),
        norepinephrine_receptor_sensitivity(0.85f),
        norepinephrine_arousal_level(0.5f),
        
        // Serotonin defaults
        serotonin_baseline_level(0.03f),
        serotonin_phasic_strength(0.4f),
        serotonin_receptor_sensitivity(0.8f),
        serotonin_mood_inhibition(0.5f),
        
        // Cross-modulation defaults
        dopamine_acetylcholine_interaction(1.0f),
        norepinephrine_dopamine_interaction(1.0f),
        serotonin_neuropeptide_interaction(1.0f),
        
        // Activity-dependent release defaults
        neuromodulator_activity_sensitivity(0.1f),
        neuromodulator_history_window(5.0f) {}

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
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJsonString() const;
    
    // JSON schema validation (optional)
    bool validateWithSchema(const std::string& schemaPath) const;
    
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
    
    // JSON conversion helpers
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    bool configValueFromJson(const nlohmann::json& j, ConfigValue& value);
    static nlohmann::json vectorToJson(const auto& vec);
    template<typename T>
    static std::vector<T> jsonToVector(const nlohmann::json& j);
};

} // namespace nlm

} // namespace nlm
