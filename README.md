# NLM (Neural Learning Machine) - Project Overview

## What is NLM?

NLM (熙然) is an advanced neural simulation framework that implements Phase 6 of an artificial developmental brain project. It provides:

- **Real neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms** (STDP, Hebbian, Homeostatic)
- **Integrated memory systems** (working, episodic, associative)
- **Cognition and planning** capabilities
- **Neuromodulation** (dopamine, curiosity, novelty detection)
- **Development systems** for brain maturation
- **Python bindings** for easy access and scripting

## Key Features

### 🧠 Advanced Neural Models
- Real LIF neuron dynamics with biological plausibility
- Event-driven spike propagation with realistic delays
- Multiple plasticity rules for different learning mechanisms

### 🧩 Integrated Systems
- **Memory**: Working memory, episodic memory, semantic memory
- **Cognition**: Concept formation, neural planning
- **Sensory**: Vision, audio, internal signals processing
- **Motor**: Action selection and execution
- **World**: Environment simulation with objects and resources

### 🚀 Performance Optimizations
- **Memory pools** for 20-100x faster allocation/deallocation
- **O(n log n) STDP algorithm** instead of O(n²)
- **Cache-aligned data structures** for better memory locality
- **SIMD-friendly layouts** for parallel processing

### 🎮 User Interface
- **Python API** for easy scripting and experimentation
- **Command-line interface** for simulation control
- **Real-time monitoring** and statistics
- **Interactive mode** for exploratory simulations

### 🛡️ Robustness
- **Comprehensive error handling** throughout
- **Input validation** for all critical operations
- **Graceful failure** with informative error messages
- **Memory safety** with proper resource management

## Getting Started

### Quick Start (Python)

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Quick Start (Command Line)

```bash
# Run default simulation
./nlm

# Run with custom parameters
./nlm --neuron_count 5000 --connection_probability 0.15 --random_seed 42

# Interactive mode
./nlm
> help
> set neuron_count 2000
> run 1000
```

## Installation

### Build from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build (multiple cores)
make -j$(nproc)

# Install (optional)
sudo make install
```

### Python Package

```bash
# Install Python package
pip install -e .

# Test installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

## Project Structure

```
/src
  /core                    # Core infrastructure
    /Config                # Configuration management
    /Logger                # Logging system
    /Random                # Random number generation
    /SimulationClock       # Time management
  /brain                  # Neural core
    /Neuron               # LIF neurons
    /Synapse              # Synaptic connections
    /NeuralPopulation     # Neuron populations
    /NeuralRegion         # Brain regions
    /Brain                # Central simulation coordinator
  /dynamics               # Neural dynamics
    /NeuralDynamics        # General neural dynamics
    /SpikeSystem          # Spike propagation
  /plasticity            # Learning mechanisms
    /PlasticityRule        # Base plasticity rule
    /STDP                 # Spike-timing dependent plasticity
    /Hebbian              # Hebbian learning
    /HomeostaticPlasticity # Firing rate homeostasis
    /StructuralPlasticity  # Synapse/neuron creation/removal
  /development           # Developmental processes
    /DevelopmentSystem     # Brain development
    /Synaptogenesis        # Synapse creation
    /Pruning              # Synapse elimination
    /Maturation            # Adult properties
  /neuromodulation        # Neuromodulatory systems
    /Neuromodulator        # Generic neuromodulation
    /Reward               # Reward prediction error
    /PredictionError       # Prediction error signals
    /Novelty              # Novelty detection
    /Curiosity            # Curiosity signals
  /memory                # Memory systems
    /WorkingMemory         # Working memory
    /EpisodicMemory        # Episodic memory
    /SemanticMemory        # Semantic memory
    /ProceduralMemory     # Procedural memory
    /AssociativeMemory    # Associative memory
    /NeuralWorkingMemory   # Neural working memory
    /NeuralEpisodicMemory  # Neural episodic memory
  /cognition            # Cognitive functions
    /ConceptFormation     # Pattern recognition
    /NeuralPlanner        # Action planning
  /sensory              # Sensory systems
    /SensoryInput         # Base sensory input
    /Vision              # Visual processing
    /Audio              # Audio processing
    /InternalSignals     # Internal state
  /prediction           # Prediction systems
    /PredictionSystem    # Prediction engines
    /NeuralPrediction    # Neural predictions
  /motor               # Motor systems
    /MotorSystem         # Motor control
  /environment          # Environment simulation
    /Environment         # Base environment
    /Observation         # World observations
    /Action              # Actions and movements
  /experiments          # Experimental systems
    /Experiment          # Base experiment
    /ExperimentRunner    # Experiment management
    /Metrics             # Performance metrics
  /visualization        # Visualization
    /VisualizationInterface # Visualization output
  /agent               # Agent interfaces
    /SensoryPercept      # Agent perception
    /AgentBrain          # Agent-brain interface
    /AgentBody           # Agent body state
  /world               # World simulation
    /SimpleWorld         # Simple 2D world
    /Observation         # World observations
  /performance         # Performance optimization
    /MemoryPool          # Memory management
    /EventQueue          # Event system
    /SIMDVectorization   # SIMD optimizations
    /ParallelProcessing   # Parallel execution
    /SparseConnectivity   # Sparse connection management
    /OptimizedBrain      # Optimized brain structures
    /CheckpointSystem    # State persistence

/python
  /bindings.cpp          # Python bindings
  /CMakeLists.txt         # Python build configuration

/src/main.cpp            # Main executable
/src/benchmark.cpp      # Performance benchmarks
/src/cli/               # Command-line interface
  /CommandLineInterface.hpp  # CLI header
  /CommandLineInterface.cpp  # CLI implementation

/docs/                  # Documentation
  /README.md           # This file
  /UPGRADE_GUIDE.md     # Upgrade guide
  /API_REFERENCE.md     # API reference

/configs/              # Configuration files

*.md, *.txt           # Documentation and text files
```

## Configuration

### Configuration File Format

Configuration is stored in simple key=value format:

```
# Example config file
# Configuration comments
random_seed = 42
neuron_count = 1000
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
target_firing_rate = 5.0
homeostatic_learning_rate = 0.01
```

### Command Line Options

```bash
./nlm [options]

options:
  --neuron_count N      Number of neurons (default: 1000)
  --connection_probability P  Connection probability (default: 0.1)
  --random_seed S       Random seed (default: 42)
  --stdp_ltp_weight W    STDP LTP weight (default: 0.02)
  --stdp_ltd_weight W    STDP LTD weight (default: 0.015)
  --stdp_tau T          STDP time constant (default: 20.0)
  --synaptogenesis_rate R Synaptogenesis rate (default: 0.0001)
  --pruning_rate R      Pruning rate (default: 0.00001)
  --config FILE         Load configuration from file
  --help                Show help message
```

## Configuration Parameters

### Core Parameters
- `random_seed`: Random number generator seed
- `neuron_count`: Total number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections
- `simulation_timestep`: Simulation timestep in seconds

### Plasticity Parameters
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant (ms)
- `hebbian_learning_rate`: Hebbian learning rate
- `synaptogenesis_rate`: Rate of new synapse creation
- `pruning_rate`: Rate of synapse elimination
- `target_firing_rate`: Target firing rate for homeostatic plasticity
- `homeostatic_learning_rate`: Learning rate for homeostatic plasticity

### Development Parameters
- `replay_interval`: Memory replay interval (steps)
- `consolidation_interval`: Memory consolidation interval (steps)
- `development_stage`: Current developmental stage

### Performance Parameters
- `checkpoint_dir`: Directory for checkpoint files
- `checkpoint_frequency`: Checkpoint save frequency
- `memory_pool_size`: Size of memory pools

## Usage Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Brain in a World

```python
import pynlm

# Setup brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Setup agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # Get what agent sees
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 3: Advanced Agent with Learning

```python
import pynlm

# Setup configuration with learning
config = pynlm.createDefaultConfig()
config.set("enable_reward_modulation", True)
config.set("enable_curiosity", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)
config.set("target_firing_rate", 5.0)
config.set("homeostatic_learning_rate", 0.01)

# Create components
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run complex simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation (reward-based learning)
    # This would be based on task-specific rewards
    # reward = calculate_reward(world, agent)
    # agent.applyRewardModulation(reward, 0.0)  # No predicted reward
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")

print("Agent simulation complete!")
```

### Example 4: Command Line Usage

```bash
# Run basic simulation
./nlm

# Run with custom parameters
./nlm --neuron_count 5000 --connection_probability 0.15 --random_seed 42

# Interactive mode
./nlm
> help
> set neuron_count 2000
> set random_seed 12345
> run 1000
> stats

# Save and load brain states
> save my_brain.checkpoint
> load my_brain.checkpoint

# Run performance test
./nlm --neuron_count 10000 --connection_probability 0.05
```

## Command Line Interface

The NLM includes a comprehensive command-line interface for simulation control:

### Available Commands

- **help**: Show available commands
- **set <key> <value>**: Set configuration values
- **get <key>**: Get configuration values
- **run <steps> [timestep]**: Run simulation
- **stats**: Show simulation statistics
- **save <file>**: Save brain state
- **load <file>**: Load brain state
- **reset**: Reset simulation
- **step <steps>**: Run specific steps
- **start/stop**: Control simulation
- **status**: Show current status
- **exit/quit**: Exit CLI

### Interactive Mode

```
NLM > help
Available commands:
  help <command>     Show help for specific command
  set <key> <value>   Set configuration value
  get <key>           Get configuration value
  run <steps>         Run simulation
  stats               Show statistics
  save <file>         Save brain state
  load <file>         Load brain state
  reset               Reset simulation
  step <steps>        Run steps
  start               Start simulation
  stop                Stop simulation
  status              Show status
  exit                Exit

NLM > set neuron_count 2000
NLM > run 1000
Simulation running for 1000 steps...
NLM > stats
=== Simulation Status ===
Brain state: Running
Total spikes: 4523
Total steps: 1000
Active neurons: 156
Firing neurons: 23
Average firing rate: 2.85 Hz

Region Information:
  Region 1: 2000 neurons, 12345 synapses, avg weight: 0.342
NLM > save my_brain.checkpoint
Configuration saved to: my_brain.checkpoint
NLM > exit
Exiting command line interface.
```

## Performance Monitoring

The NLM provides real-time performance monitoring:

```bash
NLM > stats
=== Simulation Status ===
Brain state: Running
Total spikes: 4523
Total steps: 1000
Active neurons: 156
Firing neurons: 23
Average firing rate: 2.85 Hz

=== Performance Statistics ===
Total simulation time: 2.34 seconds
Performance: 427 steps/second
Spike rate: 1931 spikes/second

Region Information:
  Region 1: 2000 neurons, 12345 synapses, avg weight: 0.342
```

## Configuration Examples

### Minimal Configuration

```
# config.minimal.cfg
random_seed = 42
neuron_count = 500
connection_probability = 0.1
```

### Advanced Configuration

```
# config.advanced.cfg
random_seed = 12345
neuron_count = 5000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
enable_structural_plasticity = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Homeostatic plasticity
enable_homeostatic_plasticity = true
target_firing_rate = 5.0
homeostatic_learning_rate = 0.01

# Development
enable_development = true
replay_interval = 100
consolidation_interval = 1000

# Neuromodulation
enable_reward_modulation = true
enable_curiosity = true
enable_novelty = true

# Memory
memory_capacity = 1000
max_episodes = 100
```

## Testing and Validation

### Unit Tests

```bash
# Run unit tests
make test

# Run specific test modules
./tests/test_main
./tests/test_brain
./tests/test_plasticity
```

### Performance Tests

```bash
# Run performance benchmarks
make benchmark

# Profile specific components
./benchmark --neuron_count 1000 --connection_probability 0.1
```

### Integration Tests

```bash
# Run integration tests
make integration

# Test Python bindings
python -m pytest tests/test_python_bindings.py
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
**Solution**:
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is properly connected

#### "The agent isn't moving"
**Solution**:
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the brain has motor neurons

#### "Everything is 0"
**Solution**:
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration values for neuron count and connection probability

#### "Performance issues"
**Solution**:
- Check memory pool configuration
- Optimize STDP parameters for your simulation size
- Use the command-line interface to monitor performance

### Getting Help

#### Command Line Help
```bash
./nlm help
./nlm help set
./nlm help run
```

#### Python Documentation
```python
import pynlm
help(pynlm)
help(pynlm.createBrain)
help(pynlm.Brain)
```

#### Stack Traces
```bash
# Enable stack traces for debugging
export NLM_DEBUG=1

# Run with valgrind (if available)
valgrind --tool=memcheck ./nlm --neuron_count 1000
```

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgements

This project is based on the original NLM Phase 2 research and has been enhanced with:

- **Phase 6 Integration**: All integrated brain systems and advanced features
- **Performance Optimizations**: Memory pools, efficient algorithms
- **User Experience**: Command-line interface, comprehensive documentation
- **Code Quality**: Error handling, parameter validation, testing

## References

1. **NLM Phase 2**: Real neural computation with LIF neurons and STDP
2. **Homeostatic Plasticity**: Firing rate regulation mechanisms
3. **Memory Systems**: Working memory, episodic memory, semantic memory
4. **Neuromodulation**: Reward, curiosity, and novelty detection
5. **Development**: Brain maturation and structural changes

## Contact

For questions, issues, or contributions:
- **Documentation**: See `docs/` directory
- **Bug Reports**: Use issue tracker
- **Discussions**: GitHub discussions
- **Support**: Project maintainers

---

*This documentation is automatically generated. For detailed API reference, see `docs/API_REFERENCE.md`.*
