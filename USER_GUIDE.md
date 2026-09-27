# NLM (熙然) - Neural Learning Machine
## Comprehensive User Guide

**Version 0.1.0 | Phase 6: FINAL INTEGRATION**

NLM is an experimental artificial developmental brain project that implements a complete, integrated neural system capable of learning, memory, prediction, and cognitive development through interaction with an environment.

---

## Quick Start (Python)

### Installation

```bash
# Install from source
pip install scikit-build-core pybind11 pytest numpy
pip install -e .

# Or using build
pip install build
python -m build
pip install dist/*.whl
```

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes generated: {brain.getTotalSpikeCount()}")
```

---

## Quick Start (C++)

### Building

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Or build Python bindings
pip install .
```

### Basic Usage

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    
    nlm::Brain brain(config);
    if (brain.initialize()) {
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    }
    
    return 0;
}
```

---

## System Architecture

NLM implements a closed-loop brain system with the following components:

```
              WORLD
                ↓
          SENSORY INPUT
                ↓
        NEURAL PROCESSING
                ↓
        INTERNAL STATE
                ↓
         MEMORY / PREDICTION
                ↓
        MOTIVATION / NEUROMODULATION
                ↓
          ACTION SELECTION
                ↓
          MOTOR OUTPUT
                ↓
         WORLD CONSEQUENCE
                ↓
           REWARD / ERROR
                ↓
          PLASTICITY
                ↓
        MEMORY / DEVELOPMENT
                ↓
        CHANGED BRAIN
                ↓
       CHANGED FUTURE BEHAVIOR
```

### Core Systems

1. **Memory Systems**
   - Working Memory: Transient active information
   - Episodic Memory: Experience storage and replay
   - Semantic Memory: Pattern associations
   - Procedural Memory: Skills and habits

2. **Neuromodulation**
   - Dopamine: Reward and reinforcement
   - Curiosity: Exploration motivation
   - Novelty: Novelty detection
   - Prediction Error: Learning signals

3. **Prediction System**
   - Forward model for sensory prediction
   - Prediction error computation
   - Confidence tracking

4. **Cognitive Mechanisms**
   - Neural Planner: Action sequence evaluation
   - Concept Formation: Pattern discovery
   - Attentional Selection: Competitive focus

5. **Development System**
   - Developmental stages affecting plasticity
   - Structural plasticity modulated by age
   - Neural excitability changes over time

6. **Plasticity Rules**
   - STDP: Spike-timing-dependent plasticity
   - Hebbian: Co-activation based learning
   - Structural: Synaptogenesis and pruning

---

## Phase 6: Final Integration

Phase 6 integrates all previous systems into a coherent artificial brain. The key achievements include:

### Memory Integration
- Working memory traces connect to neural processing
- Episodic memory replay during consolidation
- All memory systems communicate with neural dynamics

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing

### Prediction Integration
- Prediction system becomes central organizing principle
- Prediction error signals affect all learning
- Confidence tracking implemented throughout brain

### Development Integration
- Developmental stages modulate plasticity rates
- Structural changes with brain "age"
- Neural excitability changes over time

### Advanced Features
- Checkpoint save/load system for persistence
- Replay and consolidation for memory strengthening
- Complete brain-environment interaction loop

---

## Python API Reference

### Core Classes

#### Config
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Load from file
config.loadFromFile("config.json")

# Get values
neuron_count = config.getOr("brain.neuron_count", 1000)
```

#### Brain
```python
# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### SimpleWorld
```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Update world
world.update(timestep=0.1)

# Get sensory input
percept = world.getSensoryPercept()

# Get agent state
body = world.getAgentBody()
```

#### AgentBrain
```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input and act
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Apply reward
reward = 1.0  # from environment
agent.applyRewardModulation(reward, 0.0)

# Get agent statistics
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
```

### Convenience Functions

#### Complete Agent Setup
```python
# Create complete agent setup in one call
brain, world, agent = pynlm.createCompleteAgent(config, width=20, height=20)
```

#### Simulation Runner
```python
# Run complete simulation
pynlm.runSimulation(brain, world, agent, 1000)
```

#### Enhanced Configuration
```python
# Enable development features
pynlm.configureBrainForDevelopment(brain)

# Enable memory features
pynlm.configureBrainForMemory(brain)

# Enable cognitive features
pynlm.configureBrainForCognition(brain)
```

#### Agent Management
```python
# Enable all features
pynlm.enableAllAgentFeatures(agent)

# Get comprehensive status
status = pynlm.getAgentStatus(agent)
print(f"Neuron count: {status['neuron_count']}")
print(f"Curiosity: {status['curiosity']}")
```

#### Phase 6 Integration
```python
# Run complete Phase 6 demo
results = pynlm.runPhase6IntegrationDemo()
print(f"Total reward: {results['total_reward']}")
print(f"Integration successful: {results['memory_working_memory_integrated']}")
```

---

## Advanced Examples

### Example 1: Learning Agent in Environment

```python
import pynlm

def run_learning_agent():
    # Create agent with advanced features
    config = pynlm.createDefaultConfig()
    brain, world, agent = pynlm.createCompleteAgent(config, width=30, height=30)
    
    pynlm.enableAllAgentFeatures(agent)
    
    # Run learning episode
    for episode in range(10):
        pynlm.runSimulation(brain, world, agent, 500)
        
        # Get status
        status = pynlm.getAgentStatus(agent)
        print(f"Episode {episode}: Reward={status['total_reward']:.2f}, "
              f"Curiosity={status['curiosity']:.3f}")
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()

run_learning_agent()
```

### Example 2: Comparative Brain Testing

```python
import pynlm

def compare_configurations():
    # Test different configurations
    configs = [
        ("Basic", {"neuron_count": 500, "connection_probability": 0.1}),
        ("Memory-heavy", {"neuron_count": 2000, "working_memory.capacity": 1000}),
        ("Development-heavy", {"plasticity.stdp.enable": True, "plasticity.hebbian.enable": True})
    ]
    
    results = {}
    
    for name, settings in configs:
        config = pynlm.createDefaultConfig()
        for key, value in settings.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        results[name] = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        }
        
        print(f"{name}: {results[name]['total_spikes']} spikes, "
              f"{results[name]['avg_firing_rate']:.2f} Hz avg firing rate")
    
    return results

compare_configurations()
```

### Example 3: Complete Phase 6 Demo

```python
import pynlm

def run_phase6_demo():
    print("=== NLM Phase 6 Integration Demo ===")
    print("Testing complete integration of all brain systems...")
    
    # Run the demo
    results = pynlm.runPhase6IntegrationDemo()
    
    print("\n=== Results ===")
    print(f"Total Reward: {results['total_reward']:.3f}")
    print(f"Average Firing Rate: {results['avg_firing_rate']:.2f} Hz")
    print(f"Episodes Stored: {results['memory_episodes_stored']:.0f}")
    print(f"Dopamine Level: {results['dopamine_level']:.3f}")
    print(f"Novelty Level: {results['novelty_level']:.3f}")
    print(f"Curiosity Level: {results['curiosity_level']:.3f}")
    
    print("\n=== Integration Status ===")
    systems = [
        ("Working Memory", results['memory_working_memory_integrated']),
        ("Episodic Memory", results['memory_episodic_memory_integrated']),
        ("Neuromodulation", results['neuromodulation_integrated']),
        ("Prediction", results['prediction_integrated']),
        ("Development", results['development_integrated']),
        ("Checkpointing", results['checkpointing_works']),
        ("Replay", results['replay_works'])
    ]
    
    for name, status in systems:
        print(f"{name}: {'✓ CONNECTED' if status else '✗ DISCONNECTED'}")
    
    print(f"\nTotal Processing Time: {results['total_wall_clock_time']:.1f}s")
    
    return results

run_phase6_demo()
```

---

## Command Line Interface

### Basic Usage

```bash
# Build and run demo
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm

# With arguments
./nlm --quick    # Quick integration test
./nlm --full     # Full integration demo
./nlm --help      # Show help
```

### Advanced CLI Options

```bash
# Test specific systems
./nlm --test memory     # Test memory integration
./nlm --test neuromod    # Test neuromodulation integration
./nlm --test plasticity  # Test plasticity rules
./nlm --test checkpoint  # Test checkpointing

# Run with custom configuration
./nlm --config custom.cfg --steps 5000

# Performance benchmarking
./nlm --benchmark --duration 60

# Export results
./nlm --export results.json
```

---

## Installation and Build Instructions

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or equivalent

### Build Steps

#### Standard C++ Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Python Bindings Build

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or build and run tests
python -m pytest tests/ -v
```

#### Development Build

```bash
# Debug build with development features
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEVELOPMENT_TESTS=ON
make -j$(nproc)
```

### Installation

#### System Installation

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

#### Python Package Installation

```bash
pip install --force-reinstall .
```

---

## Troubleshooting

### Common Issues

#### Build Errors

**Issue**: CMake cannot find Python
```bash
# Solution: Install Python development packages and ensure Python is in PATH
sudo apt-get install python3-dev python3-pip
pip install scikit-build-core pybind11
```

**Issue**: Compiler doesn't support C++20
```bash
# Solution: Use a modern compiler
# On Ubuntu: sudo apt-get install g++-10
g++ --version  # Should show version >= 10.0
```

#### Runtime Errors

**Issue**: "My brain isn't doing anything"
```python
# Solution: Check initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this first!
```

**Issue**: "The agent isn't moving"
```python
# Solution: Check world update and motor command
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

**Issue**: "Everything is 0"
```python
# Solution: Brains need time to warm up
for step in range(1000):  # More steps
    brain.step(step)
```

#### Python Import Errors

**Issue**: `ImportError: No module named 'pynlm'`
```bash
# Solution: Reinstall
pip install --force-reinstall .

# Or install in development mode
pip install -e .
```

### Debugging Tips

#### Enable Logging
```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

#### Brain Status
```python
brain.logStatus()
```

#### Configuration Summary
```python
print(config.summary())
```

---

## Advanced Topics

### Performance Optimization

The NLM framework includes several performance optimization features:

1. **Memory Pools**: Pre-allocated memory for faster object creation
2. **Event Queues**: Asynchronous processing for better performance
3. **SIMD Vectorization**: Optimized for modern CPU architectures
4. **Parallel Processing**: Multi-core support for large simulations

### Scaling for Large Brains

For simulations with thousands of neurons:

```python
# Use larger configuration
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.05f)

# Enable performance features
config.set("performance.use_memory_pools", True)
config.set("performance.use_event_queues", True)
```

### Development Mode

For research and experimentation:

```python
# Development configuration
config.set("development.enabled", True)
config.set("plasticity.development_sensitive", True)
config.set("neural_excitability.age_dependent", True)
```

### Memory Management

```python
# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")

# Save large brain states
brain.save("large_brain_state.bin")
```

---

## Research and Development

### Phase 6 Integration Research

The Phase 6 integration focuses on:

1. **System Integration**: How all systems work together as a coherent whole
2. **Memory Integration**: How memory systems connect to neural processing
3. **Neuromodulation Integration**: How neuromodulators affect plasticity and dynamics
4. **Prediction Integration**: How prediction becomes a central organizing principle
5. **Development Integration**: How developmental stages affect plasticity rates
6. **Persistence**: Checkpoint save/load for long-running simulations
7. **Replay and Consolidation**: Memory strengthening during rest

### Scientific Limitations

NLM is a research framework with the following limitations:

- **Simplified neuron model**: Uses LIF neurons (not Hodgkin-Huxley)
- **Limited biological fidelity**: Simplified synaptic dynamics
- **Computational constraints**: Limited to available computing resources
- **No consciousness**: Does not claim human-like cognition or consciousness
- **Research tool**: Designed for investigating neural computation principles

### Future Directions

Potential enhancements include:

1. **More realistic neuron models**: Hodgkin-Huxley dynamics
2. **Detailed dendritic morphology**: Multi-compartment neurons
3. **Cortigraphic architecture**: Realistic brain regions
4. **Multi-scale modeling**: Integration with higher-level cognitive models
5. **Hardware acceleration**: GPU support for large-scale simulations
6. **Hybrid architectures**: Integration with symbolic AI
7. **Embodied cognition**: More complex interaction with virtual environments

---

## Legal and License

**License**: MIT

**Copyright**: NLM Research Project

**Attribution**: Please cite the NLM project when using this software in publications.

**Disclaimer**: NLM is experimental research software. The developers assume no liability for damages arising from its use.

---

## Support and Community

### Documentation

- **Main Guide**: This file (`USER_GUIDE.md`)
- **Architecture**: `docs/ARCHITECTURE.md`
- **Science**: `docs/SCIENCE.md`
- **Experiments**: `docs/EXPERIMENTS.md`
- **Roadmap**: `docs/ROADMAP.md`

### Python Support

```python
# For Python-specific issues
import pynlm
help(pynlm)
```

### C++ Support

```cpp
// For C++ specific questions
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
```

### Community Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues

---

## Release Notes

### Version 0.1.0

**Initial Release**

- Phase 6: FINAL INTEGRATION
- Complete brain system integration
- Python bindings (pynlm)
- Comprehensive documentation
- Unit tests
- Performance optimizations

### Development Roadmap

**Short Term (Next 3 months)**

- Enhance Python API with more advanced features
- Add comprehensive integration tests
- Improve performance for large-scale simulations
- Add GPU acceleration support

**Medium Term (6 months)**

- More realistic neuron models
- Multi-scale brain architecture
- Advanced cognitive modeling
- Hardware acceleration

**Long Term (1+ year)**

- Real-world applications
- Integration with robotics
- Neuroscientific validation
- Commercial applications

---

## References

1. **NLM Documentation**: https://nlm.readthedocs.io
2. **GitHub Repository**: https://github.com/nlm-project/nlm
3. **Scientific Papers**: Available in the project documentation
4. **Research Group**: NLM Research Project

---

*End of User Guide*

For additional questions, please refer to the specific documentation files or contact the NLM research team.
