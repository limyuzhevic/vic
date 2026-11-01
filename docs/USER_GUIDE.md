# NLM (熙然) - User Guide

## Welcome to NLM!

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide will help you get started with NLM and explore its features.

## What is NLM?

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements:

- **Leaky Integrate-and-Fire (LIF) neurons**: Biologically-inspired neuron models
- **Event-driven spike propagation**: Efficient signal transmission with delays
- **Multiple plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Integrated cognitive systems**: Memory, prediction, attention, and planning
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Development**: Brain maturation over time

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Memory**: 4GB RAM minimum (8GB+ recommended for larger simulations)
- **Disk Space**: 1GB free space

### Installation

#### From Source (Recommended for Development)

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build the C++ project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Install Python Library

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode (keeps source accessible)
pip install -e .
```

#### Quick Start Script

For a faster setup, you can use the provided setup script:

```bash
cd nlm
./scripts/setup.sh  # Automated build and installation
```

## First Steps

### Step 1: Verify Installation

After installation, verify that everything works:

```bash
# Test the C++ executable
./nlm

# Test Python bindings
python3 -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### Step 2: Run Basic Simulation

The simplest way to start is to run the basic simulation:

```bash
./nlm
```

This will run Phase 2, which includes:

1. **Test 1**: Basic neural connectivity test
2. **Test 2**: Plasticity learning experiment  
3. **Test 3**: STDP verification

### Step 3: Explore Python API

The Python bindings provide an easier interface for most users:

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 1000 steps
for step in range(1000):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Using the Command Line Interface

### Basic Commands

| Command | Description |
|---------|-------------|
| `--help` | Show basic help message |
| `--version` | Show version information |
| `--advanced-help` | Show detailed command-line options |

### Quick Reference

```bash
# Run simulation with default settings
./nlm

# Run with custom configuration
./nlm --config my_config.cfg

# Show detailed options
./nlm --advanced-help

# Run minimal test
./nlm --steps 100 --quiet
```

## Configuration

### Default Configuration

NLM uses a simple configuration file (`configs/default.cfg`) with the following settings:

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

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Creating Custom Configuration

```bash
# Copy default configuration
cp configs/default.cfg my_config.cfg

# Edit the configuration file with your desired values
nano my_config.cfg

# Run with custom configuration
./nlm --config my_config.cfg
```

### Command Line Overrides

Command line arguments can override configuration file values:

```bash
# Override multiple values
./nlm \
  --neuron-count 2000 \
  --steps 5000 \
  --seed 12345 \
  --plasticity-on

# Enable development and advanced features
./nlm \
  --development \
  --enable-neuromodulation \
  --enable-curiosity \
  --enable-novelty \
  --enable-prediction
```

## Understanding the Brain

### What Happens During Simulation

1. **Input Processing**: Sensory neurons receive input signals
2. **Neural Computation**: Neurons integrate inputs and may fire spikes
3. **Signal Propagation**: Spikes travel between neurons through synapses
4. **Learning**: Synaptic weights change based on activity patterns
5. **Output Generation**: Motor neurons produce action commands

### Key Metrics to Monitor

| Metric | Description |
|--------|-------------|
| `TotalNeuronCount()` | Total number of neurons in the brain |
| `TotalSynapseCount()` | Total number of connections |
| `FiringNeuronCount()` | Number of currently firing neurons |
| `TotalSpikeCount()` | Total spikes generated during simulation |
| `AverageFiringRate()` | Average firing rate across all neurons |

### Development Stages

NLM simulates brain development through different stages:

- **Initial**: Primitive network with basic connectivity
- **CriticalPeriod**: High plasticity, rapid learning
- **Maturation**: Fine-tuning of neural circuits
- **Adult**: Stable, optimized network
- **Aging**: Gradual decline in plasticity

## Experiments and Demos

### Available Demos

| Demo | Description |
|------|-------------|
| `./nlm` | Phase 2: Basic neural computation |
| `./nlm_phase3_demo` | Phase 3: World interaction |
| `./nlm_phase4_demo` | Phase 4: Emerging cognition |
| `./nlm_phase6_demo` | Phase 6: Final integration |

### Running Different Demos

```bash
# Phase 2: Basic neural computation (already covered)
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Emerging cognition
./nlm_phase4_demo

# Phase 6: Final integration
./nlm_phase6_demo
```

## Advanced Features

### Neuromodulation Systems

Enable advanced neuromodulation systems:

```bash
# Enable all neuromodulation
./nlm --enable-neuromodulation

# Enable specific systems
./nlm --enable-curiosity --enable-novelty --enable-prediction
```

### Memory Systems

NLM includes sophisticated memory systems:

- **Working Memory**: Maintains current activity patterns
- **Episodic Memory**: Stores experience episodes
- **Associative Memory**: Forms pattern associations

Enable memory features:

```bash
./nlm --enable-memory
```

### Attention and Planning

Enable cognitive features:

```bash
./nlm --enable-attention --enable-planning --enable-prediction
```

### Checkpoints and Persistence

Save and reload brain state:

```bash
# Save checkpoint
./nlm --save-checkpoint my_brain.bin

# Load from checkpoint
./nlm --load-checkpoint my_brain.bin

# Periodic checkpointing
./nlm --checkpoint-dir ./checkpoints --checkpoint-interval 10000
```

## Python Bindings

The Python API provides an easier interface for most users:

### Installation

```bash
# Install the Python package
pip install pynlm

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Optional: Add sensory input
    # brain.receiveSensoryInput(sensory_data)
    
    # Optional: Get actions
    # action = brain.produceAction()

# Get statistics
print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
```

### Complete Example

```python
def run_agent_simulation(world_config, brain_config, num_steps=1000):
    """Complete agent simulation with brain and world."""
    
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation({}, {}, 1000)
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem**: The brain appears inactive during simulation.

**Solutions**:
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Check that `--steps` is large enough (default: 2000)
3. Enable plasticity: `--plasticity-on` (default is on)
4. Enable neuromodulation: `--enable-neuromodulation`
5. Check log output: `--log-level DEBUG`

#### "The agent isn't moving"

**Problem**: The agent in the world doesn't respond to stimuli.

**Solutions**:
1. Ensure world is being updated: check world timestep
2. Verify sensory input is being processed
3. Check motor decoding is working
4. Debug with: `--log-level INFO --log-file debug.log`

#### "Everything is 0"

**Problem**: All outputs are zero.

**Solutions**:
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - ensure sensory input is connected
3. Enable proper neuromodulation: `--enable-curiosity --enable-novelty`
4. Check development is working: `--development`

#### "Performance issues"

**Problem**: Simulation is too slow.

**Solutions**:
1. Enable vectorization: `--vectorize`
2. Optimize for CPU: `--threads 4`
3. Reduce neuron count: `--neuron-count 1000`
4. Use checkpoints: `--checkpoint-interval 5000`

### Debug Output Options

```bash
# Show all debugging information
./nlm --log-level DEBUG

# Log to file for analysis
./nlm --log-level INFO --log-file nlm.log

# Minimal output (only errors)
./nlm --quiet
```

## Performance Optimization

### Tips for Faster Simulations

1. **Use vectorization**: Enable SIMD instructions
   ```bash
   ./nlm --vectorize
   ```

2. **Utilize multiple CPU cores**:
   ```bash
   ./nlm --threads 4
   ```

3. **Reduce neuron count** for faster testing:
   ```bash
   ./nlm --neuron-count 1000
   ```

4. **Use checkpoints** for large simulations:
   ```bash
   ./nlm --checkpoint-interval 10000
   ```

5. **Optimize configuration**:
   ```bash
   # Use fewer regions for smaller simulations
   ./nlm --region-count 1
   
   # Use larger timestep for faster simulation
   ./nlm --timestep 0.01
   ```

## Scaling and Large Simulations

### Running Large Networks

For simulations with thousands of neurons:

```bash
# Example: 10,000 neurons with 4 regions
./nlm \
  --neuron-count 10000 \
  --region-count 4 \
  --plasticity-off \
  --steps 50000 \
  --checkpoint-dir ./checkpoints \
  --checkpoint-interval 20000 \
  --checkpoint-keep 5 \
  --vectorize \
  --threads 8
```

### Monitoring Performance

Enable performance profiling:

```bash
./nlm --profile --log-level INFO --log-file performance.log
```

## Scientific Research

### Reproducible Experiments

Use deterministic random seeds:

```bash
# Set random seed for reproducibility
./nlm --seed 12345

# Use configuration files for reproducibility
cat > experiment_config.cfg << EOF
random_seed = 12345
neuron_count = 2000
region_count = 2
simulation_timestep = 0.001
EOF

./nlm --config experiment_config.cfg
```

### Data Collection

Export results in different formats:

```bash
# Export to JSON
./nlm --json-output results.json

# Export to CSV
./nlm --csv-output data.csv

# Export both formats
./nlm --json-output results.json --csv-output data.csv
```

## Extensions and Customization

### Adding New Features

NLM is designed to be extensible. To add new features:

1. **Add new plasticity rules**: Modify `src/brain/Brain.cpp` in the step loop
2. **Add new neuromodulators**: Implement new classes in `src/neuromodulation/`
3. **Add new memory systems**: Implement new classes in `src/memory/`
4. **Add new cognition**: Implement new classes in `src/cognition/`

### Custom Configuration

Create custom configuration files with your specific settings:

```bash
# Create a custom configuration for your research
./nlm --validate-only --config my_research_config.cfg
```

## Getting Help

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

### Local Documentation

After installation, documentation is available locally:

```bash
# Help system (if available)
./nlm --help

# Advanced options
./nlm --advanced-help

# Documentation files
ls docs/*.md
```

### Community Support

- **Slack/Discord**: Join the NLM community
- **Mailing List**: Subscribe for updates
- **Stack Overflow**: Ask questions with `[nlm]` tag

## Future Developments

### Planned Features

1. **Full Agent Framework**: Enhanced agent-brain interfaces
2. **Advanced Visualization**: Interactive brain visualization tools
3. **Distributed Computing**: Scale to massive networks across multiple machines
4. **GPU Acceleration**: Leverage GPU for faster simulations
5. **Real-time Interaction**: Live brain control and monitoring

### Research Directions

- **Biological Accuracy**: Improve neuron and synapse models
- **Cognitive Modeling**: Bridge neuroscience and AI
- **Developmental Psychology**: Study learning and development
- **Neuromodulation**: Understanding brain chemistry

## Conclusion

NLM provides a powerful platform for exploring artificial brain development and learning. Whether you're a researcher, student, or developer, NLM offers tools to:

- **Study neural dynamics** through spiking simulations
- **Develop learning algorithms** with multiple plasticity mechanisms
- **Explore cognitive development** from simple reflexes to complex behavior
- **Conduct scientific research** with reproducible experiments

Get started today with the simple `./nlm` command, and discover the fascinating world of artificial brain development!

## Acknowledgments

This project builds on decades of research in computational neuroscience and artificial intelligence. We acknowledge the contributions of the scientific community and welcome new researchers to extend and improve this work.

---

**Last updated:** November 1, 2026
**Version:** Based on NLM Phase 6
**Contact:** For questions, issues, or feature requests, visit the project repository.
