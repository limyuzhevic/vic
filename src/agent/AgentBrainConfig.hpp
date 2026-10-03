#ifndef NLM_AGENT_BRAIN_CONFIG_HPP
#define NLM_AGENT_BRAIN_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace nlm {

class AgentBrainConfig {
public:
    // Default values for all configuration parameters
    struct Defaults {
        // Motor scaling factors
        static constexpr float MOTOR_FORWARD = 1.0f;
        static constexpr float MOTOR_BACKWARD = 1.0f;
        static constexpr float MOTOR_LEFT = 1.0f;
        static constexpr float MOTOR_RIGHT = 1.0f;
        static constexpr float MOTOR_INTERACT = 1.0f;
        
        // Sensory scaling factors
        static constexpr float SENSORY_VISION = 5.0f;
        static constexpr float SENSORY_TOUCH = 8.0f;
        static constexpr float SENSORY_INTERNAL = 5.0f;
        static constexpr float SENSORY_PROPRIOCEPTION = 3.0f;
        
        // Neuromodulation parameters
        static constexpr float DOPAMINE_SENSITIVITY = 1.0f;
        static constexpr float REWARD_PREDICTION_ERROR_SENSITIVITY = 1.0f;
        static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
        static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
        
        // Development parameters
        static constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
        static constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
        static constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
        static constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;
        static constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;
        static constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;
        static constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0;
        static constexpr double DEVELOPMENT_STAGE_ADULT = 1e6;
        static constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;
        static constexpr float PRUNING_RATE_BASE = 0.00001f;
        
        // Curiosity/Exploration parameters
        static constexpr float CURIOSITY_THRESHOLD = 0.3f;
        static constexpr float EXPLORATION_CHANCE_MAX = 0.3f;
        static constexpr float CURIOSITY_SENSITIVITY = 2.0f;
        static constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
        
        // Novelty detection parameters
        static constexpr float NOVELTY_DECAY_RATE = 0.99f;
        static constexpr float NOVELTY_SENSITIVITY = 1.0f;
        
        // Behavioral parameters
        static constexpr float ACTION_SELECTION_THRESHOLD = 0.5f;
        static constexpr float RANDOM_EXPLORATION_THRESHOLD = 0.5f;
        static constexpr int RANDOM_ACTION_COUNT = 8;
        static constexpr float MOTOR_NOISE_LEVEL = 0.0f;
        
        // Configuration flags
        static constexpr bool REWARD_MODULATION_ENABLED = true;
        static constexpr bool STRUCTURAL_PLASTICITY_ENABLED = true;
        static constexpr bool DEVELOPMENT_ENABLED = true;
        static constexpr bool CURIOSITY_ENABLED = true;
    };
    
    // Parameter identifiers for validation and parsing
    enum class ParameterID {
        // Motor parameters
        MOTOR_FORWARD_SCALE,
        MOTOR_BACKWARD_SCALE,
        MOTOR_LEFT_SCALE,
        MOTOR_RIGHT_SCALE,
        MOTOR_INTERACT_SCALE,
        
        // Sensory parameters
        SENSORY_VISION_SCALE,
        SENSORY_TOUCH_SCALE,
        SENSORY_INTERNAL_SCALE,
        SENSORY_PROPRIOCEPTION_SCALE,
        
        // Neuromodulation parameters
        DOPAMINE_SENSITIVITY,
        REWARD_PREDICTION_ERROR_SENSITIVITY,
        DOPAMINE_CLAMP_MIN,
        DOPAMINE_CLAMP_MAX,
        
        // Development parameters
        PLASTICITY_MODIFIER_INITIAL,
        PLASTICITY_MODIFIER_CRITICAL,
        PLASTICITY_MODIFIER_MATURATION,
        PLASTICITY_MODIFIER_ADULT,
        DEVELOPMENT_STAGE_INITIAL,
        DEVELOPMENT_STAGE_CRITICAL,
        DEVELOPMENT_STAGE_MATURATION,
        DEVELOPMENT_STAGE_ADULT,
        SYNAPTOGENESIS_RATE_BASE,
        PRUNING_RATE_BASE,
        
        // Curiosity/Exploration parameters
        CURIOSITY_THRESHOLD,
        EXPLORATION_CHANCE_MAX,
        CURIOSITY_SENSITIVITY,
        PREDICTION_ERROR_WEIGHT,
        
        // Novelty detection parameters
        NOVELTY_DECAY_RATE,
        NOVELTY_SENSITIVITY,
        
        // Behavioral parameters
        ACTION_SELECTION_THRESHOLD,
        RANDOM_EXPLORATION_THRESHOLD,
        RANDOM_ACTION_COUNT,
        MOTOR_NOISE_LEVEL,
        
        // Configuration flags
        REWARD_MODULATION_ENABLED,
        STRUCTURAL_PLASTICITY_ENABLED,
        DEVELOPMENT_ENABLED,
        CURIOSITY_ENABLED
    };
    
    AgentBrainConfig();
    ~AgentBrainConfig() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char* argv[]);
    
    // Get parameter value with type safety
    template<typename T>
    T getParameter(ParameterID id) const;
    
    // Set parameter value with validation
    template<typename T>
    bool setParameter(ParameterID id, T value);
    
    // Get raw parameter value as string
    std::string getParameterAsString(ParameterID id) const;
    
    // Set parameter from string
    template<typename T>
    bool setParameterFromString(ParameterID id, const std::string& value);
    
    // Check if parameter was explicitly set
    bool isParameterSet(ParameterID id) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Print current configuration
    void printConfiguration() const;
    
    // Reset to defaults
    void resetToDefaults();
    
private:
    // Helper methods for validation
    template<typename T>
    bool validateParameter(ParameterID id, T value) const;
    
    // Parameter storage
    std::unordered_map<ParameterID, std::string> stringParameters_;
    std::unordered_map<ParameterID, bool> setFlags_;
    
    // File path for configs
    static const std::string CONFIG_FILE_NAME;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_CONFIG_HPP