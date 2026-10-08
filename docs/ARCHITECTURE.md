# NLM Brain - Complete User Guide
# Comprehensive Documentation and Examples

## Overview

NLM (熙然) is an experimental artificial developmental brain project that implements:
- Real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- Multiple plasticity rules (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation systems (dopamine, curiosity, novelty)
- Cognitive mechanisms (attention, planning, concept formation)
- Developmental stages affecting plasticity rates

## Quick Start

### Installation

```bash
# Clone the repository
mkdir nlm_project
cd nlm_project

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Basic Usage

```bash
# Start the NLM brain in interactive mode (default)
./nlm

# Run a specific experiment
./nlm run demo

# Save brain state
./nlm save my_brain.checkpoint

# Load brain state
./nlm load my_brain.checkpoint

# Run simulation for 1000 steps
./nlm step 1000
```

## Command-Line Interface

### Overview

The NLM brain provides a comprehensive command-line interface for controlling and monitoring the neural system.

### Basic Commands

| Command | Description |
|---------|-------------|
| `help` | Show this help message |
| `status` | Display current brain status and statistics |
| `stats` | Show detailed performance statistics |
| `reset` | Reset brain to initial state |
| `exit/quit` | Exit the program |

### Brain Control Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `step` | `step [n]` | Run simulation for n steps (default: 100) |
| `save` | `save <file>` | Save current brain state to checkpoint file |
| `load` | `load <file>` | Load brain state from checkpoint file |
| `inject` | `inject <values>` | Inject sensory input (comma-separated) |

### Memory System Commands

| Command | Description |
|---------|-------------|
| `memory` | Show working memory and episodic memory status |
| `episodic` | Show episodic memory details |
| `working` | Show working memory details |
| `consolidate` | Manually consolidate memories |
| `replay` | Replay recent episodes |

### Neuromodulation Commands

| Command | Description |
|---------|-------------|
| `neuromod` | Show dopamine, curiosity, novelty levels |
| `set-dopamine` | Set dopamine level (0.0-1.0) |
| `set-curiosity` | Set curiosity level (0.0-1.0) |
| `set-novelty` | Set novelty detection sensitivity |

### Configuration Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `set` | `set <key> <value>` | Set configuration parameter |
| `get` | `get <key>` | Get configuration parameter value |
| `config` | Show all configuration parameters |

### Experiment Commands

| Command | Description |
|---------|-------------|
| `run <name>` | Run specific experiment |
| `demo` | Run Phase 6 integration demo |
| `benchmark` | Run performance benchmark |
| `ablation` | Run ablation study |

## Configuration Parameters

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int64 | 1000 | Number of neurons in the brain |
| `region_count` | int64 | 1 | Number of brain regions |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `random_seed` | int64 | 42 | Random seed for reproducibility |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int64 | neuron_count/10 | Capacity of working memory |
| `episodic_max_episodes` | int64 | 1000 | Maximum episodic memory episodes |
| `replay_interval` | size_t | 100 | Steps between memory replay |
| `consolidation_interval` | size_t | 1000 | Steps between memory consolidation |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_level` | float | 0.5 | Baseline dopamine level |
| `dopamine_plasticity_factor` | float | 1.0 | Neuromodulatory effect on plasticity |
| `curiosity_level` | float | 0.5 | Baseline curiosity level |
| `novelty_sensitivity` | float | 0.5 | Sensitivity to novel stimuli |

## Available Experiments

### 1. Connectivity Experiment

Tests basic neural connectivity by injecting current into neurons and observing spike propagation.

```bash
./nlm run connectivity
```

### 2. Plasticity Experiment

Demonstrates synaptic plasticity through experience. The brain learns by applying repeated input patterns.

```bash
./nlm run plasticity
```

### 3. STDP Verification

Verifies Spike-Timing-Dependent Plasticity by creating correlated pre-post neuronal activity.

```bash
./nlm run stdp
```

### 4. Learning Experiment

Comprehensive learning experiment showing weight changes through experience.

```bash
./nlm run learning
```

### 5. Phase 6 Demo

Integration demo showing all brain systems working together:
- Memory systems coordinated
- Neuromodulation affecting learning
- Prediction and cognition integrated
- Development affecting plasticity
- Replay and consolidation functional

```bash
./nlm run demo
```

### 6. Performance Benchmark

Measures and reports brain performance metrics.

```bash
./nlm run benchmark
```

### 7. Ablation Study

Tests the impact of removing specific brain components.

```bash
./nlm run ablation
```

## Scripting

The NLM brain supports running commands from script files for automation and batch processing.

### Script Format

Scripts are plain text files where each line is a command:
```bash
# Comments are allowed
step 500
save brain_state.checkpoint
run demo
load brain_state.checkpoint
```

### Running Scripts

```bash
./nlm script my_commands.nlm
```

### Script Examples

#### Basic Script
```bash
# Initialize and run simulation
reset
step 1000
stats

# Save and exit
save learning_session.checkpoint
```

#### Learning Script
```bash
# Start learning experiment
run plasticity

# Monitor progress
step 100
stats

# Continue learning
step 500
stats

# Save learned state
save learned_brain.checkpoint
```

#### Research Script
```bash
# Experiment 1: Baseline
run connectivity

# Experiment 2: Plasticity
run plasticity

# Experiment 3: STDP verification
run stdp

# Experiment 4: Integration demo
run demo

# Save all results
save all_experiments.checkpoint
```

## Advanced Usage

### Custom Configuration

Create a custom configuration file for your experiments:

```bash
# Create custom config
cat > my_config.cfg << EOF
neuron_count 5000
region_count 2
simulation_timestep 0.0005
connection_probability 0.15
stdp_ltp_weight 0.025
stdp_ltd_weight 0.02
synaptogenesis_rate 0.00005
pruning_rate 0.000005
replay_interval 50
consolidation_interval 500
EOF

# Use custom config
./nlm --config my_config.cfg
```

### Automation with Shell Scripts

Create automated experiment sequences:

```bash
#!/bin/bash

# Run learning experiment
echo "Running learning experiment..."
./nlm run learning

# Monitor progress
echo "Monitoring..."
./nlm step 1000

# Save results
echo "Saving results..."
./nlm save learning_results.checkpoint

# Load and test
echo "Testing saved model..."
./nlm load learning_results.checkpoint
./nlm run demo

echo "All experiments completed!"
```

### Integration with Other Tools

The NLM brain can be integrated with:

- **Python scripts**: For analysis and visualization
- **MATLAB/Octave**: For mathematical modeling
- **TensorFlow/PyTorch**: For hybrid approaches
- **ROS**: For robotics applications
- **Web APIs**: For remote control

## Performance Tips

### For Large Brains

- Use multiple regions for better performance
- Adjust timestep for faster/slower simulation
- Configure memory parameters based on available RAM
- Use checkpoint files to save progress

### For Development

- Use `step 100` for quick testing
- Check `stats` after each experiment
- Monitor `memory` usage regularly
- Use `reset` between experiments

### For Production

- Use `step 1000` or more for stable results
- Implement automated checkpointing
- Monitor with external tools
- Use scripts for batch processing

## Troubleshooting

### Common Issues

**Brain fails to initialize**
```bash
# Try with default configuration
./nlm

# Check for errors in console output
# If persistent, report an issue
```

**No spikes generated**
```bash
# Check neuron parameters
./nlm stats

# Ensure neurons have sufficient input
./nlm inject 50.0,30.0,40.0

# Run connectivity test
./nlm run connectivity
```

**Memory usage too high**
```bash
# Reduce memory parameters
./nlm set working_memory_capacity 100
./nlm set episodic_max_episodes 500

# Run with smaller brain
./nlm set neuron_count 500
```

**Performance issues**
```bash
# Use fewer regions
./nlm set region_count 1

# Increase timestep
./nlm set simulation_timestep 0.001

# Run with smaller brain
./nlm set neuron_count 1000
```

### Getting Help

If you encounter issues:

1. **Check the console output** for error messages
2. **Use the `help` command** for available commands
3. **Check the statistics** with `stats`
4. **Use `reset`** to return to a clean state
5. **Run experiments** to test specific functionality
6. **Check documentation** in the docs/ directory
7. **Report issues** with detailed error messages

## Files and Directories

### Project Structure

```
NLM/
├── src/
│   ├── core/                    # Core utilities
│   ├── brain/                   # Neural components
│   ├── dynamics/                # Neural dynamics
│   ├── plasticity/              # Plasticity rules
│   ├── development/             # Developmental system
│   ├── neuromodulation/          # Neuromodulators
│   ├── memory/                  # Memory systems
│   ├── prediction/              # Prediction systems
│   ├── cognition/               # Cognitive mechanisms
│   ├── sensory/                 # Sensory processing
│   ├── motor/                   # Motor system
│   ├── environment/             # Environment interface
│   ├── experiments/             # Experiment framework
│   └── visualization/           # Visualization
├── configs/                     # Configuration files
├── docs/                        # Documentation
│   ├── ARCHITECTURE.md          # System architecture
│   ├── SCIENCE.md               # Scientific background
│   ├── ROADMAP.md               # Development roadmap
│   ├── EXPERIMENTS.md           # Experiment documentation
│   └── LIMITATIONS.md           # Known limitations
├── examples/                    # Example scripts and configurations
└── build/                       # Build artifacts
```

### Key Files

- **src/main.cpp**: Command-line interface
- **src/brain/Brain.hpp**: Brain class interface
- **src/brain/Brain.cpp**: Brain implementation
- **configs/default.cfg**: Default configuration
- **docs/ARCHITECTURE.md**: System architecture
- **examples/*.sh**: Example scripts

## Acknowledgements

NLM is an open-source research project. We acknowledge:

- All contributors to the codebase
- The scientific community for inspiration and guidance
- Users who provide feedback and suggestions
- The C++ community for excellent language features

## License

MIT License. See LICENSE file for details.

---

This documentation is continuously updated. For the latest version, visit the project repository.

© 2026 NLM Project. All rights reserved.
