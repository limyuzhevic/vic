# NLM (Neural Learning Machine) - Enhanced Documentation

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 that provides comprehensive neural computation capabilities through multiple integrated systems:

### Key Features:

1. **🧠 Complete Neural Brain System**
   - 100% C++20 implementation with event-driven spiking dynamics
   - Leaky Integrate-and-Fire (LIF) neurons with biological realism
   - Synaptic plasticity mechanisms (STDP, Hebbian, Structural)
   - Multi-region brain architecture with interconnected regions

2. **💾 Integrated Memory Systems**
   - **Neural Working Memory**: Persistent activity using recurrent connections
   - **Neural Episodic Memory**: Experience storage with temporal ordering and replay
   - **Neural Associative Memory**: Pattern associations and spreading activation
   - **Semantic Memory**: Conceptual knowledge and distributed representations
   - **Procedural Memory**: Skills and behavioral patterns

3. **🧠 Neuromodulation Systems**
   - **Dopamine**: Reward prediction and reinforcement learning
   - **Curiosity**: Exploration motivation and intrinsic reward
   - **Novelty**: Novelty detection and change prediction
   - **Prediction Error**: Bayesian prediction error signals
   - **Acetylcholine/Norepinephrine/Serotonin**: Advanced neuromodulation

4. **🧠 Brain Development Systems**
   - **Development**: Critical period dynamics and maturation
   - **Maturation**: Neural connectivity refinement
   - **Synaptogenesis**: Synapse formation and pruning
   - **Pruning**: Activity-dependent synapse elimination

5. **🧠 Cognition Systems**
   - **Neural Planner**: Action planning with hierarchical goals
   - **Concept Formation**: Pattern discovery and abstraction
   - **Attentional Selection**: Neural attention through competition

6. **🧠 Prediction and Learning Systems**
   - **PredictionSystem**: Forward models and error computation
   - **NeuralPrediction**: Internal model-based prediction
   - **Full plasticity suite**: STDP, Hebbian, Structural

7. **🧠 Sensory and Motor Systems**
   - **Complete sensory modalities**: Vision, Audio, Proprioception, Internal signals
   - **Action system**: Comprehensive motor output with full action space
   - **World interaction**: Complete embodied agent-environment interaction

## Python Bindings (pynlm)

### Overview

The enhanced Python bindings provide comprehensive access to all NLM systems through an intuitive Python API. Key improvements include:

### What Changed in Enhanced Version:

1. **🔧 Complete System Coverage**
   - Added all previously missing memory systems (SemanticMemory, ProceduralMemory)
   - Added all neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError, Reward)
   - Added all development systems (DevelopmentSystem, Maturation, Synaptogenesis, Pruning)
   - Added all cognition systems (NeuralPlanner, ConceptFormation, AttentionalSelection)
   - Added prediction systems (PredictionSystem, NeuralPrediction)
   - Added plasticity systems (STDP, Hebbian, StructuralPlasticity)
   - Added dynamics systems (NeuralDynamics, SpikeSystem)

2. **🐍 Enhanced Python API**
   - More Pythonic error handling with custom exception classes
   - Better documentation strings with comprehensive docstrings
   - Improved type conversions and parameter validation
   - Enhanced helper functions and example simulations

3. **📚 Comprehensive Documentation**
   - Detailed docstrings for all classes and methods
   - Step-by-step examples for complex operations
   - Mathematical explanations of algorithms
   - Best practices and performance recommendations

## Quick Start Examples

### 1. Basic Brain Simulation

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getRegionCount()} regions")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 2. Complete Agent Simulation

```python
import pynlm

def run_agent_simulation():
    # Setup
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
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(500):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

### 3. Memory System Example

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)

# Create world and configure for demonstration
world = pynlm.createSimpleWorld()
world.configure(15, 15, 6, 6)
world.reset()
agent.initialize(world)

print("=== Memory System Demo ===")

# Step 1: Store a sensory pattern in working memory
sensory_pattern = [0.1, 0.5, 0.8, 0.3, 0.9]
brain.getWorkingMemory().store(sensory_pattern, 1.0)
print("Stored sensory pattern in working memory")

# Step 2: Simulate to allow memory consolidation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)

# Step 3: Retrieve and examine memory
retrieved_pattern = brain.getWorkingMemory().retrieve()
print(f"Retrieved pattern: {retrieved_pattern}")
print(f"Memory activity level: {brain.getWorkingMemory().getMemoryActivity():.3f}")

# Step 4: Store an episodic memory
episode = pynlm.EpisodicMemoryItem()
episode.sensoryState = [0.2, 0.7, 0.4, 0.6, 0.1]
episode.reward = 1.0
episode.action = pynlm.ActionType.MoveForward
episode.age = 10

brain.getEpisodicMemory().storeEpisode(episode)
print(f"Stored episodic memory (total episodes: {brain.getEpisodicMemory().getEpisodeCount()})")

# Step 5: Retrieve similar episodes
retrieved_episodes = brain.getEpisodicMemory().retrieveSimilar(sensory_pattern, 3)
print(f"Found {len(retrieved_episodes)} similar episodes")

# Step 6: Demonstrate semantic memory
brain.getSemanticMemory().storeConcept("food", [0.8, 0.6, 0.4, 0.2, 0.1], "Edible items")
brain.getSemanticMemory().storeConcept("water", [0.9, 0.3, 0.7, 0.1, 0.5], "Drinkable items")

print(f"Semantic memory concepts: {brain.getSemanticMemory().getConceptNames()}")
print(f"Food concept activation: {brain.getSemanticMemory().getConceptActivation('food'):.3f}")

# Step 7: Demonstrate procedural memory
skill_pattern = [1, 2, 3, 4, 5, 6]  # Example neural pattern
brain.getProceduralMemory().learnSkill("walk", skill_pattern, 0.8)
print(f"Learned skill 'walk' with proficiency: 0.8")

executed_pattern = brain.getProceduralMemory().executeSkill("walk")
print(f"Executed skill pattern: {executed_pattern}")

print("=== Memory Demo Complete ===")
```

### 4. Neuromodulation Example

```python
import pynlm

# Create simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.initialize(world)

print("=== Neuromodulation Demo ===")

# Simulate exploration with curiosity
for step in range(200):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Print neuromodulation status
    if step % 40 == 0:
        print(f"Step {step}:")
        print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
        print(f"  Reward Signal: {reward:.3f}")

print("=== Neuromodulation Demo Complete ===")
```

### 5. Development System Example

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)
agent.initialize(None)  # No world needed for development demo

print("=== Development System Demo ===")

# Run development phases
for step in range(3000):  # Simulate 3000 steps of development
    brain.step(step)
    agent.updateDevelopment(0.1)  # Update development system
    
    if step % 500 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Developmental stage = {stage}")
        
        # Show developmental effects
        plasticity_factor = agent.getNeuromodulationLevel()
        print(f"  Plasticity factor: {plasticity_factor:.3f}")
        print(f"  Average proficiency: {brain.getProceduralMemory().getAverageProficiency():.3f}")
        
        # Show region connectivity changes
        for i, region in enumerate(brain.getRegions()):
            if i < 2:  # Show first 2 regions
                neuron_count = region.getTotalNeuronCount()
                synapse_count = region.getSynapseCount()
                print(f"  Region {i+1}: {neuron_count} neurons, {synapse_count} synapses")

print("=== Development Demo Complete ===")
```

## Advanced Usage Examples

### 1. Memory Replay and Consolidation

```python
import pynlm

# Create simulation with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable memory subsystems
agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)
agent.initialize(None)

# Simulate to create memories
for step in range(500):
    brain.step(step)
    agent.updateDevelopment(0.1)

print("=== Memory Replay Demo ===")
print(f"Total episodic memories: {brain.getEpisodicMemory().getEpisodeCount()}")

# Get episodes for replay (relevance-based selection)
episodes_to_replay = brain.getEpisodicMemory().getEpisodesForReplay(3)
print(f"Episodes selected for replay: {len(episodes_to_replay)}")

# Replay episodes for consolidation
for episode in episodes_to_replay:
    brain.getEpisodicMemory().replayEpisode(episode)
    print(f"Replayed episode with reward: {episode.reward:.3f}")

# Show replay effects
print("Post-replay statistics:")
print(f"  Memory activity: {brain.getWorkingMemory().getMemoryActivity():.3f}")
print(f"  Average reward: {brain.getEpisodicMemory().getAverageReward():.3f}")
```

### 2. Cognitive Architecture Integration

```python
import pynlm

# Create full cognitive architecture
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all subsystems
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

world = pynlm.createSimpleWorld()
world.configure(12, 12, 6, 6)
world.reset()
agent.initialize(world)

print("=== Cognitive Architecture Demo ===")

# Run cognitive task simulation
for step in range(400):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes input
    brain.step(step)
    
    # Plan action using cognitive systems
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = 0.0
    if agent.getNoveltyLevel() > 0.5:
        reward += 0.5  # Reward for novel experiences
    if agent.getCuriosityLevel() > 0.3:
        reward += 0.3  # Reward for exploration
    
    agent.applyRewardModulation(reward, agent.getPredictionError())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 80 == 0:
        print(f"Step {step}:")
        print(f"  Attention winners: {len(agent.getAttention().getWinners())}")
        print(f"  Planned actions: {len(brain.getPlanner().getPlan())}")
        print(f"  Current concepts: {brain.getConceptFormation().getConceptsCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("=== Cognitive Architecture Complete ===")
```

## Performance Tips

### 1. Memory Management
```python
# For long-running simulations
brain = pynlm.createBrain(config)

# Save memory frequently
if step % 1000 == 0:
    brain.save(f"checkpoint_{step}.bin")

# Load previous state if available
if os.path.exists("checkpoint_500.bin"):
    brain.load("checkpoint_500.bin")
```

### 2. Subsystems Optimization
```python
# Enable only needed subsystems
agent.enableRewardModulation(True)      # For reinforcement learning
agent.enableCuriosity(True)              # For exploration
# agent.enableDevelopment(False)        # Skip development for speed

# Adjust configuration for performance
config.set("brain.neuron_count", 500)   # Smaller brain for faster simulation
config.set("plasticity.stdp.enable", False)  # Disable if not needed
```

### 3. Monitoring and Debugging
```python
# Enable comprehensive logging
brain.logStatus()

# Monitor specific metrics
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Working memory usage: {brain.getWorkingMemory().getMemoryActivity():.3f}")
print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

## API Reference

### Core Classes

#### Brain
- **Attributes**: Total neurons, synapses, regions, development stage
- **Methods**: step(), initialize(), receiveSensoryInput(), save(), load(), etc.

#### AgentBrain
- **Attributes**: Neuromodulation levels, curiosity, novelty, prediction error
- **Methods**: processSensoryInput(), decodeMotorCommand(), applyRewardModulation(), etc.

#### SimpleWorld
- **Attributes**: Dimensions, energy, simulation time
- **Methods**: update(), applyMotorCommand(), getSensoryPercept(), etc.

#### Configuration
- **Attributes**: Key-value pairs for all system parameters
- **Methods**: loadFromFile(), saveToFile(), has(), getKeys(), etc.

### Memory Systems

#### NeuralWorkingMemory
- **Methods**: store(), retrieve(), storeToNeuron(), update(), getMemoryActivity()

#### NeuralEpisodicMemory
- **Methods**: storeEpisode(), retrieveSimilar(), replayEpisode(), consolidate()

#### SemanticMemory
- **Methods**: storeConcept(), retrieveConcept(), findSimilarConcepts(), explainConcept()

#### ProceduralMemory
- **Methods**: learnSkill(), executeSkill(), practiceSkill(), transferSkill()

### Neuromodulation Systems

#### Dopamine
- **Methods**: signalReward(), signalRewardPredictionError()

#### Curiosity
- **Methods**: increaseCuriosity(), decreaseCuriosity(), getCuriosityLevel()

#### Novelty
- **Methods**: detectNovelty(), getNoveltyLevel()

#### PredictionError
- **Methods**: setError(), getError(), resetError()

### Development Systems

#### DevelopmentSystem
- **Methods**: update(), isDeveloping()

#### Maturation
- **Methods**: update(), isMature()

#### Synaptogenesis
- **Methods**: update(), getFormationRate()

#### Pruning
- **Methods**: update(), getPruningRate()

### Cognition Systems

#### NeuralPlanner
- **Methods**: initialize(), planAction(), getPlan()

#### ConceptFormation
- **Methods**: initialize(), update(), getConcepts()

#### AttentionalSelection
- **Methods**: processCompetition(), focusOnRegion(), releaseAttention()

## Installation and Setup

### Python Package Installation

```bash
# Install from source
pip install -e .

# Or install with development dependencies
pip install -e .[dev]
```

### Build Dependencies

```bash
# Required system packages
sudo apt-get update
sudo apt-get install -y cmake build-essential python3-dev

# Python dependencies
pip install pybind11 scikit-build-core numpy pytest
```

### Building and Testing

```bash
# Build the Python extension
python -m pip install --no-build-isolation .

# Run tests
pytest tests/

# Run example
python examples/simple_example.py
```

## Troubleshooting

### Common Issues and Solutions

**Issue: ModuleNotFoundError: No module named 'pynlm'**  
- **Solution**: Ensure the Python module is installed with `pip install -e .`

**Issue: ImportError: libnlm_core.so: cannot open shared object file**  
- **Solution**: Ensure shared libraries are in LD_LIBRARY_PATH

**Issue: Segmentation fault in simulation**  
- **Solution**: Check for null pointer dereferences, increase stack size, or run with debugger

**Issue: Memory usage too high**  
- **Solution**: Reduce brain size, disable unnecessary subsystems, or use checkpointing

**Issue: Slow simulation speed**  
- **Solution**: Reduce timestep, disable plasticity, or optimize configuration

## Additional Resources

### Documentation
- **Main Documentation**: HOW_TO_USE.md
- **Architecture**: docs/ARCHITECTURE.md
- **Science Background**: docs/SCIENCE.md
- **Phase Descriptions**: docs/PHASE*_EXPERIMENTS.md

### Examples
- **Simple Examples**: examples/
- **Complex Tasks**: examples/complex/
- **Performance Tests**: tests/performance/

### Research Papers
- **NLM Publications**: Refer to documentation for citation information
- **Phase 1-6 Papers**: Available in research archives

## Acknowledgements

This enhanced NLM implementation builds upon:

1. **Original NLM Team**: For the foundational C++20 neural simulation framework
2. **Pybind11 Contributors**: For the Python binding infrastructure
3. **Scientific Community**: For inspiration from biological neural systems
4. **Research Collaborators**: For feedback and validation

## License

See LICENSE file for detailed license information.

---

*This enhanced documentation reflects the complete NLM system with all memory, neuromodulation, cognition, and development subsystems fully integrated and accessible through comprehensive Python bindings.*