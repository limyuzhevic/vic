# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Enhanced configuration management system with:
  - Type-safe configuration values
  - Configuration validation
  - Nested configuration support
  - Schema validation
  - Value interpolation
  - Environment-specific configurations
  - Template support
  - Better error handling and logging
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 4)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Configuration Architecture

### Enhanced Configuration System

The enhanced NLM configuration system provides a comprehensive solution for configuration management with the following key features:

#### 1. Type-Safe Configuration Values

```cpp
using ConfigValue = std::variant<
    int, int64_t, double, bool,
    std::string,
    std::vector<int>, std::vector<double>,
    std::vector<std::string>, std::vector<bool>
>;
```

The configuration system uses a variant type to store multiple value types, ensuring type safety and flexibility.

#### 2. Nested Configuration Support

```cpp
// Set nested configuration
config.set({"brain", "neuron_count"}, 2000);
config.set({"brain", "plasticity", "stdp", "enable"}, true);

// Get nested values
if (auto value = config.get<bool>({"brain", "plasticity", "stdp", "enable"})) {
    // Use value
}
```

Nested configuration allows for better organization of complex settings, eliminating flat key pollution.

#### 3. Configuration Sources

```cpp
enum class ConfigSource {
    Default,     // Built-in defaults
    File,        // Configuration file
    CommandLine, // Command line arguments
    Runtime,     // Runtime modifications
    Environment, // Environment variables
    Template,    // Template-based configuration
    Interpolation // Interpolated values
};
```

Different configuration sources are tracked separately, allowing for dependency resolution and override priority.

#### 4. Validation Framework

The validation framework provides comprehensive validation:

```cpp
// Add validation rule
config.addValidator("brain.neuron_count", 
    [](const std::string& key, const ConfigValue& value) {
        if (auto val = std::get_if<int>(&value)) {
            if (*val < 100 || *val > 10000) {
                return ValidationResult::error(
                    "Neuron count must be between 100 and 10000", key);
            }
        }
        return ValidationResult::success();
    }
);

// Validate configuration
ValidationResult result = config.validate();
if (result.valid) {
    // Configuration is valid
} else {
    // Handle validation error
}
```

#### 5. Schema Validation

Schema validation provides structural validation using JSON Schema:

```cpp
// Define JSON schema
nlohmann::json schema = {
    {"type", "object"},
    {"properties", {
        {"brain.neuron_count", {
            {"type", "integer"},
            {"minimum", 100},
            {"maximum", 10000}
        }},
        {"brain.synapse_density", {
            {"type", "number"},
            {"minimum", 0.0},
            {"maximum", 1.0}
        }}
    }},
    {"required", {"brain.neuron_count", "brain.synapse_density"}}
};

config.setSchema(schema);
ValidationResult result = config.validateSchema();
```

#### 6. Value Interpolation

Value interpolation allows configuration values to reference other values:

```cpp
// Enable interpolation
config.enableInterpolation(true);

// Set base values
config.set("brain.base", 1000);
config.set("brain.threshold", -55.0);

// Add interpolation references
config.addInterpolationReference("brain.neuron_count", "brain.base");

// Set interpolated values
config.set("brain.neuron_count", "${brain.base}");

// Resolve interpolations
config.resolveInterpolations();
```

#### 7. Environment-Specific Configuration

Environment-specific configuration allows for different settings based on deployment:

```cpp
// Create environment configurations
EnvironmentConfig devEnv;
devEnv.name = "development";
devEnv.isDevelopment = true;
devEnv.overrides = {
    {"brain.neuron_count", 512},
    {"brain.synapse_density", 0.2},
    {"simulation.debug", true}
};

EnvironmentConfig prodEnv;
prodEnv.name = "production";
prodEnv.isDevelopment = false;
prodEnv.overrides = {
    {"brain.neuron_count", 10000},
    {"brain.synapse_density", 0.05},
    {"simulation.debug", false}
};

// Set environment
config.setEnvironment("production");
```

#### 8. Template Support

Templates allow for reusable configuration sets:

```cpp
// Create configuration template
ConfigTemplate template;
template.name = "default";
template.defaults = {
    {"brain.neuron_count", 1000},
    {"brain.synapse_density", 0.1},
    {"brain.threshold", -55.0},
    {"plasticity.stdp.enable", true}
};

// Create configuration from template
config = Config::fromTemplate(template);
```

#### 9. File I/O Support

The enhanced system supports multiple file formats:

```cpp
// Save as JSON
config.saveToFile("config.json");

// Save as YAML
config.saveToFile("config.yaml");

// Load from JSON
Config loaded = Config::fromFile("config.json");

// Load from YAML
Config loaded = Config::fromFile("config.yaml");
```

#### 10. Error Handling and Logging

Comprehensive error handling and logging:

```cpp
// Set up custom logger
config.setLogger([](const std::string& msg) {
    std::cout << "[CONFIG] " << msg << std::endl;
});

// Log configuration changes
config.set("brain.neuron_count", 2000);
// Output: [CONFIG] Set brain.neuron_count to 2000 (source: Runtime)
```

## Data Flow

### Configuration in Brain Processing

The enhanced configuration system integrates with the NLM architecture as follows:

```
Environment Variables → Configuration Files → Command Line → Runtime Settings
         │                       │                   │              │
         ▼                       ▼                   ▼              ▼
    Environment Config    File-based Config    CLI Config     Code Config
         │                       │                   │              │
         ▼                       ▼                   ▼              ▼
    Validation Framework → Schema Validation → Interpolation → Application
```

### Configuration Loading Pipeline

1. **Environment Detection**: System detects deployment environment (development, production, etc.)
2. **Configuration Sources**: Loads configuration from multiple sources in order of priority
3. **Interpolation Resolution**: Resolves interpolated values based on source dependencies
4. **Validation**: Validates configuration against schema and validation rules
5. **Application**: Applies validated configuration to brain components

### Configuration Usage in Components

#### Brain Configuration

```cpp
// Access configuration in brain components
class Brain {
public:
    void initialize(const Config& config) {
        // Configure neurons
        auto neuronCount = config.get<int>("brain.neuron_count");
        auto threshold = config.get<double>("brain.threshold");
        auto synapseDensity = config.get<double>("brain.synapse_density");
        
        // Apply configuration
        setupNeurons(neuronCount.value_or(1000), threshold.value_or(-55.0));
        configureSynapses(synapseDensity.value_or(0.1));
    }
};
```

#### Plasticity Configuration

```cpp
// Configure plasticity rules based on configuration
class PlasticitySystem {
public:
    void setup(const Config& config) {
        // STDP configuration
        bool stdpEnable = config.get<bool>("plasticity.stdp.enable").value_or(true);
        double stdpLearningRate = config.get<double>("plasticity.stdp.learning_rate").value_or(0.001);
        
        // Hebbian configuration
        bool hebbianEnable = config.get<bool>("plasticity.hebbian.enable").value_or(true);
        
        // Apply plasticity configuration
        enableSTDP(stdpEnable, stdpLearningRate);
        enableHebbian(hebbianEnable);
    }
};
```

#### Neuromodulation Configuration

```cpp
// Configure neuromodulation based on configuration
class NeuromodulationSystem {
public:
    void setup(const Config& config) {
        // Dopamine configuration
        double dopamineScale = config.get<double>("neuromod.dopamine.scale").value_or(1.0);
        
        // Curiosity configuration
        bool curiosityEnable = config.get<bool>("neuromod.curiosity.enable").value_or(true);
        
        // Novelty configuration
        bool noveltyEnable = config.get<bool>("neuromod.novelty.enable").value_or(true);
        
        // Apply neuromodulation configuration
        configureDopamine(dopamineScale);
        enableCuriosity(curiosityEnable);
        enableNovelty(noveltyEnable);
    }
};
```

## Key Design Principles

### 1. Separation of Concerns

- **Configuration Management**: Pure configuration storage and retrieval
- **Validation**: Independent validation framework
- **Interpolation**: Separate interpolation logic
- **Environment Support**: Environment-specific configuration handling

### 2. Type Safety

- Compile-time type checking for configuration getters
- Strong typing for nested configuration keys
- Validation of type consistency

### 3. Performance

- Efficient lookup for configuration values
- Caching for frequently accessed values
- Lazy validation for performance-critical operations

### 4. Extensibility

- Plugin architecture for custom validators
- Extensible interpolation system
- Modular configuration sources

### 5. Backward Compatibility

- Maintains compatibility with existing configuration API
- Supports migration from simple to enhanced configuration
- Provides fallback mechanisms for missing features

## Migration Guide

### From Basic to Enhanced Configuration

#### Basic Configuration

```cpp
// Old style
nlm::Config config;
config.set("neuron_count", 1000);
config.set("threshold", -55.0);
auto value = config.get<int>("neuron_count");
```

#### Enhanced Configuration

```cpp
// New style (backward compatible)
nlm::Config config;
config.set("brain.neuron_count", 1000);
config.set("brain.threshold", -55.0);
auto value = config.get<int>("brain.neuron_count");

// Or use nested configuration
config.set({"brain", "neuron_count"}, 1000);
config.set({"brain", "threshold"}, -55.0);
auto value = config.get<int>({"brain", "neuron_count"});
```

### Migration Steps

1. **Update Configuration Keys**: Update configuration keys to use nested format
2. **Add Validation**: Add validation rules for new configuration structure
3. **Update Documentation**: Update documentation to reflect new configuration structure
4. **Test Migration**: Test configuration loading and validation
5. **Gradual Migration**: Gradually migrate existing configurations

## Testing and Validation

### Configuration Tests

```cpp
// Test configuration loading
void testConfigFromFile() {
    nlm::Config config = nlm::Config::fromFile("config.json");
    assert(config.get<int>("brain.neuron_count") == 1000);
}

// Test nested configuration
void testNestedConfig() {
    nlm::Config config;
    config.set({"brain", "neuron_count"}, 2000);
    assert(config.get<int>({"brain", "neuron_count"}) == 2000);
}

// Test validation
void testValidation() {
    nlm::Config config;
    config.set("brain.neuron_count", 1000);
    auto result = config.validate();
    assert(result.valid);
}

// Test interpolation
void testInterpolation() {
    nlm::Config config;
    config.enableInterpolation(true);
    config.set("brain.base", 1000);
    config.set("brain.target", "${brain.base}");
    assert(config.resolveInterpolations());
    assert(config.get<int>("brain.target") == 1000);
}
```

### Validation Rules

Common validation rules include:

- **Type validation**: Ensure values match expected types
- **Range validation**: Ensure values are within valid ranges
- **Pattern validation**: Ensure values match specific patterns
- **Required validation**: Ensure required keys are present
- **Enum validation**: Ensure values are from a predefined set

## Performance Considerations

### Memory Usage

- Use smart pointers for configuration management
- Implement copy-on-write for large configurations
- Consider reference counting for frequently accessed values

### CPU Performance

- Cache validation results
- Use efficient data structures for nested configuration
- Implement lazy evaluation for expensive operations

### I/O Performance

- Use efficient serialization formats (JSON, YAML)
- Implement configuration compression for large configurations
- Consider lazy loading for large configuration files

## Security Considerations

### Configuration Security

- Validate configuration file permissions
- Sanitize configuration values
- Log configuration access for security auditing
- Use environment variables for sensitive configuration

### Interpolation Security

- Validate interpolation references
- Implement reference cycles detection
- Use whitelist for allowed interpolation sources

## Future Enhancements

### Planned Features

1. **Dynamic Configuration Reload**: Automatic reloading of configuration files
2. **Distributed Configuration**: Support for distributed configuration systems
3. **Configuration Versioning**: Support for configuration versioning and rollback
4. **Configuration Analytics**: Analytics and monitoring of configuration usage
5. **Configuration Templates**: Advanced template system with variable substitution

### Research Directions

1. **Configuration Optimization**: Automatic optimization of configuration values
2. **Machine Learning**: Machine learning-based configuration recommendation
3. **Self-Configuration**: Self-configuring systems based on runtime conditions
4. **Configuration Validation**: Advanced validation techniques using formal methods

## CLI Architecture

### 1. Command-Line Interface (Phase 6 Implementation)

The enhanced NLM Command-Line Interface (CLI) provides a powerful, flexible, and user-friendly command-line experience for NLM users. It was implemented as part of Phase 6 (Emerging Cognition) to address the limitations of the basic CLI and provide advanced functionality for complex use cases.

#### CLI Architecture Components:

**1. Core CLI Parser (`cli/CLIParser.hpp/cpp`)**
- Modern command-line argument parsing with comprehensive option support
- Rich command structure with subcommands (run, configure, experiment, analyze, benchmark)
- Configuration file loading and command-line override handling
- Verbosity control and logging configuration
- Help system with detailed documentation

**2. Command Execution Engine (`cli/CommandHandler.hpp/cpp`)**
- Abstract base for command execution
- Implementation of simulation commands (run, experiment)
- Support for different simulation types: basic, plasticity, development, cognitive, social, memory, attention
- Output formatting for multiple formats (text, JSON, CSV)
- Progress monitoring and status reporting

**3. Configuration Management (`cli/ConfigManager.hpp/cpp`)**
- Integration with the enhanced core configuration system
- Support for configuration file I/O (JSON, YAML, text formats)
- Command-line argument to configuration value mapping
- Built-in configuration presets for common use cases
- Configuration validation and error handling

**4. Logging and Monitoring (`cli/LoggerConfig.hpp/cpp`)**
- Advanced logging configuration with multiple output formats
- Verbosity levels from quiet to trace
- Console and file logging support
- Progress monitoring for long-running operations

**5. Help System (`cli/HelpSystem.hpp/cpp`)**
- Comprehensive help documentation with command-specific help
- Automatic help generation from code metadata
- Examples and usage documentation
- Preset information and configuration guidance

**6. Progress Monitor (`cli/ProgressMonitor.hpp/cpp`)**
- Real-time progress reporting for long-running operations
- Progress bar display with estimated time remaining
- Configurable update intervals and message support
- Integration with simulation reporting

#### CLI Features:

**Command Structure:**
```
nlm [options] <command> [arguments]

Commands:
  run           Run simulation
  configure     Manage configuration
  experiment    Run experiment
  analyze       Analyze results
  benchmark     Performance benchmarking
```

**Configuration Presets:**
- `basic`: Simple neuron dynamics (500 neurons, 1 region)
- `plasticity`: Learning and adaptation with STDP and Hebbian plasticity (1000 neurons, 2 regions)
- `development`: Developmental stages with synaptogenesis and pruning (800 neurons, 2 regions)
- `cognitive`: Full cognitive capabilities including memory and attention (2000 neurons, 4 regions)
- `social`: Social interaction simulation with reward-based learning (1500 neurons, 3 regions)
- `memory`: Memory-intensive scenarios with high synaptic stability (3000 neurons, 2 regions)
- `attention`: Attention and focus simulation with competitive dynamics (1200 neurons, 3 regions)

**Scripting Support:**
- JSON configuration files
- Python batch runners (batch_run.py, parameter_sweep.py, experiment_runner.py)
- Parameter sweeps and optimization scripts
- Integration with external tools and workflows

#### CLI Integration Benefits:

1. **Backward Compatibility**: The enhanced CLI maintains compatibility with existing configuration files and usage patterns
2. **Type Safety**: Integration with the core configuration system ensures type-safe operations
3. **Rich Help**: Comprehensive help system with examples and documentation
4. **Progress Monitoring**: Real-time feedback for long-running simulations
5. **Configuration Management**: Advanced configuration loading, saving, and validation
6. **Output Flexibility**: Multiple output formats for different use cases
7. **Error Handling**: Robust error detection and reporting
8. **Cross-Platform**: Support for different operating systems and environments

#### Example Usage:

```bash
# Run a basic simulation
nlm run --type basic --steps 100 --neurons 500

# Run a plasticity experiment
nlm experiment --type plasticity --steps 1000 --neurons 1000

# Configure and save a custom preset
nlm configure --preset development --save custom_config.json

# Analyze simulation results
nlm analyze --format json --output results.json

# Performance benchmarking
nlm benchmark --duration 60 --neurons 500

# List available presets
nlm --list-presets

# Get help for specific command
nlm help run
```

### 2. Enhanced Configuration Integration

The CLI system integrates seamlessly with the enhanced core configuration system, providing:

- Type-safe configuration value handling
- Configuration validation with detailed error messages
- Environment-specific configuration support
- Template-based configuration management
- Advanced configuration features (interpolation, schema validation)

### 3. Progress and Status Reporting

The enhanced CLI provides comprehensive progress reporting:

- Real-time progress bars for long-running operations
- Estimated time remaining calculations
- Status reporting with configuration summaries
- Detailed logging with configurable verbosity levels
- Progress monitoring for simulation steps and experiments

### 4. Scripting and Automation

The CLI includes comprehensive scripting support:

- Python batch runners for automated execution
- Parameter sweep scripts for optimization
- Experiment runner for complex experimental designs
- Integration with external analysis tools
- Support for CI/CD pipelines and automated testing

## Conclusion

The enhanced NLM CLI architecture provides a powerful, flexible, and user-friendly command-line experience while maintaining backward compatibility with existing usage patterns. It represents a significant advancement in NLM's usability and functionality, enabling users to effectively configure, run, and analyze complex neural network simulations with confidence and flexibility.

The CLI system integrates seamlessly with all NLM components, providing a unified approach to configuration management, execution, and analysis throughout the entire simulation pipeline. It enables researchers and developers to work with NLM more efficiently and effectively, supporting both simple use cases and complex experimental setups.