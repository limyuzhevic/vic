# NLM Advanced Configuration Presets

This file provides advanced configuration presets for different experimental scenarios.

## Available Presets

### 1. Neural Debate (`neural_debate.json`)
**Purpose**: Optimized for studying neural mechanisms of argumentation and reasoning
**Key Features**:
- High neuromodulation levels for exploratory behavior
- Enhanced working memory for argument maintenance
- Strong curiosity drives exploration of novel perspectives
- Balanced plasticity for both learning and stability

```json
{
  "description": "Configuration for neural debate and argumentation experiments",
  
  "basic": {
    "neuron_count": 1500,
    "region_count": 3,
    "simulation_timestep": 0.001
  },
  
  "plasticity": {
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 30.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002
  },
  
  "neuromodulation": {
    "dopamine_baseline": 0.3,
    "curiosity_enabled": true,
    "novelty_sensitivity": 0.8,
    "reward_discount_factor": 0.95
  },
  
  "memory": {
    "working_memory_capacity": 200,
    "episodic_memory_max_episodes": 500,
    "memory_replay_interval": 50
  },
  
  "prediction": {
    "prediction_horizon": 10,
    "prediction_error_weight": 0.5
  },
  
  "cognition": {
    "planner_planning_depth": 8,
    "concept_formation_enabled": true,
    "attention_inhibition_strength": 0.6
  },
  
  "development": {
    "development_enabled": true,
    "developmental_stage": "adult",
    "plasticity_modifier": 0.2
  },
  
  "environment": {
    "world_width": 30,
    "world_height": 30,
    "vision_range": 20,
    "action_interval": 0.1
  },
  
  "performance": {
    "checkpoint_interval": 2000,
    "enable_visualization": false,
    "max_simulation_steps": 20000
  }
}
```

### 2. Memory Task (`memory_task.json`)
**Purpose**: Focused on memory performance and learning capacity
**Key Features**:
- Large working memory for complex tasks
- Enhanced episodic memory consolidation
- Balanced novelty-seeking with stability
- Repetition suppression for efficient learning

```json
{
  "description": "Configuration for memory task experiments",
  
  "basic": {
    "neuron_count": 2000,
    "region_count": 2,
    "simulation_timestep": 0.001
  },
  
  "plasticity": {
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.013,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
  },
  
  "neuromodulation": {
    "dopamine_baseline": 0.2,
    "curiosity_enabled": true,
    "novelty_sensitivity": 0.5,
    "reward_discount_factor": 0.98
  },
  
  "memory": {
    "working_memory_capacity": 500,
    "episodic_memory_max_episodes": 1000,
    "memory_replay_interval": 100,
    "consolidation_threshold": 0.7
  },
  
  "prediction": {
    "prediction_horizon": 20,
    "prediction_error_weight": 0.3
  },
  
  "cognition": {
    "planner_planning_depth": 5,
    "concept_formation_enabled": true,
    "attention_inhibition_strength": 0.8
  },
  
  "environment": {
    "world_width": 40,
    "world_height": 40,
    "vision_range": 24,
    "action_interval": 0.1
  }
}
```

### 3. Development Study (`development.json`)
**Purpose**: Study developmental processes and plasticity changes over time
**Key Features**:
- Strong developmental effects
- Stage-dependent plasticity
- Aging-related changes
- Critical period phenomena

```json
{
  "description": "Configuration for developmental process experiments",
  
  "basic": {
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001
  },
  
  "plasticity": {
    "stdp_ltp_weight": 0.025,
    "stdp_ltd_weight": 0.023,
    "stdp_tau": 35.0,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.0005
  },
  
  "neuromodulation": {
    "dopamine_baseline": 0.1,
    "curiosity_enabled": true,
    "novelty_sensitivity": 0.3,
    "reward_discount_factor": 0.99
  },
  
  "development": {
    "development_enabled": true,
    "developmental_stage": "initial",
    "plasticity_modifier": 1.5,
    "critical_period_enabled": true,
    "maturation_enabled": true
  },
  
  "environment": {
    "world_width": 20,
    "world_height": 20,
    "vision_range": 16,
    "action_interval": 0.1
  }
}
```

## Using Presets

### Command Line Usage

```bash
# Load neural debate preset
nlm --preset neural_debate.json

# Load memory task preset
nlm --preset memory_task.json

# Load development study preset
nlm --preset development.json

# Create custom preset by merging
nlm --preset neural_debate.json --merge memory_task.json --output custom_preset.json
```

### Programmatic Usage

```cpp
#include "core/Config/Config.hpp"
#include <nlm/ConfigPresets.hpp>

using namespace nlm;

int main() {
    Config config;
    
    // Load neural debate preset
    if (config.applyPreset("neural_debate")) {
        std::cout << "Loaded neural debate preset" << std::endl;
    }
    
    // Load from file
    if (config.loadFromFile("memory_task.json")) {
        std::cout << "Loaded memory task preset" << std::endl;
    }
    
    // Get available presets
    auto presets = Config::getAvailablePresets();
    for (const auto& preset : presets) {
        std::cout << "Available preset: " << preset << std::endl;
    }
    
    return 0;
}
```

### Preset Categories

1. **Cognitive Experiments**: neural_debate, concept_formation, attention_control
2. **Memory Studies**: memory_task, episodic_memory, working_memory
3. **Development**: development, critical_period, maturation, aging
4. **Neuromodulation**: curiosity, novelty, reward, exploration
5. **Performance**: high_performance, real_time, embedded

## Preset Modification

### Adding a New Preset

1. **Create JSON file**: Save in `configs/presets/` directory
2. **Register preset**: Add entry to preset registry
3. **Test configuration**: Validate preset with system tests
4. **Document**: Add description to documentation

### Modifying Existing Preset

```bash
# Backup original
nlm --preset neural_debate.json --backup

# Edit preset
# (Edit the JSON file)

# Validate modified preset
nlm --validate-preset modified_neural_debate.json

# Load modified preset
nlm --preset modified_neural_debate.json
```

## Preset Validation

### Validation Rules

1. **Syntax Validation**: JSON format validation
2. **Type Validation**: Configuration type validation
3. **Range Validation**: Value range validation
4. **Dependency Validation**: Configuration dependency validation
5. **Performance Validation**: Performance impact assessment

### Validation Tools

```bash
# Validate all presets
nlm --validate-all-presets

# Validate specific preset
nlm --validate-preset neural_debate.json

# Show validation errors
nlm --validate-preset memory_task.json --verbose
```

## Future Preset Extensions

### 1. Dynamic Presets

Presets that can change during runtime based on conditions:

```json
{
  "dynamic_preset": {
    "conditions": ["curiosity_level > 0.8"],
    "action": "switch_to_curiosity_mode",
    "target_preset": "exploration_preset"
  }
}
```

### 2. AI-Generated Presets

AI-assisted preset generation:

```json
{
  "ai_generation": {
    "prompt": "Create preset for argument learning",
    "parameters": {
      "neuron_count": "optimize_for_memory",
      "plasticity": "balanced_learning"
    }
  }
}
```

## Conclusion

These advanced configuration presets provide powerful tools for:
- **Experimental Control**: Precise control over experimental conditions
- **Reproducibility**: Consistent configuration across experiments
- **Comparison**: Easy comparison between different experimental setups
- **Exploration**: Rapid exploration of parameter spaces
- **Automation**: Automated configuration management

The preset system, combined with the enhanced configuration management, makes NLM more accessible to both beginners and advanced researchers, enabling more efficient and reliable neural experiments.