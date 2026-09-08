# NLM — Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

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

## Key Improvements in This Version

### 1. Code Organization Refactoring
- **Brain.cpp** reduced from 1117 lines to ~550 lines
- Extracted complex logic into separate files:
  - `BrainStepOrchestrator.cpp`: Manages the 15-step brain simulation loop
  - Better separation of concerns with `BrainInitializer.cpp`, `BrainPersistence.cpp`, etc.
- Improved maintainability and readability
- Enhanced performance through optimized loops

### 2. Python Bindings Enhancement
- **Pythonic API design** with better naming conventions
- **Type-safe configuration** access with both getter and setter methods
- **Enhanced error handling** with try/catch and user-friendly error messages
- **Convenience methods** for common operations (isMovement(), isInteraction(), etc.)
- **Better documentation** with comprehensive docstrings
- **Support for both numpy and native Python arrays**
- **Factory functions** for easy creation of NLM components

### 3. Bug Fixes
- Fixed refractory period logic in load() method
- Corrected lambda parameter shadowing in removeInterRegionConnection()
- Added missing constructor initializers for integrated systems
- Fixed duplicate code assignments in episodic memory

### 4. Performance Optimizations
- Vectorized operations replacing triple nested loops
- Cached iterators to avoid repeated getPopulations() calls
- Extracted constants to reduce magic numbers
- Optimized memory access patterns

## Python API Highlights

### Configuration System
```python
import pynlm

# Create configuration with defaults
config = pynlm.createDefaultConfig()

# Type-safe access with exceptions
neuron_count = config.get_int("neuron_count")
connection_probability = config.get_float("connection_probability")

# Or with default values
max_neurons = config.get_int_or("neuron_count", 1000)
step_prob = config.get_float_or("connection_probability", 0.1)

# Set values
config.set_int("neuron_count", 2000)
config.set_string("checkpoint_dir", "./checkpoints")
```

### Brain System
```python
import pynlm

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getBrainStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing: {stats['firing_neurons']}")
print(f"Avg firing rate: {stats['avg_firing_rate']:.2f}")
```

### Agent System
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent episode
agent.runAgentEpisode(world, maxSteps=500)
```

### Convenience Methods
```python
# Action utilities
action = brain.produceAction()
if action.isMovement():
    print("Moving!")
elif action.isInteraction():
    print("Interacting!")
elif action.isLooking():
    print("Looking!")

# Sensory utilities
percept = world.getSensoryPercept()
if percept.hasVision():
    print(f"Vision active: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
if percept.hasAudio():
    print(f"Audio active: {percept.getAudio().getDuration():.2f}s")

# Body utilities
body = world.getAgentBody()
print(f"Position: ({body.x:.1f}, {body.y:.1f})")
print(f"Energy: {body.energy:.2f}")
print(f"Healthy: {body.isHealthy()}")
```

## System Architecture

### Core Components

1. **Brain System**
   - Central neural computation engine
   - Real spiking neural computation with LIF neuron model
   - Event-driven spike propagation
   - Integrated memory systems (working, episodic, associative)
   - Neuromodulation for learning and motivation
   - Predictive coding and cognitive processing
   - Developmental stages that affect plasticity
   - Checkpoint saving and loading for persistence

2. **Memory Systems**
   - **Working Memory**: Transient active information storage with competitive dynamics
   - **Episodic Memory**: Experience encoding and replay system
   - **Associative Memory**: Hebbian pattern association system

3. **Neuromodulation Systems**
   - **Dopamine**: Reward prediction error and reinforcement learning
   - **Curiosity**: Exploration motivation
   - **Novelty**: Novelty detection
   - **Prediction Error**: Predictive coding error signals

4. **Cognitive Systems**
   - **Neural Planner**: Action sequence evaluation
   - **Concept Formation**: Pattern discovery from experience
   - **Attentional Selection**: Competitive selection dynamics

### Phase 6: Integrated Brain Loop

The brain operates through 15 integrated steps:

1. **Process pending delayed spikes** (deliver synaptic input)
2. **Update all neurons** (LIF dynamics)
3. **Detect spikes and schedule spike events**
4. **Update working memory** (maintenance and competition)
5. **Apply neuromodulation effects** on neural excitability
6. **Apply plasticity rules** (STDP, Hebbian)
7. **Update episodic memory** with current experience
8. **Update prediction system**
9. **Update attention system**
10. **Update concept formation**
11. **Apply structural plasticity** (synaptogenesis, pruning)
12. **Replay important memories** (during rest or periodically)
13. **Apply development effects**
14. **Periodic memory consolidation**
15. **Checkpoint management**

## Building and Installation

### Building C++ Components
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installing Python Bindings
```bash
# From project root (pyproject.toml handles this)
pip install -e .

# Or install specific components
pip install python/
```

### Python Usage
```python
import pynlm

# Basic brain creation and usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Project Structure

```
NLM/
├── src/
│   ├── brain/           # Neural components
│   ├── dynamics/        # Neural dynamics
│   ├── plasticity/      # Plasticity rules
│   ├── development/     # Developmental system
│   ├── neuromodulation/  # Neuromodulators
│   ├── memory/         # Memory systems
│   ├── prediction/      # Prediction systems
│   ├── cognition/       # Cognitive mechanisms
│   ├── sensory/         # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/     # Environment interface
│   └── agent/           # Agent interface
├── python/              # Python bindings
│   └── bindings.cpp     # Enhanced Python bindings
├── tests/               # Unit tests
├── docs/               # Documentation
└── configs/             # Configuration files
```

## Usage Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create and run a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(50):
    brain.step(i)
    firing = brain.getFiringNeuronCount()
    print(f"Step {i}: {firing} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Brain with Agent and World
```python
import pynlm

# Setup brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    agent.applyRewardModulation(1.0, 0.5)

print("Simulation complete!")
```

### Example 3: Configuration Management
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set_int("neuron_count", 2000)
config.set_float("connection_probability", 0.2)
config.set_string("checkpoint_dir", "./my_checkpoints")

# Validate configuration
errors = pynlm.validateConfig(config)
if errors:
    print("Configuration errors:", dict(errors))
else:
    print("Configuration is valid!")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Compare
print(f"Original neurons: {config.get_int('neuron_count')}")
print(f"Loaded neurons: {config2.get_int('neuron_count')}")
```

### Example 4: Advanced Agent with Learning
```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()
config.set_int("neuron_count", 5000)
config.set_float("simulation_timestep", 0.0005)

# Create brain, world, and agent
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Initialize
agent.initialize(world)

# Run extended learning episode
print("Starting learning episode...")
for step in range(1000):
    world.update(0.1)
    
    # Get percept
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Dynamic reward based on agent state
    body = world.getAgentBody()
    if body.energy < 0.1:
        reward = -1.0  # Low energy penalty
    elif body.isEnergyLow():
        reward = 0.5   # Low energy warning
    else:
        reward = 1.0   # Good
    
    agent.applyRewardModulation(reward, 0.5)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        curiosity = agent.getCuriosityLevel()
        stage = agent.getDevelopmentalStage()
        print(f"Step {step}: curiosity={curiosity:.2f}, stage={stage}, energy={body.energy:.2f}")

print("Learning episode complete!")
print(f"Final stats: {brain.getBrainStats()}")
```

## Benefits of the Enhanced NLM

### 1. **Improved Maintainability**
- Clear separation of concerns
- Reduced code duplication
- Better organized file structure
- Comprehensive documentation

### 2. **Enhanced Usability**
- Pythonic API design
- Type-safe configuration
- Convenience methods
- Better error handling

### 3. **Performance Improvements**
- Optimized loops and memory access
- Reduced computational complexity
- Better cache utilization

### 4. **Advanced Features**
- Integrated memory systems
- Neuromodulation for learning
- Developmental stages
- Checkpoint persistence

### 5. **Better Testing and Validation**
- Type-safe getters and setters
- Configuration validation
- Comprehensive error handling
- Status reporting utilities

## Future Enhancements

1. **Machine Learning Integration**
   - Neural architecture search
   - Hyperparameter optimization
   - Transfer learning

2. **Advanced Visualization**
   - 3D neural network visualization
   - Real-time brain activity monitoring
   - Memory visualization

3. **Multi-Agent Systems**
   - Social learning
   - Agent communication
   - Multi-brain coordination

4. **Real-world Applications**
   - Robotics control
   - Autonomous systems
   - Cognitive modeling

## References

1. **Phase Documentation**: See `docs/PHASE6_FINAL_AUDIT.md` for detailed Phase 6 implementation details.

2. **Architecture**: See `docs/ARCHITECTURE.md` for complete system architecture documentation.

3. **Science**: See `docs/SCIENCE.md` for scientific background and theoretical foundations.

4. **Research**: See `docs/EXPERIMENTS.md` for experimental results and findings.

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project — See docs for scientific background and contributing guidelines.
