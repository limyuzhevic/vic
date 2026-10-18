# NLM Neural Learning Machine - Project Structure

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing a neural system that develops and adapts through interaction with an environment. Currently in Phase 6 (Final Integration), NLM provides a comprehensive framework for simulating brain-like intelligence using spiking neural networks.

## System Architecture

```
NLM/
├── CMakeLists.txt              # C++ build system
├── pyproject.toml              # Python package configuration
├── python/                     # Python bindings
│   ├── bindings.cpp            # Main pybind11 module
│   └── CMakeLists.txt          # Python bindings build
├── src/                        # C++ source code
│   ├── brain/                  # Neural components
│   ├── agent/                  # Agent systems
│   ├── world/                  # Environment simulation
│   ├── memory/                 # Memory systems
│   ├── neuromodulation/         # Neuromodulators
│   ├── ...                     # Other subsystems
│   └── main.cpp                # Main executable
├── docs/                       # Documentation
│   ├── STRUCTURE.md            # This file
│   ├── SCIENCE.md              # Scientific background
│   └── ...                     # Other documentation
├── examples/                   # Usage examples
├── tests/                      # Unit tests
└── configs/                    # Default configuration
```

## Key Components

### 1. Core Brain System
- **Neurons**: Leaky Integrate-and-Fire (LIF) model with event-driven spikes
- **Synapses**: Excitatory/inhibitory connections with delays and plasticity
- **Brain Class**: Central neural simulation controller
- **Regions**: Organized neural populations (cortex, hippocampus, etc.)

### 2. Agent System
- **AgentBrain**: Interface between brain and environment
- **AgentBody**: Physical agent state (position, velocity, etc.)
- **SensoryPercept**: Processed sensory input
- **Action**: Motor output commands

### 3. Environment System
- **SimpleWorld**: 2D simulation environment
- **WorldObjects**: Resources, hazards, walls, markers
- **SensoryInput**: Vision, audio, internal signals
- **MotorCommands**: Actions that affect the world

### 4. Learning and Development
- **Plasticity**: STDP, Hebbian learning, structural changes
- **Neuromodulation**: Dopamine, curiosity, reward prediction error
- **Development**: Age-based plasticity changes
- **Memory**: Working, episodic, semantic systems

## Current Status

### Phase 6: Final Integration

**Achieved**:
- ✅ Core LIF neuron dynamics with event-driven spike propagation
- ✅ Synaptic transmission with delays and plasticity rules
- ✅ Spike propagation with precise timing
- ✅ STDP and Hebbian plasticity mechanisms
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Neuromodulation (dopamine, curiosity, novelty)
- ✅ Development stages with age-dependent effects
- ✅ Working memory with persistent activity
- ✅ Episodic memory framework
- ✅ Prediction system integration
- ✅ Checkpoint save/load (stubs)

**Remaining Challenges**:
- ❌ AgentBrain integration with higher cognition
- ❌ Neural planner and concept formation usage
- ❌ Semantic and procedural memory implementation
- ❌ Complete save/load functionality
- ❌ Full cognitive system integration

## Technical Specifications

### Neural Model
- **Neuron Type**: Leaky Integrate-and-Fire (LIF)
- **Dynamics**: Event-driven spike processing
- **Time Resolution**: Millisecond precision
- **Scalability**: Memory pools and event queues

### Simulation Parameters
- **Time Step**: Configurable (default: 0.1ms)
- **Brain Size**: Configurable neuron/synapse counts
- **World Size**: Configurable 2D environment
- **Plasticity Rates**: Adjustable learning parameters

### Performance Features
- **Parallel Processing**: SIMD vectorization
- **Memory Efficiency**: Memory pools and event queues
- **Optimization**: Release build with compiler optimizations

## Usage Overview

### Basic Usage Pattern

```python
import pynlm

# 1. Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# 2. Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 3. Enable learning systems (optional)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 4. Run simulation loop
for step in range(1000):
    # Environment update
    world.update(0.1)
    
    # Sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Motor output
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Example Scripts

1. **basic_brain.py**: Silent brain simulation (neurons only)
2. **agent_world.py**: Complete agent-environment interaction
3. **learning_loop.py**: Full learning scenario with development
4. **debug_analysis.py**: Analysis and debugging utilities

## Configuration Options

### Brain Configuration
```
brain.neuron_count=1000
brain.synapse_density=0.1
brain.connection_probability=0.05
brain.v_thresh=-50.0
brain.v_rest=-70.0
brain.v_reset=-75.0
brain.tau_mem=20.0
brain.tau_ref=2.0
```

### Plasticity Configuration
```
plasticity.stdp.enable=true
plasticity.stdp.learning_rate=0.001
plasticity.hebbian.enable=true
plasticity.structural.enable=true
```

### Neuromodulation Configuration
```
neuromod.dopamine.scale=1.0
neuromod.curiosity.enable=true
neuromod.novelty.enable=true
```

## Development Guidelines

### Coding Standards
- **Language**: C++20 with pybind11 for Python bindings
- **Build System**: CMake with scikit-build-core
- **Testing**: Unit tests with pytest
- **Documentation**: Markdown with consistent formatting

### Development Process
1. **Phase 6 Focus**: Integrate all existing systems
2. **Memory Integration**: Connect working memory to brain loop
3. **Prediction Integration**: Make prediction system operational
4. **Save/Load**: Implement real serialization
5. **Optimization**: Performance improvements

### Learning Pathway
1. **Beginner**: Start with `basic_brain.py` example
2. **Intermediate**: Use `agent_world.py` with full agent simulation
3. **Advanced**: Implement custom learning scenarios with `learning_loop.py`
4. **Expert**: Modify source code and contribute to the project

## Next Steps

### For Users
1. **Installation**: Follow `docs/INSTALLATION.md`
2. **Getting Started**: Read `docs/USAGE/BASIC.md`
3. **Learning**: Work through `docs/TUTORIALS/` sections
4. **Advanced Usage**: Consult `docs/USAGE/ADVANCED.md`

### For Developers
1. **Code Review**: Check `docs/PHASE6_FINAL_AUDIT.md` for integration status
2. **Testing**: Run unit tests in `tests/` directory
3. **Building**: Use `scripts/build.py` for automated builds
4. **Contributing**: Follow project coding standards and guidelines

## References

### Scientific Background
- ** docs/SCIENCE.md**: Detailed scientific foundations
- ** docs/ARCHITECTURE.md**: System architecture overview
- ** docs/EXPERIMENTS.md**: Experiment descriptions

### Documentation
- ** docs/DOCUMENTATION_STRUCTURE.md**: This documentation structure
- ** examples/**: Working examples for learning
- ** scripts/**: Development tools and utilities

## Project Goals

1. **Create a viable artificial brain** that develops and learns
2. **Demonstrate cognitive integration** of multiple systems
3. **Provide educational tools** for understanding neural computation
4. **Enable research** in brain-inspired artificial intelligence
5. **Maintain high code quality** with comprehensive documentation

## Contact

For questions, issues, or contributions:
- **Documentation**: Consult this documentation structure
- **Code Issues**: Check existing test suite and examples
- **Development**: Review Phase 6 audit report for integration status

This document serves as the foundation for understanding, using, and developing the NLM neural learning machine framework.
