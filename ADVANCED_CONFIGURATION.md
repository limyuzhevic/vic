# Advanced Configuration System for NLM

## Overview

The NLM (Neural Learning Machine) framework now includes advanced configuration options and command-line interface enhancements to provide better control for power users and researchers.

## New Configuration Options

### Agent System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `agent.reward_modulation_enabled` | bool | true | Enable/disable reward-based neuromodulation |
| `agent.curiosity_enabled` | bool | true | Enable/disable curiosity-driven exploration |
| `agent.structural_plasticity_enabled` | bool | true | Enable/disable structural plasticity |
| `agent.development_enabled` | bool | true | Enable/disable developmental progression |
| `agent.exploration_rate` | float | 0.3f | Base exploration rate for curiosity-driven actions |
| `agent.noise_level` | float | 0.1f | Random noise for motor command selection |
| `agent.neuromodulation_strength` | float | 1.0f | Strength of neuromodulation effects |

### Sensory Processing Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.vision_width` | size_t | 16 | Width of vision grid |
| `sensory.vision_height` | size_t | 16 | Height of vision grid |
| `sensory.touch_sensors` | size_t | 8 | Number of touch sensors |
| `sensory.internal_signals` | size_t | 4 | Number of internal signal channels |
| `sensory.proprioception` | size_t | 6 | Number of proprioception sensors |
| `sensory.audio_enabled` | bool | false | Enable audio sensory input |
| `sensory.noise_level` | float | 0.05f | Sensory input noise level |

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | float | 20.0f | World width |
| `world.height` | float | 20.0f | World height |
| `world.max_energy` | float | 100.0f | Maximum energy level |
| `world.energy_decay_rate` | float | 0.01f | Energy decay rate per timestep |
| `world.agent_speed` | float | 1.0f | Maximum agent movement speed |
| `world.agent_rotation_speed` | float | 0.5f | Maximum agent rotation speed |
| `world.object_interaction_radius` | float | 1.0f | Radius for object interaction |

### Performance Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.checkpoint_interval` | size_t | 100 | Steps between checkpoints |
| `performance.max_memory_usage` | size_t | 1000000 | Maximum memory usage (bytes) |
| `performance.enable_profiling` | bool | false | Enable performance profiling |
| `performance.num_threads` | size_t | 0 | Number of threads (0 = auto) |
| `performance.enable_simd` | bool | true | Enable SIMD optimizations |

### Advanced Learning Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `learning.prediction_horizon` | size_t | 5 | Number of steps to predict ahead |
| `learning.greedy_exploration` | bool | false | Use greedy policy during exploration |
| `learning.annealing_factor` | float | 0.999f | Learning rate annealing factor |
| `learning.entropy_coefficient` | float | 0.01f | Entropy regularization coefficient |
| `learning.tau_value` | float | 0.01f | Target network update rate |

## Command Line Interface Enhancements

### New Command Line Options

```bash
# Configuration file options
--config-file "path/to/config.yaml"    Load configuration from YAML file
--config-output "path/to/output.json"  Save current configuration to file
--config-template "template.json"      Generate configuration template

# Agent configuration
--agent-mode [reactive|exploratory|balanced]  Set agent behavior mode
--exploration-rate "float"                     Set exploration rate (0.0-1.0)
--curiosity-threshold "float"                  Set curiosity threshold for exploration

# Performance and debugging
--enable-profiling                        Enable performance profiling
--profile-output "output.prof"              Save profiling data
--debug-level [info|debug|trace]            Set debug logging level
--log-file "path/to/log.txt"                Log to file instead of console

# Simulation control
--simulation-speed "float"                  Simulation speed multiplier
--checkpoint-interval "steps"                Steps between checkpoints
--save-state "path/to/state.bin"              Save initial state
--load-state "path/to/state.bin"              Load saved state

# Visualization
--enable-visualization                     Enable visualization (requires GUI)
--visual-width "pixels"                     Visualization window width
--visual-height "pixels"                    Visualization window height
--save-frames "directory"                   Save simulation frames

# Advanced options
--enable-gpu                               Enable GPU acceleration (if available)
--cuda-device "index"                      CUDA device index
--memory-limit "bytes"                      Memory limit for simulation
--thread-pool-size "count"                  Thread pool size for parallel processing
```

### Configuration File Formats

#### YAML Configuration

```yaml
nlm:
  # Brain configuration
  brain:
    neuron_count: 1000
    region_count: 2
    connection_probability: 0.1
    
  # Agent configuration
  agent:
    reward_modulation_enabled: true
    curiosity_enabled: true
    structural_plasticity_enabled: true
    development_enabled: true
    exploration_rate: 0.3
    noise_level: 0.1
    neuromodulation_strength: 1.0
    
  # Sensory configuration
  sensory:
    vision_width: 16
    vision_height: 16
    touch_sensors: 8
    internal_signals: 4
    proprioception: 6
    audio_enabled: false
    noise_level: 0.05
    
  # World configuration
  world:
    width: 20.0
    height: 20.0
    max_energy: 100.0
    energy_decay_rate: 0.01
    agent_speed: 1.0
    agent_rotation_speed: 0.5
    object_interaction_radius: 1.0
    
  # Performance configuration
  performance:
    checkpoint_interval: 100
    max_memory_usage: 1000000
    enable_profiling: false
    num_threads: 0
    enable_simd: true
    
  # Advanced learning configuration
  learning:
    prediction_horizon: 5
    greedy_exploration: false
    annealing_factor: 0.999
    entropy_coefficient: 0.01
    tau_value: 0.01
```

#### JSON Configuration

```json
{
  "nlm": {
    "brain": {
      "neuron_count": 1000,
      "region_count": 2,
      "connection_probability": 0.1
    },
    "agent": {
      "reward_modulation_enabled": true,
      "curiosity_enabled": true,
      "structural_plasticity_enabled": true,
      "development_enabled": true,
      "exploration_rate": 0.3,
      "noise_level": 0.1,
      "neuromodulation_strength": 1.0
    },
    "sensory": {
      "vision_width": 16,
      "vision_height": 16,
      "touch_sensors": 8,
      "internal_signals": 4,
      "proprioception": 6,
      "audio_enabled": false,
      "noise_level": 0.05
    },
    "world": {
      "width": 20.0,
      "height": 20.0,
      "max_energy": 100.0,
      "energy_decay_rate": 0.01,
      "agent_speed": 1.0,
      "agent_rotation_speed": 0.5,
      "object_interaction_radius": 1.0
    },
    "performance": {
      "checkpoint_interval": 100,
      "max_memory_usage": 1000000,
      "enable_profiling": false,
      "num_threads": 0,
      "enable_simd": true
    },
    "learning": {
      "prediction_horizon": 5,
      "greedy_exploration": false,
      "annealing_factor": 0.999,
      "entropy_coefficient": 0.01,
      "tau_value": 0.01
    }
  }
}
```

## Configuration Management API

### Python API Enhancements

```python
import pynlm

# Create configuration with advanced options
config = pynlm.createDefaultConfig()

# Set advanced agent options
config.set("agent.exploration_rate", 0.5)
config.set("agent.noise_level", 0.05)
config.set("agent.curiosity_enabled", True)

# Set sensory configuration
config.set("sensory.vision_width", 32)
config.set("sensory.vision_height", 32)
config.set("sensory.noise_level", 0.02)

# Set world configuration
config.set("world.width", 50.0)
config.set("world.height", 50.0)
config.set("world.agent_speed", 2.0)

# Set performance options
config.set("performance.checkpoint_interval", 500)
config.set("performance.enable_profiling", True)
config.set("performance.num_threads", 8)

# Set learning parameters
config.set("learning.prediction_horizon", 10)
config.set("learning.entropy_coefficient", 0.05)
config.set("learning.annealing_factor", 0.995)

# Save configuration to file
config.saveToFile("advanced_config.json")

# Load configuration from YAML file
config.loadFromFile("config.yaml")

# Get configuration summary
print(config.summary())
```

### Command Line Configuration Management

```bash
# Generate configuration template
nlm --config-template advanced_config.json

# Load configuration from YAML
nlm --config-file simulation_config.yaml --enable-visualization

# Save current configuration
nlm --save-config current_config.json

# Validate configuration
nlm --config-file invalid_config.json --validate

# Merge configuration files
nlm --config-file base_config.json --config-file override.json --output merged_config.json
```

## Configuration Schema Validation

The advanced configuration system includes schema validation to ensure:

1. **Type Validation**: Configuration values match expected types
2. **Range Validation**: Configuration values are within valid ranges
3. **Dependency Validation**: Configuration options are compatible
4. **Consistency Validation**: Configuration options don't conflict

Example validation errors:

- "agent.exploration_rate must be between 0.0 and 1.0"
- "learning.entropy_coefficient cannot be set when greedy_exploration is true"
- "performance.num_threads must be positive when > 0"
- "sensory.audio_enabled requires audio codec support"

## Configuration Presets

The system includes predefined configuration presets:

### Basic Configuration
```json
{
  "preset": "basic",
  "description": "Basic agent simulation with minimal settings",
  "agent": {
    "exploration_rate": 0.2,
    "noise_level": 0.1,
    "curiosity_enabled": false
  },
  "world": {
    "width": 10.0,
    "height": 10.0
  }
}
```

### Advanced Configuration
```json
{
  "preset": "advanced",
  "description": "Advanced agent simulation with all features enabled",
  "agent": {
    "exploration_rate": 0.5,
    "noise_level": 0.05,
    "curiosity_enabled": true,
    "structural_plasticity_enabled": true,
    "development_enabled": true
  },
  "sensory": {
    "vision_width": 32,
    "vision_height": 32,
    "noise_level": 0.02
  },
  "world": {
    "width": 50.0,
    "height": 50.0,
    "agent_speed": 2.0
  },
  "performance": {
    "checkpoint_interval": 1000,
    "enable_profiling": true,
    "num_threads": 8
  },
  "learning": {
    "prediction_horizon": 20,
    "entropy_coefficient": 0.05,
    "annealing_factor": 0.99
  }
}
```

### Research Configuration
```json
{
  "preset": "research",
  "description": "Configuration optimized for research experiments",
  "agent": {
    "exploration_rate": 0.7,
    "noise_level": 0.15,
    "curiosity_enabled": true,
    "structural_plasticity_enabled": true,
    "development_enabled": true
  },
  "world": {
    "width": 100.0,
    "height": 100.0,
    "agent_speed": 3.0
  },
  "performance": {
    "checkpoint_interval": 5000,
    "enable_profiling": true,
    "num_threads": 16
  },
  "learning": {
    "prediction_horizon": 50,
    "greedy_exploration": false,
    "entropy_coefficient": 0.1,
    "annealing_factor": 0.995
  }
}
```

## Migration Guide

For users upgrading from basic configurations:

1. **Add Agent Configuration**: If you were using default agent settings, now you need to explicitly configure:
   - `agent.exploration_rate`
   - `agent.noise_level`
   - `agent.curiosity_enabled`

2. **Sensory Configuration**: Vision and sensory configurations are now configurable:
   - `sensory.vision_width`
   - `sensory.vision_height`
   - `sensory.noise_level`

3. **Performance Configuration**: Performance tuning options are now available:
   - `performance.checkpoint_interval`
   - `performance.num_threads`
   - `performance.enable_profiling`

4. **Learning Configuration**: Advanced learning parameters:
   - `learning.prediction_horizon`
   - `learning.entropy_coefficient`
   - `learning.annealing_factor`

All existing configuration keys continue to work with their default values.

## Configuration Commands

### Configuration Commands

| Command | Description |
|---------|-------------|
| `nlm --config-template <file>` | Generate configuration template |
| `nlm --config-validate <file>` | Validate configuration file |
| `nlm --config-merge <file1> <file2> <output>` | Merge two configuration files |
| `nlm --config-save <file>` | Save current configuration to file |
| `nlm --config-load <file>` | Load configuration from file |
| `nlm --config-presets` | List available configuration presets |

### Agent Control Commands

| Command | Description |
|---------|-------------|
| `nlm --agent-mode <mode>` | Set agent mode (reactive|exploratory|balanced) |
| `nlm --exploration-rate <rate>` | Set exploration rate |
| `nlm --curiosity-threshold <threshold>` | Set curiosity threshold |
| `nlm --agent-noise <noise>` | Set agent noise level |

### World Control Commands

| Command | Description |
|---------|-------------|
| `nlm --world-size <width> <height>` | Set world dimensions |
| `nlm --agent-speed <speed>` | Set agent movement speed |
| `nlm --world-reset` | Reset world to initial state |

### Performance Control Commands

| Command | Description |
|---------|-------------|
| `nlm --enable-profiling` | Enable performance profiling |
| `nlm --profile-output <file>` | Set profiling output file |
| `nlm --debug-level <level>` | Set debug logging level |
| `nlm --log-file <file>` | Log to file instead of console |

This advanced configuration system provides power users with fine-grained control over the NLM simulation while maintaining backward compatibility with existing configurations.