# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

This project demonstrates an experimental brain simulation that develops and learns from experience. Perfect for researchers, students, and developers interested in brain-inspired AI.

```bash
# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic neural computation demo
./nlm

# Run Phase 6 integration demo  
./nlm_phase6_demo
```

## What Makes NLM Special

✅ **Real Spiking Neurons**: Leaky Integrate-and-Fire (LIF) dynamics
✅ **Complete Brain Integration**: Memory, neuromodulation, prediction, cognition
✅ **Developmental Learning**: Brain matures and adapts over time
✅ **Experience-based Learning**: Changes synaptic connections through interaction
✅ **Advanced Features**: Working memory, episodic memory, curiosity, planning

## Key Features

### 🧠 Neural Architecture
- **LIF Neurons**: Real biological neuron dynamics
- **Synaptic Plasticity**: STDP and Hebbian learning rules
- **Structural Plasticity**: Growing and pruning connections
- **Event-driven Processing**: Efficient spike propagation

### 💾 Memory Systems
- **Working Memory**: Active traces and competition dynamics
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern completion and storage
- **Memory Replay**: Consolidation during rest/sleep

### 🧪 Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Novelty detection and exploration drive
- **Prediction Error**: Learning from unexpected outcomes
- **Novelty**: Adaptation to new environments

### 🧠 Cognitive Functions
- **Neural Planning**: Action sequence evaluation
- **Concept Formation**: Pattern abstraction and categorization
- **Attention**: Competitive selection of information
- **Self-model**: Internal representation of the agent

### 🌱 Developmental Processes
- **Maturation**: Increasing neural efficiency over time
- **Plasticity Modulation**: Age-dependent learning rates
- **Excitability Changes**: Dynamic neural responsiveness

## Installation

### Prerequisites
```bash
# C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
# CMake 3.16 or higher
# Make or Ninja build system
```

### Building from Source
```bash
# Standard release build
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Debug build for development
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Installation (optional)
sudo make install
```

### Quick Test
```bash
# Build and run basic tests
cd /path/to/nlm
mkdir build
cd build
cmake ..
make test
```

## Running Examples

### Basic Neural Computation (`./nlm`)
```bash
Runs Phase 2 neural computation tests:
- LIF neuron dynamics
- Synaptic transmission with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Learning experiments with measurable changes
```

### Integration Demo (`./nlm_phase6_demo`)
```bash
Comprehensive Phase 6 integration test verifying all brain systems:
- Memory systems integration
- Neuromodulation integration
- Prediction system integration
- Development system integration
- Checkpoint save/load functionality
- Replay and consolidation features
```

### Configuration Options
```bash
./nlm --neuron_count=2000 --region_count=4 --stdp_ltp_weight=0.03
./nlm --config=/path/to/custom_config.cfg
./nlm_phase6_demo --neuronCount=1000 --maxSteps=5000
```

## Advanced Usage

### Configuration Files
NLM supports configuration via files and command-line arguments:

**Basic Config (`configs/default.cfg`):**
```
# Core settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Learning parameters  
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
```

**Command Line Arguments:**
```bash
# Override config values
./nlm \
  --neuron_count=2000 \
  --region_count=3 \
  --stdp_ltp_weight=0.025 \
  --log_level=Debug
```

### Python Bindings
For Python users, NLM provides a high-level interface:

```python
import pynlm

# Create brain with default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

### Complete Agent Simulation
```python
import pynlm

# Setup complete agent with world interaction
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True) 
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent in environment
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, Reward={world.computeReward(brain.getRegions()[0].get())}")
```

## Documentation Resources

### 📚 In-Depth Documentation
- **ARCHITECTURE.md**: Detailed system architecture and design
- **SCIENCE.md**: Scientific background and biological inspiration
- **EXPERIMENTS.md**: Experimental methodology and results
- **PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit and validation

### 🎯 Learning Resources
- **HOW_TO_USE.md**: Comprehensive usage guide with examples
- **SCALING.md**: Performance optimization and scaling strategies
- **EXPERIMENTS.md**: Experimental setup and analysis

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Logger, Random, Types)
│   ├── brain/          # Neural components (Neuron, Synapse, Brain)
│   ├── dynamics/       # Neural dynamics (Spike system)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, Structural)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (Dopamine, Curiosity, etc.)
│   ├── memory/        # Memory systems (Working, Episodic, etc.)
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms (Planner, Concepts)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
│   └── visualization/  # Visualization interface
│
├── configs/            # Configuration files
├── tests/              # Unit tests
├── docs/               # Documentation
└── python/             # Python bindings (pynlm)
```

## Development Guide

### Adding New Components
1. **Create new directory** under `src/` (e.g., `src/behavior/`)
2. **Implement core classes** with proper headers in `src/<module>/<Module>.hpp`
3. **Add source implementation** in `src/<module>/<Module>.cpp`
4. **Update CMakeLists.txt** to include new library
5. **Add tests** in `tests/` directory
6. **Update documentation** as needed

### Contributing
1. **Fork** the repository
2. **Create feature branch** for your changes
3. **Follow existing coding style** (C++20, CMake 3.16+)
4. **Add tests** for new functionality
5. **Update documentation** with examples
6. **Submit pull request** with detailed description

## Performance Tips

### For Large Networks
```bash
# Optimize for memory usage
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_USE_MEMORY_POOLS=ON

# Optimize for speed  
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_USE_SIMD=ON -DNLM_PARALLEL_PROCESSING=ON
```

### Runtime Optimization
```python
# Use smaller configuration for faster testing
brain = pynlm.createBrain(pynlm.createConfig({
    'neuron_count': 500,
    'region_count': 2,
    'max_steps': 1000
}))

# Enable memory-efficient modes
agent.enableMemoryEfficiency(True)
```

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Check log level: `--log_level=Debug`
- Verify sensory input is connected
- Increase simulation steps

### "Memory systems not working"
- Check memory integration in Phase 6 demo
- Verify neuromodulation is enabled
- Ensure brain has proper configuration

### "Configuration errors"
- Fix duplicate keys in config files
- Use proper key=value format
- Check for typos in configuration names

### "Performance issues"
- Reduce neuron count for initial testing
- Disable visualization if not needed
- Use Release build type
- Check system resources

## Project Status

### Current Phase
**Phase 6: Final Integration** ✅
- All brain systems integrated into coherent architecture
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load functional
- Replay and consolidation working

### Future Phases
Phase 7: Advanced Learning
Phase 8: Social Learning
Phase 9: Transfer Learning
Phase 10: Open-ended Development

## Contact

For questions, issues, or collaboration:
- **Documentation**: Check `docs/` directory
- **Issues**: GitHub repository issues
- **Research**: Scientific papers in `docs/SCIENCE.md`
- **Community**: Project Discord/Slack (if available)

---

**NLM (熙然) is an experimental research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains.**
