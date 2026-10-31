# NLM Command-Line Interface - User Guide

This document provides comprehensive information about the NLM command-line interface (CLI) tool.

## Overview

The NLM CLI tool provides a user-friendly interface for exploring, configuring, and running neural simulations. It replaces the complex C++ executables with a more intuitive, command-driven interface that supports both simple usage and advanced operations.

## Installation

The CLI tool is installed automatically when you build NLM from source. To use it, simply run `nlm` from your terminal.

## Getting Started

### Basic Commands

```bash
# Show help
nlm help

# Show version information
nlm version

# List available experiments
nlm experiments list

# Run a simple demo
nlm run demo
```

### Quick Start

Here's how to get started with a simple simulation:

```bash
# Run a Phase 6 integration demo
nlm run --experiment phase6 --steps 2000 --neurons 500

# Save and load brain configurations
nlm brain init --neurons 1000
nlm config save my_first_brain.cfg
nlm brain step 10
nlm config save my_evolved_brain.cfg
```

## Command Reference

### Core Commands

#### `nlm help [command]`
Display help information about a specific command or list all commands.

**Usage:**
```bash
nlm help                    # List all commands
nlm help brain             # Help for brain commands
nlm help explore           # Help for explore commands
```

#### `nlm version`
Display NLM version information.

**Usage:**
```bash
nlm version
```

### Brain Management Commands

#### `nlm brain init [options]`
Initialize a new brain with specified parameters.

**Options:**
```
--neurons COUNT        Number of neurons (default: 1000)
--regions COUNT        Number of brain regions (default: 1)
--seed SEED            Random seed for reproducibility
--config FILE          Load configuration from file
--fast                 Use faster but less accurate initialization
```

**Examples:**
```bash
# Create a small brain
nlm brain init --neurons 500 --regions 2 --seed 42

# Initialize from existing config
nlm brain init --config my_settings.cfg
```

#### `nlm brain step [steps]`
Run simulation steps.

**Options:**
```
STEPS                  Number of steps to run (default: 1)
--timestep SECONDS     Simulation timestep in seconds (default: 0.001)
--monitor              Enable real-time monitoring
--verbose              Print detailed progress information
```

**Examples:**
```bash
# Run 100 steps
nlm brain step 100

# Run with monitoring
nlm brain step 500 --monitor
```

#### `nlm brain explore [category] [options]`
Explore brain state and statistics.

**Categories:**
- `neurons` - Explore neural populations
- `synapses` - Explore synaptic connections
- `plasticity` - Explore plasticity mechanisms
- `memory` - Explore memory systems
- `neuromodulation` - Explore neuromodulation

**Options (varies by category):**
```
--region REGION_ID     Brain region to explore
--type TYPE            Neuron or synapse type
--min-value VALUE      Minimum value for filtering
--max-value VALUE      Maximum value for filtering
--count N              Maximum number of items to show
--sort-by ATTRIBUTE    Sort by this attribute (default: depends on category)
--ascending            Sort in ascending order
```

**Examples:**
```bash
# Explore neurons in region 1
nlm brain explore neurons --region 1 --count 50

# Find strong synapses
nlm brain explore synapses --min-weight 0.5 --max-weight 1.0

# Check plasticity status
nlm brain explore plasticity --stdp-enabled --hebbian-enabled
```

### Experiment Commands

#### `nlm experiments list`
List available experiments.

**Usage:**
```bash
nlm experiments list
```

**Output:**
```
Available experiments:
  1. phase3_demo     - Phase 3: World interaction demo
  2. phase4_demo     - Phase 4: Emerging cognition demo
  3. phase6_demo     - Phase 6: Integration test demo
  4. custom         - Custom experiment configuration

Run 'nlm experiments info <number>' for more details about a specific experiment.
```

#### `nlm experiments run <number> [options]`
Run a specific experiment.

**Options:**
```
--steps STEPS         Number of simulation steps (default: experiment-specific)
--neurons NEURONS     Number of neurons (default: experiment-specific)
--regions REGIONS     Number of regions (default: experiment-specific)
--save FILE           Save experiment results to file
--load FILE           Load experiment configuration from file
--continue           Continue from saved state
--random-seed SEED    Set random seed for reproducibility
```

**Examples:**
```bash
# Run Phase 6 demo with custom settings
nlm experiments run 3 --steps 2000 --neurons 500 --save results.json

# Load and run a saved experiment
nlm experiments run 2 --load saved_config.cfg --continue
```

### Configuration Commands

#### `nlm config [action] [file]`
Manage configuration files.

**Actions:**
- `save <file>` - Save current configuration to file
- `load <file>` - Load configuration from file
- `show` - Display current configuration
- `edit <file>` - Edit configuration file

**Examples:**
```bash
# Save current brain state
nlm config save my_brain.cfg

# Load a saved configuration
nlm config load my_brain.cfg

# View current settings
nlm config show
```

### Advanced Commands

#### `nlm script [action] <file>`
Execute or save simulation scripts.

**Actions:**
- `run <file>` - Execute a simulation script
- `save <file>` - Save current state as script
- `list` - List available scripts

**Script Format:**
Scripts are Python-like syntax with NLM-specific commands:
```python
# Example script
brain.init(neurons=1000, regions=2)
brain.step(100)
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

**Examples:**
```bash
# Run a script
nlm script run my_simulation.nls

# Save current state
nlm script save current_state.nls
```

#### `nlm monitor [action]`
Control real-time monitoring.

**Actions:**
- `start` - Start monitoring
- `stop` - Stop monitoring
- `status` - Show monitoring status

**Options:**
```
--output FILE     File to write monitoring data to
--format FORMAT   Output format: text, json, csv (default: text)
--interval SECONDS  Monitoring update interval
```

### Exploration Commands

#### `nlm explore patterns [options]`
Explore neural activity patterns.

**Options:**
```
--type TYPE          Pattern type: synaptic, neural, memory, neuromod
--min-occurrence N   Minimum occurrence count
--max-occurrence N   Maximum occurrence count
--similarity VALUE   Similarity threshold for pattern matching
```

**Examples:**
```bash
# Find common synaptic patterns
nlm explore patterns --type synaptic --min-occurrence 10

# Find active neural patterns
nlm explore patterns --type neural --similarity 0.8
```

## Interactive Mode

The NLM CLI supports an interactive mode for exploration and experimentation.

### Starting Interactive Mode
```bash
nlm interactive
```

### Interactive Commands
```
 help                    Show help
 exit                    Exit interactive mode
 explore category [options]  Explore brain state
 brain init [options]      Initialize brain
 brain step [steps]         Run simulation steps
 experiment run <number>   Run experiment
 config save <file>        Save configuration
 config load <file>        Load configuration
 monitor start             Start monitoring
 monitor stop              Stop monitoring
```

### Example Interactive Session
```
nlm interactive

> help explore neurons
Available neuron exploration options:
  --region REGION_ID     Brain region to explore
  --type TYPE            Neuron type (Sensory, Internal, Motor)
  --count N              Maximum number of neurons to show

> explore neurons --region 1 --count 20

Neurons in Region 1:
  1. ID: 1, Type: Sensory, Activity: 0.45
  2. ID: 2, Type: Sensory, Activity: 0.38
  ...

> brain step 50 --monitor
Running 50 simulation steps...
Step 25: 147 neurons firing (7.3% activity)
Step 50: 189 neurons firing (9.5% activity)
Monitoring data saved to monitor_data.csv

> exit
```

## Scripting and Automation

### Batch Processing
Create batch processing scripts for automated simulations:

```bash
#!/bin/bash
# batch_simulation.sh

# Initialize brain
nlm brain init --neurons 1000 --regions 2

# Run multiple experiments
for steps in 100 500 1000; do
    echo "Running ${steps} steps..."
    nlm brain step ${steps} --verbose
    nlm config save "results_${steps}steps.cfg"
done

# Analyze results
echo "Analysis complete."
nlm explore patterns --type neural --min-occurrence 5
```

### Automated Experimentation
```bash
#!/bin/bash
# auto_experiment.sh

# Run experiment with different parameters
for neurons in 500 1000 2000; do
    for regions in 1 2 4; do
        echo "Running experiment: neurons=${neurons}, regions=${regions}"
        nlm experiments run 3 \
            --steps 1000 \
            --neurons ${neurons} \
            --regions ${regions} \
            --save "results_n${neurons}_r${regions}.json"
    done
done

# Generate summary report
echo "Experiment complete. Summary:"
nlm experiments list
```

## Configuration Files

### Default Configuration
The NLM CLI uses configuration files in JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "random_seed": 42
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000
    },
    "plasticity": {
        "stdp_enabled": true,
        "hebbian_enabled": true,
        "structural_enabled": false
    }
}
```

### Configuration Commands
```bash
# Create a custom configuration
nlm config edit custom.cfg

# View current configuration
nlm config show

# Import from Python
nlm config import --python script.py --output config.json
```

## Performance Tips

### Optimizing Simulations
```bash
# Use faster initialization for prototyping
nlm brain init --neurons 100 --fast

# Monitor performance during simulation
nlm brain step 1000 --monitor

# Profile simulation performance
nlm profile brain --steps 500 --output profile_data.txt
```

### Working with Large Brains
```bash
# Use more efficient exploration
nlm brain explore neurons --region 1 --count 10

# Focus on specific neuron types
nlm brain explore neurons --type Motor --count 5

# Analyze synaptic patterns for optimization
nlm explore patterns --type synaptic --min-occurrence 20
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "Command not found: nlm"**
```bash
# Install NLM
pip install .
# or build from source
cmake .. && make
```

**Issue: Brain initialization fails**
```bash
# Check system resources
nml system check

# Try with smaller brain
nlm brain init --neurons 100

# Check configuration file
nlm config show
```

**Issue: Script execution fails**
```bash
# Check script syntax
nlm script validate my_script.nls

# Run with debug output
nlm script run my_script.nls --debug
```

### Error Codes
- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Simulation error
- `4`: File I/O error
- `5`: Memory error

## Examples

### Example 1: Basic Usage
```bash
# Create and run a simple brain
nlm brain init --neurons 500
nlm brain step 100
nlm config save my_brain.cfg
```

### Example 2: Experiment Runner
```bash
# Run Phase 6 demo with custom settings
nlm experiments run 3 \
    --steps 2000 \
    --neurons 500 \
    --regions 2 \
    --save experiment_results.json
```

### Example 3: Interactive Exploration
```bash
nlm interactive
> explore neurons --region 1 --count 10
> brain step 50 --monitor
> explore patterns --type neural --min-occurrence 3
> exit
```

### Example 4: Automated Analysis
```bash
#!/bin/bash
# analyze_brain.sh

# Initialize and run simulation
nlm brain init --neurons 1000
nlm brain step 500 --monitor

# Analyze results
nlm explore neurons --region 1 --count 20
nlm explore patterns --type synaptic --min-occurrence 5

# Save configuration
nlm config save analyzed_results.cfg

echo "Analysis complete. Results saved to analyzed_results.cfg"
```

## Advanced Usage

### Custom Commands
Create custom commands for frequently used operations:

```bash
# Add custom command
nlm command add custom-experiment "nlm experiments run 3 --steps 1000 --neurons 500"

# Run custom command
nlm custom-experiment
```

### Command Aliases
```bash
# Create aliases in ~/.nlmrc
alias fast="nlm brain init --neurons 100 --fast"
alias demo="nlm experiments run 3 --steps 500"

# Use aliases
echo "Running fast demo..."
fast
demo
```

## Conclusion

The NLM CLI tool provides a comprehensive interface for working with neural simulations. Whether you're a beginner looking for simple commands or an advanced user needing complex scripting capabilities, the NLM CLI has you covered.

For more information about specific commands or advanced features, use `nlm help` followed by the command name you're interested in.
