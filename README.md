# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Phase Summary

### Phase 1 (Complete) - Project Skeleton
- Project skeleton and core infrastructure
- Basic configuration and logging systems
- Neural interfaces and communication protocols

### Phase 2 (Complete) - Real Neural Computation
- Real spiking neural computation with LIF neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3 (Complete) - World Interaction
- Neural control of behavior in simulated environments
- Reward prediction error and action selection
- Developmental stages and structural plasticity
- Novelty and curiosity drives

### Phase 4 (Complete) - Emerging Cognition
- Neural prediction systems and temporal sequence learning
- Working memory and episodic-like memory
- Concept formation from experience patterns
- Neural attention (NOT Transformer) with competitive dynamics
- Predictive planning and action evaluation
- Self-model formation and social learning
- Continual learning mechanisms

### Phase 5 (Complete) - Performance and Scaling
- Performance optimizations for large-scale simulation
- Memory pools and event queues
- SIMD vectorization and parallel processing
- Comprehensive experiment framework
- Multi-seed statistical analysis
- Damage and recovery experiments

### Phase 6 (Complete) - Final Integration
- All brain systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Phase 6 Integration Achievements

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 5 Performance Demo
```bash
./nlm_phase5_demo
```

This demonstrates Phase 5 performance optimizations and scaling capabilities.

### Phase 2 Real Neural Computation
```bash
./nlm
```

This demonstrates Phase 2 real neural computation with LIF neurons and plasticity.

## Python Interface

NLM includes Python bindings via pybind11:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example
```python
import pynlm

# Setup a complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Advanced Configuration
```python
import pynlm

# Create optimized configuration for large-scale simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100000)
config.set("performance.enable_multithreading", True)
config.set("performance.enable_simd", True)
config.set("memory.working_memory_capacity", 10000)

# Create experiment configuration
experiment_config = pynlm.createExperimentConfig("phase5_lifelong")

# Create benchmark suite
import json
config.saveToFile("my_config.json")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   ├── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
│   └── ...                    # Other documentation
├── src/
│   ├── core/                  # Core utilities
│   │   ├── Config/            # Configuration system
│   │   ├── Random/            # Random number generation
│   │   ├── Logger/            # Logging system
│   │   └── SimulationClock/   # Time management
│   ├── brain/                # Neural components
│   ├── dynamics/             # Neural dynamics
│   ├── plasticity/           # Plasticity rules
│   ├── development/          # Developmental system
│   ├── neuromodulation/       # Neuromodulators
│   ├── memory/               # Memory systems
│   ├── prediction/           # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization
│   ├── Phase5Demo.cpp       # Phase 5 performance demo
│   └── Phase6Demo.cpp       # Phase 6 integration demo
├── python/                   # Python bindings
│   └── bindings.cpp         # pybind11 bindings
├── tests/                   # Test suite
├── configs/                 # Configuration files
│   └── default.cfg
├── pyproject.toml          # Python package configuration
├── easy_usage.md            # Quick usage guide for beginners
└── HOW_TO_USE.md            # Comprehensive usage guide
```

## Scientific Background

NLM is grounded in neuroscience research investigating computational brain-like systems. Key scientific hypotheses being tested:

- Can memory systems integrate with neural dynamics?
- Can neuromodulation affect plasticity in a coordinated way?
- Can prediction become a central organizing principle?
- Can developmental stages modulate learning?
- Can replay reinforce memory consolidation?
- Can the complete brain loop function coherently?

Current limitations include:
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT

## Authors

Research project — See docs for scientific background.

## Key Features

### Neural Computation
- **LIF Neurons**: Leaky Integrate-and-Fire neurons with realistic dynamics
- **Event-Driven Processing**: Efficient spike propagation with delays
- **Multiple Plasticity Rules**: STDP, Hebbian, reward-modulated
- **Structural Plasticity**: Synapse formation and pruning

### Cognitive Architecture
- **Working Memory**: Persistent activity for temporary storage
- **Episodic Memory**: Experience encoding and replay
- **Concept Formation**: Pattern discovery without labels
- **Predictive Planning**: Multi-step action evaluation
- **Attention**: Competitive selection without transformers

### Neuromodulation
- **Dopamine**: Reward prediction error signaling
- **Curiosity**: Exploration motivation
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signals

### Developmental Learning
- **Critical Periods**: Age-dependent plasticity
- **Structural Development**: Growing and pruning connections
- **Maturation**: Developmental stage progression

### Performance Optimizations
- **Large Scale**: Capable of 100k+ neurons
- **Multithreading**: Parallel processing support
- **SIMD Vectorization**: Optimized for modern CPUs
- **Event-Driven**: Efficient for sparse activity

### Integration and Testing
- **Phase 6 Integration**: All systems working together
- **Performance Demonstrations**: Real-world benchmarks
- **Error Handling**: Comprehensive error reporting
- **Debugging Tools**: Network inspection and visualization

## Getting Started

### 1. Install Dependencies

```bash
# Install system dependencies
 sudo apt-get install cmake build-essential python3-dev python3-pip

# Install Python dependencies
 pip install pybind11 scikit-build-core pytest numpy
```

### 2. Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)
```

### 3. Install Python Package

```bash
# Install Python bindings
cd ../..
pip install .
```

### 4. Run Examples

```bash
# Phase 6 integration demo
./nlm_phase6_demo

# Phase 5 performance demo
./nlm_phase5_demo

# Phase 2 real neural computation
./nlm
```

Python examples:
```python
import pynlm

# Simple brain creation and simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 5. Run Tests

```bash
# Run unit tests
./nlm_test

# Or use Python tests
python -m pytest tests/ -v
```

## Advanced Usage

### Custom Configuration

Create a custom configuration file:

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "brain": {
    "neuron_count": 50000,
    "region_count": 2,
    "connection_probability": 0.05,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  "performance": {
    "enable_multithreading": true,
    "enable_simd": true,
    "thread_count": 4
  },
  "memory": {
    "working_memory_capacity": 5000,
    "episodic_memory_max_episodes": 10000
  }
}
```

### Performance Benchmarking

```python
import pynlm
from time import time

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)

brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark performance
start_time = time()
for i in range(1000):
    brain.step(i)
end_time = time()

print(f"Performance: {(1000.0 / (end_time - start_time)):.2f} steps/s")
print(f"Memory efficiency: {brain.getTotalSynapseCount() / brain.getTotalNeuronCount():.4f} synapses/neuron")
```

### Network Analysis

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect firing rate data
firing_rates = []
for i in range(1000):
    brain.step(i)
    firing_rates.append(brain.getAverageFiringRate())

# Plot firing rate over time
plt.figure(figsize=(10, 6))
plt.plot(firing_rates)
plt.title('Firing Rate Over Time')
plt.xlabel('Simulation Step')
plt.ylabel('Average Firing Rate (Hz)')
plt.grid(True)
plt.show()
```

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
- Validate configuration parameters are within valid ranges

### Performance Issues

**Simulation runs slowly:**
- Reduce neuron count for testing
- Disable multithreading if CPU cores are limited
- Reduce simulation steps for quick testing

**Memory usage high:**
- Monitor memory usage with performance tools
- Consider reducing neuron or synapse counts
- Enable memory pools and sparse connectivity

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and limitations
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization details

## Community and Support

### Getting Help

For questions about using NLM:
1. Check the documentation in this repository
2. Look at example scripts in the source code
3. Search for known issues and solutions

### Contributing

NLM is an open research project. Contributions are welcome for:
- Bug fixes and improvements
- New feature implementations
- Performance optimizations
- Documentation updates
- Example demonstrations

Please follow the project's coding standards and contribute through appropriate channels.

## Future Directions

### Research Goals

1. **Full Integration**: Complete integration of all cognitive systems
2. **Scalability**: Demonstrate capabilities on million-neuron scale
3. **Biological Plausibility**: Improve biological fidelity while maintaining performance
4. **Developmental Learning**: Study developmental trajectories
5. **Social Cognition**: Extend to multi-agent learning and social interaction

### Technical Goals

1. **Performance**: Optimize for real-time simulation
2. **Memory Efficiency**: Scale to larger networks
3. **Error Handling**: Comprehensive error reporting and recovery
4. **Testing**: Comprehensive test suite and validation
5. **Documentation**: Complete API and usage documentation

### Applications

1. **Education**: Teaching neural computation principles
2. **Research**: Testing brain-inspired algorithms
3. **Simulation**: Modeling neural systems
4. **Benchmarking**: Comparing neural architectures
5. **Development**: Studying developmental processes

## Conclusion

NLM represents a comprehensive attempt to build a brain-inspired artificial system that can learn, remember, and adapt through interaction with an environment. While no single project can fully capture the complexity of biological brains, NLM provides a testbed for exploring fundamental principles of neural computation and cognitive architecture.

The project has successfully integrated multiple brain systems into a coherent whole, demonstrating that complex cognition can emerge from simple spiking neural dynamics with appropriate plasticity rules, memory systems, and neuromodulation.

NLM is not a finished product but a research platform for investigating the principles of artificial brain design. Future work will continue to explore new architectures, learning mechanisms, and applications.

---

**Phase 6 Complete - All Systems Integrated**

The NLM brain is now a functioning artificial neural substrate capable of:
- Learning from experience through multiple plasticity mechanisms
- Remembering and recalling experiences through memory systems
- Selecting actions based on predictions and values
- Developing and maturing over time
- Adapting to new environments and challenges
- Integrating sensory input with motor output
- Maintaining persistent activity for working memory
- Forming concepts from experience patterns
- Planning multi-step actions
- Distinguishing self from environment
- Learning from observation of others

The journey continues as we explore what truly complex cognition can emerge from these biological-inspired foundations.
