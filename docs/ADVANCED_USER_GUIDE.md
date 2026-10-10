# NLM Phase 6 Advanced User Guide

This guide provides advanced usage examples and documentation for NLM (Neural Learning Machine) Phase 6 integrated artificial brain.

## Overview

NLM Phase 6 represents a complete integration of multiple cognitive systems into a coherent artificial brain architecture. This guide covers:

1. **Advanced Brain Configuration** - Fine-tuning brain parameters for specific applications
2. **Memory System Management** - Working with integrated memory systems
3. **Neuromodulation Control** - Managing learning and exploration
4. **Prediction and Planning** - Advanced predictive capabilities
5. **Concept Discovery** - Automatic concept formation and categorization
6. **Development and Maturation** - Managing brain development lifecycle
7. **Performance Optimization** - Tuning for best performance
8. **Integration with Environments** - Working with world simulations

## Advanced Brain Configuration

### Custom Brain Creation

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 5000)        # More neurons for complex tasks
config.set("brain.synapse_density", 0.15)      # Higher connectivity
config.set("brain.connection_probability", 0.08)

# Set memory system parameters
config.set("memory.working_memory_capacity", 500)
config.set("memory.episodic_memory_max_episodes", 5000)
config.set("memory.associative_memory_strength", 0.8)

# Set learning parameters
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 30.0)
config.set("plasticity.stdp.tau_minus", 30.0)

# Set neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.threshold", 0.2)
config.set("neuromod.novelty.sensitivity", 0.8)

# Set development parameters
config.set("development.initial_plasticity", 1.2)
config.set("development.critical_period_duration", 300.0)
config.set("development.maturation_rate", 0.05)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multi-Region Brain Architecture

```python
import pynlm

# Create multi-region brain for specialized processing
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Add different brain regions for different functions
region_cortex = brain.addRegion("cortex")      # Primary processing
region_hippocampus = brain.addRegion("hippocampus")  # Memory formation
region_amygdala = brain.addRegion("amygdala")  # Emotional processing
region_prefrontal = brain.addRegion("prefrontal")  # Executive function

# Configure region-specific parameters
# (In a real implementation, you'd have region-specific config options)

print(f"Brain has {brain.getRegionCount()} regions")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getTotalNeuronCount()} neurons")
```

## Memory System Management

### Working Memory Operations

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get working memory system
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()} traces")
    
    # Check current memory activity
    active_traces = working_memory.getActiveTraces()
    print(f"Active memory traces: {active_traces}")
    
    # Get memory neurons (for attention and planning)
    memory_neurons = working_memory.getMemoryNeurons()
    print(f"Memory involves {len(memory_neurons)} neurons")
    
    # Monitor memory activity over time
    for step in range(1000):
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            traces = working_memory.getActiveTraces()
            activity = working_memory.getMemoryActivity()
            print(f"Step {step}: {traces} traces, activity level: {activity:.3f}")
    
    # Clear working memory
    working_memory.clear()
    print("Working memory cleared")
```

### Episodic Memory Management

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Enable memory recording
agent.enableDevelopment(True)

# Run simulation to collect episodic data
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Enable episodic memory
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        episode_count = episodic_memory.getEpisodeCount()
        if step % 100 == 0:
            print(f"Episodic memory episodes: {episode_count}")

# Retrieve and analyze episodic memory
if episodic_memory:
    recent_episodes = episodic_memory.getRecentEpisodes(5)
    print(f"Retrieved {len(recent_episodes)} recent episodes")
    
    for i, episode in enumerate(recent_episodes):
        print(f"Episode {i}: timestamp={episode.timestamp}, "
              f"neurons={len(episode.activeNeurons)}, "
              f"reward={episode.reward}")
```

## Neuromodulation Control

### Reward-Based Learning

```python
import pynlm

# Create brain and agent with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

print("Starting reward-based learning simulation...")
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Compute reward (simplified: based on position relative to center)
    body = world.getAgentBody()
    center_distance = ((body.x - 7.5)**2 + (body.y - 7.5)**2)**0.5
    reward = max(0.0, 10.0 - center_distance)  # Closer to center = higher reward
    
    # Apply reward modulation
    predicted_reward = 0.0  # In a real implementation, this would be from prediction system
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor neuromodulation levels
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Reward: {reward:.2f}")
        print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
        print(f"  Development Stage: {agent.getDevelopmentalStage()}")

print("Learning simulation completed!")
```

### Curiosity-Driven Exploration

```python
import pynlm

# Create agent with strong curiosity
agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Enable curiosity-driven exploration
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Monitor curiosity levels
print("Curiosity-driven exploration simulation...")
for step in range(200):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action (will include curiosity-based exploration)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    
    if step % 50 == 0:
        print(f"Step {step}: Curiosity={curiosity_level:.3f}, Novelty={novelty_level:.3f}")
        
        # Curiosity should drive exploration of novel areas
        if curiosity_level > 0.5:
            print(f"  High curiosity - agent exploring novel environments")
```

## Prediction and Planning

### Predictive Processing

```python
import pynlm

# Create brain with prediction system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable prediction system
# (In Phase 6, prediction is integrated into the main brain loop)

# Run simulation to train prediction system
print("Training prediction system...")
for step in range(500):
    brain.step(step, step * 0.001)
    
    if step % 100 == 0:
        # Check prediction system status
        prediction_system = brain.getPredictionSystem()
        if prediction_system:
            error = prediction_system.getPredictionError()
            confidence = prediction_system.getConfidence()
            print(f"Step {step}: Prediction error={error:.3f}, confidence={confidence:.3f}")
```

### Neural Planning

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Run simulation with planning
print("Running simulation with neural planning...")
for step in range(300):
    # Get sensory input
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Decode motor command (includes planning)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check if planning is active
    developmental_stage = agent.getDevelopmentalStage()
    if step % 50 == 0:
        print(f"Step {step}: Planning active in {developmental_stage} stage")
```

## Concept Discovery

### Automatic Concept Formation

```python
import pynlm

# Create brain with concept formation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable concept formation (integrated into Phase 6)
print("Concept formation simulation...")

# Simulate experience with different patterns
concept_patterns = [
    [1.0, 0.0, 0.0, 0.0],  # Pattern 1
    [0.0, 1.0, 0.0, 0.0],  # Pattern 2
    [1.0, 0.0, 1.0, 0.0],  # Pattern 3 (combination)
    [0.0, 1.0, 1.0, 0.0],  # Pattern 4 (combination)
    [1.0, 0.0, 0.0, 1.0],  # Pattern 5
]

for step, pattern in enumerate(concept_patterns * 20):  # Repeat patterns
    brain.step(step, step * 0.001)
    
    # Present experience to concept formation system
    concept_id = brain.getConceptFormation().presentExperience(
        pattern, pattern,  # pattern and features (simplified)
        1.0 if step % 5 == 0 else 0.5,  # varying reward
        step
    )
    
    if step % 25 == 0:
        concept_count = brain.getConceptFormation().getConceptCount()
        print(f"Step {step}: Discovered {concept_count} concepts")

# Retrieve and analyze discovered concepts
concept_formation = brain.getConceptFormation()
concept_count = concept_formation.getConceptCount()
print(f"\nConcept discovery summary:")
print(f"Total concepts discovered: {concept_count}")

for i in range(min(concept_count, 5)):  # Show first 5 concepts
    concept = concept_formation.getConcept(i)
    if concept:
        print(f"Concept {i}: stability={concept.avgStability:.3f}, "
              f"instances={concept.totalObservations}, "
              f"category_hint='{concept.categoryHint}'")
```

## Development and Maturation

### Brain Development Lifecycle

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
config.set("enable_development", True)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

print("Brain development lifecycle simulation...")
print("Initial developmental stage: ", agent.getDevelopmentalStage())

# Simulate development over time
for day in range(1, 31):  # 30 days
    for hour in range(24):  # 24 hours per day
        for minute in range(60):  # 60 minutes per hour
            step = (day - 1) * 1440 + hour * 60 + minute
            timestep = 0.001  # 1ms timestep
            
            # Update brain
            brain.step(step, step * timestep)
            
            # Update agent development
            agent.updateDevelopment(timestep)
            
            # Log developmental stage changes
            if step % 10000 == 0:  # Log every ~7 hours
                stage = agent.getDevelopmentalStage()
                plasticity = agent.getNeuromodulationLevel()
                print(f"Day {day:2d} Hour {hour:2d}: {stage} stage, plasticity={plasticity:.3f}")

# Final developmental stage
print(f"\nFinal developmental stage: {agent.getDevelopmentalStage()}")
print("Development simulation completed!")
```

## Performance Optimization

### Brain Performance Monitoring

```python
import pynlm
import time

# Create optimized brain configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # Balanced size
config.set("enable_development", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Brain performance optimization test...")

# Measure performance
start_time = time.time()
step_count = 0

# Run simulation and measure performance
for step in range(1000):
    brain.step(step, step * 0.001)
    step_count += 1

end_time = time.time()
wall_clock_time = end_time - start_time

# Calculate performance metrics
steps_per_second = step_count / wall_clock_time
spikes_per_step = brain.getTotalSpikeCount() / step_count if step_count > 0 else 0
neurons_per_millisecond = brain.getTotalNeuronCount() / 1.0

print(f"\nPerformance Results:")
print(f"  Steps completed: {step_count}")
print(f"  Wall clock time: {wall_clock_time:.2f} seconds")
print(f"  Steps per second: {steps_per_second:.0f}")
print(f"  Spikes per step: {spikes_per_step:.2f}")
print(f"  Neurons per millisecond: {neurons_per_millisecond:.0f}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Total spike count: {brain.getTotalSpikeCount()}")
```

## Integration with Environments

### Complete Agent-World Integration

```python
import pynlm

print("Complete agent-world integration example...")
print("=" * 50)

# Create all system components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure environment
world.configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8
)

# Initialize agent with world
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Set agent starting position
world.setAgentStart(15.0, 15.0)

# Run complete simulation episode
print("Running simulation episode...")
for step in range(100):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get sensory input from world
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input in agent
    agent.processSensoryInput(percept)
    
    # 4. Run brain step
    brain.step(step)
    
    # 5. Decode motor command from brain activity
    motor_cmd = agent.decodeMotorCommand()
    
    # 6. Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # 7. Apply reward modulation
    reward = 0.0  # Simplified reward function
    agent.applyRewardModulation(reward, 0.0)
    
    # 8. Update development
    agent.updateDevelopment(0.1)
    
    # Print status periodically
    if step % 20 == 0:
        body = world.getAgentBody()
        print(f"Step {step:3d}: Pos=({body.x:5.1f}, {body.y:5.1f}), "
              f"Action={motor_cmd}, Reward={reward:5.2f}, "
              f"Curiosity={agent.getCuriosityLevel():5.2f}")

# Print final statistics
body = world.getAgentBody()
print(f"\nEpisode completed:")
print(f"  Final position: ({body.x:.1f}, {body.y:.1f})")
print(f"  Distance from center: {((body.x-15)**2 + (body.y-15)**2)**0.5:.1f}")
print(f"  Development stage: {agent.getDevelopmentalStage()}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")

# Test checkpoint functionality
print("\nTesting checkpoint functionality...")
checkpoint_file = "./test_checkpoint.bin"
if brain.save(checkpoint_file):
    print(f"Checkpoint saved successfully to {checkpoint_file}")
    
    # Load checkpoint into new brain
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    
    if new_brain.load(checkpoint_file):
        print("Checkpoint loaded successfully")
        print("Integration test completed successfully!")
    else:
        print("Failed to load checkpoint")
else:
    print("Failed to save checkpoint")
```

## Advanced Configuration Examples

### Exploration vs Exploitation Balance

```python
import pynlm

# Configuration for exploration-focused agent
config1 = pynlm.createDefaultConfig()
config1.set("neuromod.curiosity.threshold", 0.1)  # Low threshold = more exploration
config1.set("neuromod.novelty.sensitivity", 1.0)  # High novelty sensitivity
config1.set("plasticity.stdp.learning_rate", 0.005)  # Lower learning rate = more exploration

# Configuration for exploitation-focused agent  
config2 = pynlm.createDefaultConfig()
config2.set("neuromod.curiosity.threshold", 0.8)  # High threshold = less exploration
config2.set("neuromod.novelty.sensitivity", 0.2)  # Low novelty sensitivity
config2.set("plasticity.stdp.learning_rate", 0.02)  # Higher learning rate = more exploitation
```

### Memory-Efficient Configuration

```python
import pynlm

# Configuration optimized for memory efficiency
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)                    # Smaller brain
config.set("memory.working_memory_capacity", 50)   # Smaller working memory
config.set("memory.episodic_memory_max_episodes", 100)  # Limited episodes
config.set("enable_development", False)          # Disable development to save memory
config.set("enable_structural_plasticity", False) # Disable structural plasticity
```

### High-Performance Configuration

```python
import pynlm

# Configuration optimized for performance
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)                  # Larger brain for performance
config.set("enable_development", True)             # Enable development for adaptation
config.set("enable_structural_plasticity", True)  # Enable structural changes
config.set("plasticity.stdp.learning_rate", 0.01) # Aggressive learning
```

## Troubleshooting

### Common Issues and Solutions

1. **Memory Systems Not Working**
   ```
   Symptoms: getWorkingMemory() returns nullptr
   Solution: Ensure brain.initialize() is called before accessing memory systems
   ```

2. **Learning Not Occurring**
   ```
   Symptoms: Neuromodulation levels remain constant
   Solution: Enable reward modulation and provide appropriate reward signals
   ```

3. **Performance Issues**
   ```
   Symptoms: Slow simulation speed
   Solution: Reduce neuron count, disable development, or optimize hardware
   ```

4. **Checkpoint Issues**
   ```
   Symptoms: Checkpoint save/load fails
   Solution: Ensure write permissions and sufficient disk space
   ```

### Debugging Tools

```python
import pynlm

# Create brain and enable debugging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Log detailed brain status
brain.logStatus()

# Monitor specific systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory: {working_memory.getActiveTraces()} traces")
    
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory: {episodic_memory.getEpisodeCount()} episodes")

# Check system health
systems_ok = (
    working_memory != None and episodic_memory != None and
    brain.getPredictionSystem() != None and
    brain.getConceptFormation() != None
)

if systems_ok:
    print("All major systems are operational")
else:
    print("Some systems are not properly initialized")
```

## Summary

NLM Phase 6 provides a comprehensive integrated artificial brain architecture with:

- **Multiple memory systems** for temporary and long-term storage
- **Neuromodulation** for learning and exploration control
- **Prediction and planning** for prospective behavior
- **Concept formation** for automatic categorization
- **Development** for lifespan learning and maturation
- **Performance optimizations** for efficient operation

This advanced user guide provides the tools needed to:

1. Configure and customize the brain for specific applications
2. Monitor and debug system behavior
3. Optimize performance and memory usage
4. Integrate with simulated environments
5. Troubleshoot and maintain the system

The Phase 6 integrated brain represents a complete artificial cognitive system that can learn, adapt, and interact with environments in sophisticated ways.