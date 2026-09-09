# NLM (Neural Learning Machine) - Comprehensive Documentation

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

**Key Differentiators:**
- NOT a transformer or LLM
- NOT a chatbot
- NOT a deep learning model
- NOT a pretrained AI system

**What NLM IS:**
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## 🚀 Quick Start

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getAverageFiringRate():.2f} firing rate")

print("Simulation complete!")
```

## 📋 Table of Contents

- [Basic Usage](#basic-usage)
- [Agent-Based Learning](#agent-based-learning)
- [Memory Systems](#memory-systems)
- [Neuromodulation](#neuromodulation)
- [Development](#development)
- [Advanced Features](#advanced-features)
- [Troubleshooting](#troubleshooting)
- [API Reference](#api-reference)
- [Examples](#examples)

## Basic Usage

### Creating and Running a Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run the brain simulation
for i in range(1000):
    brain.step(i)
    
    # Check statistics periodically
    if i % 100 == 0:
        print(f"Step {i}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### Configuration

```python
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()

# Adjust brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## Agent-Based Learning

### Complete Agent Example

```python
import pynlm

def run_complete_agent_simulation(steps=1000):
    """Run a complete agent simulation with world interaction."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if agent supports it
        reward = result.reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  World position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print(f"\nSimulation complete after {steps} steps!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation(500)
```

### Interactive Agent

```python
import pynlm
import time

def interactive_agent_demo():
    """Demo an agent that explores and learns in the world."""
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting interactive agent demo!")
    print("Agent can move, explore, and learn from experience.")
    print("Press Ctrl+C to stop.\n")
    
    try:
        for step in range(1000):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
            
            if step % 50 == 0:
                print(f"Step {step}: "
                      f"Pos=({world.getAgentBody().x:.1f},{world.getAgentBody().y:.1f}) "
                      f"Energy={world.getAgentBody().energy:.1f} "
                      f"Firing={brain.getFiringNeuronCount()} "
                      f"Curiosity={agent.getCuriosityLevel():.2f}")
                
                time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\nDemo stopped by user.")
    
    return brain, agent, world

# Run interactive demo
brain, agent, world = interactive_agent_demo()
```

## Memory Systems

### Working Memory

```python
import pynlm

# Create brain with working memory
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Working memory traces: {working_memory.getActiveTraces()}")
```

### Episodic Memory

```python
import pynlm

# Create agent and let it build episodic memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for a while to build memory
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    print(f"Available for replay: {episodic_memory.getEpisodesForReplay(3).size()} episodes")
```

### Episodic Memory Replay

```python
import pynlm

# Create scenario for replay demonstration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps to create memory
for i in range(100):
    brain.step(i)

# Get the episodic memory system
memory = brain.getEpisodicMemory()
if memory:
    # Try to replay some episodes
    episodes = memory.getEpisodesForReplay(2)
    for i, episode_ptr in enumerate(episodes) {
        if (episode_ptr) {
            std::cout << "Replaying episode: " << episode_ptr->timestamp << std::endl;
            memory.replayEpisode(episode_ptr);
        }
    }
}
```

## Neuromodulation

### Reward Learning

```python
import pynlm

# Setup agent with reward modulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

print("Training agent with reward modulation...")

# Training loop with rewards
for step in range(300):
    world.update(0.1)
    
    # Get action and apply
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward signal
    agent.applyRewardModulation(result.reward, 0.0)
    
    if step % 50 == 0:
        print(f"Step {step}: Reward={result.reward:.2f}, "
              f"Neuromod={agent.getNeuromodulationLevel():.3f}, "
              f"Prediction Error={agent.getPredictionError():.3f}")
```

### Curiosity and Exploration

```python
import pynlm

# Create agent with curiosity
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

print("Training agent with curiosity-driven exploration...")

# Curiosity-driven exploration
for step in range(400):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Decode action (curiosity may cause exploration)
    action = agent.decodeMotorCommand()
    
    # Apply action
    result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
              f"Action={action}")
```

## Development

### Developmental Stages

```python
import pynlm

# Create brain and observe development
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableDevelopment(True)

print("Development stages over time:")
print("Step | Stage          | Plasticity | Age")
print("-----|----------------|------------|-----" )

for step in range(0, 2000, 100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)
    
    stage = agent.getDevelopmentalStage()
    stage_str = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"][int(stage)]
    print(f"{step:4d} | {stage_str:16} |    {agent.getNeuromodulationLevel():.3f}    | {agent.getBrain()->getDevelopmentSystem()->getCurrentAge():.1f}")
```

## Advanced Features

### Custom Actions and Movement

```python
import pynlm

# Create brain with enhanced capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)

brain = pynlm.createBrain(config)
brain.initialize()

# Create custom actions
custom_action = pynlm.Action(pynlm.ActionType.Custom)
custom_action.setType(pynlm.ActionType.Custom)
# Add custom parameters
custom_action.getParameters().push_back(1.0f)  # Speed
custom_action.getParameters().push_back(0.5f)  # Turn amount

# Test action system
action = brain.produceAction()
print(f"Produced action: {action.getType()}")
```

### Advanced Configuration

```python
import pynlm

# Create configuration with all advanced settings
config = pynlm.createDefaultConfig()

# Brain configuration
config.set("brain.neuron_count", 10000)
config.set("brain.region_count", 4)
config.set("brain.connection_probability", 0.05)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
config.set("plasticity.structural.pruning_rate", 0.00001)

# Neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development
config.set("development.enable_critical_period", True)
config.set("development.critical_period_duration", 300.0)

# Save configuration
config.saveToFile("advanced_config.json")
print("Advanced configuration saved to advanced_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("advanced_config.json")
```

### Performance Optimization

```python
import pynlm
import time

# Performance measurement
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Time the simulation
start_time = time.time()

for i in range(1000):
    brain.step(i)

end_time = time.time()
elapsed = end_time - start_time

print(f"Simulated {1000} steps in {elapsed:.2f} seconds")
print(f"Speed: {1000/elapsed:.0f} steps/second")

# Memory usage information
if hasattr(brain, 'get_memory_usage') {
    memory_info = brain.get_memory_usage()
    print(f"Memory usage: {memory_info['total_mb']} MB")
}
```

## 🛠️ Troubleshooting

### Common Issues

**Issue 1: "Brain isn't doing anything"**
```python
# Solution 1: Check initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Always call initialize first!

# Solution 2: Run enough steps
for i in range(100, 1000):  # Run more steps
    brain.step(i)
```

**Issue 2: ImportError - No module named 'pynlm'**
```bash
# Fix with pip install
pip install -e .

# Or from source with scikit-build
cd path/to/nlm
pip install scikit-build-core pybind11
python -m pip install .
```

**Issue 3: Segmentation fault**
```python
# Solution: Proper error handling
try {
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### Performance Tips

- Use larger neuron counts for more complex behavior
- Adjust plasticity parameters based on your task
- Enable all neuromodulation systems for richer learning
- Use development for long-term adaptation
- Consider checkpoint/restart for very long simulations

## 📚 API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create a new brain
- `initialize()` - Initialize the brain
- `step(step_number)` - Run a simulation step
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Get motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### Config
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `set(key, value)` - Set configuration value
- `has(key)` - Check if key exists
- `get(key)` - Get configuration value

#### AgentBrain
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode brain activity to action
- `applyRewardModulation(reward, predicted)` - Apply reward signals
- `updateDevelopment(timestep)` - Update developmental state

#### SimpleWorld
- `createSimpleWorld()` - Create a new world
- `configure(width, height, visionWidth, visionHeight)` - Set world parameters
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, time)` - Apply action to world
- `getSensoryPercept()` - Get current sensory percept
- `reset()` - Reset world to initial state

### Enumerations

- `NeuronType`: Excitatory, Inhibitory, Sensory, Motor, Internal, Modulatory
- `ActionType`: MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Eat, Drink, Rest, Wait, Custom
- `MotorCommand`: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
- `WorldObjectType`: Empty, Resource, Hazard, Wall, Marker
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging

### Error Types

- `RuntimeError`: General runtime errors

### Constants

- `INVALID_NEURON_ID`, `INVALID_SYNAPSE_ID`, etc. - Invalid ID constants
- `VISION_DEFAULT_WIDTH`, `VISION_DEFAULT_HEIGHT` - Vision defaults
- `WORLD_DEFAULT_WIDTH`, `WORLD_DEFAULT_HEIGHT` - World defaults

## 📝 Examples

### 1. Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### 2. Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### 3. Complete Agent (Learning and Exploration)

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## 🚀 Advanced Usage

### Batch Processing

```python
import pynlm

def batch_simulation(num_simulations, steps_per_simulation):
    """Run multiple simulations in batch."""
    results = []
    
    for sim in range(num_simulations):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        results.append({
            'simulation': sim,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage()
        })
        
        print(f"Completed simulation {sim + 1}/{num_simulations}")
    
    return results

# Run batch simulations
results = batch_simulation(10, 500)
print(f"Average spikes per simulation: {sum(r['total_spikes'] for r in results) / len(results):.0f}")
```

### Parameter Sweeps

```python
import pynlm

def parameter_sweep(neuron_counts, plasticity_rates):
    """Test different parameter combinations."""
    results = []
    
    for neuron_count in neuron_counts:
        for plasticity_rate in plasticity_rates:
            # Create config with parameters
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            config.set("plasticity.stdp.learning_rate", plasticity_rate)
            
            # Run simulation
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(200):
                brain.step(step)
            
            results.append({
                'neurons': neuron_count,
                'plasticity': plasticity_rate,
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
            
            print(f"Tested: {neuron_count} neurons, {plasticity_rate} plasticity")
    
    return results

# Run parameter sweep
neuron_counts = [500, 1000, 2000]
plasticity_rates = [0.0001, 0.001, 0.01]
results = parameter_sweep(neuron_counts, plasticity_rates)

# Find best parameters
best_result = max(results, key=lambda x: x['firing_rate'])
print(f"Best parameters: {best_result['neurons']} neurons, {best_result['plasticity']} plasticity")
print(f"Result: {best_result['firing_rate']:.3f} firing rate")
```

## 📖 Learning Resources

### For Beginners

1. **Start with the "Silent Brain" example**
2. **Follow the "Brain Watching a World" tutorial**
3. **Try the "Complete Agent" example**
4. **Read the "How to Use" documentation**

### For Intermediate Users

1. **Experiment with memory systems**
2. **Study neuromodulation effects**
3. **Explore developmental stages**
4. **Create custom actions and behaviors**

### For Advanced Users

1. **Implement custom plasticity rules**
2. **Design complex world environments**
3. **Analyze neural dynamics and patterns**
4. **Contribute to the project**

## 🔧 Installation

### From Source (Recommended for Development)

```bash
# Clone the repository
cd /path/to/nlm

# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install -e .

# Or build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/pynlm-0.1.0-py3-none-any.whl
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

## 📋 Building the C++ Project

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

## 🏷️ Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── pyproject.toml
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## 📊 Performance Metrics

### Simulation Speed
- Typical firing rate: 1-10 Hz
- Spike processing: Event-driven (fast)
- Memory usage: Linear with neuron count
- Scalability: Good up to 10,000+ neurons

### Learning Capabilities
- Plasticity: STDP + Hebbian + structural
- Memory: Working + episodic + associative
- Neuromodulation: Dopamine + curiosity + novelty
- Development: Multi-stage plasticity modulation

## 📚 Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Project roadmap

## 📄 License

MIT License

## 👥 Authors

Research project — See docs for scientific background.

## 🔄 Version Control

**Current Phase:** Phase 6: FINAL INTEGRATION

**Phase 6 Integration Achievements:**
- ✅ Memory systems connected to neural processing
- ✅ Neuromodulation affects plasticity and dynamics
- ✅ Prediction integrated with learning
- ✅ Development affects plasticity rates
- ✅ Checkpoint save/load working
- ✅ Replay and consolidation functional
- ✅ Phase 6 integration experiment created

## 🎯 Project Status

NLM is a **research project investigating computational brain-like systems**. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

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

## 🛠️ Additional Resources

### Command Line Interface

```bash
# Check build status
./nlm --help

# Run Phase 6 demo
./nlm_phase6_demo

# Run unit tests
./nlm_test

# Generate documentation
./generate_docs
```

### Python API Utilities

```python
import pynlm

# Print available information
pynlm.print_version()

# Get system information
info = pynlm.get_system_info()
print(info)

# List examples
examples = pynlm.examples()
for name, code in examples.items():
    print(f"\n=== {name} ===")
    print(code)
```

## 🎓 Educational Applications

NLM can be used for:
- **Neuroscience education**: Demonstrating neural dynamics and learning
- **AI research**: Testing brain-inspired learning algorithms
- **Cognitive modeling**: Simulating cognitive processes
- **Robotics**: Developing adaptive control systems
- **Neurology**: Studying brain-inspired computation

## 🚀 Getting Help

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Read the comprehensive documentation
- **Examples**: Run the provided examples
- **Community**: Join discussions and share experiences

## ✨ Acknowledgements

This project builds on decades of research in:
- Computational neuroscience
- Neuromorphic engineering
- Machine learning
- Cognitive modeling
- Artificial intelligence

## 📈 Future Enhancements

### Short Term (1-3 months)
- [ ] Enhanced visualization tools
- [ ] More complex world environments
- [ ] Improved documentation
- [ ] Additional Python examples

### Medium Term (3-6 months)
- [ ] Distributed computing support
- [ ] GPU acceleration
- [ ] Real-time interaction capabilities
- [ ] Web-based interfaces

### Long Term (6-12 months)
- [ ] Cognitive architecture improvements
- [ ] Social learning capabilities
- [ ] Transfer learning capabilities
- [ ] Explainable AI features

---

*For the latest updates, documentation, and community support, visit: https://github.com/nlm-project/nlm*

---

**Happy simulating! 🚀**

This comprehensive documentation provides everything you need to get started with NLM, from basic usage to advanced features, including examples, troubleshooting, and performance tips. Enjoy exploring the world of brain-inspired computation! 🤖🧠