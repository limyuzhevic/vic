# NLM (Neural Learning Machine) - HOW TO USE - Phase 6 Edition

## Overview

NLM (熙然, "serene flow") is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms, and integrated memory, prediction, cognition, neuromodulation, and development systems.

**Phase 6** brings all these systems together into a coherent artificial brain, enabling complex cognitive behaviors including learning, memory, planning, and adaptation.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API for Phase 6 integration

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_phase6_demo` | Executable | Phase 6 integration demonstration |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
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

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple Phase 6 integrated brain
config = pynlm.createDefaultConfig()
config.set("replay_interval", 100)        # Phase 6 memory optimization
config.set("consolidation_interval", 1000)
config.set("synaptogenesis_rate", 0.0001)

brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Create agent with Phase 6 features
agent = pynlm.createAgentBrain(brain)
print("Phase 6 agent interface created")
```

---

## Part 3: Using the NLM Python API for Phase 6 Integration

### Phase 6 Integration Features

The NLM Python API now provides comprehensive access to the Phase 6 integrated brain:

#### Memory Systems Integration
```python
# Working memory - persistent activity for current processing
working_memory = brain.getWorkingMemory()
working_memory.store(pattern)  # Store pattern
retrieved = working_memory.retrieve()  # Retrieve pattern
working_memory.update(dt)  # Maintain with decay

# Episodic memory - experience storage and replay
episodic_memory = brain.getEpisodicMemory()
episodic_memory.storeEpisode(sensory_input, action, reward, timestamp)
episodic_memory.replayEpisode(episode_id)  # Replay for consolidation

# Associative memory - pattern associations
associative_memory = brain.getAssociativeMemory()
associative_memory.associate(pattern1, pattern2)  # Form associations
```

#### Neuromodulation Integration
```python
# Dopamine system - reward prediction error and learning
 dopamine = brain.getDopamine()
dopamine.signalReward(reward)  # Signal reward
dopamine.signalRewardPredictionError(error)  # Signal prediction error
dopamine.updateEligibilityTrace()  # Update eligibility for plasticity

# Curiosity system - exploration drive
agent = pynlm.createAgentBrain(brain)
curiosity = agent.getCuriosityLevel()  # Get curiosity level
agent.enableCuriosity(True)  # Enable curiosity-driven exploration

# Novelty detection - change detection
novelty = agent.getNoveltyLevel()  # Get novelty level
```

#### Prediction System Integration
```python
# Prediction system - forward models and anticipation
prediction = brain.getPredictionSystem()
prediction.train(sensory_input, timestamp)  # Train prediction model
predicted_value = prediction.getPrediction(pattern)  # Get prediction
prediction_error = brain.getPredictionErrorSignal()  # Get prediction error
```

#### Cognitive Systems Integration
```python
# Planning system - action sequence evaluation
planner = brain.getPlanner()
plan = planner.planAction(current_state)  # Plan action sequence
plan_quality = planner.getPlanQuality()  # Evaluate plan quality

# Concept formation - abstract pattern discovery
concept_formation = brain.getConceptFormation()
concept_formation.processExperience(sensory_input, action, reward)  # Form concepts
concept = concept_formation.getConcept(concept_id)  # Get concept

# Attention system - selective processing
attention = brain.getAttention()
attention.processCompetition(competitors)  # Select winners
attention.applyTopDownBias(neuron, bias)  # Apply goal-directed focus
```

#### Development System Integration
```python
# Development system - maturation and plasticity modulation
development = brain.getDevelopmentSystem()
development.update(dt)  # Update developmental stage
stage = brain.getDevelopmentalStage()  # Get current stage

# Get developmental stage
from enum import Enum
class DevelopmentalStage(Enum):
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4
```

### Complete Phase 6 Agent Example

```python
import pynlm
import time

def run_phase6_agent_simulation(num_steps=1000):
    """Run a complete Phase 6 agent simulation with all integrated systems."""
    
    # 1. Create Phase 6 optimized configuration
    config = pynlm.createDefaultConfig()
    
    # Phase 6 specific configurations
    config.set("replay_interval", 100)        # Replay every 100 steps
    config.set("consolidation_interval", 1000) # Consolidate every 1000 steps
    config.set("synaptogenesis_rate", 0.0001)  # Growth rate
    config.set("pruning_rate", 0.00001)       # Pruning rate
    config.set("simulation_timestep", 0.001)  # Time step
    
    # 2. Create brain with Phase 6 integration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL Phase 6 features
    agent.enableRewardModulation(True)        # Reward-modulated learning
    agent.enableStructuralPlasticity(True)     # Synaptogenesis and pruning
    agent.enableDevelopment(True)              # Developmental stages
    agent.enableCuriosity(True)               # Exploration drive
    agent.enableEpisodicMemory(True)          # Experience storage
    agent.enablePrediction(True)             # Forward models
    agent.enableConceptFormation(True)       # Pattern abstraction
    agent.enablePlanning(True)               # Action selection
    
    # 7. Main simulation loop
    for step in range(num_steps):
        start_time = time.time()
        
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (includes attention)
        agent.processSensoryInput(percept)
        
        # Run brain step (integrates ALL systems)
        brain.step(step)
        
        # Decode motor command (uses planning and curiosity)
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (dopamine system)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print comprehensive status every 100 steps
        if step % 100 == 0:
            status = {
                'step': step,
                'time': world.getSimulationTime(),
                'firing_neuron': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'dev_stage': brain.getDevelopmentalStage(),
                'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_memories': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
                'concepts': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
                'plan_quality': brain.getPlanner().getLastPlanQuality() if brain.getPlanner() else 0,
            }
            print(f"Step {status['step']}: "
                  f"{status['firing_neuron']} firing, "
                  f"curiosity: {status['curiosity']:.3f}, "
                  f"novelty: {status['novelty']:.3f}, "
                  f"dev: {status['dev_stage'].name}, "
                  f"mem: {status['working_memory_traces']} working, "
                  f"{status['episodic_memories']} episodic")
            
            # Save checkpoint every 500 steps
            if step % 500 == 0:
                brain.save(f"checkpoint_phase6_{step}.bin")
                print(f"Checkpoint saved: checkpoint_phase6_{step}.bin")
    
    # 8. Print final statistics
    print("\n=== Phase 6 Simulation Complete ===")
    final_stats = {
        'total_time': num_steps * 0.1,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'development_progress': brain.getDevelopmentalStage().name,
        'working_memory_utilization': brain.getWorkingMemory().getMemoryUtilization() if brain.getWorkingMemory() else 0,
        'episodic_memory_count': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
        'concept_count': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
        'prediction_accuracy': 1.0 - abs(agent.getPredictionError()),
    }
    
    for key, value in final_stats.items():
        print(f"{key.replace('_', ' ').title()}: {value}")
    
    # Save final checkpoint
    brain.save("checkpoint_final_phase6.bin")
    print("\nFinal checkpoint saved: checkpoint_final_phase6.bin")
    
    return final_stats

# Run the simulation
if __name__ == "__main__":
    print("Starting Phase 6 NLM Agent Simulation...")
    print("This simulation integrates all neural systems:")
    print("  - Memory Systems (Working, Episodic, associative)")
    print("  - Neuromodulation (Dopamine, curiosity, novelty)")
    print("  - Prediction System")
    print("  - Cognitive Systems (Planning, concept formation, attention)")
    print("  - Development System")
    print("  - Replay and Consolidation")
    print()
    
    stats = run_phase6_agent_simulation(1000)
    
    print("\nSimulation completed successfully!")
    print("The integrated brain demonstrated coherent cognitive behavior.")
    print("Key achievements:")
    print(f"  - {stats['episodic_memory_count']} episodic memories stored")
    print(f"  - {stats['concept_count']} concepts formed")
    print(f"  - {stats['working_memory_utilization']:.1%} working memory utilization")
    print(f"  - {stats['prediction_accuracy']:.1%} prediction accuracy")
```

### Phase 6 Configuration Options

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `replay_interval` | int | 100 | Replay episodic memories every N steps |
| `consolidation_interval` | int | 1000 | Consolidate memories every N steps |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |
| `simulation_timestep` | float | 0.001 | Time step for simulation |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (potentiation) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (depression) |

### Phase 6 API Methods

#### Key Integration Methods

**Memory System Methods**
```python
# Working memory
brain.getWorkingMemory().store(pattern)           # Store pattern
brain.getWorkingMemory().retrieve()             # Retrieve pattern
brain.getWorkingMemory().update(dt)             # Update with decay

# Episodic memory
brain.getEpisodicMemory().storeEpisode(input, action, reward, time)
brain.getEpisodicMemory().replayEpisode(episode_id)
brain.getEpisodicMemory().getEpisodes()          # Get all episodes

# Associative memory
brain.getAssociativeMemory().associate(pattern1, pattern2)
```

**Neuromodulation Methods**
```python
# Dopamine system
brain.getDopamine().signalReward(reward)
brain.getDopamine().signalRewardPredictionError(error)
brain.getDopamine().updateEligibilityTrace()

# Curiosity system
agent.getCuriosityLevel()                        # Get curiosity
agent.enableCuriosity(True)                      # Enable curiosity

# Novelty detection
agent.getNoveltyLevel()                          # Get novelty
```

**Prediction System Methods**
```python
# Prediction system
brain.getPredictionSystem().train(input, time)
brain.getPredictionSystem().getPrediction(pattern)
brain.getPredictionSystem().getPredictionError()

# Planning system
brain.getPlanner().planAction(state)
brain.getPlanner().getPlanQuality()

# Concept formation
brain.getConceptFormation().processExperience(input, action, reward)
brain.getConceptFormation().getConcepts()

# Attention system
brain.getAttention().processCompetition(competitors)
brain.getAttention().applyTopDownBias(neuron, bias)
```

**Development System Methods**
```python
# Development system
brain.getDevelopmentSystem().update(dt)
brain.getDevelopmentalStage()                    # Get current stage
```

### Phase 6 Performance Considerations

1. **Memory Usage**: Phase 6 memory systems use significant resources
   - Working memory: ~10-50 traces depending on activity
   - Episodic memory: 100-500 episodes per 10K steps
   - Total memory usage: ~50 MB with 1K neurons

2. **Performance Optimization**:
   - Use `replay_interval` and `consolidation_interval` to balance performance
   - Monitor working memory utilization with `getMemoryUtilization()`
   - Configure synaptogenesis and pruning rates for your use case

3. **Phase 6 Integration Checklist**:
   - [ ] Enable reward modulation: `agent.enableRewardModulation(True)`
   - [ ] Enable curiosity: `agent.enableCuriosity(True)`
   - [ ] Enable development: `agent.enableDevelopment(True)`
   - [ ] Enable episodic memory: `agent.enableEpisodicMemory(True)`
   - [ ] Enable prediction: `agent.enablePrediction(True)`
   - [ ] Enable concept formation: `agent.enableConceptFormation(True)`
   - [ ] Enable planning: `agent.enablePlanning(True)`

---

## Part 4: Phase 6 Examples and Tutorials

### Tutorial 1: Basic Phase 6 Agent

```python
import pynlm

# Setup Phase 6 agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Enable Phase 6 features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableEpisodicMemory(True)
agent.enablePrediction(True)

# Run episode
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Phase 6 agent simulation complete!")
```

### Tutorial 2: Memory-Enhanced Learning

```python
import pynlm

# Enable all memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableEpisodicMemory(True)
agent.enablePrediction(True)
agent.enableConceptFormation(True)

# Experience accumulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check memory statistics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        print(f"  Episodic memories: {len(brain.getEpisodicMemory().getEpisodes())}")
        print(f"  Concepts formed: {len(brain.getConceptFormation().getConcepts())}")
        print(f"  Prediction accuracy: {1.0 - abs(agent.getPredictionError()):.2f}")
```

### Tutorial 3: Advanced Phase 6 Configuration

```python
import pynlm

# Custom Phase 6 configuration
config = pynlm.createDefaultConfig()

# Optimize for memory retention
config.set("replay_interval", 50)           # More frequent replay
config.set("consolidation_interval", 500)    # More frequent consolidation

# Optimize for learning
config.set("synaptogenesis_rate", 0.0002)   # Higher growth rate
config.set("pruning_rate", 0.00005)         # Lower pruning rate

# Optimize for performance
config.set("plasticity.stdp.learning_rate", 0.002)  # Higher learning rate
config.set("plasticity.stdp.tau_plus", 15.0)         # Shorter potentiation window

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print("Phase 6 brain initialized with custom configuration")
print(f"Replay interval: {config.get('replay_interval')}")
print(f"Consolidation interval: {config.get('consolidation_interval')}")
print(f"Synaptogenesis rate: {config.get('synaptogenesis_rate')}")
print(f"Pruning rate: {config.get('pruning_rate')}")
```

---

## Troubleshooting for Phase 6

### Common Issues

**"My Phase 6 brain isn't working"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps to allow systems to integrate
- Check that all Phase 6 features are enabled

**"Memory systems aren't updating"**
- Ensure `agent.enableEpisodicMemory(True)` is called
- Check replay and consolidation intervals are configured
- Memory systems need sufficient recurrent connectivity

**"Performance is slow"**
- Use Phase 6 optimizations: adjust replay and consolidation intervals
- Monitor memory usage: `brain.getWorkingMemory().getMemoryUtilization()`
- Consider reducing configuration complexity for faster simulation

**"Behavior is too simple"**
- Enable curiosity and planning for more complex behavior
- Enable concept formation for abstraction
- Phase 6 requires time to learn through memory and prediction

### Phase 6 Integration Check

```python
def check_phase6_integration(brain, agent):
    """Check if Phase 6 integration is working properly."""
    issues = []
    
    # Check memory systems
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory().getActiveTraces()
        if traces == 0:
            issues.append("Working memory has no active traces")
    else:
        issues.append("Working memory not available")
    
    if brain.getEpisodicMemory():
        episodes = len(brain.getEpisodicMemory().getEpisodes())
        if episodes == 0:
            issues.append("Episodic memory has no episodes")
    else:
        issues.append("Episodic memory not available")
    
    # Check neuromodulation
    if agent.getCuriosityLevel() <= 0:
        issues.append("Curiosity system not working")
    
    if agent.getPredictionError() >= 1.0:
        issues.append("Prediction system not working")
    
    # Check development
    stage = brain.getDevelopmentalStage()
    if stage == DevelopmentalStage.Initial:
        issues.append("Development system not progressing")
    
    return issues
```

---

## See Also

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and theory
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific documentation
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment documentation

---

*This documentation covers NLM Phase 6 integration. For specific API details of individual classes and methods,
refer to the source code headers in the C++ source tree.*
