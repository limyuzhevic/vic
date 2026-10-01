# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

NLM (熙然, meaning "serene flow") is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### Prerequisites

- C++17 compatible compiler
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Building

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j$(nproc)
```

### Quick Run

After building, run the Phase 6 integration test:

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Core Components

NLM consists of several interconnected systems that work together:

### 🧠 Brain System
- **Real LIF neurons** with spike dynamics
- **Synaptic connections** with Hebbian and STDP learning
- **Structural plasticity** for new synapses and pruning
- **Memory systems** (working, episodic, associative)

### 🌍 Environment System
- **Simple 2D world** with physics simulation
- **Sensory input** (vision, touch, proprioception)
- **Motor output** actions and interactions

### 🎯 Agent System
- **Sensorimotor loop** connecting brain to world
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Developmental stages** affecting learning

### 📊 Integration Features
- **Phase-based development** from primitive to complex
- **Checkpoint system** for saving/loading brains
- **Replay and consolidation** for memory strengthening
- **Real-time monitoring** of brain activity

## Python API

NLM provides comprehensive Python bindings via `pynlm`:

### Basic Usage

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run for 100 steps of thinking
for i in range(100):
    brain.step(i)

print(f"Your brain thought {brain.getTotalSpikeCount()} times!")
```

### Complete Agent Example

```python
import pynlm

# Setup brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run the agent for 1000 steps
for step in range(1000):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What the agent sees
    agent.processSensoryInput(percept)  # Brain processes it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Optional: monitor progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Key Features

### 🎯 Realistic Neural Dynamics
- **Leaky Integrate-and-Fire (LIF) neurons**
- **Event-driven spike propagation** with delays
- **Synaptic plasticity rules** (STDP, Hebbian)
- **Structural plasticity** (synaptogenesis, pruning)

### 🧠 Learning and Development
- **Experience-dependent synaptic changes**
- **Developmental stages** affecting plasticity rates
- **Neuromodulation** guiding learning
- **Memory consolidation** through replay

### 🔄 Closed-Loop Interaction
- **Real-time sensorimotor control**
- **Reward prediction error** learning
- **Curiosity-driven exploration**
- **Adaptive behavior** based on experience

### 📈 Performance Optimizations
- **Memory pools** and event queues
- **SIMD vectorization** for numerical operations
- **Parallel processing** for large networks
- **Efficient data structures** for sparse connectivity

## Configuration Options

NLM supports extensive configuration through JSON files or command-line arguments:

### Key Configuration Parameters

- `random_seed`: Random seed for reproducible experiments
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Random connectivity density
- `simulation_timestep`: Time step for simulation
- `plasticity_learning_rate`: Learning rate for plasticity
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `dopamine_baseline`: Baseline dopamine level
- `environment_width`, `environment_height`: World dimensions
- `log_level`: Logging verbosity (DEBUG, INFO, WARNING, ERROR)

### Custom Configuration

Create your own configuration file:

```bash
./nlm --config my_config.cfg
```

Or save and load brain states:

```python
brain.save("my_brain_state.nlm")
# ... later ...
brain.load("my_brain_state.nlm")
```

## Performance Tips

### For Faster Development
- Use smaller network sizes initially (`neuron_count = 100`)
- Increase `simulation_timestep` (e.g., `0.01` instead of `0.001`)
- Reduce `max_simulation_steps` for prototyping

### For Production Use
- Use larger networks (`neuron_count = 10000` or more)
- Enable parallel processing (`-j` flag in make)
- Use SSD storage for checkpoint I/O
- Monitor memory usage with `ulimit -v`

## Monitoring and Debugging

### Runtime Monitoring
```python
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
```

### Error Handling
NLM provides extensive error handling with detailed logging:

- **Null pointer checks** in all core operations
- **Bounds checking** for array accesses
- **Graceful degradation** when components fail
- **Detailed error messages** for debugging

## Extensibility

### Adding New Neuron Types
Create new neuron classes inheriting from base neuron types and register them with the brain system.

### Custom Plasticity Rules
Implement new plasticity rules by extending the base plasticity classes and registering them.

### Environment Extensions
Create custom environment classes to simulate different physical constraints or sensory modalities.

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. **We make NO claims** that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Getting Help

For questions and support:

1. **Check the documentation** in the `docs/` directory
2. **Review examples** in the `examples/` directory
3. **Examine tests** in the `tests/` directory for usage patterns
4. **Check the issue tracker** for known issues and discussions

NLM is an **experimental research project** intended for scientific investigation and educational purposes. Results should be interpreted as computational models rather than validated scientific findings.
