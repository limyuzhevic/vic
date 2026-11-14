# NLM Python Bindings - Documentation

This documentation provides an overview of the improvements made to the NLM (Neural Learning Machine) Python bindings and the example scripts available.

## Overview

NLM is an experimental artificial developmental brain system written in C++ with Python bindings. The Python bindings provide an intuitive interface for creating, configuring, and simulating neural networks.

## Major Improvements

### 1. Enhanced Python API

#### New Convenience Methods:
- `world.getConfig()`: Get world configuration details
- `brain.getConfig()`: Get brain configuration details  
- `validateConfig(config)`: Validate configuration and throw errors for invalid configs
- `safeStep(brain, step)`: Safely perform a simulation step with error handling

#### New Helper Functions:
- `createExperimentSetup()`: Creates a complete experiment setup with default configuration
- `validateConfig()`: Validates configuration parameters
- `safeStep()`: Safely performs simulation steps with error handling

#### Improved Configuration:
- Better default configuration presets
- More flexible parameter parsing
- Enhanced error messages

### 2. Command Line Interface Enhancements

#### New Command Line Options:
- `--help, -h`: Display comprehensive help message
- `--verbose, -v`: Enable verbose logging
- `--benchmark, --perf`: Run performance benchmarks
- `--output, -o <file>`: Save output to file
- `--steps, --steps=<number>, -s <number>`: Number of simulation steps
- `--log-file, --log <file>`: Log output to file
- `--save-config`: Save current configuration to "last_config.cfg"
- Direct parameter setting: `--neuron_count=1000 --region_count=2`

#### Improved Argument Parsing:
- Support for `--key=value` format
- Better error handling for invalid parameters
- Configuration file loading with enhanced error messages
- Support for multiple configuration overrides

### 3. Configuration Presets

#### Built-in Configuration Templates:
- **minimal**: Small network for quick testing (100 neurons)
- **medium**: Medium-sized network for normal testing (1000 neurons)
- **large**: Large network for more realistic simulation (5000 neurons)
- **research**: Research-oriented configuration with high plasticity

#### Example Usage:
```python
from example_configurations import create_preset_config

# Create different preset configurations
minimal_config = create_preset_config("minimal")
medium_config = create_preset_config("medium")
large_config = create_preset_config("large")
research_config = create_preset_config("research")
```

### 4. Example Scripts

#### Quick Start Example:
**File**: `example_quickstart.sh`
A bash script that demonstrates the simplest way to get started with NLM.

#### Complete Python Example:
**File**: `example_complete.py`
Demonstrates the full workflow of using NLM with Python:
1. Create configuration
2. Create brain
3. Create agent interface
4. Create world
5. Initialize agent
6. Run simulation loop
7. Process results
8. Test checkpointing

#### Configuration Examples:
**File**: `example_configurations.py`
Shows different configuration presets and world configurations.

#### Benchmark Example:
**File**: `example_benchmark.py`
Demonstrates performance benchmarking of NLM Python bindings and API operations.

## Usage Examples

### Quick Start
```bash
# Run quickstart guide
./example_quickstart.sh

# Run the basic NLM with default options
nlm

# Run with verbose logging and custom configuration
nlm --verbose --config my_config.cfg --neuron_count=1000

# Get help
nlm --help
```

### Python Examples
```python
import pynlm
from example_complete import run_complete_example

# Run the complete example
run_complete_example()

# Or use the Python bindings directly
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### Command Line Examples
```bash
# Basic usage
nlm

# With custom configuration
nlm --config=my_config.cfg --steps=5000

# With verbose logging and benchmarking
nlm --verbose --benchmark

# Multiple configuration overrides
nlm --neuron_count=2000 --region_count=2 --random_seed=123

# Save configuration
nlm --save-config

# Save output to file
nlm --output=simulation_results.txt
```

## Configuration File Format

### Default Configuration (`configs/default.cfg`)
```
# Network parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Simulation parameters
random_seed = 42
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
```

### Command Line Override Format
```bash
nlm --neuron_count=2000 --region_count=2 --random_seed=123
nlm --config=my_config.cfg --neuron_count=1000
```

## Error Handling and Validation

### Configuration Validation
The `validateConfig()` function validates configuration and throws descriptive errors:

```python
from example_configurations import validateConfig

# This will throw an error because neuron_count is missing
validateConfig(config)  # Raises: RuntimeError("Invalid configuration: Missing required key 'neuron_count'.")
```

### Safe Simulation Steps
The `safeStep()` function safely performs simulation steps with error handling:

```python
from example_configurations import safeStep

# Safely step the brain
spike_count = safeStep(brain, 100)  # Returns the number of spikes generated
```

## Performance Considerations

### Python Bindings Performance
The benchmark script (`example_benchmark.py`) provides insights into:
- Configuration creation performance
- Brain initialization performance
- World creation performance
- Agent creation performance
- Neural simulation performance

### Recommended Usage Patterns

1. **For Development and Testing**:
   - Use the `minimal` preset
   - Enable verbose logging
   - Use smaller network sizes

2. **For Production Simulations**:
   - Use the `medium` or `large` presets
   - Disable verbose logging unless debugging
   - Monitor performance with benchmark tools

3. **For Research**:
   - Use the `research` preset
   - Enable all plasticity features
   - Use larger networks

## Troubleshooting

### Common Issues and Solutions

1. **Configuration File Not Found**
   ```bash
   # Ensure the configuration file exists
   touch my_config.cfg
   nlm --config=my_config.cfg
   ```

2. **Import Errors**
   ```bash
   # Install or rebuild the Python bindings
   pip install -e .
   ```

3. **Memory Issues**
   ```bash
   # Reduce network size for memory-constrained environments
   nlm --neuron_count=100
   ```

4. **Performance Issues**
   ```bash
   # Use smaller networks or disable certain features
   nlm --neuron_count=500 --benchmark
   ```

## Additional Resources

### Documentation Files
- `README.md`: High-level project overview
- `HOW_TO_USE.md`: Detailed usage instructions
- `docs/`: Additional documentation files

### Source Code
- `src/`: C++ source code
- `python/`: Python bindings source code
- `tests/`: Test suite

### Community
- Report issues: Submit GitHub issues
- Contribute: Fork and submit pull requests
- Discuss: Join community forums

## License

MIT License. See `LICENSE` file for details.

## Version Information

Current version: NLM Python Bindings v0.1.0
Phase 6: FINAL INTEGRATION

---

This documentation covers the major improvements made to the NLM Python bindings, including enhanced API, better command-line interface, configuration presets, and comprehensive example scripts.