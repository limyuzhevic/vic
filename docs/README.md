# NLM (Neural Learning Machine) Documentation

# Overview

NLM (Neural Learning Machine) is an experimental artificial neural network simulation framework that implements real spiking neural computation with:

- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Neuromodulation systems (dopamine, curiosity, novelty)
- Developmental stages and cognitive functions

# Installation

## Prerequisites

- C++20 compatible compiler
- CMake 3.16 or higher
- Optional: nlohmann/json for JSON support
- Optional: yaml-cpp for YAML support

## Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
 cmake -DCMAKE_BUILD_TYPE=Release \
       -DHAVE_NLOHMANN_JSON=ON \
       -DHAVE_YAML_CPP=ON ..

# Build
make -j$(nproc)

# Install (optional)
sudo make install
```

## Quick Start

```bash
# Run the default demonstration
./nlm

# View help information
./nlm --help

# Run with custom configuration
./nlm --config myconfig.cfg init

# Run a specific experiment
./nlm experiment connectivity-test
```

# Architecture

## Core Components

### 1. Brain Class (`brain/Brain.hpp`)

The central coordinator of the neural system that implements:

- **Real Spiking Neural Computation**: LIF neurons with event-driven dynamics
- **Integrated Memory Systems**: Working memory, episodic memory, associative memory
- **Prediction System**: Sensory prediction and error computation
- **Cognition Systems**: Neural planning, concept formation, attentional selection
- **Development System**: Developmental stages and structural changes
- **Neuromodulation Systems**: Dopamine, curiosity, novelty, prediction error

### 2. AgentBrain Class (`agent/AgentBrain.hpp`)

Connects NLM brain to the environment:

- **Sensory Processing**: Converts sensory input to neural currents
- **Motor Decoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward-based learning
- **Development**: Manages developmental stages and plasticity

### 3. AgentBody Class (`agent/AgentBody.hpp`)

Physical model of the agent in the environment:

- **Position and Movement**: X, Y, orientation, velocity
- **Physical Properties**: Energy, health, age
- **State Management**: Moving, turning, action tracking
- **Collision Detection**: Object collision handling

### 4. Configuration System (`core/Config/Config.hpp`)

Enhanced configuration system with:

- **Multiple Format Support**: JSON, YAML, legacy key=value format
- **Validation**: Configuration validation with regex support
- **Source Tracking**: File, command-line, runtime source tracking
- **Statistics**: Configuration statistics and validation scores

### 5. DebugSystem (`debug/DebugSystem.hpp`)

Advanced debugging and visualization system:

- **Real-time Monitoring**: Live metrics and statistics
- **Console Visualization**: Text-based neural network visualization
- **Export Capabilities**: HTML, JSON, CSV export formats
- **Performance Tracking**: CPU, memory, simulation time monitoring

### 6. CommandLineInterface (`cli/CommandLineInterface.hpp`)

Comprehensive command-line interface:

- **Advanced Configuration**: JSON/YAML config file support
- **Experiment Control**: Predefined experiment management
- **Real-time Control**: Start, stop, step, reset commands
- **Monitoring**: Real-time statistics and visualization

## Neural Network Architecture

### Neuron Model

Each neuron in NLM follows Leaky Integrate-and-Fire dynamics:

```
dV/dt = -(V - V_rest)/τ + I_syn
```

Where:
- V = membrane potential
- V_rest = resting potential
- τ = membrane time constant
- I_syn = synaptic current

When V ≥ V_threshold, the neuron fires a spike and V is reset.

### Synapse Model

Synapses transmit spikes with delays and plastic weights:

```
w(t + 1) = w(t) + Δw
```

Where Δw is determined by plasticity rules (STDP, Hebbian, etc.).

### Network Organization

- **Regions**: Groups of neurons with similar functions
- **Populations**: Groups within regions with specific neuron types
- **Inter-region Connections**: Long-range connectivity between regions

# Configuration

## Configuration Files

NLM supports multiple configuration file formats:

### JSON Configuration

```json
{
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "environment_name": "GridWorld",
  "log_level": "INFO"
}
```

### YAML Configuration

```yaml
neuron_count: 500
region_count: 1
connection_probability: 0.15
simulation_timestep: 0.001
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
environment_name: GridWorld
log_level: INFO
```

### Legacy Configuration

```
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 1

# Connection probability for random connectivity
connection_probability = 0.15
```

## Configuration Keys

### Core Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int64 | 500 | Number of neurons in the brain |
| `region_count` | int64 | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Connection probability for random connectivity |

### Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant |
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate |

### Structural Plasticity

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Pruning rate |

### Neuromodulation

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Dopamine baseline level |
| `reward_discount_factor` | float | 0.99 | Reward discount factor for temporal difference learning |

### Environment Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Name of the environment |
| `environment_width` | int | 10 | Environment width |
| `environment_height` | int | 10 | Environment height |

### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARN, ERROR) |
| `log_to_file` | bool | false | Write logs to file |
| `log_filename` | string | "nlm.log" | Log filename |

### Simulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | float | 0.0 | Simulation time limit in seconds (0 = unlimited) |

### Visualization Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `visualization_enabled` | bool | false | Enable visualization |
| `visualization_update_rate` | float | 30.0 | Visualization update rate in Hz |

# Usage

## Basic Usage

### Initialization

```bash
# Initialize brain and environment with default settings
./nlm init

# Initialize with custom configuration
./nlm --config myconfig.cfg init
```

### Running Simulation

```bash
# Start the simulation (runs default plasticity experiment)
./nlm start

# Run specific number of steps
./nlm step 100

# Run a specific experiment
./nlm experiment plasticity-learning

# Reset brain to initial state
./nlm reset
```

### Saving and Loading

```bash
# Save brain state to file
./nlm save brain_state.nml

# Load brain state from file
./nlm load brain_state.nml
```

### Experiment Control

```bash
# List available experiments
./nlm experiment list

# Run connectivity test experiment
./nlm experiment connectivity-test

# Run plasticity learning experiment
./nlm experiment plasticity-learning

# Run STDP verification experiment
./nlm experiment stdp-verification

# Run developmental trajectory experiment
./nlm experiment developmental-trajectory

# Run curiosity-driven exploration experiment
./nlm experiment curiosity-driven

# Run reward-based learning experiment
./nlm experiment reward-based
```

## Advanced Usage

### Configuration Management

```bash
# Validate current configuration
./nlm --validate

# Show configuration summary
./nlm --config-summary

# Set configuration values
./nlm --set neuron_count=1000 --set simulation_timestep=0.01 init

# Add configuration values
./nlm --add neuron_count=500 --add region_count=2 init

# Remove configuration keys
./nlm --remove neuron_count --remove region_count init
```

### Configuration File Management

```bash
# Load JSON configuration
./nlm --json-config config.json init

# Load YAML configuration
./nlm --yaml-config config.yaml init

# Export current configuration
./nlm --export-config exported_config.cfg init

# Import configuration from file
./nlm --import-config exported_config.cfg init
```

### Experiment with Parameters

```bash
# Run experiment with custom parameters
./nlm experiment plasticity-learning --steps 500 --learning-rate 0.02

# Run experiment in exploration mode
./nlm --explore experiment plasticity-learning

# Run experiment in exploitation mode
./nlm --exploit experiment plasticity-learning
```

### Monitoring and Statistics

```bash
# Show current status
./nlm status

# Show statistics
./nlm stats

# Show spike statistics
./nlm spikes

# Show plasticity statistics
./nlm plasticity

# Start monitoring
./nlm monitor start

# Stop monitoring
./nlm monitor stop
```

### Plugin Management

```bash
# List available plugins
./nlm plugins list

# Load a plugin
./nlm plugins load curiosity

# Unload a plugin
./nlm plugins unload curiosity
```

## Programming Examples

### Basic C++ Usage

```cpp
#include <nlm/Brain.hpp>
#include <nlm/AgentBrain.hpp>
#include <nlm/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Get statistics
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

### Configuration Example

```cpp
#include <nlm/Config.hpp>

int main() {
    // Create configuration
    nlm::Config config;
    
    // Set values
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 500);
    config.set("region_count", 1);
    config.set("connection_probability", 0.15);
    
    // Validate configuration
    if (!config.validateAll()) {
        std::cerr << "Configuration validation failed" << std::endl;
        return 1;
    }
    
    // Load from file
    if (!config.loadFromFile("config.json")) {
        std::cerr << "Failed to load configuration from file" << std::endl;
        return 1;
    }
    
    // Save to file
    if (!config.saveToFile("config.json", true)) {
        std::cerr << "Failed to save configuration to file" << std::endl;
        return 1;
    }
    
    // Show configuration summary
    std::cout << config.summary();
    
    return 0;
}
```

### Debug Example

```cpp
#include <nlm/DebugSystem.hpp>
#include <nlm/Brain.hpp>
#include <nlm/AgentBrain.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create debug system
    nlm::DebugSystem debugSystem;
    debugSystem.initialize(brain, agentBrain);
    
    // Set debug mode
    debugSystem.setDebugMode(nlm::DebugMode::DETAILED);
    debugSystem.setVisualizationMode(nlm::VisualizationMode::CONSOLA);
    
    // Run simulation with debugging
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        debugSystem.updateMetrics();
        
        // Print visualization every 10 steps
        if (step % 10 == 0) {
            std::cout << debugSystem.generateConsoleVisualization() << std::endl;
        }
    }
    
    // Export debug data
    debugSystem.exportVisualization("debug_output", nlm::VisualizationMode::HTML);
    
    // Shutdown debug system
    debugSystem.shutdown();
    
    return 0;
}
```

# Experiments

## Available Experiments

### 1. Connectivity Test

Tests basic neural connectivity by injecting current into neurons and measuring spike propagation.

**Purpose**: Verify that the neural network can transmit signals and generate spikes.

**Command**: `./nlm experiment connectivity-test`

### 2. Plasticity Learning Experiment

Tests synaptic plasticity by enabling STDP and Hebbian learning with repeated input patterns.

**Purpose**: Verify that synapses can strengthen and weaken through experience.

**Command**: `./nlm experiment plasticity-learning`

### 3. STDP Verification

Verifies Spike-Timing-Dependent Plasticity by creating correlated pre-post activity.

**Purpose**: Confirm that STDP rules produce expected weight changes.

**Command**: `./nlm experiment stdp-verification`

### 4. Developmental Trajectory

Tests developmental stages by simulating brain development over time.

**Purpose**: Verify that the brain can undergo developmental changes.

**Command**: `./nlm experiment developmental-trajectory`

### 5. Curiosity-Driven Exploration

Tests curiosity-based exploration with neuromodulation.

**Purpose**: Verify that curiosity drives exploration behavior.

**Command**: `./nlm experiment curiosity-driven`

### 6. Reward-Based Learning

Tests reward-based learning with dopamine neuromodulation.

**Purpose**: Verify that reward signals can modify neural activity.

**Command**: `./nlm experiment reward-based`

## Experiment Parameters

Each experiment can be run with custom parameters:

```bash
./nlm experiment plasticity-learning --steps 500 --learning-rate 0.02
```

Available parameters vary by experiment. Use `--help` with experiment names for details.

# Plugin System

## Available Plugins

### Curiosity Plugin

Implements curiosity-driven exploration behavior.

**Features**:
- Novelty detection
- Exploration motivation
- Balance of exploration vs. exploitation

### Reward Plugin

Implements reward-based learning and reinforcement.

**Features**:
- Reward prediction error
- Dopamine modulation
- Value-based learning

### Developmental Plugin

Implements developmental stages and maturation.

**Features**:
- Critical periods
- Plasticity reduction over time
- Structural changes

### Structural Plasticity Plugin

Implements synaptogenesis and pruning.

**Features**:
- New synapse formation
- Synapse elimination
- Network optimization

### Cognitive Plugin

Implements cognitive functions and planning.

**Features**:
- Working memory
- Attentional selection
- Concept formation

## Plugin Management

### Loading Plugins

```bash
./nlm plugins load curiosity
./nlm plugins load reward
```

### Unloading Plugins

```bash
./nlm plugins unload curiosity
./nlm plugins unload reward
```

### Listing Plugins

```bash
./nlm plugins list
```

## Plugin Configuration

Plugins can be configured through the configuration system:

```bash
./nlm --set curiosity.enabled=true --set curiosity.exploration_rate=0.5 init
```

# Troubleshooting

## Common Issues

### Issue: Configuration File Not Found

**Solution**: Check that the configuration file exists and is readable.

```bash
# Check if file exists
ls -la config.cfg

# Create the file if it doesn't exist
cat > config.cfg << EOF
neuron_count = 100
simulation_timestep = 0.001
EOF
```

### Issue: Brain Initialization Fails

**Solution**: Check the error messages and ensure all dependencies are available.

```bash
# Run with verbose output to see detailed errors
./nlm init --verbose
```

### Issue: Visualization Not Working

**Solution**: Check that the terminal supports ANSI colors and enable visualization.

```bash
# Enable visualization
./nlm --set visualization_enabled=true init

# Run experiment with visualization
./nlm experiment connectivity-test
```

### Issue: Debug System Not Working

**Solution**: Check that the debug system is properly initialized and configured.

```bash
# Enable debug mode
./nlm --set debug.mode=true init

# Set debug level
./nlm --set debug.level=detailed init
```

## Error Messages

### "Failed to initialize brain"

**Cause**: Brain initialization failed due to invalid configuration or missing dependencies.

**Solution**:
1. Check configuration values
2. Ensure all required dependencies are available
3. Check file permissions
4. Run with verbose output for detailed error messages

### "Configuration validation failed"

**Cause**: Configuration contains invalid values or missing required keys.

**Solution**:
1. Check configuration for invalid values
2. Ensure all required keys are present
3. Use `--config-summary` to see detailed configuration information
4. Run with `--validate` to see validation errors

### "Unknown command"

**Cause**: Command-line command is not recognized.

**Solution**:
1. Check the command spelling
2. Use `--help` to see available commands
3. Check for typos in command names

## FAQ

### Q: What are the system requirements?

A: NLM requires a C++20 compatible compiler and CMake 3.16 or higher. For full functionality, optional dependencies include nlohmann/json and yaml-cpp.

### Q: How do I get started?

A: The easiest way to get started is to run the default demonstration:

```bash
./nlm
```

This will run the connectivity test experiment and show you how the system works.

### Q: Can I modify the experiments?

A: Yes! Experiments are defined in the `src/experiments/` directory. You can create new experiments or modify existing ones by editing the experiment source files.

### Q: How do I add new plugins?

A: Plugins are implemented as shared libraries that follow the plugin interface. See the `src/plugins/` directory for examples and documentation on creating new plugins.

### Q: What formats does NLM support for configuration files?

A: NLM supports JSON, YAML, and legacy key=value format. JSON and YAML are preferred for their structured format and validation support.

### Q: How do I export debug data?

A: You can export debug data using the debug system:

```bash
./nlm experiment connectivity-test --export-debug debug_output
```

This will export debug data in multiple formats (HTML, JSON, CSV) for analysis.

### Q: Can I run multiple experiments?

A: Yes, you can run multiple experiments in sequence:

```bash
./nlm experiment connectivity-test
./nlm experiment plasticity-learning
./nlm experiment stdp-verification
```

Or run them in a single command:

```bash
./nlm experiment connectivity-test --run-plasticity --run-stdp
```

# Reference

## Command Line Reference

### General Options

- `--help`: Show help information
- `--version`: Show version information
- `--verbose` or `-v`: Enable verbose output
- `--quiet` or `-q`: Suppress non-error output

### Configuration Options

- `--config <file>`: Load configuration from file
- `--json-config <file>`: Load JSON configuration
- `--yaml-config <file>`: Load YAML configuration
- `--set <key>=<value>`: Set configuration value
- `--add <key>=<value>`: Add configuration value
- `--remove <key>`: Remove configuration key
- `--validate`: Validate current configuration
- `--config-summary`: Show configuration summary
- `--export-config <file>`: Export configuration
- `--import-config <file>`: Import configuration

### Control Options

- `init`: Initialize brain and environment
- `reset`: Reset brain to initial state
- `start` or `run`: Start simulation
- `stop` or `pause`: Stop simulation
- `step <n>`: Run N simulation steps
- `status`: Show current status
- `save <file>`: Save brain state to file
- `load <file>`: Load brain state from file

### Experiment Options

- `experiment <name>`: Run predefined experiment
- `--explore` or `--explore-mode`: Enable exploration mode
- `--exploit` or `--exploit-mode`: Enable exploitation mode

### Monitoring Options

- `monitor start`: Start monitoring
- `monitor stop`: Stop monitoring
- `stats`: Show statistics
- `spikes`: Show spike statistics
- `plasticity`: Show plasticity statistics

### Plugin Options

- `plugins list`: List available plugins
- `plugins load <name>`: Load plugin
- `plugins unload <name>`: Unload plugin

## Configuration Reference

See the Configuration section for detailed information about all configuration options and their types.

## Debug System Reference

See the Debug System section for detailed information about debugging and visualization options.

## Plugin System Reference

See the Plugin System section for detailed information about plugin development and management.

# Acknowledgments

## Contributors

- NLM Core Team
- Research Collaborators
- Documentation Authors
- Plugin Contributors

## Funding

This project is supported by the NLM Research Foundation and various academic institutions.

## License

This project is licensed under the MIT License. See LICENSE file for details.

# Changes

## Version 2.0.0

- Complete rewrite with advanced configuration system
- Added JSON/YAML support
- Enhanced command-line interface
- Added debug system with visualization
- Improved documentation
- Added plugin system framework
- Fixed numerous bugs and performance issues

## Version 1.0.0

- Initial release
- Basic neural network simulation
- Limited configuration support
- Simple command-line interface
- Basic debugging capabilities

# Conclusion

NLM (Neural Learning Machine) is a sophisticated neural network simulation framework that provides a comprehensive platform for researching and implementing artificial neural systems. With its advanced configuration system, extensive command-line interface, visualization capabilities, and plugin architecture, NLM offers a powerful tool for both research and educational purposes.

The improvements made in this version significantly enhance the user experience, making it easier to use, more configurable, and more extensible. The addition of JSON/YAML support, advanced debugging tools, and comprehensive documentation makes NLM accessible to users of all skill levels.

For more information and updates, visit the NLM project website or consult the documentation for your specific use case.