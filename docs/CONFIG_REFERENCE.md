# NLM Configuration Reference

## Overview
NLM's configuration system provides a comprehensive set of configuration options for neural network simulation, plasticity, memory, and system behavior.

## Configuration Categories

### Core Configuration
Core configuration parameters control the basic neural network simulation.

### Plasticity Configuration
Plasticity configuration parameters control learning rules and synaptic changes.

### Memory Configuration
Memory configuration parameters control working and episodic memory systems.

### Development Configuration
Development configuration parameters control developmental processes.

### Neuromodulation Configuration
Neuromodulation configuration parameters control dopamine, curiosity, and other neuromodulators.

### Environment Configuration
Environment configuration parameters control the simulation environment.

### Logging Configuration
Logging configuration parameters control logging behavior.

### Performance Configuration
Performance configuration parameters control system performance.

## Core Configuration

### neuron_count
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 1000 |
| Minimum | 10 |
| Maximum | 100000 |
| Description | Number of neurons in the brain |

### region_count
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 2 |
| Minimum | 1 |
| Maximum | 100 |
| Description | Number of brain regions |

### connection_probability
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.1 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Probability of connection between neurons |

### simulation_timestep
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.001 |
| Minimum | 0.0001 |
| Maximum | 0.1 |
| Description | Simulation timestep in seconds |

### random_seed
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 42 |
| Minimum | 0 |
| Maximum | 2147483647 |
| Description | Random seed for reproducible experiments |

## Plasticity Configuration

### plasticity_learning_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.01 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Global learning rate for plasticity rules |

### stdp_ltp_weight
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.01 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | STDP LTP weight |

### stdp_ltd_weight
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.012 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | STDP LTD weight |

### stdp_time_constant
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 20.0 |
| Minimum | 0.0 |
| Maximum | 100.0 |
| Description | STDP time constant |

### hebbian_learning_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.005 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Hebbian learning rate |

### reward_modulated_learning_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.008 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Reward-modulated learning rate |

## Development Configuration

### development_synaptogenesis_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.001 |
| Minimum | 0.0 |
| Maximum | 0.1 |
| Description | Synaptogenesis rate |

### development_pruning_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.0001 |
| Minimum | 0.0 |
| Maximum | 0.01 |
| Description | Pruning rate |

### development_stage_duration
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 100.0 |
| Minimum | 0.0 |
| Maximum | 1000.0 |
| Description | Development stage duration |

### development_maturation_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.5 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Maturation rate |

## Neuromodulation Configuration

### dopamine_baseline
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.0 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Baseline dopamine level |

### dopamine_modulation_strength
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.1 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Dopamine modulation strength |

### curiosity_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.1 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Curiosity modulation rate |

### novelty_threshold
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.5 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Novelty threshold |

### prediction_error_gain
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 1.0 |
| Minimum | 0.0 |
| Maximum | 10.0 |
| Description | Prediction error gain |

## Memory Configuration

### working_memory_capacity
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 100 |
| Minimum | 10 |
| Maximum | 1000 |
| Description | Working memory capacity |

### working_memory_decay_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.01 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Working memory decay rate |

### episodic_memory_max_episodes
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 1000 |
| Minimum | 100 |
| Maximum | 10000 |
| Description | Maximum episodic memory episodes |

### episodic_memory_consolidation_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.1 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Episodic memory consolidation rate |

### episodic_memory_replay_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.2 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Episodic memory replay rate |

## Environment Configuration

### environment_name
| Property | Value |
|----------|-------|
| Type | String |
| Default | GridWorld |
| Description | Name of the environment |

### environment_width
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 10 |
| Minimum | 1 |
| Maximum | 100 |
| Description | Width of the environment |

### environment_height
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 10 |
| Minimum | 1 |
| Maximum | 100 |
| Description | Height of the environment |

### reward_discount_factor
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.99 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Reward discount factor |

### exploration_bonus
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.1 |
| Minimum | 0.0 |
| Maximum | 1.0 |
| Description | Exploration bonus |

## Logging Configuration

### log_level
| Property | Value |
|----------|-------|
| Type | String |
| Default | INFO |
| Description | Log level |

### log_to_file
| Property | Value |
|----------|-------|
| Type | Boolean |
| Default | false |
| Description | Whether to log to file |

### log_filename
| Property | Value |
|----------|-------|
| Type | String |
| Default | nlm.log |
| Description | Log filename |

### log_max_size
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 10485760 |
| Minimum | 1024 |
| Maximum | 1073741824 |
| Description | Maximum log file size in bytes |

### log_rotation
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 5 |
| Minimum | 1 |
| Maximum | 100 |
| Description | Number of log files to keep |

## Performance Configuration

### max_simulation_steps
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 10000 |
| Minimum | 100 |
| Maximum | 1000000 |
| Description | Maximum number of simulation steps |

### simulation_time_limit
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 0.0 |
| Minimum | 0.0 |
| Maximum | 1000.0 |
| Description | Simulation time limit in seconds |

### visualization_enabled
| Property | Value |
|----------|-------|
| Type | Boolean |
| Default | false |
| Description | Whether to enable visualization |

### visualization_update_rate
| Property | Value |
|----------|-------|
| Type | Float |
| Default | 30.0 |
| Minimum | 1.0 |
| Maximum | 1000.0 |
| Description | Visualization update rate in Hz |

### checkpoint_enabled
| Property | Value |
|----------|-------|
| Type | Boolean |
| Default | false |
| Description | Whether to enable checkpointing |

### checkpoint_interval
| Property | Value |
|----------|-------|
| Type | Integer |
| Default | 1000 |
| Minimum | 100 |
| Maximum | 100000 |
| Description | Checkpoint interval in steps |

### checkpoint_filename
| Property | Value |
|----------|-------|
| Type | String |
| Default | checkpoint.bin |
| Description | Checkpoint filename |

## Configuration File Formats

### NLM Configuration Format
```
# NLM Configuration File
# This is a configuration file for NLM

# Core configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

# Plasticity configuration
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development configuration
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation configuration
dopamine_baseline = 0.1
dopamine_modulation_strength = 0.1
curiosity_rate = 0.2

# Memory configuration
working_memory_capacity = 100
working_memory_decay_rate = 0.01
episodic_memory_max_episodes = 1000

# Environment configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10
reward_discount_factor = 0.99

# Logging configuration
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Performance configuration
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
```

### YAML Configuration Format
```yaml
nlm:
  # Core configuration
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.15
  simulation_timestep: 0.001
  random_seed: 42
  
  # Plasticity configuration
  plasticity:
    learning_rate: 0.02
    stdp:
      ltp_weight: 0.02
      ltd_weight: 0.015
      time_constant: 20.0
    hebbian:
      learning_rate: 0.005
      
  # Development configuration
  development:
    synaptogenesis_rate: 0.001
    pruning_rate: 0.0001
    
  # Neuromodulation configuration
  neuromodulation:
    dopamine:
      baseline: 0.1
      modulation_strength: 0.1
    curiosity:
      rate: 0.2
      
  # Memory configuration
  memory:
    working_memory:
      capacity: 100
      decay_rate: 0.01
    episodic_memory:
      max_episodes: 1000
      consolidation_rate: 0.1
      replay_rate: 0.2
      
  # Environment configuration
  environment:
    name: GridWorld
    width: 10
    height: 10
    reward_discount_factor: 0.99
    exploration_bonus: 0.1
    
  # Logging configuration
  logging:
    level: INFO
    to_file: true
    filename: nlm.log
    max_size: 10485760
    rotation: 5
    
  # Performance configuration
  performance:
    max_simulation_steps: 10000
    simulation_time_limit: 0.0
    visualization_enabled: false
    visualization_update_rate: 30.0
    checkpoint_enabled: false
    checkpoint_interval: 1000
    checkpoint_filename: checkpoint.bin
```

### JSON Configuration Format
```json
{
  "nlm": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    
    "plasticity": {
      "learning_rate": 0.02,
      "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "time_constant": 20.0
      },
      "hebbian": {
        "learning_rate": 0.005
      }
    },
    
    "development": {
      "synaptogenesis_rate": 0.001,
      "pruning_rate": 0.0001
    },
    
    "neuromodulation": {
      "dopamine": {
        "baseline": 0.1,
        "modulation_strength": 0.1
      },
      "curiosity": {
        "rate": 0.2
      }
    },
    
    "memory": {
      "working_memory": {
        "capacity": 100,
        "decay_rate": 0.01
      },
      "episodic_memory": {
        "max_episodes": 1000,
        "consolidation_rate": 0.1,
        "replay_rate": 0.2
      }
    },
    
    "environment": {
      "name": "GridWorld",
      "width": 10,
      "height": 10,
      "reward_discount_factor": 0.99,
      "exploration_bonus": 0.1
    },
    
    "logging": {
      "level": "INFO",
      "to_file": true,
      "filename": "nlm.log",
      "max_size": 10485760,
      "rotation": 5
    },
    
    "performance": {
      "max_simulation_steps": 10000,
      "simulation_time_limit": 0.0,
      "visualization_enabled": false,
      "visualization_update_rate": 30.0,
      "checkpoint_enabled": false,
      "checkpoint_interval": 1000,
      "checkpoint_filename": "checkpoint.bin"
    }
  }
}
```

## Configuration Validation

### Required Parameters
| Parameter | Description |
|----------|-------------|
| neuron_count | Number of neurons |
| region_count | Number of brain regions |
| connection_probability | Probability of connection between neurons |
| simulation_timestep | Simulation timestep in seconds |

### Optional Parameters
| Parameter | Default | Description |
|----------|---------|-------------|
| random_seed | 42 | Random seed |
| plasticity_learning_rate | 0.01 | Global learning rate |
| development_synaptogenesis_rate | 0.001 | Synaptogenesis rate |
| development_pruning_rate | 0.0001 | Pruning rate |
| dopamine_baseline | 0.0 | Baseline dopamine level |
| curiosity_rate | 0.1 | Curiosity modulation rate |
| working_memory_capacity | 100 | Working memory capacity |
| episodic_memory_max_episodes | 1000 | Maximum episodic memory episodes |

## Configuration Validation Rules

### Core Validation Rules
1. **Neuron Count**: Must be between 10 and 100000
2. **Region Count**: Must be between 1 and 100
3. **Connection Probability**: Must be between 0.0 and 1.0
4. **Simulation Time Step**: Must be between 0.0001 and 0.1

### Plasticity Validation Rules
1. **Learning Rate**: Must be between 0.0 and 1.0
2. **STDP Weights**: Must be between 0.0 and 1.0
3. **STDP Time Constant**: Must be between 0.0 and 100.0
4. **Hebbian Learning Rate**: Must be between 0.0 and 1.0
5. **Reward-Modulated Learning Rate**: Must be between 0.0 and 1.0

### Development Validation Rules
1. **Synaptogenesis Rate**: Must be between 0.0 and 0.1
2. **Pruning Rate**: Must be between 0.0 and 0.01
3. **Development Stage Duration**: Must be between 0.0 and 1000.0
4. **Maturation Rate**: Must be between 0.0 and 1.0

### Neuromodulation Validation Rules
1. **Dopamine Baseline**: Must be between 0.0 and 1.0
2. **Dopamine Modulation Strength**: Must be between 0.0 and 1.0
3. **Curiosity Rate**: Must be between 0.0 and 1.0
4. **Novelty Threshold**: Must be between 0.0 and 1.0
5. **Prediction Error Gain**: Must be between 0.0 and 10.0

### Memory Validation Rules
1. **Working Memory Capacity**: Must be between 10 and 1000
2. **Working Memory Decay Rate**: Must be between 0.0 and 1.0
3. **Episodic Memory Max Episodes**: Must be between 100 and 10000
4. **Episodic Memory Consolidation Rate**: Must be between 0.0 and 1.0
5. **Episodic Memory Replay Rate**: Must be between 0.0 and 1.0

### Environment Validation Rules
1. **Environment Width**: Must be between 1 and 100
2. **Environment Height**: Must be between 1 and 100
3. **Reward Discount Factor**: Must be between 0.0 and 1.0
4. **Exploration Bonus**: Must be between 0.0 and 1.0

### Performance Validation Rules
1. **Max Simulation Steps**: Must be between 100 and 1000000
2. **Simulation Time Limit**: Must be between 0.0 and 1000.0
3. **Visualization Update Rate**: Must be between 1.0 and 1000.0
4. **Checkpoint Interval**: Must be between 100 and 100000

## Configuration Examples

### Basic Configuration
```ini
# Basic NLM configuration
neuron_count = 100
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001

# Plasticity
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Memory
working_memory_capacity = 50
episodic_memory_max_episodes = 500

# Performance
max_simulation_steps = 1000
```

### Advanced Configuration
```ini
# Advanced NLM configuration
neuron_count = 5000
region_count = 3
connection_probability = 0.15
simulation_timestep = 0.0005

# Plasticity
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1
curiosity_rate = 0.2

# Memory
working_memory_capacity = 500
working_memory_decay_rate = 0.01
episodic_memory_max_episodes = 5000

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20
reward_discount_factor = 0.99

# Performance
max_simulation_steps = 100000
visualization_enabled = true
visualization_update_rate = 60.0
checkpoint_enabled = true
checkpoint_interval = 5000
```

### Experiment Configuration
```ini
# Experiment NLM configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

# Plasticity
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Memory
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Environment
environment_name = GridWorld
environment_width = 10
environment_height = 10
reward_discount_factor = 0.99

# Performance
max_simulation_steps = 10000
visualization_enabled = false
```

### Benchmarking Configuration
```ini
# Benchmarking NLM configuration
neuron_count = 2000
region_count = 3
connection_probability = 0.2
simulation_timestep = 0.0005

# Performance
max_simulation_steps = 50000
checkpoint_enabled = true
checkpoint_interval = 10000

# Logging
log_level = WARNING
log_to_file = true
log_filename = benchmark.log
```

## Configuration Commands

### Set Configuration
```bash
nlm --set-config <key> <value>
```

**Examples:**
```bash
# Set neuron count
nlm --set-config neuron_count 1000

# Set connection probability
nlm --set-config connection_probability 0.15

# Set learning rate
nlm --set-config plasticity_learning_rate 0.02
```

### Get Configuration
```bash
nlm --get-config <key>
```

**Examples:**
```bash
# Get neuron count
nlm --get-config neuron_count

# Get connection probability
nlm --get-config connection_probability
```

### List Configuration
```bash
nlm --list-config
```

### Validate Configuration
```bash
nlm --validate-config
```

### Export Configuration
```bash
nlm --export-config <file>
```

**Examples:**
```bash
# Export to YAML file
nlm --export-config config.yaml

# Export to JSON file
nlm --export-config config.json
```

### Import Configuration
```bash
nlm --import-config <file>
```

**Examples:**
```bash
# Import from YAML file
nlm --import-config config.yaml

# Import from JSON file
nlm --import-config config.json
```

## Configuration API

### Configuration Class
```cpp
class Config {
public:
    Config();
    ~Config();
    
    // Configuration management
    bool loadFromFile(const std::string& filename);
    bool loadFromYAML(const std::string& filename);
    bool loadFromJSON(const std::string& filename);
    
    bool saveToFile(const std::string& filename) const;
    bool saveToYAML(const std::string& filename) const;
    bool saveToJSON(const std::string& filename) const;
    
    // Configuration parameters
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, float value);
    
    std::string get(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration summary
    std::string summary() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration copying
    Config(const Config& other);
    Config& operator=(const Config& other);
    
    // Configuration merging
    void merge(const Config& other);
    
    // Configuration clearing
    void clear();
};
```

### Configuration Factory
```cpp
class ConfigFactory {
public:
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createConfigFromFile(const std::string& filename);
    static std::shared_ptr<Config> createConfigFromString(const std::string& configString);
    
    static std::shared_ptr<Config> createConfigFromMap(const std::map<std::string, std::string>& configMap);
    static std::shared_ptr<Config> createConfigFromMap(const std::map<std::string, int>& configMap);
    static std::shared_ptr<Config> createConfigFromMap(const std::map<std::string, float>& configMap);
    
    static std::shared_ptr<Config> createConfigFromYAML(const std::string& filename);
    static std::shared_ptr<Config> createConfigFromJSON(const std::string& filename);
};
```

### Configuration Validator
```cpp
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    void addValidationRule(const ValidationRule& rule);
    bool validate(const Config& config) const;
    std::vector<std::string> getValidationErrors(const Config& config) const;
    
    // Built-in validation rules
    static ValidationRule createNeuronCountRule(int min = 10, int max = 100000);
    static ValidationRule createRegionCountRule(int min = 1, int max = 100);
    static ValidationRule createConnectionProbabilityRule(float min = 0.0, float max = 1.0);
    static ValidationRule createSimulationTimestepRule(float min = 0.0001, float max = 0.1);
    
    // Custom validation rules
    static ValidationRule createCustomRule(const std::string& name,
                                          bool (*validator)(const Config&),
                                          const std::string& errorMessage);
};
```

## Configuration Testing

### Configuration Testing
```cpp
#include <gtest/gtest.h>
#include "Config.hpp"

TEST(ConfigTest, DefaultConfiguration) {
    Config config;
    EXPECT_EQ(config.getInt("neuron_count"), 1000);
    EXPECT_EQ(config.getInt("region_count"), 2);
    EXPECT_EQ(config.getFloat("connection_probability"), 0.1f);
}

TEST(ConfigTest, LoadFromFile) {
    Config config;
    EXPECT_TRUE(config.loadFromFile("config.cfg"));
    EXPECT_EQ(config.getInt("neuron_count"), 1000);
}

TEST(ConfigTest, ValidateConfiguration) {
    Config config;
    EXPECT_TRUE(config.validate());
}

TEST(ConfigTest, InvalidConfiguration) {
    Config config;
    config.set("neuron_count", 0); // Invalid
    EXPECT_FALSE(config.validate());
}
```

## Configuration Migration

### Configuration Migration
```cpp
class ConfigMigration {
public:
    static std::shared_ptr<Config> migrate(const std::string& oldFormat, const std::string& oldConfig);
    static std::shared_ptr<Config> migrate(const std::string& filename);
    
    // Migration from v1.0 to v2.0
    static std::shared_ptr<Config> migrate_v1_to_v2(const std::string& oldConfig);
    
    // Migration from v2.0 to v3.0
    static std::shared_ptr<Config> migrate_v2_to_v3(const std::string& oldConfig);
    
    // Migration from JSON to YAML
    static std::shared_ptr<Config> migrate_json_to_yaml(const std::string& jsonConfig);
    
    // Migration from YAML to JSON
    static std::shared_ptr<Config> migrate_yaml_to_json(const std::string& yamlConfig);
};
```

## Configuration Documentation

### Configuration Documentation Generator
```cpp
class ConfigDocumentationGenerator {
public:
    static std::string generateConfigDocumentation(const Config& config);
    static std::string generateConfigSchema(const Config& config);
    static std::string generateConfigExamples(const Config& config);
    
    // Generate configuration documentation
    static std::string generateConfigDocumentation(
        const std::vector<std::shared_ptr<Config>>& configs);
    
    // Generate configuration schema
    static std::string generateConfigSchema(
        const std::vector<std::shared_ptr<Config>>& configs);
    
    // Generate configuration examples
    static std::string generateConfigExamples(
        const std::vector<std::shared_ptr<Config>>& configs);
};
```

## Configuration Best Practices

### 1. Use Default Configuration
Use default configuration for basic simulations and customize only as needed.

### 2. Validate Configuration
Always validate configuration before running simulations.

### 3. Use Configuration Files
Use configuration files for complex simulations and experiments.

### 4. Document Configuration
Document configuration parameters with descriptions and examples.

### 5. Test Configuration
Test configuration changes before using them in production.

## Configuration Support

### Configuration Documentation
- **Documentation**: https://nlm.readthedocs.io/configuration
- **API Reference**: https://nlm.readthedocs.io/api/configuration
- **Configuration Examples**: https://github.com/nlm/configuration-examples

### Configuration Support
- **Issue Tracker**: https://github.com/nlm/configuration/issues
- **Forum**: https://forum.nlm.ai/configuration
- **Slack**: https://slack.nlm.ai/configuration

### Configuration Development
- **Contributing**: https://github.com/nlm/configuration/blob/main/CONTRIBUTING.md
- **Code Style**: https://github.com/nlm/configuration/blob/main/CODE_STYLE.md
- **Testing**: https://github.com/nlm/configuration/blob/main/TESTING.md

## Conclusion

The NLM configuration system provides a comprehensive set of configuration options for neural network simulation, plasticity, memory, and system behavior. This reference guide covers all available configuration options, validation rules, and examples to help users effectively use the NLM configuration system.

For more information about specific configuration options or features, use the `--help` option or refer to the NLM documentation.
