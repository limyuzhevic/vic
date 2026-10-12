# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 6: Using the Enhanced NLM Command-Line Interface

The NLM now includes a comprehensive command-line interface (CLI) that provides powerful tools for running neural network simulations, experiments, and analysis. This section covers the enhanced CLI functionality.

### Overview

The enhanced NLM CLI provides:

- Rich command-line interface with subcommands
- Configuration presets for common use cases
- Scripting support for batch operations
- Help system with detailed documentation
- Logging configuration and verbosity control
- Progress monitoring and status reporting

### CLI Command Structure

All NLM CLI commands follow the pattern:

```bash
nlm [options] <command> [arguments]
```

**Global Options:**
- `--help, -h`: Show help information
- `--version, -v`: Show version information
- `--list-presets`: List available configuration presets
- `--verbose=<level>`: Verbosity level (quiet, error, warning, info, debug, trace)
- `--config=<file>`: Configuration file path
- `--output=<file>`: Output file path
- `--log=<file>`: Log file path

**Commands:**
- `run`: Run simulation
- `configure`: Manage configuration
- `experiment`: Run experiment
- `analyze`: Analyze results
- `benchmark`: Performance benchmarking

### Basic CLI Usage

#### Getting Started

```bash
# Show help information
nlm --help

# List available presets
nlm --list-presets

# Run a basic simulation
nlm run --type basic --steps 100

# Run a plasticity experiment
nlm experiment --type plasticity --steps 1000
```

#### Running Simulations

The `run` command executes neural network simulations:

```bash
# Basic neuron dynamics
nlm run --type basic --steps 100 --neurons 500

# Plasticity learning
nlm run --type plasticity --steps 1000 --neurons 1000

# Spike-timing-dependent plasticity (STDP) verification
nlm run --type stdp --steps 500

# Developmental simulation
nlm run --type development --steps 5000

# Cognitive simulation
nlm run --type cognitive --steps 10000

# Social interaction simulation
nlm run --type social --steps 3000

# Memory consolidation
nlm run --type memory --steps 20000

# Attention focus simulation
nlm run --type attention --steps 2000
```

#### Experiments

The `experiment` command runs specialized neural network experiments:

```bash
# Plasticity learning experiment
nlm experiment --type plasticity --steps 5000

# Development experiment
nlm experiment --type development --steps 10000

# Social learning experiment
nlm experiment --type social --steps 8000

# Memory formation experiment
nlm experiment --type memory --steps 15000

# Attention training experiment
nlm experiment --type attention --steps 3000
```

#### Configuration Management

The `configure` command manages configuration files:

```bash
# Show current configuration
nlm configure --show

# Save current configuration to file
nlm configure --save my_config.json

# Load configuration from file
nlm run --config my_config.json --type basic

# Reset to default configuration
nlm configure --reset
```

#### Analysis and Benchmarking

The `analyze` and `benchmark` commands provide analysis and performance measurement:

```bash
# Analyze simulation results
nlm analyze --format json --output analysis.json

# Performance benchmarking
nlm benchmark --duration 60

# Benchmark with specific parameters
nlm benchmark --duration 30 --neurons 500
```

#### Presets

NLM includes built-in configuration presets:

```bash
# Simple neuron dynamics
nlm run --preset basic --steps 100

# Learning and adaptation
nlm run --preset plasticity --steps 1000

# Developmental stages
nlm run --preset development --steps 5000

# Cognitive capabilities
nlm run --preset cognitive --steps 10000

# Social interaction
nlm run --preset social --steps 3000

# Memory-intensive scenarios
nlm run --preset memory --steps 20000

# Attention and focus
nlm run --preset attention --steps 2000
```

### Advanced CLI Usage

#### Custom Configuration

Create custom simulations with specific parameters:

```bash
# Custom simulation parameters
nlm run --type basic --steps 200 --neurons 750 --regions 2

# Combined preset and custom parameters
nlm run --preset plasticity --steps 2000 --neurons 1200 --regions 3

# Output to specific file
nlm run --type basic --steps 100 --output simulation_results.json
```

#### Verbosity Levels

Control output detail with different verbosity levels:

```bash
# Quiet - only errors and important messages
nlm run --type basic --verbose quiet

# Error - only errors
nlm run --type basic --verbose error

# Warning - warnings and errors
nlm run --type basic --verbose warning

# Info - information messages (default)
nlm run --type basic --verbose info

# Debug - detailed debug information
nlm run --type basic --verbose debug

# Trace - all available information
nlm run --type basic --verbose trace
```

#### Scripting and Automation

The CLI includes Python scripts for automation and batch processing:

```bash
# Run batch of simulations
python scripts/batch_run.py --presets basic plasticity --neurons 500 1000 --steps 100 500

# Run parameter sweep
python scripts/parameter_sweep.py --param neurons=500,1000 --param steps=100,500

# Run experiment suite
python scripts/experiment_runner.py --type plasticity,development --neurons 1000 1500
```

#### Help System

The CLI includes a comprehensive help system:

```bash
# Show general help
nlm --help

# Show help for specific command
nlm help run

# Show help for experiment command
nlm help experiment

# Get information about a specific preset
nlm help preset
```

### CLI Command Reference

#### Global Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help, -h` | Show help information | `nlm --help` |
| `--version, -v` | Show version information | `nlm --version` |
| `--list-presets` | List available presets | `nlm --list-presets` |
| `--verbose=<level>` | Verbosity level | `nlm --verbose debug` |
| `--config=<file>` | Configuration file | `nlm --config my_config.json` |
| `--output=<file>` | Output file | `nlm --output results.json` |
| `--log=<file>` | Log file | `nlm --log simulation.log` |

#### Commands

##### Run Command

| Option | Description | Example |
|--------|-------------|---------|
| `--type <type>` | Simulation type | `nlm run --type basic` |
| `--steps <n>` | Number of steps | `nlm run --steps 100` |
| `--neurons <n>` | Number of neurons | `nlm run --neurons 500` |
| `--regions <n>` | Number of regions | `nlm run --regions 2` |
| `--preset <preset>` | Configuration preset | `nlm run --preset basic` |
| `--output <file>` | Output file | `nlm run --output data.json` |

##### Experiment Command

| Option | Description | Example |
|--------|-------------|---------|
| `--type <type>` | Experiment type | `nlm experiment --type plasticity` |
| `--steps <n>` | Number of steps | `nlm experiment --steps 1000` |
| `--neurons <n>` | Number of neurons | `nlm experiment --neurons 1000` |
| `--regions <n>` | Number of regions | `nlm experiment --regions 3` |
| `--preset <preset>` | Configuration preset | `nlm experiment --preset plasticity` |
| `--output <file>` | Output file | `nlm experiment --output results.json` |

##### Configure Command

| Option | Description | Example |
|--------|-------------|---------|
| `--show` | Show current configuration | `nlm configure --show` |
| `--save <file>` | Save configuration to file | `nlm configure --save config.json` |
| `--reset` | Reset to default configuration | `nlm configure --reset` |
| `--load <file>` | Load configuration from file | `nlm configure --load config.json` |

##### Analyze Command

| Option | Description | Example |
|--------|-------------|---------|
| `--format <format>` | Output format | `nlm analyze --format json` |
| `--output <file>` | Output file | `nlm analyze --output analysis.json` |
| `--filter <category>` | Filter results | `nlm analyze --filter spikes` |

##### Benchmark Command

| Option | Description | Example |
|--------|-------------|---------|
| `--duration <sec>` | Benchmark duration | `nlm benchmark --duration 60` |
| `--steps <n>` | Number of steps | `nlm benchmark --steps 1000` |
| `--neurons <n>` | Number of neurons | `nlm benchmark --neurons 500` |
| `--output <file>` | Output file | `nlm benchmark --output benchmark.json` |

### Example CLI Workflows

#### Workflow 1: Basic Learning Simulation

```bash
# Create and configure simulation
nlm experiment --type plasticity --steps 1000 --neurons 1000

# Save results for analysis
nlm experiment --type plasticity --steps 1000 --neurons 1000 --output learning_results.json
```

#### Workflow 2: Development Research

```bash
# Run developmental simulation
nlm run --type development --steps 5000 --neurons 800

# Save configuration for reproducibility
nlm configure --save developmental_config.json

# Analyze results
nlm analyze --format json --output developmental_analysis.json
```

#### Workflow 3: Parameter Sweep

```bash
# Run parameter sweep using Python script
python scripts/parameter_sweep.py \
    --param neurons=500,1000,1500 \
    --param steps=100,500,1000 \
    --output parameter_results.csv \
    --save-analysis
```

#### Workflow 4: Batch Processing

```bash
# Run batch of simulations
python scripts/batch_run.py \
    --presets basic plasticity development \
    --neurons 500 1000 1500 \
    --steps 100 500 1000 \
    --output-dir batch_results \
    --save-results
```

#### Workflow 5: Experiment Suite

```bash
# Run comprehensive experiment suite
python scripts/experiment_runner.py \
    --type all \
    --neurons 1000 1500 2000 \
    --steps 500 1000 2000 \
    --output-dir experiment_suite \
    --create-comparison \
    --verbose
```

### CLI Integration with Python API

The CLI can be integrated with the Python API for complex workflows:

```python
import pynlm
import subprocess
import json

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save configuration for CLI
config.saveToFile("simulation_config.json")

# Run simulation using CLI
result = subprocess.run([
    "nlm", "run", 
    "--config", "simulation_config.json",
    "--type", "plasticity",
    "--steps", "1000",
    "--verbose", "quiet"
], capture_output=True, text=True)

print(f"Simulation completed with exit code: {result.returncode}")

# Analyze results
if result.returncode == 0:
    # Parse CLI output or use Python API
    print("Simulation completed successfully")
```

### Configuration File Format

NLM configuration files can be in various formats:

#### JSON Configuration

```json
{
    "random_seed": "42",
    "simulation_timestep": "0.001",
    "neuron_count": "1000",
    "region_count": "2",
    "connection_probability": "0.1",
    "plasticity_learning_rate": "0.01",
    "plasticity_stdp_enable": "true",
    "max_simulation_steps": "10000"
}
```

#### Text Configuration

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
plasticity_stdp_enable = true
max_simulation_steps = 10000
```

### Advanced Features

#### Environment Variables

Set environment variables to configure NLM:

```bash
export NLM_LOG_LEVEL=info
export NLM_OUTPUT_DIR=results/
export NLM_DEFAULT_PRESET=plasticity
```

#### Shell Integration

Add NLM to your shell configuration:

```bash
# Add NLM to PATH if not already there
export PATH="$PATH:/path/to/nlm/bin"

# Create alias for common commands
alias nlm-run="nlm run --type basic --steps 100"
alias nlm-explore="nlm experiment --type plasticity --steps 1000"
```

#### Custom Scripts

Create custom shell scripts for common tasks:

```bash
#!/bin/bash
# run-learning.sh

nlm experiment --type plasticity --steps "$1" --neurons "$2" \
    --output "learning_results_${1}_${2}.json" \
    --verbose info

# Usage: ./run-learning.sh 1000 1000
```

### Troubleshooting

#### Common CLI Issues

**Issue: "Command not found: nlm"**
```
Solution: Ensure NLM is installed and in your PATH
```

**Issue: Unknown command**
```
Solution: Check available commands with 'nlm --help'
```

**Issue: Invalid parameter**
```
Solution: Check parameter names and values with 'nlm help run'
```

**Issue: Configuration file not found**
```
Solution: Use absolute path or ensure file exists in current directory
```

**Issue: Simulation timeout**
```
Solution: Increase timeout or reduce simulation parameters
```

**Issue: Output file permissions**
```
Solution: Check file permissions or specify different output location
```

#### Common Python Script Issues

**Issue: "ModuleNotFoundError: No module named 'pynlm'"**
```
Solution: Install NLM with 'pip install .' or 'pip install -e .'
```

**Issue: "ImportError: cannot import name 'createDefaultConfig'"**
```
Solution: Check NLM version and installation
```

**Issue: "Configuration file not found"**
```
Solution: Use absolute path or ensure file exists
```

### Performance Tips

#### Optimizing CLI Usage

1. **Use presets for common configurations**
   ```bash
   nlm run --preset basic --steps 100
   ```

2. **Batch processing with Python scripts**
   ```bash
   python scripts/batch_run.py --presets basic plasticity
   ```

3. **Limit verbosity for production runs**
   ```bash
   nlm run --type basic --steps 100 --verbose quiet
   ```

4. **Save configurations for reproducibility**
   ```bash
   nlm configure --save production_config.json
   ```

5. **Use output files for analysis**
   ```bash
   nlm run --type basic --steps 100 --output results.json
   nlm analyze --format json --output analysis.json
   ```

#### System Resources

- **Memory**: For simulations with >2000 neurons, ensure sufficient RAM
- **CPU**: Multithreaded builds may use multiple cores
- **Disk space**: Large simulations may require significant disk space
- **Network**: Not required for local simulations

### Integration with Development Workflow

#### CI/CD Integration

```yaml
# Example GitHub Actions workflow
- name: Run NLM Simulation
  run: |
    nlm experiment --type plasticity --steps 1000 --neurons 1000
    nlm analyze --format json --output results.json
```

#### Testing

```bash
# Create test configurations
nlm run --type basic --steps 10 --neurons 100 --output test_results.json

# Verify output
python -c "import json; data = json.load(open('test_results.json')); print(f'Success: {data.get(\"spikes\", 0)} spikes')"
```

### Conclusion

The enhanced NLM CLI provides a comprehensive set of tools for running neural network simulations, experiments, and analysis. With its flexible command structure, configuration management, and scripting support, it enables users to efficiently configure, run, and analyze complex neural network simulations.

For more advanced usage, refer to the Python scripts in the scripts/ directory and the example configuration files in the configs/ directory. The CLI system is designed to be both powerful for complex workflows and simple for basic use cases.

The CLI integrates seamlessly with the NLM Python API, allowing users to leverage the strengths of both approaches for optimal results.

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### Enhanced Configuration Features

The NLM configuration system now provides advanced features for type-safe, validated, and organized configuration management:

#### 1. Type-Safe Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Type-safe getters with automatic type checking
if (neuron_count := config.get[int]("brain.neuron_count")):
    print(f"Neuron count: {neuron_count}")

if (density := config.get[float]("brain.synapse_density")):
    print(f"Synapse density: {density}")

# Get with default values
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
```

#### 2. Nested Configuration Support

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set nested values
config.set({"brain", "neuron_count"}, 2000)
config.set({"brain", "plasticity", "stdp", "enable"}, True)
config.set({"brain", "neuromodulation", "dopamine", "scale"}, 1.5)

# Get nested values
if (stdp_enable := config.get[bool]({"brain", "plasticity", "stdp", "enable"})):
    print(f"STDP enabled: {stdp_enable}")

if (dopamine_scale := config.get[float]({"brain", "neuromodulation", "dopamine", "scale"})):
    print(f"Dopamine scale: {dopamine_scale}")
```

#### 3. Validation Framework

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Add validation rules
config.addValidator("brain.neuron_count", 
    lambda key, value: (
        nlm.ValidationResult.success() 
        if isinstance(value.get[int](), int) and 100 <= value.get[int]() <= 10000
        else nlm.ValidationResult.error("Invalid neuron count", key)
    )
)

config.addValidator("brain.synapse_density",
    lambda key, value: (
        nlm.ValidationResult.success()
        if isinstance(value.get[float](), float) and 0.0 <= value.get[float]() <= 1.0
        else nlm.ValidationResult.error("Invalid synapse density", key)
    )
)

# Validate configuration
result = config.validate()
if result.valid:
    print("Configuration is valid!")
else:
    print(f"Validation error: {result.error}")
```

#### 4. Schema Validation

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Define JSON schema
schema = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {
            "type": "integer",
            "minimum": 100,
            "maximum": 10000,
            "description": "Total number of neurons"
        },
        "brain.synapse_density": {
            "type": "number",
            "minimum": 0.0,
            "maximum": 1.0,
            "description": "Synapse connectivity density"
        },
        "simulation.enabled": {
            "type": "boolean",
            "description": "Enable simulation"
        }
    },
    "required": ["brain.neuron_count", "brain.synapse_density", "simulation.enabled"]
}

# Set schema and validate
config.setSchema(schema)
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("simulation.enabled", True)

result = config.validateSchema()
if result.valid:
    print("Schema validation passed!")
else:
    print(f"Schema validation failed: {result.error}")
```

#### 5. Environment-Specific Configuration

```python
import pynlm

# Create configuration for different environments
config = pynlm.createDefaultConfig()

# Set environment
config.setEnvironment("development")

# Environment-specific overrides based on development mode
if config.isDevelopment():
    config.set("brain.debug", True)
    config.set("logging.level", "debug")
    config.set("simulation.max_steps", 1000)  # Longer training for debugging
else:
    config.set("brain.debug", False)
    config.set("logging.level", "info")
    config.set("simulation.max_steps", 100)   # Faster for production

# Create brain with environment-specific configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

#### 6. Value Interpolation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Enable interpolation
config.enableInterpolation(True)

# Set base values
config.set("brain.base_neurons", 1000)
config.set("brain.threshold", -55.0)

# Add interpolation references
config.addInterpolationReference("brain.neuron_count", "brain.base_neurons")
config.addInterpolationReference("brain.effective_threshold", "brain.threshold")

# Set values with interpolation
config.set("brain.neuron_count", "${brain.base_neurons}")
config.set("brain.effective_threshold", "${brain.threshold}")

# Resolve interpolations
success = config.resolveInterpolations()
if success:
    print("Interpolation resolved successfully")
    if (neuron_count := config.get[int]("brain.neuron_count")):
        print(f"Resolved neuron count: {neuron_count}")
```

#### 7. Template Configuration

```python
import pynlm

# Create configuration template
config = pynlm.createDefaultConfig()

# Define template
template = {
    "name": "default",
    "defaults": {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "brain.threshold": -55.0,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "simulation.enabled": True,
        "simulation.max_steps": 1000
    }
}

# Create configuration from template
config = pynlm.Config.fromTemplate(template)

# Modify and save
config.set("simulation.max_steps", 2000)
config.saveToFile("custom_config.json")
```

#### 8. File I/O with Format Support

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Save in different formats
config.saveToFile("config.json")   # JSON format
config.saveToFile("config.yaml")   # YAML format
config.saveToFile("config.txt")    # Simple text format

# Load from different formats
loaded_config = pynlm.Config.fromFile("config.json")
loaded_config2 = pynlm.Config.fromFile("config.yaml")
loaded_config3 = pynlm.Config.fromFile("config.txt")
```

#### 9. Advanced Usage Examples

```python
import pynlm

def runAdvancedSimulation():
    # Create advanced configuration
    config = pynlm.createDefaultConfig()
    
    # Set up comprehensive configuration
    config.set({"brain", "neuron_count"}, 5000)
    config.set({"brain", "synapse_density"}, 0.2)
    config.set({"brain", "threshold"}, -50.0)
    config.set({"brain", "plasticity", "stdp", "enable"}, True)
    config.set({"brain", "plasticity", "stdp", "learning_rate"}, 0.001)
    config.set({"brain", "neuromodulation", "curiosity", "enable"}, True)
    config.set({"brain", "development", "enable"}, True)
    config.set({"simulation", "max_steps"}, 10000)
    config.set({"simulation", "dt"}, 0.1)
    config.set({"environment", "name"}, "simulation")
    
    # Create environment-specific overrides
    if config.getEnvironment() == "development":
        config.set({"simulation", "debug"}, True)
        config.set({"logging", "level"}, "debug")
    
    # Add validation
    config.addValidator("brain.neuron_count",
        lambda key, value: (
            nlm.ValidationResult.success()
            if isinstance(value.get[int](), int) and value.get[int]() <= 10000
            else nlm.ValidationResult.error("Neuron count too high", key)
        )
    )
    
    # Enable interpolation for dynamic values
    config.enableInterpolation(True)
    config.addInterpolationReference("brain.effective_threshold", "brain.threshold")
    
    # Validate configuration
    validation_result = config.validate()
    if validation_result.valid:
        # Resolve interpolations
        if config.resolveInterpolations():
            # Create brain and world
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            for step in range(1000):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            print("Advanced simulation completed successfully!")
        else:
            print("Interpolation failed!")
    else:
        print(f"Configuration validation failed: {validation_result.error}")
    
    # Save configuration
    config.saveToFile("advanced_simulation_config.json")

# Run advanced simulation
runAdvancedSimulation()
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Enhanced Configuration System

### Advanced Configuration Features

The NLM configuration system now provides enhanced capabilities for type-safe, validated, and organized configuration management. This document covers the advanced configuration features available in the enhanced system.

### 1. Type-Safe Configuration

The configuration system provides type-safe getters that return `std::optional` values:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Type-safe getters
if (neuron_count := config.get[int]("brain.neuron_count")):
    print(f"Neuron count: {neuron_count}")

if (density := config.get[float]("brain.synapse_density")):
    print(f"Synapse density: {density}")

# Get with default values
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
```

### 2. Nested Configuration Support

Nested configuration allows for better organization of complex settings:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set nested values
config.set({"brain", "neuron_count"}, 2000)
config.set({"brain", "plasticity", "stdp", "enable"}, True)
config.set({"brain", "neuromodulation", "dopamine", "scale"}, 1.5)

# Get nested values
if (stdp_enable := config.get[bool]({"brain", "plasticity", "stdp", "enable"})):
    print(f"STDP enabled: {stdp_enable}")

# Get all keys
keys = config.getKeys()
for key in keys:
    print(f"Key: {key}")
```

### 3. Validation Framework

The validation framework provides comprehensive validation with clear error messages:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Add validation rules
config.addValidator("brain.neuron_count", 
    lambda key, value: (
        nlm.ValidationResult.success()
        if isinstance(value.get[int](), int) and value.get[int]() <= 10000
        else nlm.ValidationResult.error("Neuron count too high", key)
    )
)

config.addValidator("brain.synapse_density",
    lambda key, value: (
        nlm.ValidationResult.success()
        if isinstance(value.get[float](), float) and value.get[float]() <= 1.0
        else nlm.ValidationResult.error("Synapse density too high", key)
    )
)

# Validate configuration
result = config.validate()
if result.valid:
    print("Configuration is valid!")
else:
    print(f"Validation error: {result.error}")
```

### 4. Schema Validation

Schema validation provides structural validation of configuration:

```python
import pynlm
import json

config = pynlm.createDefaultConfig()

# Define JSON schema
schema = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {
            "type": "integer",
            "minimum": 100,
            "maximum": 10000
        },
        "brain.synapse_density": {
            "type": "number",
            "minimum": 0.0,
            "maximum": 1.0
        }
    },
    "required": ["brain.neuron_count", "brain.synapse_density"]
}

# Set schema and validate
config.setSchema(schema)
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

result = config.validateSchema()
if result.valid:
    print("Schema validation passed!")
else:
    print(f"Schema validation failed: {result.error}")
```

### 5. Environment-Specific Configuration

Environment-specific configuration allows for different settings based on deployment environment:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set environment
config.setEnvironment("development")

# Environment-specific overrides
if config.isDevelopment():
    config.set("brain.debug", True)
    config.set("logging.level", "debug")
    config.set("simulation.max_steps", 1000)
else:
    config.set("brain.debug", False)
    config.set("logging.level", "info")
    config.set("simulation.max_steps", 100)

print(f"Running in {config.getEnvironment()} environment")
```

### 6. Value Interpolation

Value interpolation allows configuration values to reference other values:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Enable interpolation
config.enableInterpolation(True)

# Set base values
config.set("brain.base_neurons", 1000)
config.set("brain.threshold", -55.0)

# Add interpolation references
config.addInterpolationReference("brain.neuron_count", "brain.base_neurons")

# Set values with interpolation
config.set("brain.neuron_count", "${brain.base_neurons}")

# Resolve interpolations
success = config.resolveInterpolations()
if success:
    if (neuron_count := config.get[int]("brain.neuron_count")):
        print(f"Interpolated neuron count: {neuron_count}")
```

### 7. Template Support

Templates allow for reusable configuration sets:

```python
import pynlm

# Create template
template = {
    "name": "default",
    "defaults": {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "brain.threshold": -55.0,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001
    }
}

# Create configuration from template
config = pynlm.Config.fromTemplate(template)

# Apply template
config.applyTemplate("default")
```

### 8. File I/O Support

The enhanced system supports multiple file formats:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

# Save in different formats
config.saveToFile("config.json")   # JSON format
config.saveToFile("config.yaml")   # YAML format
config.saveToFile("config.txt")    # Simple text format

# Load from different formats
loaded_config = pynlm.Config.fromFile("config.json")
```

### 9. Configuration Comparison and Merge

Configuration comparison and merge operations:

```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 1000)

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 2000)

# Check equality
if config1 == config2:
    print("Configurations are equal")
else:
    print("Configurations differ")

# Merge configurations
config1.merge(config2)
```

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
