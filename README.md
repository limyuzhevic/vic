# Quick Start Guide

Welcome to NLM (Neural Learning Machine)! This is a **Phase 6 complete** artificial brain implementation that demonstrates brain-inspired AI capabilities.

## What Makes NLM Special?

- **Real spiking neural computation** - LIF neurons with event-driven dynamics
- **Complete brain integration** - All major cognitive systems working together
- **Developmental learning** - Adapts over time like a biological brain
- **Neuromodulation** - Curiosity, reward, novelty-driven behavior
- **Memory systems** - Working memory, episodic memory, concept formation
- **Planning and prediction** - Forward models and action planning
- **Social learning** - Can learn from observing other agents
- **Text processing** - Text as additional sensory input

## Installation

### Basic Setup

```bash
# Clone the repository
git clone <repo-url> nlm

cd nlm

# Install Python package
pip install .
```

### Development Setup

```bash
# For development, install with editable mode
pip install -e .

# Or install with additional dependencies
pip install scikit-build-core pybind11 pytest numpy
```

## Getting Started Examples

### 1. Basic Brain Simulation

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("✓ Brain simulation complete")
```

### 2. Agent in Environment

```python
import pynlm

# Create brain and environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Configure world (16x16 grid with 16 ray vision)
world.configure(16, 16, 16, 16)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(1000):
    world.update(0.001)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, step * 0.001)
    
    agent.applyRewardModulation(result.reward, 0.0)

print(f"Episode completed with total reward: {sum(r for r, _ in results):.2f}")
```

### 3. Learning Experiment

```python
import pynlm

def learning_experiment():
    """Demonstrates how NLM learns through experience"""
    
    # Configure for learning
    config = pynlm.createDefaultConfig()
    config.set("enable_curiosity", True)      # Enable exploration
    config.set("enable_planning", True)       # Enable planning
    config.set("stdp_ltp_weight", 0.02)       # STDP potentiation
    config.set("stdp_ltd_weight", 0.015)      # STDP depression
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record initial synaptic state
    region = brain.getRegion(pynlm.RegionId(1))
    initial_weights = [syn.getWeight() for syn in region.getSynapses()]
    initial_mean = sum(initial_weights) / len(initial_weights)
    
    # Create environment and agent
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Apply learning experience
    for step in range(1000):
        # Present stimulus pattern
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        # Learn from experience
        agent.applyRewardModulation(result.reward, 0.0)
    
    # Check learning results
    final_weights = [syn.getWeight() for syn in region.getSynapses()]
    final_mean = sum(final_weights) / len(final_weights)
    
    learning_occurred = final_mean - initial_mean > 0.001
    
    print(f"Learning Experiment Results:")
    print(f"  Initial synaptic mean: {initial_mean:.4f}")
    print(f"  Final synaptic mean: {final_mean:.4f}")
    print(f"  Change: {final_mean - initial_mean:.4f}")
    print(f"  Learning occurred: {learning_occurred}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return learning_occurred

# Run learning experiment
learning_experiment()
```

## Advanced Usage

### Using Memory Systems

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Working Memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getActiveTraces()} traces")
    
    # Store some memory
    working_memory.storeToNeuron(pynlm.NeuronId(100), 0.8)
    
    # Retrieve memory
    activation = working_memory.getNeuronActivation(pynlm.NeuronId(100))
    print(f"Neuron activation: {activation:.3f}")

# Episodic Memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    # Get episodes for replay
    episodes = episodic_memory.getEpisodesForReplay(3)
    print(f"Replay available: {len(episodes)} episode sets")

# Concept Formation
concept_formation = brain.getConceptFormation()
if concept_formation:
    print(f"Concept formation concepts: {concept_formation.getConceptCount()}")
```

### Neuromodulation Systems

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Create agent for neuromodulation access
agent = pynlm.createAgentBrain(brain)

# Test neuromodulation
for step in range(100):
    # Simulate some environment interaction
    reward = 10.0 if step % 50 == 0 else -5.0  # Get resource sometimes, hazard other times
    
    # Apply reward modulation
    agent.applyRewardModulation(reward, 0.0)
    
    # Check neuromodulation levels
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Dopamine (reward prediction error): {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
```

### Planning and Action Selection

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Get planning information
planner = brain.getPlanner()
if planner:
    print(f"Planning depth: {planner.getPlanningDepth()}")
    print(f"Planning confidence: {planner.getPlanningConfidence():.3f}")
    print(f"Was recent plan successful: {planner.wasRecentPlanSuccessful()}")

# Test action planning
concept_formation = brain.getConceptFormation()
if concept_formation:
    print(f"Concepts formed: {concept_formation.getConceptCount()}")

# Run with planning
for step in range(500):
    # Get current state (would come from actual environment)
    # For demo, we'll use a simple vision pattern
    vision = [0.1] * 16  # Simple vision pattern
    
    # Agent processes sensory input and plans
    # (In real usage, this would come from actual environment sensing)
    
    action = agent.decodeMotorCommand()
    print(f"Step {step}: Action = {action}")
```

## Configuration Options

### Basic Configuration File

Create `configs/default.cfg`:

```
# Core settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Learning
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.001

# Neuromodulation
dopamine_baseline = 0.1

# Environment
environment_name = GridWorld
log_level = INFO
```

### Advanced Configuration

For advanced features, use `configs/advanced_nlm.cfg` which includes:

- Detailed neuromodulation settings
- Advanced memory system configuration  
- Cognitive system parameters
- Developmental stage settings
- Performance optimization options

### Command Line Configuration

```bash
# Override configuration
./nlm --neuron_count=500 --stdp_ltp_weight=0.02

# Use custom config file
./nlm --config configs/custom.cfg
```

## Running Demos

### Phase 2: Basic Neural Computation

```bash
# Basic spiking neural computation demo
./nlm
```

### Phase 3: Simple Environment

```bash
# Agent-environment interaction
./nlm_phase3_demo
```

### Phase 4: Complete Agent

```bash
# Full agent in world
./nlm_phase4_demo
```

### Phase 6: Integration Test

```bash
# Comprehensive Phase 6 demo
./nlm_phase6_demo
```

### Performance Benchmark

```bash
# Performance measurements
./nlm_benchmark
```

## Building from Source

### Prerequisites

```bash
# Install build dependencies
apt-get update
apt-get install -y build-essential cmake python3-dev

# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy
```

### Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j4
```

### Running Tests

```bash
# Python tests
pytest tests/

# C++ tests (if built with -DNLM_BUILD_TESTS=ON)
./test_suite
```

## Troubleshooting

### Common Issues

#### Import Error
```bash
# Ensure package is installed
pip install -e .

# Check Python path
python -c "import sys; print(sys.path)"
```

#### Brain Initialization Issues
```python
# Check required configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 1)
```

#### Memory Usage
```python
import psutil
process = psutil.Process()
print(f"Memory: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

## Performance Tips

1. **Reduce neuron count** for faster simulation:
   ```python
   config.set("neuron_count", 500)
   ```

2. **Disable expensive features**:
   ```python
   config.set("enable_concept_formation", False)
   config.set("enable_planning", False)
   ```

3. **Increase timestep** for faster simulation:
   ```python
   config.set("simulation_timestep", 0.01)
   ```

4. **Enable profiling** to identify bottlenecks:
   ```python
   config.set("enable_profiling", True)
   ```

## Further Reading

### Documentation

- **Main Documentation**: `docs/NLM_Documentation.md`
- **Architecture Guide**: See `docs/` directory for component details
- **API Reference**: Generated documentation in `docs/`

### Research Papers

This implementation is based on concepts from:
- "Neural Engineering Framework" for spiking neural networks
- "Brain-inspired artificial intelligence" literature
- "Neuromorphic computing" principles
- "Artificial developmental systems" research

### Related Projects

- **SpiNNaker** - Spiking neural network simulation platform
- **Nengo** - Neural engineering framework
- **BindsNET** - Brain-inspired neural network toolkit
- **PyNN** - Spiking neural network simulator

## Contributing

### Code Quality Standards

- Use `//` for single-line comments
- Use `/* ... */` for multi-line comments
- Follow Google-style C++ formatting
- Use smart pointers appropriately
- Write comprehensive unit tests
- Document public APIs

### Testing

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test system interactions
3. **Performance Tests**: Benchmark critical operations
4. **Regression Tests**: Ensure existing functionality

### Documentation

- Update API documentation
- Add usage examples
- Document configuration parameters
- Include troubleshooting guides

## License

NLM is licensed under the MIT License. See `LICENSE` for details.

## Contact

- **GitHub**: <repo-url>
- **Documentation**: <docs-url>
- **Issues**: <issues-url>
- **Email**: <contact-email>

## Quick Commands Summary

```bash
# Install development version
pip install -e .

# Build from source
mkdir build && cd build
cmake .. && make

# Run Python tests
pytest tests/

# Run C++ demos
./nlm ./nlm_phase6_demo

# Create custom configuration
cp configs/default.cfg configs/my_config.cfg
```

## Welcome to NLM!

NLM represents a significant step forward in brain-inspired artificial intelligence. By implementing real spiking neural computation with integrated cognitive systems, NLM demonstrates that complex intelligent behavior can emerge from biological principles rather than traditional deep learning approaches.

Whether you're a researcher, developer, or curious explorer, NLM provides a powerful framework for exploring the intersection of neuroscience and artificial intelligence. Enjoy the journey into brain-inspired AI!
