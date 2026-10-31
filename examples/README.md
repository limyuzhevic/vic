# NLM Examples

This directory contains educational examples demonstrating various NLM neural simulation capabilities.

## Overview

The NLM (Neural Learning Machine) framework provides a comprehensive platform for artificial brain simulation. This examples directory showcases practical applications and usage patterns through a series of progressively complex examples.

Each example demonstrates:
- Core NLM concepts and APIs
- Practical implementation patterns
- Common workflows and use cases
- Best practices and tips

## Examples

### 1. Simple Brain Simulation (`simple_brain.py`)
An introductory example showing how to create and run a basic neural simulation.

**Key concepts demonstrated:**
- Creating a brain from configuration
- Initializing and running a simulation
- Reading basic statistics
- Simple step-by-step processing

### 2. Agent in World (`agent_world.py`)
Demonstrates how to use NLM in a simulated world environment with perception and action.

**Key concepts demonstrated:**
- World-environment interaction
- Sensory processing (vision)
- Motor command execution
- Agent-body state management

### 3. Learning Agent (`learning_agent.py`)
Shows learning capabilities with reward modulation, curiosity, and structural plasticity.

**Key concepts demonstrated:**
- Neuromodulation (reward prediction error)
- Curiosity-driven exploration
- Structural plasticity (synaptogenesis)
- Developmental processes

### 4. Memory Brain (`memory_brain.py`)
Demonstrates memory formation and retrieval in NLM.

**Key concepts demonstrated:**
- Working memory (active information buffer)
- Episodic memory (event-based storage)
- Associative memory (pattern completion)
- Memory consolidation and replay

### 5. Custom Configuration (`custom_config.py`)
Shows how to create and use custom configurations.

**Key concepts demonstrated:**
- Configuration management
- Parameter customization
- Different configuration formats
- Preset configurations

### Main Example Runner (`example_main.py`)
Runs all examples in sequence, demonstrating the full range of NLM capabilities.

## Installation and Setup

### Prerequisites

1. **Build NLM**
```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

2. **Install Python dependencies**
```bash
cd /path/to/nlm
pip install -r requirements.txt
```

3. **Set up examples directory**
```bash
cd /path/to/nlm
cp -r examples/ /path/to/your/project/
```

### Running Examples

#### Method 1: Using the Main Runner
```bash
cd /path/to/nlm/examples
python3 example_main.py
```

#### Method 2: Running Individual Examples
```bash
# Basic brain simulation
python3 simple_brain.py

# Agent in world
python3 agent_world.py

# Learning agent
python3 learning_agent.py

# Memory brain
python3 memory_brain.py

# Custom configuration
python3 custom_config.py
```

## Example Usage

### Running All Examples
```bash
cd /path/to/nlm/examples
python3 example_main.py
```

This will run all examples in sequence, demonstrating:
1. Basic neural simulation
2. Agent-world interaction
3. Learning with neuromodulation
4. Memory formation and retrieval
5. Configuration management

### Running Individual Examples
```bash
# To see detailed output for each example
python3 simple_brain.py --verbose

# To run with custom configuration
python3 agent_world.py --config custom_config.json
```

### Example Output

Each example produces output showing:
- Configuration details
- Simulation progress
- Statistics and metrics
- Final results

Example output:
```
=== NLM Simple Brain Example ===
This example shows how to create and run a basic neural simulation.

Configuration:
  Neuron count: 100
  Simulation timestep: 0.001s

Creating brain...
  Brain created with 100 neurons
  Total synapses: 1,500

Running simulation...
  Step 0: 0 neurons firing
  Step 1: 2 neurons firing
  Step 2: 5 neurons firing
  Step 3: 8 neurons firing
  Step 4: 12 neurons firing

Simulation completed in 0.123 seconds

Final statistics:
  Total spikes: 45
  Average firing rate: 12.50 Hz
  Active neurons: 8
  Firing neurons (this step): 12

Example completed successfully!
```

## Advanced Usage

### Using Configuration Files

NLM supports multiple configuration file formats:

#### NLM Config Format (`.nlm`)
```ini
[simulation]
random_seed = 42
neuron_count = 500

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

[world]
world_width = 20
world_height = 20
```

#### JSON Format (`.json`)
```json
{
    "simulation": {
        "random_seed": 42,
        "neuron_count": 500
    },
    "world": {
        "world_width": 20,
        "world_height": 20
    }
}
```

#### YAML Format (`.yaml`)
```yaml
simulation:
  random_seed: 42
  neuron_count: 500
world:
  world_width: 20
  world_height: 20
```

### Configuration Management

NLM provides advanced configuration management through:

#### Python Configuration Manager
```python
from nlm_config_manager import NLMConfigManager

# Create config manager
config = NLMConfigManager("config.json")

# Set values
config.set_value("simulation.neuron_count", 2000)
config.set_value("world.world_width", 30)

# Get values
neuron_count = config.get_value("simulation.neuron_count")
print(f"Neuron count: {neuron_count}")

# Save configuration
config.save_config("new_config.json", "json")

# Create presets
config.create_preset("research_config", "Configuration for research")
config.load_preset("production_config")
```

#### Command-Line Interface
```bash
# Create default configuration
nlm_cli.sh create-config

# Load configuration
nlm_cli.sh --load-config my_config.cfg

# Set configuration value
nlm_cli.sh --set simulation.neuron_count 2000

# Get configuration value
nlm_cli.sh --get simulation.random_seed

# Run with custom configuration
nlm_cli.sh --config my_config.cfg demo
```

### Performance Optimization

#### Tips for Large-Scale Simulations

1. **Reduce time step** for accuracy
```python
config.set("simulation_timestep", 0.0001)
```

2. **Increase time step** for speed
```python
config.set("simulation_timestep", 0.001)
```

3. **Use appropriate network size**
```python
config.set("neuron_count", 10000)  # For research
config.set("neuron_count", 100)     # For development
```

4. **Enable performance optimizations**
```python
config.set("enable_performance_optimizations", True)
```

#### Profiling and Monitoring
```bash
# Run with performance monitoring
nlm --performance benchmark 1000

# Generate performance report
python3 analyze_performance.py --output performance_report.html
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Simulation is too slow
**Solution:**
1. Increase time step: `config.set("simulation_timestep", 0.001)`
2. Reduce network size: `config.set("neuron_count", 500)`
3. Enable optimizations: `config.set("enable_performance_optimizations", True)`

#### Issue: Memory usage exceeds available memory
**Solution:**
1. Reduce network size
2. Enable compression: `config.set("checkpoint_compression", "fast")`
3. Use streaming mode: `config.set("streaming_mode", True)`

#### Issue: Configuration file not found
**Solution:**
1. Check file path and permissions
2. Use absolute paths: `nlm_cli.sh --config /absolute/path/config.cfg`
3. Ensure file exists in working directory

### Error Codes

| Error Code | Description | Solution |
|------------|-------------|----------|
| ERR_001 | Configuration file not found | Check file path and permissions |
| ERR_002 | Invalid configuration value | Verify value format and range |
| ERR_003 | Memory allocation failed | Reduce network size or available memory |
| ERR_004 | Simulation step timeout | Reduce network size or increase timestep |

## Best Practices

### 1. Configuration Management
- Use descriptive keys: `neuron_count` instead of `n1`
- Document all parameters with comments
- Use presets for common configurations
- Validate parameter ranges

### 2. Performance Optimization
- Profile before optimizing
- Use appropriate data structures
- Leverage parallelism
- Cache frequently used data

### 3. Memory Management
- Use smart pointers
- Avoid circular references
- Clear unused resources
- Use object pools

### 4. Code Organization
- Follow single responsibility principle
- Use namespaces
- Document public interfaces
- Write unit tests

### 5. Simulation Best Practices
- Use reproducible seeds
- Validate parameters
- Monitor performance
- Log important events

## Advanced Topics

### 1. Plugin Architecture

NLM supports plugin architecture for extending functionality:

```cpp
class NLMPlugin {
public:
    virtual std::string getName() const = 0;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration dt) = 0;
};
```

### 2. Custom Neuron Types

Create custom neuron types by inheriting from the base Neuron class.

### 3. Experimental Variations

Test different experimental conditions:
```python
config.set("experimental_variant", "research")
config.set("experimental_parameter1", 1.5)
config.set("experimental_parameter2", 0.8)
```

### 4. Multi-Region Connectivity

Add complex connectivity between neural regions.

## Conclusion

The NLM examples provide a comprehensive introduction to the neural simulation framework. By following these examples and best practices, you can effectively use NLM for research, education, and development of artificial brain systems.

For more advanced topics and complete documentation, refer to:
- `NLM_Advanced_Guide.md` (this file)
- `docs/` directory for detailed technical documentation
- GitHub repository for the latest information and examples

## Acknowledgments

This project builds on the foundation of:
- Phase 2: Real neural computation
- Phase 6: Final integration
- Extensive research in computational neuroscience
- Open-source software communities

## License

NLM is licensed under the MIT License. See LICENSE file for details.
