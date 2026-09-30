# Configuration Guide

## Overview of NLM Brain Configuration

NLM (Neural Learning Machine) brains are highly configurable through a flexible key-value configuration system. This guide covers all aspects of configuring NLM brains for different use cases, from simple implementations to advanced research setups.

## Understanding the Configuration System

### Configuration Structure

NLM uses a hierarchical configuration system with the following components:

```cpp
struct Config {
    // Key-value pairs for settings
    std::unordered_map<std::string, ConfigValue> settings;
    
    // Configuration sources for overriding
    std::map<ConfigSource, std::unordered_map<std::string, ConfigValue>> sources;
    
    // Configuration validation
    bool validate() const;
    
    // Configuration merging
    void merge(const Config& other);
};
```

### Configuration Sources

| Source | Priority | Description |
|--------|----------|-------------|
| Default | 1 | Built-in defaults |
| User | 2 | User-defined settings |
| Command Line | 3 | Runtime overrides |
| Runtime | 4 | Programmatic changes |

## Configuration Categories

### 1. Core Brain Configuration

#### Neuron Configuration

```cpp
// Basic neuron setup
config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::User);
config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);
config.set("connection_probability", 0.1, nlm::ConfigSource::User);

// Neuron type distribution
config.set("excitatory_ratio", 0.8f, nlm::ConfigSource::User);
config.set("inhibitory_ratio", 0.2f, nlm::ConfigSource::User);
config.set("reference_ratio", 0.05f, nlm::ConfigSource::User);
config.set("modulatory_ratio", 0.05f, nlm::ConfigSource::User);

// Neuron properties
config.set("max_firing_rate", 100.0f, nlm::ConfigSource::User);
config.set("threshold", 0.5f, nlm::ConfigSource::User);
config.set("resistance", 1.0f, nlm::ConfigSource::User);
```

#### Synaptic Configuration

```cpp
// Connection properties
config.set("initial_weight", 0.1f, nlm::ConfigSource::User);
config.set("max_weight", 1.0f, nlm::ConfigSource::User);
config.set("min_weight", -1.0f, nlm::ConfigSource::User);
config.set("weight_decay_rate", 0.01f, nlm::ConfigSource::User);

// Connection dynamics
config.set("connection_delay", 1, nlm::ConfigSource::User);
config.set("max_connections_per_neuron", 50, nlm::ConfigSource::User);
config.set("connection_stereicity", 0.95f, nlm::ConfigSource::User);
```

### 2. Memory System Configuration

#### Working Memory Configuration

```cpp
// Working memory parameters
config.set("working_memory_capacity", 100, nlm::ConfigSource::User);
config.set("working_memory_decay_rate", 0.02f, nlm::ConfigSource::User);
config.set("working_memory_interference_strength", 0.1f, nlm::ConfigSource::User);
config.set("working_memory_consolidation_threshold", 0.7f, nlm::ConfigSource::User);
```

#### Episodic Memory Configuration

```cpp
// Episodic memory parameters
config.set("episodic_memory_capacity", 1000, nlm::ConfigSource::User);
config.set("episodic_memory_consolidation_rate", 0.05f, nlm::ConfigSource::User);
config.set("episodic_memory_relevance_threshold", 0.6f, nlm::ConfigSource::User);
config.set("episodic_memory_pruning_rate", 0.1f, nlm::ConfigSource::User);
```

#### Semantic Memory Configuration

```cpp
// Semantic memory parameters
config.set("semantic_memory_capacity", 10000, nlm::ConfigSource::User);
config.set("semantic_memory_update_rate", 0.1f, nlm::ConfigSource::User);
config.set("semantic_memory_forgetting_rate", 0.001f, nlm::ConfigSource::User);
config.set("semantic_memory_associative_strength", 0.3f, nlm::ConfigSource::User);
```

#### Procedural Memory Configuration

```cpp
// Procedural memory parameters
config.set("procedural_memory_capacity", 500, nlm::ConfigSource::User);
config.set("procedural_memory_learning_rate", 0.2f, nlm::ConfigSource::User);
config.set("procedural_memory_consolidation_rate", 0.1f, nlm::ConfigSource::User);
config.set("procedural_memory_execution_threshold", 0.8f, nlm::ConfigSource::User);
```

#### Associative Memory Configuration

```cpp
// Associative memory parameters
config.set("associative_memory_capacity", 2000, nlm::ConfigSource::User);
config.set("associative_memory_strength_decay", 0.05f, nlm::ConfigSource::User);
config.set("associative_memory_recency_boost", 0.3f, nlm::ConfigSource::User);
config.set("associative_memory_update_rate", 0.15f, nlm::ConfigSource::User);
```

### 3. Plasticity System Configuration

#### STDP Configuration

```cpp
// Spike Timing Dependent Plasticity
config.set("stdp_learning_rate", 0.01f, nlm::ConfigSource::User);
config.set("stdp_time_window_pre", 20, nlm::ConfigSource::User);      // ms
config.set("stdp_time_window_post", 40, nlm::ConfigSource::User);     // ms
config.set("stdp_eligibility_decay", 0.95f, nlm::ConfigSource::User);
config.set("stdp_trace_decay_rate", 0.99f, nlm::ConfigSource::User);
```

#### Hebbian Configuration

```cpp
// Hebbian Learning
config.set("hebbian_learning_rate", 0.005f, nlm::ConfigSource::User);
config.set("hebbian_coactivation_threshold", 0.7f, nlm::ConfigSource::User);
config.set("hebbian_consolidation_period", 100, nlm::ConfigSource::User);
config.set("hebbian_forgetting_rate", 0.001f, nlm::ConfigSource::User);
```

#### Structural Plasticity Configuration

```cpp
// Structural Plasticity
config.set("structural_plasticity_rate", 0.001f, nlm::ConfigSource::User);
config.set("structural_plasticity_growth_rate", 0.05f, nlm::ConfigSource::User);
config.set("structural_plasticity_pruning_threshold", 0.1f, nlm::ConfigSource::User);
config.set("structural_plasticity_resource_limit", 10000, nlm::ConfigSource::User);
```

### 4. Development System Configuration

```cpp
// Development configuration
config.set("development_enabled", true, nlm::ConfigSource::User);
config.set("developmental_stage_count", 5, nlm::ConfigSource::User);
config.set("developmental_stage_duration", 500, nlm::ConfigSource::User);
config.set("developmental_plasticity_modifier", 1.5f, nlm::ConfigSource::User);
config.set("developmental_maturation_rate", 0.8f, nlm::ConfigSource::User);
```

### 5. Prediction System Configuration

```cpp
// Prediction system
config.set("prediction_system_enabled", true, nlm::ConfigSource::User);
config.set("prediction_horizon", 10, nlm::ConfigSource::User);
config.set("prediction_error_threshold", 0.2f, nlm::ConfigSource::User);
config.set("prediction_confidence_threshold", 0.5f, nlm::ConfigSource::User);
config.set("prediction_update_rate", 0.3f, nlm::ConfigSource::User);
```

### 6. Neuromodulation Configuration

#### Dopamine Configuration

```cpp
// Dopamine system
config.set("dopamine_enabled", true, nlm::ConfigSource::User);
config.set("dopamine_learning_rate", 0.2f, nlm::ConfigSource::User);
config.set("dopamine_prediction_error_gain", 1.0f, nlm::ConfigSource::User);
config.set("dopamine_reward_scale", 1.0f, nlm::ConfigSource::User);
config.set("dopamine_noise_level", 0.1f, nlm::ConfigSource::User);
```

#### Curiosity Configuration

```cpp
// Curiosity system
config.set("curiosity_enabled", true, nlm::ConfigSource::User);
config.set("curiosity_threshold", 0.4f, nlm::ConfigSource::User);
config.set("curiosity_decay_rate", 0.95f, nlm::ConfigSource::User);
config.set("curiosity_reinforcement_rate", 0.1f, nlm::ConfigSource::User);
config.set("curiosity_exploration_bonus", 0.5f, nlm::ConfigSource::User);
```

#### Novelty Configuration

```cpp
// Novelty detection
config.set("novelty_enabled", true, nlm::ConfigSource::User);
config.set("novelty_sensitivity", 0.3f, nlm::ConfigSource::User);
config.set("novelty_decay_rate", 0.9f, nlm::ConfigSource::User);
config.set("novelty_integration_strength", 0.7f, nlm::ConfigSource::User);
config.set("novelty_threshold_adjustment", 0.2f, nlm::ConfigSource::User);
```

### 7. Cognitive Systems Configuration

#### Planning System Configuration

```cpp
// Planning system
config.set("planning_system_enabled", true, nlm::ConfigSource::User);
config.set("planning_horizon", 5, nlm::ConfigSource::User);
config.set("planning_depth", 3, nlm::ConfigSource::User);
config.set("action_selection_method", "thompson_sampling", nlm::ConfigSource::User);
config.set("exploration_exploitation_ratio", 0.3f, nlm::ConfigSource::User);
```

#### Attention System Configuration

```cpp
// Attention system
config.set("attention_enabled", true, nlm::ConfigSource::User);
config.set("attention_window_size", 8, nlm::ConfigSource::User);
config.set("attention_threshold", 0.3f, nlm::ConfigSource::User);
config.set("attention_spread", 0.5f, nlm::ConfigSource::User);
config.set("attention_decay_rate", 0.9f, nlm::ConfigSource::User);
```

#### Concept Formation Configuration

```cpp
// Concept formation
config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
config.set("concept_threshold", 0.7f, nlm::ConfigSource::User);
config.set("concept_merge_threshold", 0.8f, nlm::ConfigSource::User);
config.set("concept_spread_rate", 0.1f, nlm::ConfigSource::User);
config.set("concept_forgetting_rate", 0.01f, nlm::ConfigSource::User);
```

## Use Case Configurations

### 1. Research Brain Configuration

```cpp
nlm::Config createResearchBrainConfig() {
    nlm::Config config;
    
    // Large, complex brain for research
    config.set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::User);
    config.set("connection_probability", 0.2f, nlm::ConfigSource::User);
    
    // Advanced memory systems for research
    config.set("working_memory_capacity", 1000, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 50000, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 500000, nlm::ConfigSource::User);
    config.set("associative_memory_capacity", 200000, nlm::ConfigSource::User);
    
    // Detailed plasticity for studying learning
    config.set("stdp_learning_rate", 0.02f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.01f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.003f, nlm::ConfigSource::User);
    config.set("plasticity_eligibility_decay", 0.99f, nlm::ConfigSource::User);
    
    // Full development for studying maturation
    config.set("development_enabled", true, nlm::ConfigSource::User);
    config.set("developmental_stage_count", 8, nlm::ConfigSource::User);
    config.set("developmental_stage_duration", 2000, nlm::ConfigSource::User);
    
    // Advanced prediction for studying anticipation
    config.set("prediction_system_enabled", true, nlm::ConfigSource::User);
    config.set("prediction_horizon", 50, nlm::ConfigSource::User);
    config.set("prediction_error_threshold", 0.1f, nlm::ConfigSource::User);
    
    // Complex neuromodulation for studying motivation
    config.set("dopamine_enabled", true, nlm::ConfigSource::User);
    config.set("dopamine_learning_rate", 0.3f, nlm::ConfigSource::User);
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.2f, nlm::ConfigSource::User);
    config.set("novelty_enabled", true, nlm::ConfigSource::User);
    config.set("novelty_sensitivity", 0.5f, nlm::ConfigSource::User);
    
    // Full cognitive capabilities
    config.set("planning_system_enabled", true, nlm::ConfigSource::User);
    config.set("planning_horizon", 20, nlm::ConfigSource::User);
    config.set("attention_enabled", true, nlm::ConfigSource::User);
    config.set("attention_window_size", 20, nlm::ConfigSource::User);
    config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
    
    // Recording and logging for research
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 10, nlm::ConfigSource::User);
    config.set("enable_logging", true, nlm::ConfigSource::User);
    config.set("log_level", "DEBUG", nlm::ConfigSource::User);
    
    return config;
}
```

### 2. Real-Time Robot Configuration

```cpp
canl::Config createRealTimeRobotConfig() {
    nlm::Config config;
    
    // Fast, responsive brain
    config.set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::User);
    config.set("connection_probability", 0.3f, nlm::ConfigSource::User);  // More connections
    
    // Lightweight memory for speed
    config.set("working_memory_capacity", 50, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 200, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 1000, nlm::ConfigSource::User);
    
    // Fast learning for quick adaptation
    config.set("stdp_learning_rate", 0.05f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.02f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.005f, nlm::ConfigSource::User);
    
    // Strong curiosity for exploration
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.3f, nlm::ConfigSource::User);
    config.set("curiosity_decay_rate", 0.9f, nlm::ConfigSource::User);
    
    // Predict for quick action
    config.set("prediction_system_enabled", true, nlm::ConfigSource::User);
    config.set("prediction_horizon", 5, nlm::ConfigSource::User);
    config.set("prediction_error_threshold", 0.3f, nlm::ConfigSource::User);
    
    // Focus on quick decisions
    config.set("planning_horizon", 3, nlm::ConfigSource::User);
    config.set("action_selection_method", "epsilon_greedy", nlm::ConfigSource::User);
    config.set("exploration_exploitation_ratio", 0.4f, nlm::ConfigSource::User);
    
    // Efficient resources
    config.set("development_enabled", false, nlm::ConfigSource::User);  // Skip development for speed
    config.set("enable_recording", false, nlm::ConfigSource::User);    // Skip recording for performance
    config.set("log_level", "WARN", nlm::ConfigSource::User);         // Minimal logging
    
    return config;
}
```

### 3. Educational Agent Configuration

```cpp
canl::Config createEducationalAgentConfig() {
    nlm::Config config;
    
    // Medium-sized brain for learning
    config.set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::User);
    config.set("connection_probability", 0.12f, nlm::ConfigSource::User);
    
    // Rich semantic memory for knowledge
    config.set("working_memory_capacity", 200, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 1000, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 50000, nlm::ConfigSource::User);
    config.set("procedural_memory_capacity", 300, nlm::ConfigSource::User);
    
    // Gentle learning for educational use
    config.set("stdp_learning_rate", 0.01f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.005f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.001f, nlm::ConfigSource::User);
    
    // Strong development system
    config.set("development_enabled", true, nlm::ConfigSource::User);
    config.set("developmental_stage_count", 6, nlm::ConfigSource::User);
    config.set("developmental_stage_duration", 1000, nlm::ConfigSource::User);
    config.set("developmental_plasticity_modifier", 2.0f, nlm::ConfigSource::User);
    
    // Reward-based learning
    config.set("dopamine_enabled", true, nlm::ConfigSource::User);
    config.set("dopamine_learning_rate", 0.15f, nlm::ConfigSource::User);
    config.set("reward_sensitivity", 1.5f, nlm::ConfigSource::User);
    config.set("punishment_sensitivity", 0.8f, nlm::ConfigSource::User);
    
    // Focused attention and planning
    config.set("attention_enabled", true, nlm::ConfigSource::User);
    config.set("attention_window_size", 10, nlm::ConfigSource::User);
    config.set("planning_system_enabled", true, nlm::ConfigSource::User);
    config.set("planning_horizon", 8, nlm::ConfigSource::User);
    
    // Concept formation for learning
    config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
    config.set("concept_threshold", 0.6f, nlm::ConfigSource::User);
    
    // Educational recording and feedback
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 5, nlm::ConfigSource::User);
    config.set("enable_logging", true, nlm::ConfigSource::User);
    config.set("log_level", "INFO", nlm::ConfigSource::User);
    
    // Learning-specific features
    config.set("enable_feedback_learning", true, nlm::ConfigSource::User);
    config.set("feedback_delay", 10, nlm::ConfigSource::User);
    config.set("feedback_strength", 0.5f, nlm::ConfigSource::User);
    
    return config;
}
```

### 4. Medical Research Configuration

```cpp
canl::Config createMedicalResearchConfig() {
    nlm::Config config;
    
    // Large, detailed brain for medical research
    config.set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::User);
    config.set("region_count", static_cast<int64_t>(15), nlm::ConfigSource::User);
    config.set("connection_probability", 0.08f, nlm::ConfigSource::User);  // Sparse
    
    // Comprehensive memory systems
    config.set("working_memory_capacity", 2000, nlm::ConfigSource::User);
    config.set("episodic_memory_capacity", 100000, nlm::ConfigSource::User);
    config.set("semantic_memory_capacity", 1000000, nlm::ConfigSource::User);
    config.set("associative_memory_capacity", 500000, nlm::ConfigSource::User);
    config.set("procedural_memory_capacity", 1000, nlm::ConfigSource::User);
    
    // Precise plasticity for studying neurological conditions
    config.set("stdp_learning_rate", 0.005f, nlm::ConfigSource::User);
    config.set("hebbian_learning_rate", 0.0025f, nlm::ConfigSource::User);
    config.set("structural_plasticity_rate", 0.0005f, nlm::ConfigSource::User);
    config.set("plasticity_eligibility_decay", 0.999f, nlm::ConfigSource::User);
    
    // Extended development for studying neurodevelopment
    config.set("development_enabled", true, nlm::ConfigSource::User);
    config.set("developmental_stage_count", 10, nlm::ConfigSource::User);
    config.set("developmental_stage_duration", 5000, nlm::ConfigSource::User);
    config.set("developmental_plasticity_modifier", 3.0f, nlm::ConfigSource::User);
    
    // Detailed neuromodulation for studying brain chemistry
    config.set("dopamine_enabled", true, nlm::ConfigSource::User);
    config.set("dopamine_learning_rate", 0.1f, nlm::ConfigSource::User);
    config.set("dopamine_prediction_error_gain", 2.0f, nlm::ConfigSource::User);
    config.set("curiosity_enabled", true, nlm::ConfigSource::User);
    config.set("curiosity_threshold", 0.1f, nlm::ConfigSource::User);  // Low threshold
    config.set("novelty_enabled", true, nlm::ConfigSource::User);
    config.set("novelty_sensitivity", 0.8f, nlm::ConfigSource::User);  // High sensitivity
    
    // Comprehensive cognitive functions
    config.set("planning_system_enabled", true, nlm::ConfigSource::User);
    config.set("planning_horizon", 30, nlm::ConfigSource::User);
    config.set("attention_enabled", true, nlm::ConfigSource::User);
    config.set("attention_window_size", 30, nlm::ConfigSource::User);
    config.set("concept_formation_enabled", true, nlm::ConfigSource::User);
    config.set("concept_threshold", 0.5f, nlm::ConfigSource::User);
    
    // Medical-grade recording and analysis
    config.set("enable_recording", true, nlm::ConfigSource::User);
    config.set("recording_frequency", 20, nlm::ConfigSource::User);
    config.set("recording_format", "neurodata", nlm::ConfigSource::User);
    config.set("enable_logging", true, nlm::ConfigSource::User);
    config.set("log_level", "TRACE", nlm::ConfigSource::User);
    
    // Medical research features
    config.set("enable_brain_imaging", true, nlm::ConfigSource::User);
    config.set("enable_physiological_measures", true, nlm::ConfigSource::User);
    config.set("enable_stress_monitoring", true, nlm::ConfigSource::User);
    config.set("enable_biological_validation", true, nlm::ConfigSource::User);
    
    return config;
}
```

## Configuration File Format

### JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 5,
    "connection_probability": 0.1
  },
  "memory": {
    "working_memory_capacity": 200,
    "episodic_memory_capacity": 1000,
    "semantic_memory_capacity": 10000
  },
  "plasticity": {
    "stdp_learning_rate": 0.01,
    "hebbian_learning_rate": 0.005
  },
  "development": {
    "enabled": true,
    "stage_count": 6,
    "stage_duration": 1000
  },
  "prediction": {
    "enabled": true,
    "horizon": 10,
    "error_threshold": 0.2
  },
  "neuromodulation": {
    "dopamine": {
      "enabled": true,
      "learning_rate": 0.2
    },
    "curiosity": {
      "enabled": true,
      "threshold": 0.4
    },
    "novelty": {
      "enabled": true,
      "sensitivity": 0.3
    }
  }
}
```

### Configuration Loading

```cpp
// Load configuration from JSON file
nlm::Config loadConfigFromJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + filename);
    }
    
    nlohmann::json jsonConfig;
    file >> jsonConfig;
    
    return parseJsonToConfig(jsonConfig);
}

// Parse JSON to NLM config
nlm::Config parseJsonToConfig(const nlohmann::json& json) {
    nlm::Config config;
    
    // Parse brain settings
    if (json.contains("brain")) {
        auto brainJson = json["brain"];
        if (brainJson.contains("neuron_count")) {
            config.set("neuron_count", brainJson["neuron_count"].get<int64_t>());
        }
        if (brainJson.contains("region_count")) {
            config.set("region_count", brainJson["region_count"].get<int64_t>());
        }
        if (brainJson.contains("connection_probability")) {
            config.set("connection_probability", brainJson["connection_probability"].get<float>());
        }
    }
    
    // Parse memory settings
    if (json.contains("memory")) {
        auto memoryJson = json["memory"];
        // Parse all memory settings...
    }
    
    // Parse plasticity settings
    if (json.contains("plasticity")) {
        auto plasticityJson = json["plasticity"];
        // Parse all plasticity settings...
    }
    
    // Parse other settings...
    
    return config;
}
```

### Configuration Command Line Interface

```bash
# Create default configuration
nlm-config --create --output default.cfg

# Load configuration file
nlm-config --load my_config.json

# Override configuration values
nlm-config --set neuron_count=2000 --set connection_probability=0.2

# List current configuration
nlm-config --list

# Validate configuration
nlm-config --validate

# Save configuration
nlm-config --save current.cfg
```

## Configuration Management

### Configuration Profiles

```cpp
struct ConfigurationProfile {
    std::string name;
    nlm::Config config;
    std::string description;
};

class ConfigurationManager {
public:
    static void registerProfile(const ConfigurationProfile& profile) {
        profiles_[profile.name] = profile;
    }
    
    static nlm::Config loadProfile(const std::string& name) {
        if (profiles_.find(name) == profiles_.end()) {
            throw std::runtime_error("Unknown profile: " + name);
        }
        return profiles_[name].config;
    }
    
    static std::vector<std::string> getAvailableProfiles() {
        std::vector<std::string> profiles;
        for (const auto& pair : profiles_) {
            profiles.push_back(pair.first);
        }
        return profiles;
    }
    
    static void createDefaultProfiles() {
        // Research profile
        registerProfile({
            "research",
            createResearchBrainConfig(),
            "Configuration for brain research and neuroscience studies"
        });
        
        // Robot profile
        registerProfile({
            "robot",
            createRealTimeRobotConfig(),
            "Configuration for real-time robot control"
        });
        
        // Educational profile
        registerProfile({
            "educational",
            createEducationalAgentConfig(),
            "Configuration for educational agents"
        });
        
        // Medical profile
        registerProfile({
            "medical",
            createMedicalResearchConfig(),
            "Configuration for medical and clinical research"
        });
    }
    
private:
    static std::unordered_map<std::string, ConfigurationProfile> profiles_;
};
```

### Configuration Validation

```cpp
class ConfigurationValidator {
public:
    static bool validate(const nlm::Config& config, std::vector<std::string>& errors) {
        errors.clear();
        
        // Validate brain settings
        if (!validateBrainSettings(config, errors)) {
            return false;
        }
        
        // Validate memory settings
        if (!validateMemorySettings(config, errors)) {
            return false;
        }
        
        // Validate plasticity settings
        if (!validatePlasticitySettings(config, errors)) {
            return false;
        }
        
        // Validate development settings
        if (!validateDevelopmentSettings(config, errors)) {
            return false;
        }
        
        // Validate prediction settings
        if (!validatePredictionSettings(config, errors)) {
            return false;
        }
        
        // Validate neuromodulation settings
        if (!validateNeuromodulationSettings(config, errors)) {
            return false;
        }
        
        // Validate cognitive settings
        if (!validateCognitiveSettings(config, errors)) {
            return false;
        }
        
        return errors.empty();
    }
    
private:
    static bool validateBrainSettings(const nlm::Config& config, std::vector<std::string>& errors) {
        // Check neuron count
        if (config.has("neuron_count")) {
            int64_t neuronCount = config.get<int64_t>("neuron_count");
            if (neuronCount < 10) {
                errors.push_back("Neuron count must be at least 10");
                return false;
            }
            if (neuronCount > 100000) {
                errors.push_back("Neuron count cannot exceed 100,000");
                return false;
            }
        }
        
        // Check region count
        if (config.has("region_count")) {
            int64_t regionCount = config.get<int64_t>("region_count");
            if (regionCount < 1) {
                errors.push_back("Region count must be at least 1");
                return false;
            }
            if (regionCount > 100) {
                errors.push_back("Region count cannot exceed 100");
                return false;
            }
        }
        
        // Check connection probability
        if (config.has("connection_probability")) {
            float connectionProb = config.get<float>("connection_probability");
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                errors.push_back("Connection probability must be between 0.0 and 1.0");
                return false;
            }
        }
        
        return true;
    }
    
    // Implement other validation methods...
};
```

---

## Configuration Best Practices

### 1. Start Simple

1. **Begin with defaults**: Start with a minimal configuration
2. **Add features incrementally**: Enable features one at a time
3. **Test each feature**: Validate functionality after each change

### 2. Use Environment-Specific Configurations

```cpp
// Different configurations for different environments
nlm::Config getProductionConfig() {
    auto config = createRealTimeRobotConfig();
    config.set("enable_recording", false);
    config.set("log_level", "WARN");
    return config;
}

nlm::Config getDevelopmentConfig() {
    auto config = createAdvancedAgentConfig();
    config.set("enable_recording", true);
    config.set("log_level", "DEBUG");
    return config;
}
```

### 3. Profile for Performance

1. **Monitor resource usage**: Track CPU, memory, and I/O
2. **Adjust for constraints**: Optimize for specific hardware
3. **Test under load**: Simulate realistic usage scenarios

### 4. Document Configuration Choices

```cpp
// Document configuration choices
struct ConfigDocumentation {
    std::string parameter;
    std::string description;
    std::string recommendedValue;
    std::string useCase;
};

void documentConfiguration(const nlm::Config& config) {
    auto docs = getConfigurationDocumentation(config);
    
    std::cout << "Configuration Documentation:" << std::endl;
    for (const auto& doc : docs) {
        std::cout << "  " << doc.parameter << ": " << doc.description << std::endl;
        std::cout << "    Recommended: " << doc.recommendedValue << std::endl;
        std::cout << "    Use case: " << doc.useCase << std::endl << std::endl;
    }
}
```

### 5. Version Control Configuration

```cpp
canl::Config backupConfig(const nlm::Config& config, const std::string& filename) {
    // Save configuration to file with version
    std::string timestamp = getCurrentTimestamp();
    std::string versionedFilename = filename + "_v" + timestamp + ".cfg";
    
    std::ofstream file(versionedFilename);
    if (file.is_open()) {
        // Serialize config to file
        serializeConfig(config, file);
        file.close();
    }
    
    return config;
}
```

---

## Configuration Troubleshooting

### Common Configuration Issues

#### Issue: Brain Won't Initialize

**Symptoms**:
- Brain initialization fails
- Brain shows 0 neurons
- Memory errors

**Solutions**:
1. **Check configuration validity**: Use `nlm-config --validate`
2. **Verify required settings**: Ensure `neuron_count` and `region_count` are set
3. **Check for conflicts**: Remove conflicting settings
4. **Use defaults**: Start with minimal configuration

#### Issue: Memory Issues

**Symptoms**:
- Memory usage too high
- Memory errors
- Performance degradation

**Solutions**:
1. **Reduce memory capacity**: Lower memory configuration values
2. **Enable pruning**: Configure memory pruning strategies
3. **Optimize plasticity**: Reduce plasticity rates
4. **Increase system resources**: Add more CPU/memory

#### Issue: Learning Problems

**Symptoms**:
- No learning observed
- Plasticity not working
- Behavior not changing

**Solutions**:
1. **Check plasticity settings**: Ensure learning rates are set
2. **Enable neuromodulation**: Add reward and curiosity systems
3. **Verify memory integration**: Ensure memory systems are enabled
4. **Test with simple tasks**: Start with basic behavior

### Configuration Debugging

```cpp
def debugConfiguration(nlm::Config& config) {
    std::cout << "Current Configuration:" << std::endl;
    
    // List all settings
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::cout << "  " << key << ": " << config.get(key) << std::endl;
    }
    
    // Validate specific settings
    if (!config.has("neuron_count")) {
        std::cerr << "Error: neuron_count not set" << std::endl;
    }
    
    if (!config.has("region_count")) {
        std::cerr << "Error: region_count not set" << std::endl;
    }
    
    // Check for common issues
    if (config.has("neuron_count")) {
        int64_t neurons = config.get<int64_t>("neuron_count");
        if (neurons < 10) {
            std::cerr << "Warning: Very few neurons may cause issues" << std::endl;
        }
    }
    
    // Suggest improvements
    suggestImprovements(config);
}

void suggestImprovements(nlm::Config& config) {
    // Suggest better values for common configurations
    if (config.get<int64_t>("neuron_count") < 100) {
        std::cout << "Suggestion: Increase neuron_count for better performance" << std::endl;
    }
    
    if (!config.has("working_memory_capacity")) {
        std::cout << "Suggestion: Set working_memory_capacity for better agent behavior" << std::endl;
    }
    
    if (config.get<float>("connection_probability") < 0.05f) {
        std::cout << "Suggestion: Increase connection_probability for better connectivity" << std::endl;
    }
}
```

---

## Configuration Summary

### Key Configuration Categories

1. **Core Brain**: Neuron count, regions, connections
2. **Memory Systems**: Working, episodic, semantic, procedural, associative
3. **Plasticity**: STDP, Hebbian, structural plasticity
4. **Development**: Growth stages, maturation
5. **Prediction**: Future anticipation, error correction
6. **Neuromodulation**: Dopamine, curiosity, novelty
7. **Cognitive Systems**: Planning, attention, concept formation

### Use Case Recommendations

| Use Case | Recommended Configuration |
|----------|---------------------------|
| Research | Large brain, comprehensive memory, full development |
| Real-time Robot | Small brain, lightweight memory, fast learning |
| Educational Agent | Medium brain, rich semantic memory, gentle learning |
| Medical Research | Very large brain, detailed plasticity, extended development |

### Configuration Management Best Practices

1. **Use profiles**: Define configurations for different use cases
2. **Document choices**: Keep documentation of configuration decisions
3. **Test thoroughly**: Validate configurations before deployment
4. **Monitor performance**: Track resource usage and adjust as needed
5. **Backup configurations**: Save different versions for comparison

By following this Configuration Guide, you can effectively configure NLM brains for a wide variety of applications, from simple agents to complex research systems. The key is to start simple, add features incrementally, and always validate your configuration choices.