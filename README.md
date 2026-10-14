# NLM (Neural Learning Machine) - Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Differentiators
- **NOT** a transformer or LLM
- **NOT** a chatbot  
- **NOT** a deep learning model
- **NOT** a pretrained AI system

### What NLM IS
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start (Beginner)

For beginners, we recommend this progression:

### 1. Start with Python Examples
```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for step in range(100):
    brain.step(step)

print(f"Brain thinking for {brain.getTotalSpikeCount()} steps!")
```

### 2. Explore Agent Interactions
```python
# Complete agent example with world interaction
import pynlm

def run_agent_simulation(num_steps=1000):
    # Setup
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
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(500)
```

### 3. Build from Source (Advanced)
```bash
# Build C++ core
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install pybind11 scikit-build-core pytest numpy
cd /path/to/nlm/python
pip install -e .
```

## Project Structure

### Core Components
- **Brain**: Central neural simulation engine
- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, prediction error
- **Prediction System**: Forward models and prediction errors
- **Cognition**: Planning, attention, concept formation
- **Development**: Structural and functional maturation

### Key Modules
```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── memory/        # Memory systems
│   ├── neuromodulation/# Neuromodulators
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── development/    # Developmental system
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework
│   └── visualization/  # Visualization
├── python/              # Python bindings
├── configs/             # Configuration files
└── docs/                # Documentation
```

## Learning Path

### Phase 1: Foundation (Current)
- Project skeleton and core types
- Neural interfaces and basic neuron models
- Simple event-driven spike propagation

### Phase 2: Advanced Core (Implemented)
- **Real integrate-and-fire neuron dynamics**
- **Real synaptic transmission with plasticity**
- **Advanced neuromodulation with dopamine dynamics**

### Phase 3: World Interaction
- Sensory processing and motor control
- Reward prediction and learning
- Environmental interaction

### Phase 4: Higher Cognition  
- Working memory and attention
- Episodic memory and replay
- Predictive processing and planning

### Phase 5: Performance & Scalability
- Memory pools and event queues
- SIMD vectorization and parallel processing
- Checkpoint and persistence systems

### Phase 6: Final Integration (Current Goal)
- **All systems integrated into coherent brain loop**
- **Real-time learning and development**
- **Complex behavior emergence**

## Using the Python API

### Basic Brain Operations
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Save/load brain state
brain.save("my_brain.bin")
# ... do some work ...
brain.load("my_brain.bin")
```

### Agent Brain Interface
```python
# Connect brain to world environment
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)  # Dopamine-based learning
agent.enableCuriosity(True)         # Intrinsic motivation
agent.enableDevelopment(True)       # Brain maturation
agent.enableStructuralPlasticity(True) # Growing new connections

# Run simulation with learning
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Configuration Management
```python
# Load from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.cfg")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.05)

# Save to file
config.saveToFile("optimized_config.cfg")

# Get configuration values
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")
```

### Error Handling
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(100)
    print("Simulation completed successfully!")
except Exception as e:
    print(f"Simulation failed: {e}")
```

## Building from Source

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Build Steps
```bash
# 1. Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 2. Build Python bindings
pip install pybind11 scikit-build-core pytest numpy
cd python
pip install -e .

# 3. Run tests
ctest --output-on-failure
```

### Build Options
```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Configuration Files

### Default Configuration (`configs/default.cfg`)
The default configuration provides sensible starting values:
- 1000 neurons with 2 brain regions
- 10% connection probability
- Realistic neuron dynamics parameters
- Integrated memory and neuromodulation

### Configuration Format
```
# Comments start with #
# key = value
# Values can be numbers, booleans, or strings
# Boolean: true or false
# Numbers: integers or floats
# Strings: quoted with " or '

random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
dopamine_baseline = 0.1
log_level = INFO
```

### JSON Configuration (Phase 2)
Future versions will support JSON configuration files for better data structure and integration with modern tools.

## Advanced Features

### 1. Multi-Region Brains
```python
# Create brain with multiple regions
config = pynlm.createDefaultConfig()
config.set("brain.region_count", 3)
brain = pynlm.createBrain(config)
brain.initialize()

# Access different regions
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getTotalNeuronCount()} neurons")
```

### 2. Custom Sensory Processing
```python
# Create custom sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)
data = [0.1, 0.5, 0.9] * 24  # Example visual data
vision.setData(data)

# Use in brain
sensory_input = pynlm.SensoryInput()
sensory_input.setVision(vision)
brain.receiveSensoryInput(sensory_input)
```

### 3. Development Stages
```python
# Manipulate brain development
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Observe development effects
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Plasticity factor: {brain.getDopamine().getPlasticityFactor():.3f}")
```

### 4. Memory Management
```python
# Access memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Active memory traces: {working_memory.getActiveTraces()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")
```

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"
**Solution:**
- Ensure `brain.initialize()` is called before simulation
- Check that enough simulation steps are run (brains need time to "warm up")
- Verify sensory input is properly connected

#### "ImportError: No module named 'pynlm'"
**Solution:**
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"
**Solution:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify all required parameters are set

#### "Building fails"
**Solution:**
- Install dependencies: `pip install pybind11 scikit-build-core pytest numpy`
- Check CMake version: `cmake --version` (needs 3.16+)
- Verify all source files are present

#### "Configuration file errors"
**Solution:**
- Check syntax: `# key = value` format
- Ensure no duplicate keys
- Use proper quotes for string values

## Further Reading

### Architecture Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [ROADMAP.md](docs/ROADMAP.md) - Project roadmap and future directions

### Research Papers
See [docs/SCIENCE.md](docs/SCIENCE.md) for references and scientific foundations.

## Contact and Community

### Getting Help
1. **Documentation**: Check this documentation site
2. **GitHub**: [nlm-project/nlm](https://github.com/nlm-project/nlm)
3. **Issue Tracker**: Report bugs and request features

### Contributing
1. Fork the repository
2. Create a feature branch
3. Implement your changes with tests
4. Submit a pull request

### Development
See [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) for information about running experiments and contributing to research.

## License

MIT License. See [LICENSE](LICENSE) for details.

## Acknowledgments

This project builds on decades of neural computation research and incorporates principles from:
- Computational neuroscience
- Artificial intelligence
- Systems biology
- Cognitive modeling

The NLM project is a research endeavor and makes no claims about reproducing biological accuracy or consciousness.

---

**Current Phase:** 6 - FINAL INTEGRATION
**Status:** Most systems implemented and integrated
**Next Goals:** Complete Phase 6 integration and add advanced Python features