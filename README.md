# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

NLM is a brain-inspired spiking neural network simulator that creates virtual brains capable of learning, memory, and decision-making. This repository contains the complete implementation of NLM Phase 6 (Final Integration).

### Key Features:

- **Brain-inspired Architecture**: Spiking neurons with LIF dynamics
- **Developmental Learning**: Brains that mature and adapt over time
- **Integrated Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection for learning
- **Prediction Systems**: Forward models and prediction error signals
- **Closed-Loop Architecture**: Complete world interaction cycle

### Installation and Setup

#### Quick Setup (Most Users)

```bash
# Clone the repository
cd /path/to/nlm

# Install Python bindings
pip install -e .

# Or build from source
pip install scikit-build-core pybind11 numpy
pip install .
```

#### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Run tests (optional)
ctest --output-on-failure
```

### First Running

Once installed, you can run the Phase 6 integration demo:

```bash
./nlm_phase6_demo
```

### Python Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes fired")
```

## Getting Started

### Beginner Tutorials

If you're new to NLM, start with these resources:

1. **Easy Usage Guide** (`easy_usage.md`) - Copy-paste examples for beginners
2. **HOW_TO_USE.md** - Comprehensive Python API reference
3. **examples/advanced_agent_training.py** - Advanced agent training examples

### Example Scripts

The `examples/` directory contains:

- `advanced_agent_training.py` - Complete agent training with all subsystems
- `nlm_demo.py` - Basic NLM demonstration (in source directory)
- `test_import.py` - Basic functionality tests

### Configuration Files

Configuration options are stored in the `configs/` directory:

- `default_config.json` - Default configuration
- `development_config.json` - Configuration optimized for developmental learning
- `exploration_config.json` - Configuration for curiosity-driven exploration

### Testing

Run the test suite to verify everything works:

```bash
cd build
ctest --output-on-failure
```

Or run Python tests:

```bash
python -m pytest tests/ -v
```

## Phase 6: Final Integration

Phase 6 represents the culmination of NLM development, integrating all previous phases into a coherent artificial brain system.

### Integration Achievements

#### Core Systems
- **Memory**: Working memory, episodic memory, associative memory (fully connected)
- **Neuromodulation**: Dopamine, curiosity, novelty detection (coordinated)
- **Prediction**: Forward models, prediction error, confidence tracking
- **Cognition**: Planning, concept formation, attention (neural, not transformer)
- **Development**: Developmental stages, structural plasticity, maturation

#### Technical Features
- **Checkpoint System**: Save and load brain states
- **Replay and Consolidation**: Memory replay during rest phases
- **Performance Optimizations**: Memory pools, event queues, SIMD
- **Parallel Processing**: Multi-threaded simulation capabilities

### Example: Complete Brain Loop

```python
# 1. Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Setup world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 3. Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 4. Run complete simulation loop
for step in range(1000):
    world.update(0.1)                    # Update environment
    
    percept = world.getSensoryPercept()  # Perceive world
    agent.processSensoryInput(percept)   # Send to brain
    
    brain.step(step)                      # Neural processing
    
    action = agent.decodeMotorCommand()   # Brain's decision
    world.applyMotorCommand(action, world.getSimulationTime())  # Act
    
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)  # Learn
    
    agent.updateDevelopment(0.1)         # Develop
```

## Scientific Background

NLM is a research project investigating computational brain-like systems. We make **NO claims** that NLM accurately reproduces biological brains, but rather use it as a test bed for understanding:

- How memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in coordinated ways
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether the complete brain loop functions coherently

### Key Research Questions

- Can artificial developmental systems learn complex behaviors?
- How do multiple memory systems interact during learning?
- What role does neuromodulation play in coordinating plasticity?
- Can prediction-driven learning replace purely reward-based learning?

### Scientific Limitations

NLM uses simplified models for research purposes:

- **Neurons**: Leaky Integrate-and-Fire (LIF), not biologically realistic
- **Memory**: Simplified models of working and episodic memory
- **Architecture**: Research prototype, not full brain simulation

## Project Structure

```
NLM/
├── src/                    # Source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   └── visualization/     # Visualization
├── python/                 # Python bindings
│   ├── bindings.cpp       # Pybind11 bindings
│   └── CMakeLists.txt      # Python package build
├── examples/               # Example scripts
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md     # System architecture
│   ├── SCIENCE.md         # Scientific background
│   ├── ROADMAP.md         # Project roadmap
│   ├── EXPERIMENTS.md     # Experiment descriptions
│   ├── LIMITATIONS.md     # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md  # Phase 6 audit results
├── configs/                # Configuration files
├── tests/                  # Test suite
└── docs/                   # Documentation
```

## Getting Help

### Documentation

- **easy_usage.md** - Beginner-friendly guide with copy-paste examples
- **HOW_TO_USE.md** - Complete Python API reference
- **ARCHITECTURE.md** - Detailed system architecture
- **SCIENCE.md** - Scientific principles and background

### Support

For technical issues, check:
1. **Installation**: Did you install all dependencies?
2. **Configuration**: Are all required settings properly configured?
3. **Version**: Are you using a recent version?

### Community

- **GitHub Issues**: Report bugs and feature requests
- **Documentation**: Contribute examples and improvements
- **Research**: Publish papers based on NLM research

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project — See `docs/` directory for detailed scientific background and author information.

---

*This documentation is continuously updated. For the latest version, check the repository or documentation website.*
