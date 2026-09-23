#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <any>
#include <optional>
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace nlm {

class ConfigManager {
public:
    struct Parameter {
        std::string name;
        std::any value;
        std::string type; // "float", "int", "double", "bool", "string"
        std::string category; // "simulation", "neural", "plasticity", "memory", "development", "neuromodulation"
        std::string description;
        std::any minValue;
        std::any maxValue;
        std::any defaultValue;
        bool isActive = true; // For deprecated parameters
        
        Parameter() = default;
        Parameter(const std::string& name, const std::any& value, 
                  const std::string& type, const std::string& category,
                  const std::string& description, 
                  const std::any& min = std::any(), 
                  const std::any& max = std::any(),
                  const std::any& defaultVal = std::any())
            : name(name), value(value), type(type), category(category),
              description(description), minValue(min), maxValue(max),
              defaultValue(defaultVal) {}
        
        template<typename T>
        T getValue() const {
            return std::any_cast<T>(value);
        }
        
        template<typename T>
        void setValue(const T& val) {
            value = val;
        }
        
        bool isWithinBounds() const {
            if (!minValue.has_value() || !maxValue.has_value()) return true;
            
            if (type == "float" || type == "double") {
                double val = std::any_cast<double>(value);
                double min = std::any_cast<double>(minValue);
                double max = std::any_cast<double>(maxValue);
                return val >= min && val <= max;
            } else if (type == "int") {
                long val = std::any_cast<long>(value);
                long min = std::any_cast<long>(minValue);
                long max = std::any_cast<long>(maxValue);
                return val >= min && val <= max;
            }
            return true;
        }
    };
    
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Parameter management
    void addParameter(const Parameter& param);
    void removeParameter(const std::string& name);
    bool hasParameter(const std::string& name) const;
    
    template<typename T>
    void setParameter(const std::string& name, const T& value, 
                     const std::string& category = "general",
                     const std::string& description = "",
                     const std::any& min = std::any(),
                     const std::any& max = std::any(),
                     const std::any& defaultVal = std::any());
    
    template<typename T>
    std::optional<T> getParameter(const std::string& name) const;
    
    template<typename T>
    T getParameterOr(const std::string& name, const T& defaultValue) const;
    
    // Configuration validation
    bool validateParameter(const std::string& name) const;
    bool validateAllParameters() const;
    
    // Configuration ranges
    static Parameter createFloatParam(const std::string& name, float defaultValue,
                                     const std::string& category, 
                                     const std::string& description,
                                     float min = std::numeric_limits<float>::lowest(),
                                     float max = std::numeric_limits<float>::max());
    
    static Parameter createDoubleParam(const std::string& name, double defaultValue,
                                       const std::string& category,
                                       const std::string& description,
                                       double min = std::numeric_limits<double>::lowest(),
                                       double max = std::numeric_limits<double>::max());
    
    static Parameter createIntParam(const std::string& name, int defaultValue,
                                    const std::string& category,
                                    const std::string& description,
                                    int min = std::numeric_limits<int>::min(),
                                    int max = std::numeric_limits<int>::max());
    
    static Parameter createBoolParam(const std::string& name, bool defaultValue,
                                     const std::string& category,
                                     const std::string& description);
    
    // Parameter groups
    struct ParameterGroups {
        std::vector<Parameter> simulationParameters;
        std::vector<Parameter> neuralParameters;
        std::vector<Parameter> plasticityParameters;
        std::vector<Parameter> memoryParameters;
        std::vector<Parameter> developmentParameters;
        std::vector<Parameter> neuromodulationParameters;
        std::vector<Parameter> generalParameters;
        
        void addToGroup(const Parameter& param);
        const std::vector<Parameter>& getGroup(const std::string& category) const;
        std::string getAllGroupsSummary() const;
    };
    
    // Configuration file operations
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    bool loadFromCommandLine(int argc, char** argv);
    
    // Runtime configuration updates
    void enableParameter(const std::string& name);
    void disableParameter(const std::string& name);
    void resetParameter(const std::string& name);
    void resetAllParameters();
    
    // Configuration inspection
    std::vector<std::string> getAllParameterNames() const;
    std::vector<std::string> getParameterNamesByGroup(const std::string& category) const;
    std::string getParameterDescription(const std::string& name) const;
    std::string getParameterInfo(const std::string& name) const;
    std::string getConfigurationSummary() const;
    
    // Configuration ranges
    static std::map<std::string, std::pair<std::any, std::any>> getDefaultRanges();
    
    // Parameter aliases and descriptions
    static std::string getDefaultDescription(const std::string& name);
    
private:
    std::map<std::string, Parameter> parameters_;
    std::map<std::string, ParameterGroups> groupedParameters_;
    
    template<typename T>
    static std::string valueToString(const T& value);
    
    static ParameterGroups& getGroupByCategory(ParameterGroups& groups, const std::string& category);
    
    // Helper methods for creating parameter sets
    static void createSimulationParameters(ParameterGroups& groups);
    static void createNeuralParameters(ParameterGroups& groups);
    static void createPlasticityParameters(ParameterGroups& groups);
    static void createMemoryParameters(ParameterGroups& groups);
    static void createDevelopmentParameters(ParameterGroups& groups);
    static void createNeuromodulationParameters(ParameterGroups& groups);
    static void createGeneralParameters(ParameterGroups& groups);
};

// Global instance declaration
extern std::unique_ptr<ConfigManager> gConfigManager;

// Convenience functions
void initializeConfigManager();
ConfigManager& getConfigManager();
void shutdownConfigManager();

// Configuration parameter names - constants for easy reference
namespace Config {
    // Simulation parameters
    static constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    static constexpr const char* SIMULATION_STEPS = "simulation_steps";
    static constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    static constexpr const char* NEURON_COUNT = "neuron_count";
    static constexpr const char* REGION_COUNT = "region_count";
    static constexpr const char* MAX_SIMULATION_STEPS = "max_simulation_steps";
    
    // Neural parameters
    static constexpr const char* MEMBRANE_POTENTIAL_THRESHOLD = "membrane_potential_threshold";
    static constexpr const char* MEMBRANE_CAPACITANCE = "membrane_capacitance";
    static constexpr const char* RESTING_POTENTIAL = "resting_potential";
    static constexpr const char* RESET_POTENTIAL = "reset_potential";
    static constexpr const char* EXcitability = "excitability";
    static constexpr const char* INHIBITORY_STRENGTH = "inhibitory_strength";
    static constexpr const char* SYNAPTIC_GAIN = "synaptic_gain";
    
    // Plasticity parameters
    static constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    static constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    static constexpr const char* STDP_TIME_CONSTANT = "stdp_time_constant";
    static constexpr const char* HEBBIAN_LEARNING_RATE = "hebbian_learning_rate";
    static constexpr const char* HEBBIAN_MAX_WEIGHT = "hebbian_max_weight";
    static constexpr const char* STDP_DECAY_RATE = "stdp_decay_rate";
    
    // Structural plasticity parameters
    static constexpr const char* SYNAPTONGENESIS_RATE = "synaptogenesis_rate";
    static constexpr const char* PRUNING_RATE = "pruning_rate";
    static constexpr const char* MIN_SYNAPTIC_STRENGTH = "min_synaptic_strength";
    static constexpr const char* MAX_SYNAPTIC_STRENGTH = "max_synaptic_strength";
    
    // Memory parameters
    static constexpr const char* WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    static constexpr const char* EPISODIC_MEMORY_LIMIT = "episodic_memory_limit";
    static constexpr const char* CONSOLIDATION_INTERVAL = "consolidation_interval";
    static constexpr const char* WORKING_MEMORY_DECAY_RATE = "working_memory_decay_rate";
    static constexpr const char* EPISODIC_MEMORY_DECAY_RATE = "episodic_memory_decay_rate";
    
    // Development parameters
    static constexpr const char* DEVELOPMENTAL_STAGE_DURATION = "developmental_stage_duration";
    static constexpr const char* PLASTICITY_MODIFIER_RATE = "plasticity_modifier_rate";
    static constexpr const char* MATURATION_RATE = "maturation_rate";
    static constexpr const char* DEVELOPMENT_ENABLED = "development_enabled";
    
    // Neuromodulation parameters
    static constexpr const char* DOPAMINE_BASELINE = "dopamine_baseline";
    static constexpr const char* DOPAMINE_SCALE = "dopamine_scale";
    static constexpr const char* CURIOUSY_THRESHOLD = "curiosity_threshold";
    static constexpr const char* NOVELTY_THRESHOLD = "novelty_threshold";
    static constexpr const char* REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    
    // General parameters
    static constexpr const char* RANDOM_SEED = "random_seed";
    static constexpr const char* LOG_LEVEL = "log_level";
    static constexpr const char* LOG_TO_FILE = "log_to_file";
    static constexpr const char* VISUALIZATION_ENABLED = "visualization_enabled";
    
    // AgentBrain parameters
    static constexpr const char* DOPAMINE_LEVEL = "dopamine_level";
    static constexpr const char* NOVELTY_LEVEL = "novelty_level";
    static constexpr const char* CURIOSITY_LEVEL = "curiosity_level";
    static constexpr const char* PREDICTION_ERROR = "prediction_error";
    static constexpr const char* EXPECTED_REWARD = "expected_reward";
    static constexpr const char* PLASTICITY_MODIFIER = "plasticity_modifier";
    static constexpr const char* SENSORY_NOVELTY_DECAY = "sensory_novelty_decay";
    static constexpr const char* REWARD_MODULATION_ENABLED = "reward_modulation_enabled";
    static constexpr const char* STRUCTURAL_PLASTICITY_ENABLED = "structural_plasticity_enabled";
    static constexpr const char* DEVELOPMENT_ENABLED = "development_enabled";
    static constexpr const char* CURIOSITY_ENABLED = "curiosity_enabled";
}

} // namespace nlm
