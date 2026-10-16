# NLM Python API Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API, which provides access to the C++ neural simulation framework through Python bindings.

## Overview

The NLM Python API enables you to:
- Create and simulate artificial neural brains
- Configure brain parameters and behavior
- Run integration experiments and tests
- Monitor performance and statistics
- Save/load brain states
- Use advanced features for research and demonstrations

## Core Classes

### Brain

The main class representing the neural simulation brain.

#### Creation and Initialization

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Basic Simulation

```python
# Run a simple simulation
for step in range(1000):
    brain.step(step)
    
    # Optional: Process sensory input or get motor commands
    # percept = world.getSensoryPercept()
    # agent.processSensoryInput(percept)
    # action = agent.decodeMotorCommand()
```

#### Accessing Statistics

```python
# Get basic statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Config

Configuration class for setting brain parameters.

#### Basic Configuration

```python
config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.1f)

# Load from file
config.loadFromFile("my_config.json")

# Save to file
config.saveToFile("my_config.json")
```

#### Available Configuration Parameters

- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Probability of connections between neurons (default: 0.1)
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.012)
- `stdp_tau`: STDP time constant (default: 20.0)
- `synaptogenesis_rate`: Structural plasticity rate (default: 0.0001)
- `pruning_rate`: Structural plasticity rate (default: 0.00001)
- `simulation_timestep`: Simulation timestep (default: 0.001)

### SimpleWorld

2D world simulation for agent-environment interaction.

#### Basic Usage

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)

# Get agent percept
percept = world.getSensoryPercept()

# Get agent state
body = world.getSensoryPercept()

# Apply motor command
agent.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

### AgentBrain

Interface connecting the brain to the world for sensory processing and motor control.

#### Basic Usage

```python
# Create agent with brain
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete agent loop
for step in range(500):
    # 1. Get sensory input from world
    percept = world.getSensoryPercept()
    
    # 2. Process sensory input
    agent.processSensoryInput(percept)
    
    # 3. Brain thinks
    brain.step(step)
    
    # 4. Get action from brain
    motor_cmd = agent.decodeMotorCommand()
    
    # 5. Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

## Advanced Features

### Memory Systems

Access to integrated memory systems (Phase 6 integration).

```python
# Get working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Active traces: {working_memory.getActiveTraces()}")
    print(f"Memory neurons: {working_memory.getMemoryNeurons()}")

# Get episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodes stored: {episodic_memory.getEpisodeCount()}")

# Get associative memory
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    # Use associative memory methods
```

### Prediction System

Access to prediction and sequence learning.

```python
prediction_system = brain.getPredictionSystem()
if prediction_system:
    # Process predictions and errors
    # Get prediction confidence
```

### Cognition Systems

Access to higher cognitive functions.

```python
# Neural planner for action planning
planner = brain.getPlanner()
if planner:
    # Use planning methods

# Concept formation for pattern discovery
concept_formation = brain.getConceptFormation()
if concept_formation:
    # Process experiences to form concepts

# Attentional selection
attention = brain.getAttention()
if attention:
    # Process competition between neural populations

# Development system
development = brain.getDevelopmentSystem()
if development:
    # Update development stage
```

### Neuromodulation Systems

Access to neuromodulation systems for learning and behavior.

```python
# Dopamine system (reward)
dopamine = brain.getDopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.getLevel():.3f}")

# Curiosity system (exploration)
curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getCuriosityLevel():.3f}")

# Novelty detection
novelty = brain.getNovelty()
if novelty:
    print(f"Novelty level: {novelty.getNoveltyLevel():.3f}")

# Prediction error signal
pred_error = brain.getPredictionErrorSignal()
if pred_error:
    print(f"Prediction error: {pred_error.getPredictionError():.3f}")
```

### Performance Systems

Access to plasticity and performance systems.

```python
# STDP plasticity
stdp = brain.getSTDP()
if stdp:
    # Configure STDP parameters

# Hebbian plasticity
hebbian = brain.getHebbian()
if hebbian:
    # Apply Hebbian learning

# Structural plasticity
structural = brain.getStructuralPlasticity()
if structural:
    print(f"Synaptogenesis rate: {structural.getSynaptogenesisRate():.6f}")
    print(f"Pruning rate: {structural.getPruningRate():.6f}")

# Spike system
spike_system = brain.getSpikeSystem()
if spike_system:
    print(f"Pending spikes: {spike_system.getPendingSpikeCount()}")
    print(f"Pending delayed spikes: {spike_system.getPendingDelayedCount()}")
```

## Experiment Helpers

### Running Integration Experiments

```python
# Run a complete integration experiment
def run_integration_demo():
    result = pynlm.run_integration_experiment(
        neuronCount=500,
        maxSteps=2000
    )
    
    print("=== Integration Results ===")
    print(f"Total reward: {result['totalReward']:.3f}")
    print(f"Average firing rate: {result['avgFiringRate']:.3f}")
    print(f"Episodes stored: {result['memoryEpisodesStored']:.0f}")
    print(f"Dopamine level: {result['dopamineLevel']:.3f}")
    print(f"Curiosity level: {result['curiosityLevel']:.3f}")
    print(f"Wall time: {result['totalWallClockTime']:.2f}s")
    
    print("\n=== Integration Status ===")
    print(f"Working Memory: {'CONNECTED' if result['memoryWorkingMemoryIntegrated'] else 'DISCONNECTED'}")
    print(f"Episodic Memory: {'CONNECTED' if result['memoryEpisodicMemoryIntegrated'] else 'DISCONNECTED'}")
    print(f"Neuromodulation: {'CONNECTED' if result['neuromodulationIntegrated'] else 'DISCONNECTED'}")
    print(f"Prediction: {'CONNECTED' if result['predictionIntegrated'] else 'DISCONNECTED'}")
    print(f"Development: {'CONNECTED' if result['developmentIntegrated'] else 'DISCONNECTED'}")
    print(f"Checkpointing: {'WORKING' if result['checkpointingWorks'] else 'NOT WORKING'}")

# Run verification
integration_ok = pynlm.verify_integration()
print(f"Integration verification: {'PASS' if integration_ok else 'FAIL'}")
```

### Configuration Presets

```python
# Create a research configuration (comprehensive)
research_config = pynlm.create_config_for_simulation(
    enableLearning=True,
    enableDevelopment=True,
    enableCuriosity=True,
    enableCheckpointing=True
)

# Create a demo configuration (for presentations)
demo_config = pynlm.create_config_for_simulation(
    enableLearning=True,
    enableDevelopment=True,
    enableCuriosity=True,
    enableCheckpointing=False
)

# Create a minimal configuration (fast testing)
minimal_config = pynlm.create_config_for_simulation(
    enableLearning=False,
    enableDevelopment=False,
    enableCuriosity=False,
    enableCheckpointing=False
)

# Standard presets
research_config = pynlm.create_research_config()
demo_config = pynlm.create_demo_config()
minimal_config = pynlm.create_minimal_config()
```

### Performance Monitoring

```python
# Get detailed performance statistics
stats = pynlm.get_performance_stats(brain)
print(stats)

# Get system health
health = pynlm.check_system_health()
print(health)

# Get simulation recommendations
recommendations = pynlm.get_simulation_recommendations("research")
print(recommendations)
```

### Comparative Experiments

```python
# Create different configurations for comparison
configs = [
    pynlm.create_minimal_config(),
    pynlm.create_demo_config(),
    pynlm.create_research_config()
]

# Run comparative experiment
results = pynlm.run_comparative_experiment(configs, steps=1000)

for i, (neuron_count, reward, firing_rate, episodes, dopamine, curiosity) in enumerate(results):
    print(f"Config {i+1} (neurons={neuron_count}):")
    print(f"  Reward: {reward:.3f}, Firing rate: {firing_rate:.3f}")
    print(f"  Episodes: {episodes:.0f}, Dopamine: {dopamine:.3f}, Curiosity: {curiosity:.3f}")
```

### Demo Helper

```python
# Run a quick demo and get key metrics
metrics = pynlm.run_demo_simulation(steps=500)
total_reward, avg_firing_rate, episodes_stored, dopamine, curiosity, wall_time = metrics

print(f"Demo Results:")
print(f"  Total reward: {total_reward:.3f}")
print(f"  Average firing rate: {avg_firing_rate:.3f}")
print(f"  Episodes stored: {episodes_stored:.0f}")
print(f"  Dopamine level: {dopamine:.3f}")
print(f"  Curiosity level: {curiosity:.3f}")
print(f"  Wall time: {wall_time:.2f}s")
```

## Error Handling

The Python bindings include comprehensive error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
        
except RuntimeError as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Best Practices

### 1. Always Initialize Before Use

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Never skip this!
```

### 2. Use Appropriate Configurations

```python
# For research: comprehensive setup
config = pynlm.create_research_config()

# For demos: balanced setup
config = pynlm.create_demo_config()

# For testing: minimal setup
config = pynlm.create_minimal_config()
```

### 3. Monitor System Health

```python
# Verify integration before starting complex simulations
if not pynlm.verify_integration():
    print("ERROR: System not properly integrated!")
    exit(1)
```

### 4. Use Checkpoints for Long Simulations

```python
# Enable checkpointing for simulations longer than 1000 steps
config = pynlm.create_config_for_simulation(
    enableLearning=True,
    enableDevelopment=True,
    enableCheckpointing=True
)
```

### 5. Handle Memory Usage

```python
# Monitor performance for large simulations
stats = pynlm.get_performance_stats(brain)
print(stats)

# Adjust configuration if needed
if "Memory usage: " in stats and "exceeds" in stats:
    config.set("neuron_count", config.get("neuron_count") // 2)
```

## Examples

### Complete Brain Simulation with World Interaction

```python
def run_complete_simulation():
    # Create configuration and brain
    config = pynlm.create_research_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(2000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 500 == 0:
            stats = pynlm.get_performance_stats(brain)
            print(f"Step {step}: {stats}")
    
    # Save final state
    brain.save("final_checkpoint.bin")
    print("Simulation completed successfully!")
    
    return brain
```

### Memory System Test

```python
def test_memory_systems():
    config = pynlm.create_demo_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to generate experiences
    for step in range(1000):
        brain.step(step)
        
        # Inject some varied input to stimulate memory formation
        if step % 100 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, float(step) * 10.0)
    
    # Check memory systems
    print("=== Memory System Test ===")
    
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"Working memory active traces: {working_memory.getActiveTraces()}")
        print(f"Working memory neurons: {len(working_memory.getMemoryNeurons())}")
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
        print(f"Episodic memory memory episodes: {episodic_memory.getReplayableCount()}")
    
    return brain
```

### Neuromodulation Experiment

```python
def test_neuromodulation():
    config = pynlm.create_research_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run neuromodulation experiment
    total_reward = 0.0
    curiosity_sum = 0.0
    novelty_sum = 0.0
    
    for step in range(1000):
        # Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward
        reward = 1.0 if step % 100 == 0 else 0.0
        total_reward += reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect neuromodulation statistics
        if step % 100 == 0:
            curiosity_sum += agent.getCuriosityLevel()
            novelty_sum += agent.getNoveltyLevel()
    
    print("=== Neuromodulation Results ===")
    print(f"Total reward: {total_reward:.1f}")
    print(f"Average curiosity: {curiosity_sum / 10:.3f}")
    print(f"Average novelty: {novelty_sum / 10:.3f}")
    print(f"Final dopamine level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain
```

## Installation

For installation instructions, see the README.md file in the project root.

## Version Information

This is the Phase 6 integrated version of the NLM Python API, providing access to all major brain systems:
- Neural core (LIF neurons, spikes, synapses)
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Cognition (planning, concept formation, attention, development)
- Prediction systems
- Performance infrastructure (memory pools, event queues, checkpoints)
- Embodied agents with world interaction

## Future Development

The NLM project continues to evolve. Future versions will include:
- More advanced sensory modalities (audio, proprioception)
- Enhanced motor control
- Social learning and imitation
- Memory consolidation and sleep cycles
- Enhanced visualization tools
- Multi-agent interactions
- Real-time control interfaces

## Support

For questions, issues, or feature requests, please visit the NLM project repository or documentation.