#include "AgentBrainConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>

namespace nlm {

const std::string AgentBrainConfig::CONFIG_FILE_NAME = "configs/agent.cfg";

AgentBrainConfig::AgentBrainConfig() {
    resetToDefaults();
}

bool AgentBrainConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARN("Configuration file not found: " + filename + ". Using defaults.");
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool inSection = false;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Remove whitespace from start and end
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        
        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            inSection = true;
            continue;
        }
        
        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            NLM_LOG_WARN("Invalid configuration line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Remove whitespace from key and value
        start = key.find_first_not_of(" \t");
        if (start != std::string::npos) {
            end = key.find_last_not_of(" \t");
            key = key.substr(start, end - start + 1);
        }
        
        start = value.find_first_not_of(" \t");
        if (start != std::string::npos) {
            end = value.find_last_not_of(" \t");
            value = value.substr(start, end - start + 1);
        }
        
        // Parse parameter based on key
        bool parsed = parseParameter(key, value);
        if (!parsed) {
            NLM_LOG_WARN("Failed to parse parameter: " + key + " = " + value);
        }
    }
    
    NLM_LOG_INFO("Configuration loaded from: " + filename);
    return true;
}

bool AgentBrainConfig::loadFromArgs(int argc, char* argv[]) {
    if (argc <= 1) return true; // No arguments to process
    
    bool configLoaded = false;
    std::string configFile = CONFIG_FILE_NAME;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
            configLoaded |= loadFromFile(configFile);
        } else if (arg == "--help" || arg == "-h") {
            printConfiguration();
            exit(0);
        } else if (arg.find('=') != std::string::npos) {
            // Parse key=value arguments
            size_t equalPos = arg.find('=');
            std::string key = arg.substr(0, equalPos);
            std::string value = arg.substr(equalPos + 1);
            
            bool parsed = parseParameter(key, value);
            if (!parsed) {
                NLM_LOG_WARN("Failed to parse command line argument: " + arg);
            }
        }
    }
    
    // Load default config file if specified
    if (!configLoaded && configFile != CONFIG_FILE_NAME) {
        loadFromFile(configFile);
    }
    
    return true;
}

bool AgentBrainConfig::parseParameter(const std::string& key, const std::string& value) {
    // Convert to lowercase for case-insensitive matching
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    
    try {
        // Try to parse as float first (most common type)
        float floatVal = std::stof(value);
        
        // Match parameter IDs
        if (lowerKey == "motor_forward_scale") {
            setParameter(ParameterID::MOTOR_FORWARD_SCALE, floatVal);
        } else if (lowerKey == "motor_backward_scale") {
            setParameter(ParameterID::MOTOR_BACKWARD_SCALE, floatVal);
        } else if (lowerKey == "motor_left_scale") {
            setParameter(ParameterID::MOTOR_LEFT_SCALE, floatVal);
        } else if (lowerKey == "motor_right_scale") {
            setParameter(ParameterID::MOTOR_RIGHT_SCALE, floatVal);
        } else if (lowerKey == "motor_interact_scale") {
            setParameter(ParameterID::MOTOR_INTERACT_SCALE, floatVal);
        } else if (lowerKey == "sensory_vision_scale") {
            setParameter(ParameterID::SENSORY_VISION_SCALE, floatVal);
        } else if (lowerKey == "sensory_touch_scale") {
            setParameter(ParameterID::SENSORY_TOUCH_SCALE, floatVal);
        } else if (lowerKey == "sensory_internal_scale") {
            setParameter(ParameterID::SENSORY_INTERNAL_SCALE, floatVal);
        } else if (lowerKey == "sensory_proprioception_scale") {
            setParameter(ParameterID::SENSORY_PROPRIOCEPTION_SCALE, floatVal);
        } else if (lowerKey == "dopamine_sensitivity") {
            setParameter(ParameterID::DOPAMINE_SENSITIVITY, floatVal);
        } else if (lowerKey == "reward_prediction_error_sensitivity") {
            setParameter(ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY, floatVal);
        } else if (lowerKey == "dopamine_clamp_min") {
            setParameter(ParameterID::DOPAMINE_CLAMP_MIN, floatVal);
        } else if (lowerKey == "dopamine_clamp_max") {
            setParameter(ParameterID::DOPAMINE_CLAMP_MAX, floatVal);
        } else if (lowerKey == "plasticity_modifier_initial") {
            setParameter(ParameterID::PLASTICITY_MODIFIER_INITIAL, floatVal);
        } else if (lowerKey == "plasticity_modifier_critical") {
            setParameter(ParameterID::PLASTICITY_MODIFIER_CRITICAL, floatVal);
        } else if (lowerKey == "plasticity_modifier_maturation") {
            setParameter(ParameterID::PLASTICITY_MODIFIER_MATURATION, floatVal);
        } else if (lowerKey == "plasticity_modifier_adult") {
            setParameter(ParameterID::PLASTICITY_MODIFIER_ADULT, floatVal);
        } else if (lowerKey == "development_stage_initial") {
            setParameter(ParameterID::DEVELOPMENT_STAGE_INITIAL, floatVal);
        } else if (lowerKey == "development_stage_critical") {
            setParameter(ParameterID::DEVELOPMENT_STAGE_CRITICAL, floatVal);
        } else if (lowerKey == "development_stage_maturation") {
            setParameter(ParameterID::DEVELOPMENT_STAGE_MATURATION, floatVal);
        } else if (lowerKey == "development_stage_adult") {
            setParameter(ParameterID::DEVELOPMENT_STAGE_ADULT, floatVal);
        } else if (lowerKey == "synaptogenesis_rate_base") {
            setParameter(ParameterID::SYNAPTOGENESIS_RATE_BASE, floatVal);
        } else if (lowerKey == "pruning_rate_base") {
            setParameter(ParameterID::PRUNING_RATE_BASE, floatVal);
        } else if (lowerKey == "curiosity_threshold") {
            setParameter(ParameterID::CURIOSITY_THRESHOLD, floatVal);
        } else if (lowerKey == "exploration_chance_max") {
            setParameter(ParameterID::EXPLORATION_CHANCE_MAX, floatVal);
        } else if (lowerKey == "curiosity_sensitivity") {
            setParameter(ParameterID::CURIOSITY_SENSITIVITY, floatVal);
        } else if (lowerKey == "prediction_error_weight") {
            setParameter(ParameterID::PREDICTION_ERROR_WEIGHT, floatVal);
        } else if (lowerKey == "novelty_decay_rate") {
            setParameter(ParameterID::NOVELTY_DECAY_RATE, floatVal);
        } else if (lowerKey == "novelty_sensitivity") {
            setParameter(ParameterID::NOVELTY_SENSITIVITY, floatVal);
        } else if (lowerKey == "action_selection_threshold") {
            setParameter(ParameterID::ACTION_SELECTION_THRESHOLD, floatVal);
        } else if (lowerKey == "random_exploration_threshold") {
            setParameter(ParameterID::RANDOM_EXPLORATION_THRESHOLD, floatVal);
        } else if (lowerKey == "random_action_count") {
            int intVal = std::stoi(value);
            setParameter(ParameterID::RANDOM_ACTION_COUNT, intVal);
        } else if (lowerKey == "motor_noise_level") {
            setParameter(ParameterID::MOTOR_NOISE_LEVEL, floatVal);
        } else if (lowerKey == "reward_modulation_enabled") {
            bool boolVal = (value == "true" || value == "1" || value == "yes");
            setParameter(ParameterID::REWARD_MODULATION_ENABLED, boolVal);
        } else if (lowerKey == "structural_plasticity_enabled") {
            bool boolVal = (value == "true" || value == "1" || value == "yes");
            setParameter(ParameterID::STRUCTURAL_PLASTICITY_ENABLED, boolVal);
        } else if (lowerKey == "development_enabled") {
            bool boolVal = (value == "true" || value == "1" || value == "yes");
            setParameter(ParameterID::DEVELOPMENT_ENABLED, boolVal);
        } else if (lowerKey == "curiosity_enabled") {
            bool boolVal = (value == "true" || value == "1" || value == "yes");
            setParameter(ParameterID::CURIOSITY_ENABLED, boolVal);
        } else {
            NLM_LOG_WARN("Unknown parameter key: " + key);
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to parse parameter " + key + ": " + e.what());
        return false;
    }
}

template<typename T>
T AgentBrainConfig::getParameter(ParameterID id) const {
    if (!isParameterSet(id)) {
        // Return default value
        switch (id) {
            case ParameterID::MOTOR_FORWARD_SCALE:
                return Defaults::MOTOR_FORWARD;
            case ParameterID::MOTOR_BACKWARD_SCALE:
                return Defaults::MOTOR_BACKWARD;
            case ParameterID::MOTOR_LEFT_SCALE:
                return Defaults::MOTOR_LEFT;
            case ParameterID::MOTOR_RIGHT_SCALE:
                return Defaults::MOTOR_RIGHT;
            case ParameterID::MOTOR_INTERACT_SCALE:
                return Defaults::MOTOR_INTERACT;
            case ParameterID::SENSORY_VISION_SCALE:
                return Defaults::SENSORY_VISION;
            case ParameterID::SENSORY_TOUCH_SCALE:
                return Defaults::SENSORY_TOUCH;
            case ParameterID::SENSORY_INTERNAL_SCALE:
                return Defaults::SENSORY_INTERNAL;
            case ParameterID::SENSORY_PROPRIOCEPTION_SCALE:
                return Defaults::SENSORY_PROPRIOCEPTION;
            case ParameterID::DOPAMINE_SENSITIVITY:
                return Defaults::DOPAMINE_SENSITIVITY;
            case ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY:
                return Defaults::REWARD_PREDICTION_ERROR_SENSITIVITY;
            case ParameterID::DOPAMINE_CLAMP_MIN:
                return Defaults::DOPAMINE_CLAMP_MIN;
            case ParameterID::DOPAMINE_CLAMP_MAX:
                return Defaults::DOPAMINE_CLAMP_MAX;
            case ParameterID::PLASTICITY_MODIFIER_INITIAL:
                return Defaults::PLASTICITY_MODIFIER_INITIAL;
            case ParameterID::PLASTICITY_MODIFIER_CRITICAL:
                return Defaults::PLASTICITY_MODIFIER_CRITICAL;
            case ParameterID::PLASTICITY_MODIFIER_MATURATION:
                return Defaults::PLASTICITY_MODIFIER_MATURATION;
            case ParameterID::PLASTICITY_MODIFIER_ADULT:
                return Defaults::PLASTICITY_MODIFIER_ADULT;
            case ParameterID::DEVELOPMENT_STAGE_INITIAL:
                return static_cast<T>(Defaults::DEVELOPMENT_STAGE_INITIAL);
            case ParameterID::DEVELOPMENT_STAGE_CRITICAL:
                return static_cast<T>(Defaults::DEVELOPMENT_STAGE_CRITICAL);
            case ParameterID::DEVELOPMENT_STAGE_MATURATION:
                return static_cast<T>(Defaults::DEVELOPMENT_STAGE_MATURATION);
            case ParameterID::DEVELOPMENT_STAGE_ADULT:
                return static_cast<T>(Defaults::DEVELOPMENT_STAGE_ADULT);
            case ParameterID::SYNAPTOGENESIS_RATE_BASE:
                return Defaults::SYNAPTOGENESIS_RATE_BASE;
            case ParameterID::PRUNING_RATE_BASE:
                return Defaults::PRUNING_RATE_BASE;
            case ParameterID::CURIOSITY_THRESHOLD:
                return Defaults::CURIOSITY_THRESHOLD;
            case ParameterID::EXPLORATION_CHANCE_MAX:
                return Defaults::EXPLORATION_CHANCE_MAX;
            case ParameterID::CURIOSITY_SENSITIVITY:
                return Defaults::CURIOSITY_SENSITIVITY;
            case ParameterID::PREDICTION_ERROR_WEIGHT:
                return Defaults::PREDICTION_ERROR_WEIGHT;
            case ParameterID::NOVELTY_DECAY_RATE:
                return Defaults::NOVELTY_DECAY_RATE;
            case ParameterID::NOVELTY_SENSITIVITY:
                return Defaults::NOVELTY_SENSITIVITY;
            case ParameterID::ACTION_SELECTION_THRESHOLD:
                return Defaults::ACTION_SELECTION_THRESHOLD;
            case ParameterID::RANDOM_EXPLORATION_THRESHOLD:
                return Defaults::RANDOM_EXPLORATION_THRESHOLD;
            case ParameterID::RANDOM_ACTION_COUNT:
                return static_cast<T>(Defaults::RANDOM_ACTION_COUNT);
            case ParameterID::MOTOR_NOISE_LEVEL:
                return Defaults::MOTOR_NOISE_LEVEL;
            case ParameterID::REWARD_MODULATION_ENABLED:
                return static_cast<T>(Defaults::REWARD_MODULATION_ENABLED);
            case ParameterID::STRUCTURAL_PLASTICITY_ENABLED:
                return static_cast<T>(Defaults::STRUCTURAL_PLASTICITY_ENABLED);
            case ParameterID::DEVELOPMENT_ENABLED:
                return static_cast<T>(Defaults::DEVELOPMENT_ENABLED);
            case ParameterID::CURIOSITY_ENABLED:
                return static_cast<T>(Defaults::CURIOSITY_ENABLED);
            default:
                throw std::runtime_error("Unknown parameter ID");
        }
    }
    
    // This should never be reached since we store as string
    // For simplicity, we'll assume proper type safety
    T defaultValue = getParameter<T>(id);
    NLM_LOG_WARN("Parameter not properly stored, returning default for ID " + std::to_string(static_cast<int>(id)));
    return defaultValue;
}

template<typename T>
bool AgentBrainConfig::setParameter(ParameterID id, T value) {
    if (!validateParameter(id, value)) {
        NLM_LOG_WARN("Invalid parameter value for ID " + std::to_string(static_cast<int>(id)));
        return false;
    }
    
    setFlags_[id] = true;
    
    // Convert to string for storage
    std::ostringstream oss;
    oss << value;
    stringParameters_[id] = oss.str();
    
    return true;
}

std::string AgentBrainConfig::getParameterAsString(ParameterID id) const {
    auto it = stringParameters_.find(id);
    if (it == stringParameters_.end()) {
        return "";
    }
    return it->second;
}

template<typename T>
bool AgentBrainConfig::setParameterFromString(ParameterID id, const std::string& value) {
    try {
        T val;
        std::istringstream iss(value);
        iss >> val;
        
        if (iss.fail()) {
            return false;
        }
        
        return setParameter(id, val);
    } catch (...) {
        return false;
    }
}

bool AgentBrainConfig::isParameterSet(ParameterID id) const {
    return setFlags_.find(id) != setFlags_.end();
}

std::vector<std::string> AgentBrainConfig::getParameterNames() const {
    std::vector<std::string> names;
    
    const std::unordered_map<ParameterID, std::string> parameterNames = {
        {ParameterID::MOTOR_FORWARD_SCALE, "motor_forward_scale"},
        {ParameterID::MOTOR_BACKWARD_SCALE, "motor_backward_scale"},
        {ParameterID::MOTOR_LEFT_SCALE, "motor_left_scale"},
        {ParameterID::MOTOR_RIGHT_SCALE, "motor_right_scale"},
        {ParameterID::MOTOR_INTERACT_SCALE, "motor_interact_scale"},
        {ParameterID::SENSORY_VISION_SCALE, "sensory_vision_scale"},
        {ParameterID::SENSORY_TOUCH_SCALE, "sensory_touch_scale"},
        {ParameterID::SENSORY_INTERNAL_SCALE, "sensory_internal_scale"},
        {ParameterID::SENSORY_PROPRIOCEPTION_SCALE, "sensory_proprioception_scale"},
        {ParameterID::DOPAMINE_SENSITIVITY, "dopamine_sensitivity"},
        {ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY, "reward_prediction_error_sensitivity"},
        {ParameterID::DOPAMINE_CLAMP_MIN, "dopamine_clamp_min"},
        {ParameterID::DOPAMINE_CLAMP_MAX, "dopamine_clamp_max"},
        {ParameterID::PLASTICITY_MODIFIER_INITIAL, "plasticity_modifier_initial"},
        {ParameterID::PLASTICITY_MODIFIER_CRITICAL, "plasticity_modifier_critical"},
        {ParameterID::PLASTICITY_MODIFIER_MATURATION, "plasticity_modifier_maturation"},
        {ParameterID::PLASTICITY_MODIFIER_ADULT, "plasticity_modifier_adult"},
        {ParameterID::DEVELOPMENT_STAGE_INITIAL, "development_stage_initial"},
        {ParameterID::DEVELOPMENT_STAGE_CRITICAL, "development_stage_critical"},
        {ParameterID::DEVELOPMENT_STAGE_MATURATION, "development_stage_maturation"},
        {ParameterID::DEVELOPMENT_STAGE_ADULT, "development_stage_adult"},
        {ParameterID::SYNAPTOGENESIS_RATE_BASE, "synaptogenesis_rate_base"},
        {ParameterID::PRUNING_RATE_BASE, "pruning_rate_base"},
        {ParameterID::CURIOSITY_THRESHOLD, "curiosity_threshold"},
        {ParameterID::EXPLORATION_CHANCE_MAX, "exploration_chance_max"},
        {ParameterID::CURIOSITY_SENSITIVITY, "curiosity_sensitivity"},
        {ParameterID::PREDICTION_ERROR_WEIGHT, "prediction_error_weight"},
        {ParameterID::NOVELTY_DECAY_RATE, "novelty_decay_rate"},
        {ParameterID::NOVELTY_SENSITIVITY, "novelty_sensitivity"},
        {ParameterID::ACTION_SELECTION_THRESHOLD, "action_selection_threshold"},
        {ParameterID::RANDOM_EXPLORATION_THRESHOLD, "random_exploration_threshold"},
        {ParameterID::RANDOM_ACTION_COUNT, "random_action_count"},
        {ParameterID::MOTOR_NOISE_LEVEL, "motor_noise_level"},
        {ParameterID::REWARD_MODULATION_ENABLED, "reward_modulation_enabled"},
        {ParameterID::STRUCTURAL_PLASTICITY_ENABLED, "structural_plasticity_enabled"},
        {ParameterID::DEVELOPMENT_ENABLED, "development_enabled"},
        {ParameterID::CURIOSITY_ENABLED, "curiosity_enabled"}
    };
    
    for (const auto& pair : parameterNames) {
        names.push_back(pair.second);
    }
    
    return names;
}

void AgentBrainConfig::printConfiguration() const {
    NLM_LOG_INFO("=== AgentBrain Configuration ===");
    
    NLM_LOG_INFO("Motor Scaling Factors:");
    NLM_LOG_INFO("  Forward: " + std::to_string(getParameter<float>(ParameterID::MOTOR_FORWARD_SCALE)));
    NLM_LOG_INFO("  Backward: " + std::to_string(getParameter<float>(ParameterID::MOTOR_BACKWARD_SCALE)));
    NLM_LOG_INFO("  Left: " + std::to_string(getParameter<float>(ParameterID::MOTOR_LEFT_SCALE)));
    NLM_LOG_INFO("  Right: " + std::to_string(getParameter<float>(ParameterID::MOTOR_RIGHT_SCALE)));
    NLM_LOG_INFO("  Interact: " + std::to_string(getParameter<float>(ParameterID::MOTOR_INTERACT_SCALE)));
    
    NLM_LOG_INFO("Sensory Scaling Factors:");
    NLM_LOG_INFO("  Vision: " + std::to_string(getParameter<float>(ParameterID::SENSORY_VISION_SCALE)));
    NLM_LOG_INFO("  Touch: " + std::to_string(getParameter<float>(ParameterID::SENSORY_TOUCH_SCALE)));
    NLM_LOG_INFO("  Internal: " + std::to_string(getParameter<float>(ParameterID::SENSORY_INTERNAL_SCALE)));
    NLM_LOG_INFO("  Proprioception: " + std::to_string(getParameter<float>(ParameterID::SENSORY_PROPRIOCEPTION_SCALE)));
    
    NLM_LOG_INFO("Neuromodulation Parameters:");
    NLM_LOG_INFO("  Dopamine Sensitivity: " + std::to_string(getParameter<float>(ParameterID::DOPAMINE_SENSITIVITY)));
    NLM_LOG_INFO("  Reward Prediction Error Sensitivity: " + std::to_string(getParameter<float>(ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY)));
    NLM_LOG_INFO("  Dopamine Clamp Min: " + std::to_string(getParameter<float>(ParameterID::DOPAMINE_CLAMP_MIN)));
    NLM_LOG_INFO("  Dopamine Clamp Max: " + std::to_string(getParameter<float>(ParameterID::DOPAMINE_CLAMP_MAX)));
    
    NLM_LOG_INFO("Development Parameters:");
    NLM_LOG_INFO("  Plasticity Modifier Initial: " + std::to_string(getParameter<float>(ParameterID::PLASTICITY_MODIFIER_INITIAL)));
    NLM_LOG_INFO("  Plasticity Modifier Critical: " + std::to_string(getParameter<float>(ParameterID::PLASTICITY_MODIFIER_CRITICAL)));
    NLM_LOG_INFO("  Plasticity Modifier Maturation: " + std::to_string(getParameter<float>(ParameterID::PLASTICITY_MODIFIER_MATURATION)));
    NLM_LOG_INFO("  Plasticity Modifier Adult: " + std::to_string(getParameter<float>(ParameterID::PLASTICITY_MODIFIER_ADULT)));
    NLM_LOG_INFO("  Development Stage Initial: " + std::to_string(getParameter<double>(ParameterID::DEVELOPMENT_STAGE_INITIAL)));
    NLM_LOG_INFO("  Development Stage Critical: " + std::to_string(getParameter<double>(ParameterID::DEVELOPMENT_STAGE_CRITICAL)));
    NLM_LOG_INFO("  Development Stage Maturation: " + std::to_string(getParameter<double>(ParameterID::DEVELOPMENT_STAGE_MATURATION)));
    NLM_LOG_INFO("  Development Stage Adult: " + std::to_string(getParameter<double>(ParameterID::DEVELOPMENT_STAGE_ADULT)));
    
    NLM_LOG_INFO("Curiosity/Exploration Parameters:");
    NLM_LOG_INFO("  Curiosity Threshold: " + std::to_string(getParameter<float>(ParameterID::CURIOSITY_THRESHOLD)));
    NLM_LOG_INFO("  Exploration Chance Max: " + std::to_string(getParameter<float>(ParameterID::EXPLORATION_CHANCE_MAX)));
    NLM_LOG_INFO("  Curiosity Sensitivity: " + std::to_string(getParameter<float>(ParameterID::CURIOSITY_SENSITIVITY)));
    NLM_LOG_INFO("  Prediction Error Weight: " + std::to_string(getParameter<float>(ParameterID::PREDICTION_ERROR_WEIGHT)));
    
    NLM_LOG_INFO("Novelty Detection Parameters:");
    NLM_LOG_INFO("  Novelty Decay Rate: " + std::to_string(getParameter<float>(ParameterID::NOVELTY_DECAY_RATE)));
    NLM_LOG_INFO("  Novelty Sensitivity: " + std::to_string(getParameter<float>(ParameterID::NOVELTY_SENSITIVITY)));
    
    NLM_LOG_INFO("Behavioral Parameters:");
    NLM_LOG_INFO("  Action Selection Threshold: " + std::to_string(getParameter<float>(ParameterID::ACTION_SELECTION_THRESHOLD)));
    NLM_LOG_INFO("  Random Exploration Threshold: " + std::to_string(getParameter<float>(ParameterID::RANDOM_EXPLORATION_THRESHOLD)));
    NLM_LOG_INFO("  Random Action Count: " + std::to_string(getParameter<int>(ParameterID::RANDOM_ACTION_COUNT)));
    NLM_LOG_INFO("  Motor Noise Level: " + std::to_string(getParameter<float>(ParameterID::MOTOR_NOISE_LEVEL)));
    
    NLM_LOG_INFO("Configuration Flags:");
    NLM_LOG_INFO("  Reward Modulation Enabled: " + std::to_string(getParameter<bool>(ParameterID::REWARD_MODULATION_ENABLED)));
    NLM_LOG_INFO("  Structural Plasticity Enabled: " + std::to_string(getParameter<bool>(ParameterID::STRUCTURAL_PLASTICITY_ENABLED)));
    NLM_LOG_INFO("  Development Enabled: " + std::to_string(getParameter<bool>(ParameterID::DEVELOPMENT_ENABLED)));
    NLM_LOG_INFO("  Curiosity Enabled: " + std::to_string(getParameter<bool>(ParameterID::CURIOSITY_ENABLED)));
    
    NLM_LOG_INFO("=== End Configuration ===");
}

template<typename T>
bool AgentBrainConfig::validateParameter(ParameterID id, T value) const {
    switch (id) {
        case ParameterID::MOTOR_FORWARD_SCALE:
        case ParameterID::MOTOR_BACKWARD_SCALE:
        case ParameterID::MOTOR_LEFT_SCALE:
        case ParameterID::MOTOR_RIGHT_SCALE:
        case ParameterID::MOTOR_INTERACT_SCALE:
            return value >= 0.0f && value <= 10.0f;
            
        case ParameterID::SENSORY_VISION_SCALE:
        case ParameterID::SENSORY_TOUCH_SCALE:
        case ParameterID::SENSORY_INTERNAL_SCALE:
        case ParameterID::SENSORY_PROPRIOCEPTION_SCALE:
            return value >= 0.0f && value <= 20.0f;
            
        case ParameterID::DOPAMINE_SENSITIVITY:
            return value > 0.0f && value <= 10.0f;
            
        case ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY:
            return value > 0.0f && value <= 5.0f;
            
        case ParameterID::DOPAMINE_CLAMP_MIN:
            return value >= -10.0f && value < 0.0f;
            
        case ParameterID::DOPAMINE_CLAMP_MAX:
            return value > 0.0f && value <= 10.0f;
            
        case ParameterID::PLASTICITY_MODIFIER_INITIAL:
        case ParameterID::PLASTICITY_MODIFIER_CRITICAL:
        case ParameterID::PLASTICITY_MODIFIER_MATURATION:
        case ParameterID::PLASTICITY_MODIFIER_ADULT:
            return value >= 0.0f && value <= 5.0f;
            
        case ParameterID::DEVELOPMENT_STAGE_INITIAL:
        case ParameterID::DEVELOPMENT_STAGE_CRITICAL:
        case ParameterID::DEVELOPMENT_STAGE_MATURATION:
        case ParameterID::DEVELOPMENT_STAGE_ADULT:
            return static_cast<double>(value) > 0.0 && static_cast<double>(value) < 1e6;
            
        case ParameterID::SYNAPTOGENESIS_RATE_BASE:
        case ParameterID::PRUNING_RATE_BASE:
            return value >= 0.0f && value <= 0.1f;
            
        case ParameterID::CURIOSITY_THRESHOLD:
            return value >= 0.0f && value <= 1.0f;
            
        case ParameterID::EXPLORATION_CHANCE_MAX:
            return value >= 0.0f && value <= 1.0f;
            
        case ParameterID::CURIOSITY_SENSITIVITY:
            return value >= 0.0f && value <= 5.0f;
            
        case ParameterID::PREDICTION_ERROR_WEIGHT:
            return value >= 0.0f && value <= 5.0f;
            
        case ParameterID::NOVELTY_DECAY_RATE:
            return value > 0.0f && value <= 1.0f;
            
        case ParameterID::NOVELTY_SENSITIVITY:
            return value >= 0.0f && value <= 5.0f;
            
        case ParameterID::ACTION_SELECTION_THRESHOLD:
            return value >= 0.0f && value <= 1.0f;
            
        case ParameterID::RANDOM_EXPLORATION_THRESHOLD:
            return value >= 0.0f && value <= 1.0f;
            
        case ParameterID::RANDOM_ACTION_COUNT:
            return value >= 0 && value <= 100;
            
        case ParameterID::MOTOR_NOISE_LEVEL:
            return value >= 0.0f && value <= 1.0f;
            
        case ParameterID::REWARD_MODULATION_ENABLED:
        case ParameterID::STRUCTURAL_PLASTICITY_ENABLED:
        case ParameterID::DEVELOPMENT_ENABLED:
        case ParameterID::CURIOSITY_ENABLED:
            return true; // Booleans are always valid
            
        default:
            return false;
    }
}

void AgentBrainConfig::resetToDefaults() {
    stringParameters_.clear();
    setFlags_.clear();
    
    // Initialize all parameters with defaults
    setParameter(ParameterID::MOTOR_FORWARD_SCALE, Defaults::MOTOR_FORWARD);
    setParameter(ParameterID::MOTOR_BACKWARD_SCALE, Defaults::MOTOR_BACKWARD);
    setParameter(ParameterID::MOTOR_LEFT_SCALE, Defaults::MOTOR_LEFT);
    setParameter(ParameterID::MOTOR_RIGHT_SCALE, Defaults::MOTOR_RIGHT);
    setParameter(ParameterID::MOTOR_INTERACT_SCALE, Defaults::MOTOR_INTERACT);
    setParameter(ParameterID::SENSORY_VISION_SCALE, Defaults::SENSORY_VISION);
    setParameter(ParameterID::SENSORY_TOUCH_SCALE, Defaults::SENSORY_TOUCH);
    setParameter(ParameterID::SENSORY_INTERNAL_SCALE, Defaults::SENSORY_INTERNAL);
    setParameter(ParameterID::SENSORY_PROPRIOCEPTION_SCALE, Defaults::SENSORY_PROPRIOCEPTION);
    setParameter(ParameterID::DOPAMINE_SENSITIVITY, Defaults::DOPAMINE_SENSITIVITY);
    setParameter(ParameterID::REWARD_PREDICTION_ERROR_SENSITIVITY, Defaults::REWARD_PREDICTION_ERROR_SENSITIVITY);
    setParameter(ParameterID::DOPAMINE_CLAMP_MIN, Defaults::DOPAMINE_CLAMP_MIN);
    setParameter(ParameterID::DOPAMINE_CLAMP_MAX, Defaults::DOPAMINE_CLAMP_MAX);
    setParameter(ParameterID::PLASTICITY_MODIFIER_INITIAL, Defaults::PLASTICITY_MODIFIER_INITIAL);
    setParameter(ParameterID::PLASTICITY_MODIFIER_CRITICAL, Defaults::PLASTICITY_MODIFIER_CRITICAL);
    setParameter(ParameterID::PLASTICITY_MODIFIER_MATURATION, Defaults::PLASTICITY_MODIFIER_MATURATION);
    setParameter(ParameterID::PLASTICITY_MODIFIER_ADULT, Defaults::PLASTICITY_MODIFIER_ADULT);
    setParameter(ParameterID::DEVELOPMENT_STAGE_INITIAL, Defaults::DEVELOPMENT_STAGE_INITIAL);
    setParameter(ParameterID::DEVELOPMENT_STAGE_CRITICAL, Defaults::DEVELOPMENT_STAGE_CRITICAL);
    setParameter(ParameterID::DEVELOPMENT_STAGE_MATURATION, Defaults::DEVELOPMENT_STAGE_MATURATION);
    setParameter(ParameterID::DEVELOPMENT_STAGE_ADULT, Defaults::DEVELOPMENT_STAGE_ADULT);
    setParameter(ParameterID::SYNAPTOGENESIS_RATE_BASE, Defaults::SYNAPTOGENESIS_RATE_BASE);
    setParameter(ParameterID::PRUNING_RATE_BASE, Defaults::PRUNING_RATE_BASE);
    setParameter(ParameterID::CURIOSITY_THRESHOLD, Defaults::CURIOSITY_THRESHOLD);
    setParameter(ParameterID::EXPLORATION_CHANCE_MAX, Defaults::EXPLORATION_CHANCE_MAX);
    setParameter(ParameterID::CURIOSITY_SENSITIVITY, Defaults::CURIOSITY_SENSITIVITY);
    setParameter(ParameterID::PREDICTION_ERROR_WEIGHT, Defaults::PREDICTION_ERROR_WEIGHT);
    setParameter(ParameterID::NOVELTY_DECAY_RATE, Defaults::NOVELTY_DECAY_RATE);
    setParameter(ParameterID::NOVELTY_SENSITIVITY, Defaults::NOVELTY_SENSITIVITY);
    setParameter(ParameterID::ACTION_SELECTION_THRESHOLD, Defaults::ACTION_SELECTION_THRESHOLD);
    setParameter(ParameterID::RANDOM_EXPLORATION_THRESHOLD, Defaults::RANDOM_EXPLORATION_THRESHOLD);
    setParameter(ParameterID::RANDOM_ACTION_COUNT, static_cast<int>(Defaults::RANDOM_ACTION_COUNT));
    setParameter(ParameterID::MOTOR_NOISE_LEVEL, Defaults::MOTOR_NOISE_LEVEL);
    setParameter(ParameterID::REWARD_MODULATION_ENABLED, Defaults::REWARD_MODULATION_ENABLED);
    setParameter(ParameterID::STRUCTURAL_PLASTICITY_ENABLED, Defaults::STRUCTURAL_PLASTICITY_ENABLED);
    setParameter(ParameterID::DEVELOPMENT_ENABLED, Defaults::DEVELOPMENT_ENABLED);
    setParameter(ParameterID::CURIOSITY_ENABLED, Defaults::CURIOSITY_ENABLED);
}

} // namespace nlm