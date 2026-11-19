# NLM Configuration System

## Overview

A comprehensive configuration system for NLM with command-line interface, file-based configuration, environment variable support, and configuration validation.

## Usage

### Basic Usage

```bash
# Create a configuration file
nlm-config create my_config

# Set configuration values
nlm-config set my_config neuron_count 2000
nlm-config set my_config enable_stdp true

# Apply configuration to NLM demos
./nlm_phase3_demo --config my_config

# List all configurations
nlm-config list

# Show configuration details
nlm-config show my_config

# Validate configuration
nlm-config validate my_config

# Generate configuration from template
generate_nlm_config beginner
```

## Configuration File Format

### Basic Configuration

```json
{
    "name": "my_configuration",
    "version": "1.0",
    "description": "A basic NLM configuration",
    
    "neural": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "max_spike_frequency": 200.0
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0
    },
    
    "memory": {
        "working_memory_capacity": 50,
        "episodic_memory_limit": 20,
        "enable_replay": false,
        "replay_frequency": 0.1
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_window_size": 10
    },
    
    "environment": {
        "world_width": 50,
        "world_height": 50,
        "vision_range": 16,
        "action_interval": 0.1
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true,
        "dopamine_sensitivity": 1.0
    }
}
```

### Advanced Configuration

```json
{
    "name": "advanced_configuration",
    "version": "2.0",
    "expert_mode": true,
    "description": "An advanced NLM configuration for research",
    
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true,
        "enable_event_driven": true,
        "max_spike_frequency": 400.0,
        "refractory_period": 0.002,
        "integration_method": "exponential",
        "noise_level": 0.1
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "enable_stdp_gating": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 30.0,
        "homeostatic_target": 1.0,
        "plasticity_rate": 1.0
    },
    
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 100,
        "enable_replay": true,
        "replay_window": 2000,
        "replay_frequency": 0.2,
        "enable_conjunctive_memory": true,
        "memory_decay_rate": 0.001,
        "memory_reinforcement_strength": 0.5
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_selectivity": 0.8,
        "enable_self_model": true,
        "self_model_confidence": 0.9,
        "concept_stability": 0.7
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true,
        "dopamine_sensitivity": 1.5,
        "curiosity_drive": 0.8,
        "novelty_threshold": 0.1,
        "reward_prediction_error": true
    },
    
    "development": {
        "enable_development": true,
        "developmental_stages": [
            "initial", "critical_period", "maturation", "adult", "aging"
        ],
        "plasticity_rate_by_stage": {
            "initial": 2.0,
            "critical_period": 4.0,
            "maturation": 2.0,
            "adult": 1.0,
            "aging": 0.5
        },
        "development_schedule": {
            "initial_duration": 1000,
            "critical_period_duration": 5000,
            "maturation_duration": 20000
        }
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 12,
        "enable_simd": true,
        "enable_event_driven": true,
        "checkpoint_interval": 5000,
        "enable_profiling": true,
        "profile_interval": 100,
        "generate_visualizations": true,
        "visualization_formats": ["png", "svg", "pdf"],
        "output_csv": true,
        "csv_path": "./results",
        "save_checkpoints": true,
        "checkpoint_path": "./checkpoints"
    },
    
    "environment": {
        "world_width": 100,
        "world_height": 100,
        "vision_range": 16,
        "action_interval": 0.1,
        "enable_agent_collision": true,
        "enable_resource_decay": false,
        "enable_hazard_movement": true
    }
}
```

## Configuration Keys Reference

### Neural Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 500 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of synapse formation |
| `simulation_timestep` | float | 0.001 | Time step for simulation (seconds) |
| `max_spike_frequency` | float | 200.0 | Maximum firing rate (spikes/second) |
| `enable_advanced_dynamics` | bool | false | Enable advanced neuron dynamics |
| `enable_sparse_connectivity` | bool | false | Use sparse connectivity optimization |
| `enable_event_driven` | bool | false | Enable event-driven simulation |
| `refractory_period` | float | 0.002 | Neuron refractory period (seconds) |
| `integration_method` | string | "forwardEuler" | Integration method |
| `noise_level` | float | 0.0 | Neural noise level |

### Plasticity Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_stdp` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `enable_hebbian` | bool | true | Enable Hebbian learning |
| `enable_structural` | bool | false | Enable structural plasticity |
| `enable_homeostatic` | bool | false | Enable homeostatic plasticity |
| `enable_stdp_gating` | bool | false | Enable STDP gating mechanism |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `homeostatic_target` | float | 1.0 | Homeostatic target firing rate |
| `plasticity_rate` | float | 1.0 | Overall plasticity rate |

### Memory Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `working_memory_capacity` | int | 50 | Size of working memory |
| `episodic_memory_limit` | int | 20 | Maximum number of episodes |
| `enable_replay` | bool | false | Enable memory replay |
| `replay_window` | int | 1000 | Size of replay window |
| `replay_frequency` | float | 0.1 | Frequency of replay |
| `enable_conjunctive_memory` | bool | false | Enable conjunctive memory |
| `memory_decay_rate` | float | 0.001 | Memory decay rate |
| `memory_reinforcement_strength` | float | 0.5 | Memory reinforcement strength |

### Cognition Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_concept_formation` | bool | true | Enable concept formation |
| `enable_planning` | bool | true | Enable planning module |
| `enable_attention` | bool | true | Enable attention mechanism |
| `attention_window_size` | int | 10 | Size of attention window |
| `attention_selectivity` | float | 0.5 | Attention selectivity threshold |
| `enable_self_model` | bool | false | Enable self-model |
| `self_model_confidence` | float | 0.5 | Self-model confidence |
| `concept_stability` | float | 0.5 | Concept stability |

### Neuromodulation Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_dopamine` | bool | true | Enable dopamine system |
| `enable_curiosity` | bool | true | Enable curiosity system |
| `enable_novelty` | bool | true | Enable novelty detection |
| `dopamine_sensitivity` | float | 1.0 | Dopamine sensitivity |
| `curiosity_drive` | float | 0.5 | Curiosity drive strength |
| `novelty_threshold` | float | 0.1 | Novelty detection threshold |
| `reward_prediction_error` | bool | false | Enable reward prediction error |

### Development Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_development` | bool | true | Enable developmental system |
| `plasticity_rate_by_stage` | object | stage-specific rates | Plasticity rate by developmental stage |
| `development_schedule` | object | standard schedule | Development stage durations |
| `enable_aging` | bool | false | Enable aging process |
| `developmental_plasticity` | float | 1.0 | Developmental plasticity rate |

### Advanced Features Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_multithreading` | bool | false | Enable multithreading |
| `thread_count` | int | 0 | Number of threads (0 = auto) |
| `enable_simd` | bool | false | Enable SIMD optimization |
| `enable_event_driven` | bool | false | Enable event-driven simulation |
| `checkpoint_interval` | int | 1000 | Checkpoint interval |
| `enable_profiling` | bool | false | Enable profiling |
| `profile_interval` | int | 100 | Profiling interval |
| `generate_visualizations` | bool | false | Generate visualizations |
| `visualization_formats` | array | ["png"] | Visualization output formats |
| `output_csv` | bool | false | Output results as CSV |
| `csv_path` | string | "./results" | CSV output path |
| `save_checkpoints` | bool | false | Save checkpoints |
| `checkpoint_path` | string | "./checkpoints" | Checkpoint path |

### Environment Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `world_width` | int | 50 | World width |
| `world_height` | int | 50 | World height |
| `vision_range` | int | 16 | Vision range (view radius) |
| `action_interval` | float | 0.1 | Action interval (seconds) |
| `enable_agent_collision` | bool | true | Enable agent collision detection |
| `enable_resource_decay` | bool | false | Enable resource decay |
| `enable_hazard_movement` | bool | true | Enable hazard movement |

## Command Line Configuration Interface

### Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | `--config my_config.json` |
| `--set <key> <value>` | Set configuration value | `--set neuron_count 2000` |
| `--set-f <key> <float>` | Set float configuration value | `--set-f connection_probability 0.15` |
| `--set-i <key> <int>` | Set int configuration value | `--set-i region_count 3` |
| `--set-b <key> <bool>` | Set bool configuration value | `--set-b enable_stdp true` |
| `--set-s <key> <string>` | Set string configuration value | `--set-s name expert_brain` |
| `--output <file>` | Output configuration to file | `--output expert_config.json` |
| `--validate` | Validate configuration | `--validate` |
| `--help` | Show help message | `--help` |

### Usage Examples

```bash
# Load configuration from file
./nlm_phase3_demo --config expert_config.json

# Override configuration values
./nlm_phase3_demo --config expert_config.json --set neuron_count 3000 --set enable_curiosity true

# Set float configuration
./nlm_phase3_demo --config expert_config.json --set-f connection_probability 0.08

# Set int configuration
./nlm_phase3_demo --config expert_config.json --set-i region_count 5

# Set bool configuration
./nlm_phase3_demo --config expert_config.json --set-b enable_stdp true

# Set string configuration
./nlm_phase3_demo --config expert_config.json --set-s description "Expert neural network"

# Output configuration to file
./nlm_phase3_demo --output my_brain_config.json

# Validate configuration
./nlm_phase3_demo --config my_config.json --validate
```

## Environment Variable Configuration

### NLM Environment Variables

| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `NLM_CONFIG_FILE` | string | "" | Path to configuration file |
| `NLM_NEURON_COUNT` | int | 500 | Number of neurons |
| `NLM_CONNECTION_PROB` | float | 0.1 | Connection probability |
| `NLM_SIMULATION_TIMESTEP` | float | 0.001 | Simulation timestep |
| `NLM_ENABLE_STDP` | bool | true | Enable STDP |
| `NLM_ENABLE_HEBBIAN` | bool | true | Enable Hebbian learning |
| `NLM_ENABLE_STRUCTURAL` | bool | false | Enable structural plasticity |
| `NLM_WORKING_MEMORY_CAPACITY` | int | 50 | Working memory capacity |
| `NLM_ENABLE_CONCEPT_FORMATION` | bool | true | Enable concept formation |
| `NLM_ENABLE_PLANNING` | bool | true | Enable planning |
| `NLM_ENABLE_ATTENTION` | bool | true | Enable attention |
| `NLM_WORLD_WIDTH` | int | 50 | World width |
| `NLM_WORLD_HEIGHT` | int | 50 | World height |
| `NLM_ENABLE_DOPAMINE` | bool | true | Enable dopamine |
| `NLM_ENABLE_CURIOUSITY` | bool | true | Enable curiosity |
| `NLM_ENABLE_NOVELTY` | bool | true | Enable novelty detection |
| `NLM_ENABLE_DEVELOPMENT` | bool | true | Enable development |
| `NLM_LOG_LEVEL` | string | "info" | Log level (debug, info, warning, error) |
| `NLM_OUTPUT_DIR` | string | "./results" | Output directory |
| `NLM_SAVE_CHECKPOINTS` | bool | false | Save checkpoints |

### Environment Variable Usage

```bash
# Set environment variables
export NLM_NEURON_COUNT=2000
export NLM_CONNECTION_PROB=0.15
export NLM_ENABLE_CURIOUSITY=true
export NLM_WORLD_WIDTH=100
export NLM_WORLD_HEIGHT=100

# Run NLM with environment variables
./nlm_phase3_demo

# Override environment variables with command line
./nlm_phase3_demo --set-i neuron_count 3000 --set-b enable_curiosity false
```

## Configuration Validation

### Configuration Validation Rules

The configuration system validates configurations based on the following rules:

1. **Neural Configuration**:
   - `neuron_count` must be > 0
   - `region_count` must be > 0
   - `connection_probability` must be between 0.0 and 1.0
   - `simulation_timestep` must be > 0

2. **Memory Configuration**:
   - `working_memory_capacity` must be > 0
   - `episodic_memory_limit` must be > 0
   - `replay_window` must be > 0 if `enable_replay` is true

3. **Cognition Configuration**:
   - `attention_window_size` must be > 0
   - `attention_selectivity` must be between 0.0 and 1.0
   - `self_model_confidence` must be between 0.0 and 1.0

4. **Environment Configuration**:
   - `world_width` must be > 0
   - `world_height` must be > 0
   - `vision_range` must be > 0
   - `action_interval` must be > 0

### Validation Examples

```bash
# Validate configuration
nlm-config validate my_config.json

# Validate with verbose output
nlm-config validate my_config.json --verbose

# Validate and show warnings
nlm-config validate my_config.json --show-warnings

# Validate configuration from command line
./nlm_phase3_demo --config my_config.json --validate
```

## Configuration Templates

### Beginner Configuration Template

```json
{
    "name": "beginner_configuration",
    "version": "1.0",
    "description": "A simple configuration suitable for beginners",
    
    "neural": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": false,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    },
    
    "memory": {
        "working_memory_capacity": 50,
        "episodic_memory_limit": 20,
        "enable_replay": false
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true
    },
    
    "environment": {
        "world_width": 50,
        "world_height": 50,
        "vision_range": 16,
        "action_interval": 0.1
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true
    }
}
```

### Research Configuration Template

```json
{
    "name": "research_configuration",
    "version": "2.0",
    "expert_mode": true,
    "description": "An advanced configuration for research and experimentation",
    
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true,
        "enable_event_driven": true,
        "max_spike_frequency": 400.0,
        "refractory_period": 0.002
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "enable_stdp_gating": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 30.0,
        "homeostatic_target": 1.0
    },
    
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 100,
        "enable_replay": true,
        "replay_window": 2000,
        "replay_frequency": 0.2,
        "enable_conjunctive_memory": true,
        "memory_decay_rate": 0.001
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_selectivity": 0.8,
        "enable_self_model": true,
        "self_model_confidence": 0.9,
        "concept_stability": 0.7
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true,
        "dopamine_sensitivity": 1.5,
        "curiosity_drive": 0.8,
        "novelty_threshold": 0.1
    },
    
    "development": {
        "enable_development": true,
        "plasticity_rate_by_stage": {
            "initial": 2.0,
            "critical_period": 4.0,
            "maturation": 2.0,
            "adult": 1.0,
            "aging": 0.5
        }
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 12,
        "enable_simd": true,
        "enable_event_driven": true,
        "checkpoint_interval": 5000,
        "enable_profiling": true,
        "profile_interval": 100,
        "generate_visualizations": true,
        "visualization_formats": ["png", "svg", "pdf"],
        "output_csv": true,
        "csv_path": "./research_results"
    }
}
```

## Configuration Management

### Configuration Commands

The NLM configuration system includes the following commands:

#### `nlm-config` - Configuration Management Tool

**Commands:**

- `list`: List all configurations
- `show <name>`: Show configuration details
- `create <name> <file>`: Create new configuration
- `set <name> <key> <value>`: Set configuration value
- `set-f <name> <key> <float>`: Set float configuration value
- `set-i <name> <key> <int>`: Set int configuration value
- `set-b <name> <key> <bool>`: Set bool configuration value
- `set-s <name> <key> <string>`: Set string configuration value
- `remove <name> <key>`: Remove configuration key
- `export <name> <file>`: Export configuration to file
- `import <file> <name>`: Import configuration from file
- `copy <source> <destination>`: Copy configuration
- `move <source> <destination>`: Move configuration
- `delete <name>`: Delete configuration
- `validate <name>`: Validate configuration
- `validate-file <file>`: Validate configuration file

**Examples:**

```bash
# List all configurations
nlm-config list

# Show configuration details
nlm-config show expert_configuration

# Create new configuration
nlm-config create my_brain

# Set configuration value
nlm-config set my_brain neuron_count 2000

# Set float configuration value
nlm-config set-f my_brain connection_probability 0.15

# Set int configuration value
nlm-config set-i my_brain region_count 3

# Set bool configuration value
nlm-config set-b my_brain enable_stdp true

# Set string configuration value
nlm-config set-s my_brain description "Expert neural network"

# Remove configuration key
nlm-config remove my_brain description

# Export configuration to file
nlm-config export my_brain my_brain_config.json

# Import configuration from file
nlm-config import expert_config.json expert_configuration

# Copy configuration
nlm-config copy my_brain expert_brain

# Move configuration
nlm-config move my_brain expert_brain

# Delete configuration
nlm-config delete my_brain

# Validate configuration
nlm-config validate expert_configuration

# Validate configuration file
nlm-config validate-file expert_config.json
```

### Configuration Shell Functions

```bash
# Add to your shell configuration (bashrc, zshrc, etc.)

# NLM Configuration Functions
nlm_config() {
    local command="$1"
    shift
    
    case "$command" in
        "list")
            nlm_config_list "$@"
            ;;
        "show")
            nlm_config_show "$@"
            ;;
        "create")
            nlm_config_create "$@"
            ;;
        "set")
            nlm_config_set "$@"
            ;;
        "export")
            nlm_config_export "$@"
            ;;
        "import")
            nlm_config_import "$@"
            ;;
        "validate")
            nlm_config_validate "$@"
            ;;
        *)
            echo "Usage: nlm_config <command> [options]"
            echo "Commands: list, show, create, set, export, import, validate"
            ;;
    esac
}

# Configuration initialization
nlm_config_init() {
    local config_dir="${NLM_CONFIG_DIR:-~/.nlm/configs}"
    mkdir -p "$config_dir"
    export NLM_CONFIG_DIR="$config_dir"
}

# Load configuration functions
if command -v nlm_config >/dev/null 2>&1; then
    nlm_config_init
fi
```

## Configuration Integration

### Integration with NLM Demos

The configuration system integrates with NLM demos through the following mechanisms:

1. **Command Line Options**: NLM demos accept configuration files and command line overrides
2. **Environment Variables**: NLM demos read configuration from environment variables
3. **Configuration Validation**: NLM demos validate configurations before use
4. **Default Configurations**: NLM demos provide default configurations

### Integration Examples

```bash
# Run NLM demo with configuration file
./nlm_phase3_demo --config expert_config.json

# Run NLM demo with environment variables
export NLM_NEURON_COUNT=2000
export NLM_CONNECTION_PROB=0.15
./nlm_phase3_demo

# Run NLM demo with command line overrides
./nlm_phase3_demo --config expert_config.json --set neuron_count 3000 --set-b enable_curiosity true
```

### Configuration Loading

```cpp
// Configuration loading example
#include <nlm/core/Config.hpp>

// Load configuration from file
bool loadConfiguration(const std::string& config_file, nlm::Config& config) {
    // Initialize configuration
    nlm::Config config;
    
    // Load from file
    if (!config.loadFromFile(config_file)) {
        std::cerr << "Failed to load configuration from: " << config_file << std::endl;
        return false;
    }
    
    // Load from command line (overrides file)
    if (!config.loadFromArgs(argc, argv)) {
        std::cerr << "Failed to load configuration from command line" << std::endl;
        return false;
    }
    
    return true;
}
```

## Conclusion

The NLM configuration system provides:

- **Comprehensive Configuration Options**: Extensive configuration options for all NLM components
- **Flexible Configuration Sources**: Support for configuration files, command line, and environment variables
- **Configuration Validation**: Validation of configuration values and rules
- **Configuration Management**: Tools for managing configurations
- **Integration**: Integration with NLM demos and tools
- **Extensibility**: Extensible configuration system for future enhancements

Start with `nlm-config list` to see all available configurations, or use `nlm-config create my_config` to create a new configuration.
