# NLM Advanced Features - User Documentation

This document provides comprehensive information about the NLM (Neural Learning Machine) advanced features and usage.

## Overview

NLM is an experimental artificial developmental brain project that simulates real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

The advanced features in NLM Phase 6 provide enhanced capabilities for power users, including:

- **Advanced Feature Management**: Runtime control over cognitive system capabilities
- **Enhanced Integration**: Seamless connection between all cognitive subsystems
- **Performance Optimization**: Real-time monitoring and optimization of system performance
- **Export/Import**: Multiple format support for state and configuration management
- **Scripting and Automation**: Python and Lua scripting for custom workflows
- **Analysis Tools**: Comprehensive analysis and reporting capabilities

## Key Improvements

### 1. Configuration Bug Fix
- **Fixed**: Duplicate `dopamine_baseline` keys in `configs/default.cfg`
- **Impact**: Ensures consistent configuration loading and prevents unexpected behavior

### 2. Enhanced Command-Line Interface
- **New**: Advanced command-line parsing with support for:
  - Test selection and execution
  - Configuration file specification
  - Output and logging options
  - Performance benchmarking
  - Help and version information

### 3. Cognitive System Integration
- **Improved**: Integration between previously disconnected systems:
  - Memory Systems (Working, Episodic, Semantic, Procedural, Associative)
  - Prediction System with error computation and confidence tracking
  - Neuromodulation Systems (Dopamine, Curiosity, Novelty, Prediction Error)
  - Development System with age-based plasticity modulation
  - Cognitive Systems (Concept Formation, Neural Planning, Attentional Selection)

### 4. Advanced Feature Framework
- **New**: Comprehensive feature management system with:
  - Runtime feature enable/disable
  - Feature dependency management
  - Performance impact analysis
  - Event-driven feature system
  - Health monitoring and diagnostics

### 5. Enhanced API
- **New**: Advanced Application Programming Interface with:
  - Brain state access and control
  - Feature configuration and execution
  - Export/import capabilities
  - Script execution and management
  - System analysis and reporting

## Getting Started

### Basic Usage

1. **Build the Project**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

2. **Run Basic Tests**
```bash
./nlm
```

3. **Run Advanced Application**
```bash
./nlm_advanced
```

4. **View Help**
```bash
./nlm_advanced help
./nlm --help
```

### Advanced Usage

1. **Run Specific Tests**
```bash
./nlm --test basic_connectivity --test plasticity --test stdp
```

2. **Custom Configuration**
```bash
./nlm --config my_config.cfg --random_seed 12345 --neuron_count 2000
```

3. **Export/Import State**
```bash
./nlm --export state --filepath=nlm_state.txt
./nlm --import state --filepath=nlm_state.txt
```

4. **Performance Benchmarking**
```bash
./nlm --benchmark --duration 60.0
```

## Advanced Features

### Feature Management

#### Available Features

- **memory_optimization**: Optimizes memory usage and implements memory pools for better performance
- **prediction_enhancement**: Enhances prediction accuracy and implements advanced forecasting algorithms
- **neuromodulation_optimization**: Optimizes neuromodulation with dynamic learning rates and adaptive signals
- **cognitive_enhancement**: Enhances cognitive abilities with meta-cognitive monitoring and self-regulation
- **visualization**: Enhances visualization capabilities with real-time analysis and multiple output formats
- **export_analysis**: Enhances export capabilities with multiple formats and advanced analytics
- **performance_monitoring**: Real-time performance monitoring with predictive scaling and optimization
- **scripting**: Scripting support with Python and Lua integration for automation and workflow management

#### Feature Commands

```bash
# List all features
./nlm_advanced features

# Enable/disable features
./nlm_advanced feature enable memory_optimization
./nlm_advanced feature disable prediction_enhancement

# Execute specific features
./nlm_advanced execute feature memory_optimization
```

### Brain System Analysis

#### Brain Status
```bash
./nlm_advanced brain
```

Shows neural network statistics including:
- Total neurons and synapses
- Spike counts and firing rates
- Active neuron counts
- Region information
- Integration status

#### Performance Metrics
```bash
./nlm_advanced performance
```

Shows system performance including:
- CPU and memory usage
- Simulation speed
- Feature performance scores
- Resource utilization
- Optimization recommendations

#### Integration Status
```bash
./nlm_advanced integration
```

Shows integration status including:
- System health
- Connection strengths
- Integration issues
- Performance metrics

### Export/Import System

#### Export Commands
```bash
# Export brain state
./nlm_advanced export brain nlm_state.txt

# Export feature configurations
./nlm_advanced export features feature_configs.json
```

#### Import Commands
```bash
# Import brain state
./nlm_advanced import brain nlm_state.txt

# Import feature configurations
./nlm_advanced import features feature_configs.json
```

### Script Management

#### Script Commands
```bash
# List available scripts
./nlm_advanced script list

# Load a script
./nlm_advanced script load analysis "print('Hello, World!')"

# Execute a script
./nlm_advanced script execute analysis

# Remove a script
./nlm_advanced script remove analysis
```

### Analysis and Optimization

#### Analysis Commands
```bash
# Analyze brain state
./nlm_advanced analyze brain

# Analyze feature performance
./nlm_advanced analyze feature memory_optimization
```

#### Optimization Commands
```bash
# Run feature optimization
./nlm_advanced optimize features

# Run performance optimization
./nlm_advanced optimize performance
```

## Configuration

### Configuration File Format

NLM uses a simple key=value format for configuration:

```
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
```

### Command Line Overrides

Configuration values can be overridden using command line arguments:

```bash
./nlm --neuron_count 2000 --random_seed 123 --plasticity_learning_rate 0.02
```

## Advanced API

### Feature Control

The Advanced API provides programmatic control over NLM features:

```cpp
#include "AdvancedFeatures.hpp"

int main() {
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    
    // Create advanced feature manager
    auto featureManager = std::make_shared<AdvancedFeatureManager>(brain);
    
    // Create advanced API
    auto advancedAPI = std::make_shared<AdvancedAPI>(brain, featureManager);
    
    // Enable features
    advancedAPI->enableFeature("memory_optimization");
    advancedAPI->enableFeature("prediction_enhancement");
    
    // Run simulation
    for (SimulationStep step = 0; step < 10000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Analyze results
    std::cout << "Brain State:" << std::endl;
    std::cout << advancedAPI->analyzeBrainState() << std::endl;
    
    // Export state
    advancedAPI->exportState("simulation_state.txt");
    
    return 0;
}
```

### Scripting Support

NLM supports scripting in Python and Lua for custom workflows:

```python
# Python script example
import nlm

# Initialize brain
brain = nlm.Brain()

# Enable features
brain.enableFeature("memory_optimization")
brain.enableFeature("prediction_enhancement")

# Run simulation
for step in range(10000):
    brain.step(step, step * 0.001)

# Export results
brain.exportState("results.json")

# Analyze
print(brain.analyzeBrainState())
```

```lua
-- Lua script example
local nlm = require("nlm")

-- Initialize brain
local brain = nlm.Brain()

-- Enable features
brain:enableFeature("memory_optimization")
brain:enableFeature("prediction_enhancement")

-- Run simulation
for step = 0, 9999 do
    brain:step(step, step * 0.001)
end

-- Export results
brain:exportState("results.json")

-- Analyze
print(brain:analyzeBrainState())
```

## Development

### Building from Source

1. **Clone the Repository**
```bash
git clone <repository-url>
cd nlm
```

2. **Set up Build Environment**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Run Tests**
```bash
ctest
```

4. **Install**
```bash
sudo make install
```

### Contributing

1. **Fork the Repository**
2. **Create a Feature Branch**
3. **Implement Features**
4. **Add Tests**
5. **Update Documentation**
6. **Submit Pull Request**

### Code Quality Guidelines

- **C++17 or later**
- **Header-only APIs for interfaces**
- **Comprehensive error handling**
- **Thread-safe operations**
- **Memory management**
- **Performance considerations**

## Troubleshooting

### Common Issues

#### "Failed to initialize brain!"
- Check if all required libraries are installed
- Verify configuration file exists and is readable
- Ensure sufficient memory is available

#### "Script not found"
- Verify script name is correct
- Check if script file exists
- Ensure script loading syntax is correct

#### "High memory usage"
- Reduce the number of enabled features
- Increase system memory
- Use memory optimization features

#### "Performance issues"
- Disable unnecessary features
- Reduce simulation step count
- Check system resource usage

### Help Commands

```bash
# Show all available commands
./nlm_advanced help

# Show help for specific command
./nlm_advanced help <command>

# Show version information
./nlm_advanced version
./nlm --version
```

## Examples

### Example 1: Basic Simulation with Advanced Features
```bash
./nlm_advanced \
  --config my_config.cfg \
  --enable-feature memory_optimization \
  --enable-feature prediction_enhancement \
  --benchmark \
  --output simulation_results.json
```

### Example 2: Feature Analysis Script
```python
#!/usr/bin/env python3
import nlm

# Initialize application
app = nlm.AdvancedApplication()

# Load and analyze brain state
brain_state = app.loadBrainState("simulation_state.txt")
analysis = app.analyzeBrainState(brain_state)

print("Brain Analysis:")
print(analysis)

# Export analysis
app.exportAnalysis(analysis, "brain_analysis.txt")
```

### Example 3: Custom Feature Configuration
```bash
# Create custom feature configuration
cat > custom_features.json << EOF
{
    "features": {
        "memory_optimization": {
            "enabled": true,
            "parameters": {
                "pool_size": 2048,
                "compression_level": 7
            }
        },
        "prediction_enhancement": {
            "enabled": true,
            "parameters": {
                "horizon": 20.0,
                "confidence_threshold": 0.9
            }
        }
    }
}
EOF

# Apply feature configuration
./nlm_advanced apply-features custom_features.json
```

## Conclusion

The NLM advanced features provide a comprehensive set of capabilities for power users and researchers. The improved integration, enhanced CLI, and advanced feature management make NLM a powerful tool for neural simulation and cognitive research.

The documentation provides comprehensive guidance for getting started, advanced usage, and development. All examples and code snippets are designed to be easily adaptable to specific use cases.

For more information, please refer to the source code documentation and examples provided in the repository.
