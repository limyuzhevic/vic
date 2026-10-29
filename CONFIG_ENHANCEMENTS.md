NLM Configuration System Enhancement
===================================

## Overview

This document details the enhancements made to the NLM configuration system, including JSON/YAML support, advanced configuration options, validation, and improved error handling.

## Changes Made

### 1. Enhanced Config.cpp (Core Configuration System)

#### JSON/YAML Support
- **Added JSON parser** using nlohmann/json library
- **Added YAML parser** using YAML-CPP library
- **Automatic format detection** based on file extension
- **Enhanced file loading** with better error handling

#### Advanced Configuration Options
- **Type-safe configuration keys** with namespace support
- **Configuration schemas** for validation
- **Default values with fallbacks**
- **Advanced parameter categories**:
  - Brain parameters (neuron count, connectivity, plasticity)
  - Simulation parameters (timestep, duration, logging)
  - Neuromodulation parameters (dopamine, curiosity, novelty)
  - Development parameters (synaptogenesis, pruning)
  - World/environment parameters (visualization, sensing)

#### Enhanced Validation
- **Type validation** for all configuration values
- **Range validation** with configurable min/max bounds
- **Dependency validation** (e.g., neuron_count > 0, timestep > 0)
- **Cross-validation** between related parameters
- **Schema-based validation** for complex configurations

#### Improved Error Handling
- **Detailed error messages** with specific guidance
- **Error categorization** (config, file, validation, runtime)
- **Graceful degradation** with fallback defaults
- **Recovery mechanisms** for corrupted configurations

### 2. Config.hpp (Header File Enhancements)

#### Enhanced API
- **Namespace-aware configuration keys** (e.g., `brain.neuron_count`)
- **Configuration schema support**
- **Advanced query methods**
- **Bulk configuration operations**
- **Configuration comparison and diffing**

#### New Configuration Types
- **Path types** with automatic expansion
- **Duration types** (converting to timesteps)
- **Probability types** (clamped to [0,1] range)
- **Enum types** with validation

### 3. Example Configurations

#### default.json (New JSON configuration)
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.01
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.001,
      "pruning_rate": 0.0001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "baseline": 0.1,
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "baseline": 0.05,
      "exploration_bonus": 0.5
    },
    "novelty": {
      "enable": true,
      "decay_rate": 0.99
    },
    "prediction_error": {
      "enable": true,
      "learning_rate": 0.1
    }
  },
  "development": {
    "enable": true,
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "critical_period": {
      "start": 100,
      "end": 1000
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "time_limit": 0.0,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log"
  },
  "visualization": {
    "enable": false,
    "update_rate": 30.0,
    "window_width": 1280,
    "window_height": 720,
    "neuron_radius": 2.0,
    "synapse_width": 0.5
  },
  "world": {
    "environment": {
      "type": "GridWorld",
      "width": 10,
      "height": 10,
      "vision_width": 8,
      "vision_height": 8
    },
    "agent": {
      "sensory_mode": "full",
      "motor_mode": "discrete",
      "action_set": ["move_forward", "turn_left", "turn_right", "wait"]
    }
  }
}
```

#### default.yaml (Alternative YAML configuration)
```yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.001
    pruning_rate: 0.0001

neuromodulation:
  dopamine:
    baseline: 0.1
    scale: 1.0
  curiosity:
    enable: true
    baseline: 0.05
    exploration_bonus: 0.5
  novelty:
    enable: true
    decay_rate: 0.99
  prediction_error:
    enable: true
    learning_rate: 0.1

development:
  enable: true
  synaptogenesis_rate: 0.001
  pruning_rate: 0.0001
  critical_period:
    start: 100
    end: 1000

simulation:
  timestep: 0.001
  max_steps: 10000
  time_limit: 0.0
  log_level: INFO
  log_to_file: false
  log_filename: nlm.log

visualization:
  enable: false
  update_rate: 30.0
  window_width: 1280
  window_height: 720
  neuron_radius: 2.0
  synapse_width: 0.5

world:
  environment:
    type: GridWorld
    width: 10
    height: 10
    vision_width: 8
    vision_height: 8
  agent:
    sensory_mode: full
    motor_mode: discrete
    action_set:
      - move_forward
      - turn_left
      - turn_right
      - wait

# Phase-specific configurations
phase_specific:
  phase_1:
    learning_rate: 0.01
    exploration_rate: 0.1
  phase_2:
    learning_rate: 0.005
    exploration_rate: 0.05
  phase_3:
    learning_rate: 0.001
    exploration_rate: 0.01
  phase_4:
    learning_rate: 0.0005
    exploration_rate: 0.005
  phase_5:
    learning_rate: 0.0001
    exploration_rate: 0.001
  phase_6:
    learning_rate: 0.00005
    exploration_rate: 0.0005
    enable_all_systems: true
    enable_advanced_plasticity: true
    enable_developed_memory: true
```

#### configs/advanced.cfg (Backward-compatible enhanced format)
```
# NLM Advanced Configuration File
# Enhanced key=value format with additional features

# Core brain parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Enhanced plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
hebbian_learning_rate = 0.01
structural_synaptogenesis_rate = 0.001
structural_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1
dopamine_scale = 1.0
curiosity_baseline = 0.05
curiosity_exploration_bonus = 0.5
novelty_decay_rate = 0.99
prediction_error_learning_rate = 0.1

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
development_critical_period_start = 100
development_critical_period_end = 1000

# Enhanced logging
log_level = INFO
log_to_file = true
log_filename = nlm.log
log_format = detailed
max_log_size = 10485760
log_rotation_count = 5

# Performance settings
enable_performance_monitoring = true
performance_log_interval = 1000
enable_memory_tracking = true
enable_timestep_tracking = true

# Advanced simulation options
max_simulation_steps = 10000
simulation_time_limit = 0.0
enable_checkpointing = true
checkpoint_interval = 1000
checkpoint_compression = balanced

# Visualization settings
visualization_enabled = true
visualization_update_rate = 30.0
visualization_window_width = 1280
visualization_window_height = 720
visualization_neuron_radius = 2.0
visualization_synapse_width = 0.5
visualization_background_color = 0.1, 0.1, 0.1, 1.0
visualization_neuron_color = 0.3, 0.3, 0.8, 1.0
visualization_synapse_color = 0.5, 0.5, 0.5, 1.0

# World parameters
environment_name = GridWorld
environment_width = 10
environment_height = 10
vision_width = 8
vision_height = 8
agent_sensory_mode = full
agent_motor_mode = discrete
agent_action_set = move_forward,turn_left,turn_right,wait

# Phase-specific settings
enable_phase_1 = true
enable_phase_2 = true
enable_phase_3 = true
enable_phase_4 = true
enable_phase_5 = true
enable_phase_6 = true
phase_1_learning_rate = 0.01
phase_2_learning_rate = 0.005
phase_3_learning_rate = 0.001
phase_4_learning_rate = 0.0005
phase_5_learning_rate = 0.0001
phase_6_learning_rate = 0.00005
phase_6_enable_advanced_plasticity = true
phase_6_enable_developed_memory = true
phase_6_enable_advanced_cognition = true
phase_6_enable_developed_neuromodulation = true

# Security settings
enable_encryption = false
encryption_key_file = .nlm_encryption_key
enable_access_control = false
access_control_file = .nlm_access_control

# Network settings (for distributed simulations)
enable_network_mode = false
network_port = 8888
network_host = localhost
network_protocol = tcp
network_timeout = 30.0

# Performance optimization
enable_parallel_processing = true
parallel_threads = 4
enable_simd_optimization = true
enable_vectorization = true
enable_loop_unrolling = true
```

### 4. Configuration Validation and Documentation

#### Configuration Schema Definition
```cpp
struct BrainConfigSchema {
    // Required fields
    uint64_t neuron_count = 1000;
    uint64_t region_count = 1;
    float connection_probability = 0.1f;
    
    // Optional fields with validation
    float v_thresh = -50.0f;                    // Range: (-100, 0)
    float v_rest = -70.0f;                       // Range: (-80, -60)
    float v_reset = -75.0f;                      // Range: (-80, -70)
    float tau_mem = 20.0f;                       // Range: (1, 100)
    float tau_ref = 2.0f;                        // Range: (1, 10)
    
    std::string toString() const;
    bool validate() const;
    static std::string getValidationError();
};

struct PlasticityConfigSchema {
    bool stdp_enable = true;
    bool hebbian_enable = true;
    bool structural_enable = true;
    
    float stdp_learning_rate = 0.001f;         // Range: (0.0001, 0.01)
    float hebbian_learning_rate = 0.01f;       // Range: (0.001, 0.1)
    float structural_synaptogenesis_rate = 0.001f;   // Range: (0.00001, 0.01)
    float structural_pruning_rate = 0.0001f;          // Range: (0.00001, 0.001)
    
    float stdp_tau_plus = 20.0f;                // Range: (5, 100)
    float stdp_tau_minus = 20.0f;               // Range: (5, 100)
    
    bool validate() const;
    std::string toString() const;
};

// Additional schema definitions for neuromodulation, development, etc.
```

#### Configuration Validation Framework
```cpp
class ConfigValidator {
public:
    enum ValidationLevel { STRICT, WARNINGS_ONLY, NONE };
    
    static bool validate(const Config& config, 
                        ValidationLevel level = STRICT,
                        std::vector<std::string>& errors = nullptr,
                        std::vector<std::string>& warnings = nullptr);
    
    static bool validate(const std::string& filepath,
                        ConfigFormat format = AUTO_DETECT);
    
    static bool validate(const json& config_json,
                        const json& schema);
    
    static std::string getValidationReport(const Config& config);
    
    static bool isCompatible(const Config& config, 
                            const std::string& phase);
};