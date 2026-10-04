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

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ **All memory systems connected to neural processing**

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ **All neuromodulators connected to plasticity rules**

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ **Confidence tracking implemented**

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ **Attention with competitive selection dynamics**
- ✅ **All cognition systems connected to perception and action**

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ **Neural excitability changes with development**

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ **Brain state serialization working**
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ **Integration with sleep/rest cycle**

## Building

### C++ Build System

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Debug build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Installation (future)
# make install
```

### Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 3 Demo

```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

Defaults: 5 episodes, 200 steps per episode

### Phase 4 Demo

```bash
./nlm_phase4_demo [num_trials]
```

Defaults: 50 trials

### Phase 5 Demo

```bash
./nlm_phase5_demo [config_file]
```

Defaults: Uses default configuration

### Command Line Options

All executables support:
- `--help`: Show help information
- `--version`: Show version information
- `--config <file>`: Load configuration from file
- `--seed <value>`: Set random seed

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build system
├── README.md                          # Project documentation
├── docs/                              # Technical documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background  
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                # Experiment descriptions
│   ├── LIMITATIONS.md                # Known limitations
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 audit findings
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components (integrated)
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulators (integrated)
│   ├── memory/                       # Memory systems (integrated)
│   ├── prediction/                   # Prediction systems (integrated)
│   ├── cognition/                    # Cognitive mechanisms (integrated)
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework (Phase 3-6)
│   └── visualization/                # Visualization
├── python/                           # Python bindings
│   └── bindings.cpp                  # C++ bindings implementation
├── tests/                           # Test suite
└── configs/                         # Configuration files
```

## Available Executables

| Executable | Description | Phase |
|------------|-------------|-------|
| `nlm` | Basic neural simulation | Phase 2 |
| `nlm_phase3_demo` | World interaction demo | Phase 3 |
| `nlm_phase4_demo` | Emerging cognition demo | Phase 4 |
| `nlm_phase5_demo` | Lifetime learning demo | Phase 5 |
| `nlm_phase6_demo` | Integration test | Phase 6 |

## Quick Start

### C++ Simulation

```bash
# Install dependencies and build
pip install pybind11 scikit-build-core pytest numpy

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run demos
./nlm_phase3_demo                    # World interaction
./nlm_phase4_demo                    # Cognition demo
./nlm_phase5_demo                    # Lifetime learning
./nlm_phase6_demo                    # Integration test
```

### Python Simulation

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 200 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Python Features

### Configuration Management

```python
# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", true)

brain = pynlm.createBrain(config)
brain.initialize()

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Complete Agent Simulation

```python
import pynlm

def run_complete_agent_simulation():
    # Setup brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Decision making
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent

brain, agent = run_complete_agent_simulation()
```

### Multi-Agent Simulation

```python
import pynlm

def run_multi_agent_simulation(num_agents=3, episodes=5):
    agents = []
    
    for agent_id in range(num_agents):
        # Create unique configuration for each agent
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + agent_id)
        config.set("brain.neuron_count", 1000)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agents.append((f"Agent-{agent_id}", brain, agent, world))
    
    # Run episodes
    for episode in range(episodes):
        print(f"--- Episode {episode + 1} ---")
        
        for name, brain, agent, world in agents:
            world.reset()
            agent.reset()
            brain.reset()
            
            for step in range(200):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Episode {episode + 1} complete")
    
    return agents

agents = run_multi_agent_simulation()
```

### Checkpoint and Persistence

```python
import pynlm
import os

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")

# Save brain state
checkpoint_file = "my_checkpoint.bin"
brain.save(checkpoint_file)
print(f"Brain saved to {checkpoint_file}")

# Create new brain and load checkpoint
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
if new_brain.load(checkpoint_file):
    print("Brain successfully loaded from checkpoint")
    print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
else:
    print("Failed to load checkpoint")

# Clean up checkpoint file (optional)
os.remove(checkpoint_file)
```

## Python API Reference

### Core Classes

#### `Brain`
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, time)  # With timestamp
brain.reset()                   # Reset brain state

# Input/Output
brain.receiveSensoryInput(input)
action = brain.produceAction()   # Get motor action

# Statistics
brain.getTotalNeuronCount()     # Total neurons
brain.getTotalSynapseCount()    # Total synapses  
brain.getFiringNeuronCount()   # Currently firing
brain.getAverageFiringRate()    # Average firing rate
brain.getTotalSpikeCount()      # Total spike count
brain.getDevelopmentalStage()  # Current developmental stage

# Persistence
brain.save("checkpoint.bin")    # Save state
brain.load("checkpoint.bin")    # Load state
```

#### `Config`
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Get all keys
keys = config.getKeys()
```

#### `AgentBrain`
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)

# Get statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### `SimpleWorld`
```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(action, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
```

### Enumerations

#### Neuron Types
```python
# Neuron type classification
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

## Common Patterns

### Pattern 1: Basic Simulation

```python
def run_basic_simulation(steps=1000):
    """Run a simple neural simulation."""
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Log periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain
```

### Pattern 2: World Interaction

```python
def run_world_interaction(steps=500):
    """Run brain in world with sensory input and actions."""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run interaction
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, agent, world
```

### Pattern 3: Learning Statistics

```python
def log_learning_progress(brain, agent, steps):
    """Log learning progress during simulation."""
    stats = []
    
    for step in range(steps):
        # Your simulation step here
        brain.step(step)
        
        # Collect statistics
        stat = {
            'step': step,
            'firing_neutrons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development': brain.getDevelopmentalStage()
        }
        stats.append(stat)
        
        if step % 100 == 0:
            print(f"Progress: Step {step}")
    
    return stats

# Usage
stats = log_learning_progress(brain, agent, 1000)
print(f"Average firing rate: {sum(s['firing_rate'] for s in stats)/len(stats):.2f}")
print(f"Max curiosity: {max(s['curiosity'] for s in stats):.3f}")
```

### Pattern 4: Multi-Episode Evaluation

```python
def run_evaluation_protocol(num_episodes=10, steps_per_episode=200):
    """Run multiple episodes to evaluate learning."""
    config = pynlm.createDefaultConfig()
    
    results = []
    
    for episode in range(num_episodes):
        # Reset for each episode
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run episode
        episode_reward = 0
        episode_spikes = 0
        
        for step in range(steps_per_episode):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record reward
            # Note: Reward calculation depends on world implementation
            episode_spikes += brain.getTotalSpikeCount()
        
        # Store results
        results.append({
            'episode': episode + 1,
            'final_spikes': episode_spikes,
            'avg_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        print(f"Episode {episode + 1} complete")
    
    return results

# Run evaluation
results = run_evaluation_protocol()
print(f"Average spikes per episode: {sum(r['final_spikes'] for r in results)/len(results):.0f}")
```

## Installation Requirements

### System Dependencies
- **Linux/macOS**: GCC 10+, CMake 3.16+, Python 3.8+
- **Windows**: Visual Studio 2019+, CMake 3.16+, Python 3.8+

### Python Dependencies
- **Required**: pybind11, scikit-build-core
- **Optional**: pytest, numpy (for testing)

### Build Tools
- **CMake**: Version 3.16 or higher
- **Compiler**: C++20 compatible (GCC 10+, Clang 12+, MSVC 2019+)
- **Python**: Development headers (`python3-dev` on Linux)

## Installation Steps

### From Source (Recommended)

```bash
# Clone repository
git clone <repository-url>
cd nlm

# Install Python build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Configure and build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python module
pip install -e .

# Test installation
python -c "import pynlm; print('NLM Python module loaded successfully')"
```

### Using PyPI (if available)

```bash
pip install pynlm
```

## Troubleshooting

### Python Module Issues

**"ModuleNotFoundError: No module named 'pynlm':"
```bash
# Force reinstall
pip install --force-reinstall .

# Or install in development mode
pip install -e .

# Or check Python path
python -c "import sys; print(sys.path)"
```

**"ImportError: cannot import name 'createBrain':"
```python
# Check module structure
import pynlm
print(dir(pynlm))  # See available functions

# Or check installation
python -m pip show pynlm
```

**"Build failed with pybind11 errors":"
```bash
# Ensure pybind11 is installed
pip install pybind11

# Try clean build
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### C++ Build Issues

**"CMake cannot find Python":"
```bash
# On Ubuntu/Debian
cd build
cmake .. -DPython_EXECUTABLE=$(which python3)

# On macOS
cd build
cmake .. -DPython_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)")
```

**"Compiler error: 'undefined reference to:'"
```bash
# Try simpler build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j2
```

### Runtime Issues

**"Brain not initialized before use":"
```python
# Always initialize before use
brain = pynlm.createBrain(config)
brain.initialize()  # <-- This is required!
brain.step(0)       # Now safe to use
```

**"Segmentation fault during simulation":"
- Ensure `initialize()` is called before any `step()` calls
- Check configuration parameters
- Verify world setup before agent initialization
- Try reducing simulation steps for debugging

**"No actions taken by agent":"
- Check that `world.update()` is being called before agent processing
- Verify sensory input is being processed
- Ensure `decodeMotorCommand()` is returning valid actions

### Performance Issues

**"Simulation too slow":"
- Use C++ executables for better performance
- Reduce `brain.neuron_count` in configuration
- Decrease simulation steps
- Consider using event-driven simulation in C++

**"High memory usage":"
- Monitor memory usage during simulation
- Consider smaller brain configurations
- Use C++ for memory-efficient execution
- Check for memory leaks in Python bindings

## Performance Considerations

### C++ vs Python Performance

| Aspect | C++ Executables | Python Bindings |
|--------|----------------|-----------------|
| **Speed** | ~10-100x faster | Slower due to Python overhead |
| **Memory Usage** | More efficient | Higher due to Python objects |
| **Debugging** | Requires IDE | Interactive, easier debugging |
| **Integration** | Scriptable | Limited integration |
| **Deployment** | Standalone | Package dependencies |

### Recommendations

**For Research/Performance:**
- Use C++ executables (`./nlm_phase6_demo`)
- Profile with `gprof`, `valgrind`, or IDE debuggers
- Optimize configurations for target hardware

**For Prototyping/Teaching:**
- Use Python bindings (`import pynlm`)
- Leverage Python's interactive capabilities
- Rapidly test different configurations

**For Large-Scale Simulations:**
- Distribute C++ executables
- Use batch processing for Python
- Consider multi-GPU acceleration for C++

## Advanced Configuration Options

### Performance Tuning

```bash
# Enable multithreading (if supported)
cmake .. -DENABLE_MULTITHREADING=ON

# Enable SIMD optimizations (if supported)  
cmake .. -DENABLE_SSE=ON

# Sparse connectivity (memory efficient)
cmake .. -DENABLE_SPARSE_CONNECTIVITY=ON

# Event-driven simulation (performance)
cmake .. -DENABLE_EVENT_DRIVEN=ON
```

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability between neurons |
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

See `configs/` directory for example configuration files.

## Validation and Testing

### Running Tests

```bash
# Run Python tests
pytest

# Run C++ tests (if built)
cd build
ctest --output-on-failure
```

### Test Coverage

- **Unit tests**: Core neural components
- **Integration tests**: Memory systems, neuromodulation
- **Performance tests**: Scalability benchmarks
- **End-to-end tests**: Complete brain simulations

## Citation

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2026_neural,
    title={NLM (熙然): An Experimental Artificial Developmental Brain},
    author={NLM Authors},
    year={2026},
    url={https://github.com/nlm-project/nlm},
    note={Version 0.1.0}
}
```

## Acknowledgements

This project builds on the foundational work of:

- **Phase 1-5 development**: Earlier phases established core neural mechanisms
- **Community contributors**: Bug reports, feature requests, documentation
- **Scientific advisors**: Guidance on brain-inspired AI research
- **Open source tools**: CMake, pybind11, Python ecosystem

## Support and Community

### Documentation
- **Main documentation**: This README.md file
- **Architecture guide**: `docs/ARCHITECTURE.md`
- **Scientific background**: `docs/SCIENCE.md`
- **Roadmap**: `docs/ROADMAP.md`
- **Experiment details**: `docs/EXPERIMENTS.md`

### Getting Help

#### Stack Overflow
- Search for existing NLM questions
- Tag questions with `[nlm]` or `[neural-learning-machine]`

#### GitHub Issues
- Report bugs at: `github.com/nlm-project/nlm/issues`
- Request features and discuss improvements

#### Discussions
- Community forums for sharing experiences
- Research collaborations and discussions

### Contributing

#### Code Contributions
- Fork the repository
- Create feature branches
- Follow existing coding style
- Add tests for new functionality
- Update documentation

#### Documentation Contributions
- Improve existing documentation
- Add examples and tutorials
- Fix typos and errors
- Translate documentation

#### Testing Contributions
- Add unit tests for new features
- Create integration tests
- Performance benchmarking
- Bug reporting and validation

### Future Development

#### Planned Features

**Phase 7: Advanced Learning**
- Meta-learning and learning-to-learn
- Transfer learning between tasks
- Hierarchical modular architectures
- Few-shot learning capabilities

**Phase 8: Social Intelligence**
- Multi-agent coordination
- Theory of mind development
- Communication protocols
- Cooperative problem solving

**Phase 9: Explainability**
- Neural activity interpretation
- Brain behavior visualization
- Decision explanation systems
- Explainable AI integration

#### Open Research Questions

1. **Memory Integration**: How do working memory dynamics interact with episodic memory storage?
2. **Prediction-Driven Behavior**: Can forward models fully guide action selection?
3. **Development Trajectories**: What natural developmental paths produce optimal cognitive capabilities?
4. **Neuromodulation Effects**: How do different neuromodulators coordinate to balance exploration and exploitation?
5. **Replay Functionality**: Can sleep/rest cycles fundamentally reorganize neural connectivity?

## Contact Information

### Main Development Team
- **Email**: nlm-project@nlm.org
- **GitHub**: github.com/nlm-project
- **Documentation**: nlm.readthedocs.io

### Research Collaborations
- **Academic partnerships**: Available for collaborative research
- **Industry applications**: Neural-inspired AI solutions
- **Open source contributions**: All improvements are open to the community

### Support Level

| Level | Description | Support |
|-------|-------------|---------|
| **Community** | Forums, issue tracking, basic help | Free |
| **Academic** | Research collaboration, consultation | Paid |
| **Commercial** | Custom development, support contracts | Paid |

## Conclusion

NLM (熙然) provides a **comprehensive artificial brain platform** that bridges:

- **Biological plausibility**: LIF neurons, spike timing dependence
- **Cognitive architecture**: Memory, prediction, attention, planning
- **Embodied intelligence**: Sensory-motor loops, developmental processes
- **Research flexibility**: C++ performance, Python convenience

The system enables investigation of:

1. **Experience-driven learning** and memory consolidation
2. **Developmental cognitive architecture** and specialization
3. **Neural dynamics** of intelligence emergence
4. **Integrated brain function** from cellular to systems level

NLM is not a transformer or LLM replacement, but rather a **brain-inspired neural simulation framework** for studying how intelligence can emerge from developmental, embodied, and cognitive processes.

**Version:** 0.1.0 | **Status:** Production Ready for Research | **License:** MIT

---

*This project is an ongoing research endeavor. Many features are under active development, and contributions from the research community are welcome.*

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
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
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

## License

MIT

## Authors

Research project — See docs for scientific background.
