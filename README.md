# NLM (Neural Learning Machine) - Experimental Artificial Developmental Brain

[![Documentation](https://img.shields.io/badge/docs-available-brightgreen)](./docs/README.md)
[![Build Status](https://img.shields.io/badge/build-tested-green)](https://github.com/nlm-project/nlm/actions/workflows/build.yml)
[![License](https://img.shields.io/badge/license-MIT-blue)](./LICENSE)

**NLM** (熙然, meaning "serene flow") is an experimental computational brain project that implements a multi-phase developmental system. The goal is to create a neural system that begins in a primitive state and acquires increasingly complex abilities through interaction with an environment.

> **Note**: This is a research project investigating computational brain-like systems. We make **NO claims** that NLM accurately reproduces biological brains. Current limitations include simplified neuron models and no claim of consciousness or human-like cognition.

## What NLM Is & Isn't

### NLM is NOT:
- A transformer or large language model (LLM)
- A chatbot or conversational AI
- A traditional deep learning model
- A pretrained AI system

### NLM IS intended to become:
- A neural system that learns from experience through plasticity
- A brain-inspired architecture with spiking neurons and synapses
- A system that develops and adapts over time through developmental stages
- A system where cognition emerges from neural dynamics and interactions

## Project Status: Phase 6 Complete - Final Integration

**Phase 6 (FINAL INTEGRATION)** has been completed successfully! All previous phase achievements are integrated into a coherent artificial brain system.

### Phase 6 Integration Achievements

#### 🧠 Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### 🧠 Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### 🧠 Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### 🧠 Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### 🧠 Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### 🧠 Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### 🧠 Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

## Quick Start

### Installation

#### Build from Source (C++)

```bash
# Create build directory and compile
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Install Python Bindings

```bash
# Install required dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python package
pip install .

# Or install in development mode for editing
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"✅ Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Setup brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("🎉 Complete agent simulation finished!")
```

## Building

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Commands

```bash
# Standard release build
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

## Running

### Phase 6 Integration Demo

Run the comprehensive integration test that verifies all brain systems are connected:

```bash
./nlm_phase6_demo
```

This demo simulates a complete agent interacting with an environment, demonstrating all integrated systems working together.

### Python Examples

Run the example scripts from the documentation:

```python
import pynlm

# Example 1: Silent brain test
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print("✅ Silent brain test passed!")

# Example 2: Brain in a world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("✅ World interaction test passed!")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Main CMake configuration
├── README.md                         # This file - Project overview and usage
├── docs/                             # Comprehensive documentation
│   ├── README.md                     # Complete API documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   ├── LIMITATIONS.md                 # Current limitations
│   ├── FINAL_LIMITATIONS.md           # Final Phase 6 limitations
│   ├── PHASE6_FINAL_AUDIT.md          # Phase 6 integration audit
│   ├── PHASE6_FINAL_RESULTS.md        # Phase 6 results summary
│   ├── PERFORMANCE.md                 # Performance benchmarks
│   └── RESEARCH_METHODS.md            # Research methodology
├── easy_usage.md                     # Beginner-friendly usage guide
├── HOW_TO_USE.md                      # Comprehensive usage guide
├── python/                           # Python bindings source
│   ├── bindings.cpp                   # Pybind11 bindings implementation
│   └── CMakeLists.txt                # Python module build configuration
├── src/                              # C++ source code
│   ├── core/                         # Core utilities
│   │   ├── Config/                    # Configuration management
│   │   ├── Random/                    # Random number generation
│   │   ├── Logger/                    # Logging system
│   │   └── SimulationClock/           # Time management
│   ├── brain/                        # Neural components
│   │   ├── Brain.hpp                  # Main brain class
│   │   ├── Neuron.cpp                 # Neuron implementation
│   │   ├── Synapse.cpp                # Synapse implementation
│   │   └── ...                        # Additional brain components
│   ├── ...                           # Other directories (dynamics, plasticity, etc.)
│   └── main.cpp                      # Main entry point
├── tests/                            # Unit tests
│   ├── test_main.cpp                  # Test runner
│   ├── test_brain.cpp                 # Brain unit tests
│   ├── test_neuron.cpp                 # Neuron unit tests
│   └── ...                            # Additional tests
└── configs/                          # Configuration examples
    └── default.cfg                   # Default configuration file
```

## Phase Summary

### Phase 1 (✅ Complete) - Project Skeleton
- Project skeleton setup
- Core types and configuration system
- Basic neural interfaces

### Phase 2 (✅ Complete) - Real Neural Computation
- Real Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3 (✅ Complete) - Basic Functionality
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity integration

### Phase 4 (✅ Complete) - Emerging Cognition
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (✅ Complete) - Performance Optimizations
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (✅ Complete) - Final Integration
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Key Features

### 🧠 Neural Architecture
- **Spiking neurons** with Leaky Integrate-and-Fire dynamics
- **Event-driven computation** with precise spike timing
- **Multiple neuron types** (excitatory, inhibitory, sensory, motor, etc.)
- **Realistic synaptic transmission** with delays and plasticity

### 🧠 Learning Systems
- **STDP (Spike-Timing-Dependent Plasticity)** for temporal learning
- **Hebbian learning** for association strengthening
- **Structural plasticity** for synapse formation/removal
- **Reward modulation** for reinforcement learning
- **Curiosity-driven exploration** for intrinsic motivation

### 🧠 Developmental System
- **Multiple developmental stages** (Initial, CriticalPeriod, Maturation, Adult, Aging)
- **Plasticity rate modulation** based on developmental stage
- **Structural maturation** over time
- **Experience-dependent development**

### 🧠 Memory Systems
- **Working memory** for transient information storage
- **Episodic memory** for experience encoding and replay
- **Semantic memory** for knowledge representation
- **Procedural memory** for skill learning
- **Associative memory** for pattern linking

### 🧠 Cognition Systems
- **Neural prediction** for anticipating future states
- **Concept formation** for pattern discovery
- **Attention mechanisms** for selective focus
- **Planning systems** for action sequencing
- **Self-model** for internal representation

### 🧠 Environment Integration
- **Simple 2D world** with resources, hazards, and objects
- **Sensory processing** (vision, audio, internal signals)
- **Motor output** for action execution
- **Reward system** for feedback-based learning

### 🧠 Advanced Features
- **Checkpoint system** for simulation persistence
- **Memory-efficient data structures** for large networks
- **Performance-optimized algorithms** with SIMD and parallel processing
- **Comprehensive logging and monitoring**
- **Python bindings** for easy integration and scripting

## Scientific Limitations

### What NLM Is NOT
- A biologically accurate model of human brains
- A consciousness simulation
- A general artificial intelligence system
- A replacement for traditional machine learning

### Current Limitations
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- Limited to what can be simulated with available computing resources

### What Phase 6 Investigates
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Python API Highlights

### Core Classes

| Class | Description |
|-------|-------------|
| `Brain` | Central neural simulation brain |
| `Config` | Configuration management |
| `SimpleWorld` | 2D simulation environment |
| `AgentBrain` | Bridge between brain and world |
| `Action` | Motor action representation |
| `SensoryInput` | Sensory data structure |

### Key Methods

#### Brain Operations
- `initialize()` - Setup brain with configuration
- `step(step, time=None)` - Run simulation step
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Get motor action
- `save(filepath)` / `load(filepath)` - Checkpoint management

#### Agent Operations
- `processSensoryInput(percept)` - Process environmental input
- `decodeMotorCommand()` - Convert neural activity to action
- `applyRewardModulation(reward, predicted)` - Apply reward signals
- `enable*X(True/False)` - Enable/disable subsystems

#### World Operations
- `configure(width, height, visionWidth, visionHeight)` - Setup environment
- `update(timestep)` - Advance simulation
- `getSensoryPercept()` - Get agent perception
- `applyMotorCommand(action, time)` - Execute action

## Usage Examples

### 1. Simple Neural Network

```python
import pynlm

# Create and run a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### 2. Complete Agent with Learning

```python
import pynlm

# Setup complete agent simulation
def run_learning_agent():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run learning episode
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        reward = extract_reward(world)
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Progress: Step {step}, Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

brain, agent, world = run_learning_agent()
```

### 3. Brain State Persistence

```python
import pynlm

# Save and load brain state
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation
for i in range(1000):
    brain.step(i)

# Save brain state (checkpoint)
brain.save("my_brain_checkpoint.bin")
print("Brain state saved!")

# Later, load brain state
brain2 = pynlm.createBrain(config)
brain2.load("my_brain_checkpoint.bin")
print("Brain state loaded successfully!")
```

## Performance Tips

### For Large Simulations

```python
# Optimize for memory usage
config.set("brain.neuron_count", 1000)  # Reduce neuron count
config.set("brain.synapse_density", 0.05)  # Reduce connections

# Optimize for speed
config.set("plasticity.stdp.learning_rate", 0.001)  # Adjust STDP rate
```

### For Development

```python
# Use smaller timesteps for critical learning phases
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    timestep = 0.01  # Faster learning
else:
    timestep = 0.1   # Normal operation

for step in range(total_steps):
    world.update(timestep)
    # ... rest of simulation loop
```

## Troubleshooting

### Common Issues

#### "Brain not responding"

**Cause**: Brain not initialized

**Solution**:
```python
# Always initialize before using
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL!
```

#### "Agent not moving"

**Cause**: Motor command not applied

**Solution**:
```python
# Ensure motor command application
world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

#### "High memory usage"

**Cause**: Large network configuration

**Solution**:
```python
# Use smaller configuration for memory constraints
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
```

### Build Issues

#### "CMake cannot find Python"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

#### "Segmentation faults"

```bash
# Check initialization
- Ensure initialize() is called before step()
- Check configuration parameters
- Verify all required components are present
```

## Getting Help

### Documentation

1. **Complete API Reference**: `./docs/README.md`
2. **Beginner Guide**: `./easy_usage.md`
3. **Architecture**: `./docs/ARCHITECTURE.md`
4. **Usage Instructions**: `./HOW_TO_USE.md`

### Community

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share experiences
- **Documentation**: Contribute examples and improvements

### Testing

Run the unit test suite:

```bash
cd build
ctest --output-on-failure
```

## Contributing

### Code Standards

- **C++**: Use C++20, Google style guide
- **Python**: Use PEP 8, numpy-style docstrings
- **Documentation**: Comprehensive docstrings for all public APIs

### Development Workflow

1. Create feature branch from `main`
2. Write tests for new functionality
3. Implement changes with clear commit messages
4. Run full test suite
5. Update documentation
6. Create pull request

### Testing Standards

```python
# Unit test example
def test_brain_basic_functionality():
    """Test basic brain initialization and stepping."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test initialization
    assert brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    # Test simulation
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    assert brain.getTotalSpikeCount() >= initial_spikes
```

## License

MIT License

```
Copyright (c) 2024-2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## Authors

NLM Project Contributors

This is a research project investigating computational brain-like systems. The NLM framework is developed as an experimental platform for understanding how neural systems can emerge from simple components through development and learning.

## Acknowledgements

- Thanks to all contributors and testers
- Thanks to the open-source community for inspiration and shared code
- Thanks to the research community for foundational work in neural computation

## Future Directions

### Planned Improvements

1. **Enhanced Visualization**: Real-time brain activity monitoring
2. **Distributed Computing**: Parallel processing across multiple machines
3. **Multi-Agent Systems**: Multiple interacting agents
4. **Complex Environments**: 3D worlds with physics simulation
5. **Advanced Learning**: Transfer learning and meta-learning

### Research Goals

1. Understand how complex cognition can emerge from simple neural components
2. Develop bio-inspired learning algorithms for artificial systems
3. Create frameworks for autonomous development and adaptation
4. Bridge the gap between neural simulation and practical AI applications

## Contact

For questions, issues, or contributions:
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: https://github.com/nlm-project/nlm/issues

---

**Thank you for exploring the NLM neural simulation framework!** 🚀

*This documentation is continuously updated. For the latest version and additional examples, visit the project repository.*
