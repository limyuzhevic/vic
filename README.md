# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Enhanced Configuration System

NLM now includes an advanced configuration system with comprehensive configuration management capabilities:

### Key Features

1. **Advanced Configuration Management**: Enhanced `AdvancedConfigManager` class with schemas, templates, and presets
2. **Configuration Validation**: Comprehensive validation with detailed error reporting
3. **Configuration Templates**: Predefined templates for common use cases
4. **Configuration Presets**: Optimized configurations for training, simulation, research, and development
5. **Configuration Optimization**: Automatic parameter optimization for specific use cases
6. **Configuration Comparison**: Configuration comparison and similarity analysis
7. **Configuration Export/Import**: Support for JSON, YAML, and INI formats
8. **Configuration Analysis**: Pattern detection and performance estimation

### Configuration Presets

NLM provides optimized configurations for different scenarios:

- **training**: Optimized for neural network training and learning
- **simulation**: Optimized for stable simulation runs
- **research**: Optimized for research and experimentation
- **development**: Optimized for development and debugging

### Python Integration

The enhanced configuration system is fully accessible through Python bindings:

```python
from python.nlm_utils import ConfigManager, ConfigBuilder, ConfigValidator

# Create and configure
config = ConfigBuilder.create_default_config()
config.apply_preset("training")

# Validate
errors = ConfigValidator.validate_advanced_config(config)

# Create brain
brain = nlm_py.createBrain(config)
brain.initialize()
```

### Building and Running

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Configuration Testing

```bash
cd tests
./nlm_test
```

### Python Examples

```bash
python python/examples/config_builder.py
python python/examples/advanced_config_demo.py
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/
│   ├── README_ENHANCED.md          # Enhanced configuration documentation
│   ├── nlm_utils.py              # Python configuration utilities
│   ├── examples/                 # Python examples
│   │   ├── config_builder.py
│   │   └── advanced_config_demo.py
│   └── tests/                    # Python tests
│       ├── test_nlm_utils.py
│       ├── test_integration.py
│       ├── test_bindings.py
│       └── test_nlm_py.py
├── src/
│   ├── core/                     # Core utilities
│   │   ├── Config/               # Enhanced configuration system
│   │   │   ├── Config.hpp        # Basic configuration
│   │   │   ├── Config.cpp        # Basic configuration implementation
│   │   │   └── AdvancedConfigManager.hpp  # Advanced configuration
│   │   │   └── AdvancedConfigManager.cpp  # Advanced configuration implementation
│   │   ├── Types/               # Strongly-typed identifiers
│   │   ├── Logger/             # Logging system
│   │   ├── Random/             # Random number generation
│   │   └── SimulationClock/    # Simulation clock
│   ├── brain/                   # Neural components (integrated)
│   ├── dynamics/                # Neural dynamics
│   ├── plasticity/              # Plasticity rules
│   ├── development/             # Developmental system
│   ├── neuromodulation/         # Neuromodulators (integrated)
│   ├── memory/                 # Memory systems (integrated)
│   ├── prediction/              # Prediction systems (integrated)
│   ├── cognition/              # Cognitive mechanisms (integrated)
│   ├── sensory/                 # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework (Phase 6)
│   └── visualization/          # Visualization
├── tests/                      # Tests
│   ├── test_config.cpp          # Basic configuration tests
│   ├── test_advanced_config.cpp # Advanced configuration tests
│   └── ...                     # Other tests
└── configs/                    # Configuration files
    ├── default.cfg             # Default configuration
    └── advanced_template.cfg   # Advanced configuration template
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Enhanced Configuration System Features

### 1. Configuration Management

The enhanced configuration system provides:

- **AdvancedConfigManager**: Comprehensive configuration management class
- **ConfigSchema**: Schema definition for validation
- **ConfigTemplate**: Template system for common use cases
- **ConfigPreset**: Preset system for predefined configurations
- **ValidationError**: Detailed error reporting

### 2. Configuration Validation

Supports comprehensive validation:
- Type validation (int, double, bool, string, arrays)
- Range validation (min/max values)
- Enum validation (allowed values)
- Required field validation
- Custom validation rules

### 3. Configuration Templates and Presets

Predefined configurations for different scenarios:

- **training**: Optimized for neural network training
- **simulation**: Optimized for stable simulation
- **research**: Optimized for research and experimentation
- **development**: Optimized for development and debugging

### 4. Configuration Optimization

Automatic parameter optimization:
- Optimize for specific use cases
- Generate improvement suggestions
- Analyze performance characteristics
- Create optimized presets

### 5. Configuration Export/Import

Multiple format support:
- JSON: Full configuration with metadata
- YAML: Human-readable format
- INI/CFG: Legacy file format support

### 6. Configuration Analysis

Pattern detection and insights:
- Detect parameter correlations
- Estimate performance
- Generate recommendations
- Create configuration hashes

### 7. Python Integration

Full Python bindings:
- Direct access to all C++ configuration features
- Python utilities for configuration management
- Example applications for common use cases
- Comprehensive documentation

## Configuration File Examples

### Default Configuration

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2
```

### Advanced Configuration Template

```ini
# NLM Configuration File
# Advanced Configuration Template
# This template provides a comprehensive set of configuration options

# ===== BASIC SETTINGS =====
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# ===== PLASTICITY SETTINGS =====
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# ===== DEVELOPMENT SETTINGS =====
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# ===== NEUROMODULATION SETTINGS =====
dopamine_baseline = 0.0
curiosity_baseline = 0.1

# ===== PERFORMANCE SETTINGS =====
performance_enabled = true
thread_count = 4
```

## Usage Examples

### Basic Configuration

```cpp
#include "core/Config/AdvancedConfigManager.hpp"

nlm::AdvancedConfigManager config;

// Set configuration parameters
config.set("neuron_count", 2000);
config.set("plasticity_learning_rate", 0.05);
config.set("log_level", std::string("DEBUG"));

// Validate configuration
if (config.isValid()) {
    std::cout << "Configuration is valid" << std::endl;
} else {
    auto errors = config.getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "Error: " << error.key << " - " << error.message << std::endl;
    }
}
```

### Using Presets

```cpp
#include "core/Config/AdvancedConfigManager.hpp"

nlm::AdvancedConfigManager config;

// Apply training preset
config.applyPreset("training");

// Verify preset was applied
assert(config.has("neuron_count"));
assert(config.has("plasticity_learning_rate"));
```

### Configuration Optimization

```cpp
#include "core/Config/AdvancedConfigManager.hpp"

nlm::AdvancedConfigManager config;
config.loadTemplate("advanced");

// Get optimization suggestions for training use case
auto suggestions = config.optimize("training");
for (const auto& suggestion : suggestions) {
    std::cout << "Optimize " << suggestion.key 
              << " from " << suggestion.currentValue 
              << " to " << suggestion.suggestedValue 
              << " (" << suggestion.reason << ")" << std::endl;
}
```

### Python Configuration

```python
from python.nlm_utils import ConfigBuilder, ConfigValidator

# Create configuration from preset
config = ConfigBuilder.create_config_from_preset("training")

# Validate configuration
errors = ConfigValidator.validate_advanced_config(config)
if errors:
    print("Validation errors:", errors)
else:
    print("Configuration is valid")

# Create brain with configuration
brain = nlm_py.createBrain(config)
brain.initialize()
```

## Testing

### Running Tests

```bash
# Run C++ tests
./nlm_test

# Run Python tests
python -m pytest python/tests/ -v

# Run specific tests
./nlm_test --gtest_filter="*Config*"
```

### Test Coverage

The configuration system includes comprehensive tests:

- **test_advanced_config.cpp**: All advanced configuration features
- **test_config.cpp**: Basic configuration functionality
- **test_validation.cpp**: Validation features
- **test_template.cpp**: Template system
- **test_optimization.cpp**: Optimization features

## Performance Considerations

### Configuration Validation

- Validation is performed on configuration changes
- Schema validation provides type and range checking
- Performance is optimized with caching where appropriate

### Configuration Export/Import

- JSON export supports pretty printing for human readability
- File I/O is buffered for performance
- Memory usage is optimized for large configurations

### Configuration Optimization

- Optimization algorithms are heuristic-based
- Performance estimation uses simplified models
- Suggestions are prioritized by improvement factor

## Error Handling

### Validation Errors

Validation errors include:
- Error codes for easy programmatic handling
- Detailed error messages
- Parameter identification
- Error categorization

### File I/O Errors

File operations include proper error handling:
- File not found errors
- Permission errors
- Syntax errors in configuration files

### Optimization Errors

Optimization includes:
- Use case validation
- Parameter range checking
- Fallback to default values

## Configuration Security

### Access Control

- Configuration schemas control parameter access
- Required fields enforce mandatory parameters
- Type validation prevents invalid parameter types

### Data Integrity

- Configuration hashes ensure integrity
- Validation prevents corrupted configurations
- Error reporting provides detailed feedback

## Future Enhancements

The enhanced configuration system is designed to be extensible. Future enhancements may include:

- **Machine Learning Optimization**: ML-based configuration optimization
- **Configuration Versioning**: Configuration history and versioning
- **Collaboration Support**: Shared configuration workspaces
- **Web Interface**: Web-based configuration management
- **Docker Integration**: Docker-based configuration management

## Conclusion

The enhanced configuration system provides comprehensive configuration management capabilities for NLM, making it easier to create, validate, optimize, and manage NLM configurations. The system is backward compatible with the basic Config class while providing advanced features for production use.

The enhanced configuration system includes:

1. **Advanced Configuration Management**: Comprehensive configuration management with schemas, templates, and presets
2. **Configuration Validation**: Robust validation with detailed error reporting
3. **Configuration Optimization**: Automatic parameter optimization for specific use cases
4. **Configuration Comparison**: Configuration comparison and similarity analysis
5. **Configuration Export/Import**: Support for multiple formats (JSON, YAML, INI)
6. **Configuration Analysis**: Pattern detection and performance estimation
7. **Python Integration**: Full Python bindings with comprehensive utilities
8. **Backward Compatibility**: Compatible with existing configuration code

The system is ready for production use and provides all the tools needed for comprehensive configuration management in NLM.

## License

MIT License - See LICENSE file for details.

## Authors

NLM Project Team

## Version

NLM Phase 6 with Enhanced Configuration System v2.0