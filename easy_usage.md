# Advanced NLM Usage Guide

## Advanced Brain Configuration

### Custom Configuration
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set specific parameters
config.set("neuron_count", 5000)
config.set("region_count", 3)
config.set("simulation_timestep", 0.0001)
config.set("random_seed", 12345)
config.set("connection_probability", 0.15)

# Configure plasticity
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("stdp_tau", 20.0)

# Configure memory
config.set("working_memory_capacity", 500)
config.set("episodic_memory_capacity", 1000)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

### Performance Tuning
```python
import pynlm

# Create brain with performance optimizations
config = pynlm.createDefaultConfig()

# Enable SIMD vectorization if available
config.set("use_simd", True)
config.set("simd_width", 16)  # AVX-512

# Configure event queue sizes
config.set("spike_queue_capacity", 32768)
config.set("delayed_queue_capacity", 2048)

# Memory pool configuration
config.set("neuron_pool_block_size", 1024)
config.set("synapse_pool_block_size", 8192)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Advanced Agent Capabilities

### Enhanced Agent Configuration
```python
import pynlm

# Create agent with advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Advanced neuromodulation configuration
agent.enableRewardModulation(True, 0.9)  # Enable with strength
agent.enableCuriosity(True, 0.7)       # Exploration rate
agent.enableNoveltyDetection(True, 0.8)  # Novelty sensitivity
agent.enableStructuralPlasticity(True, 0.01)  # Synaptogenesis rate
agent.enableDevelopment(True, 0.005)    # Developmental speed

# Advanced attention
agent.enableAttention(True, 0.8, 0.6)    # Strength, threshold
agent.setPlanningDepth(5)               # Look-ahead steps
agent.setActionQuality(ActionType::MoveForward, 0.9)
agent.setActionQuality(ActionType::Wait, 0.1)
```

### Memory Management
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Configure memory systems
if hasattr(brain, 'getWorkingMemory'):
    working_memory = brain.getWorkingMemory()
    working_memory.setCapacity(1000)
    working_memory.setDecayRate(0.02)

if hasattr(brain, 'getEpisodicMemory'):
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.setMaxEpisodes(2000)
    episodic_memory.setReplayInterval(50)
    episodic_memory.setConsolidationStrength(0.8)

# Memory statistics
print("Memory usage:")
print(f"  Working memory: {brain.getWorkingMemory() and brain.getWorkingMemory()->getCurrentLoad() or 'N/A'}%")
print(f"  Episodic memory: {brain.getEpisodicMemory() and brain.getEpisodicMemory()->getEpisodeCount() or 'N/A'} episodes")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

## Debugging and Monitoring

### Brain Status Monitoring
```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Monitoring function
def monitor_brain(brain, agent, step):
    print(f"\n=== Step {step} ===")
    
    # Basic statistics
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Memory status
    if brain.getWorkingMemory():
        print(f"Working memory load: {brain.getWorkingMemory()->getCurrentLoad()}%")
    
    # Neuromodulation levels
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine()->getLevel():.3f}")
    
    if agent.getCuriosityLevel():
        print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    if agent.getNoveltyLevel():
        print(f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Developmental stage
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Action statistics
    print(f"Motor neurons active: {brain.getMotorNeuronCount()}")

# Run simulation with monitoring
for step in range(0, 1000, 100):
    for i in range(100):
        brain.step(i + step)
        
        if agent.getCuriosityLevel() > 0.5:
            action = agent.decodeMotorCommand()
            # apply action in world (simplified)
    
    monitor_brain(brain, agent, step + 100)
```

### Event Queue Monitoring
```python
import pynlm

# Create brain with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check event queue status
if hasattr(brain, 'getSpikeSystem'):
    spike_system = brain.getSpikeSystem()
    print("Spike system status:")
    print(f"  Pending spikes: {spike_system.getPendingSpikeCount()}")
    print(f"  Pending delayed: {spike_system.getPendingDelayedCount()}")
    
    # Event queue details (if available)
    if hasattr(spike_system, 'getEventQueue'):
        event_queue = spike_system.getEventQueue()
        if event_queue:
            print(f"  Event queue capacity: {event_queue->capacity()}")
            print(f"  Event queue size: {event_queue->size()}")
    
    # Delayed event queue
    if hasattr(spike_system, 'getDelayedQueue'):
        delayed_queue = spike_system.getDelayedQueue()
        if delayed_queue:
            print(f"  Delayed queue pending: {delayed_queue->pendingCount()}")
```

## Advanced Training and Learning

### Custom Learning Patterns
```python
import pynlm

# Create environment with specific learning challenges
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

# Set up learning scenarios
world.addObject(5, 5, pynlm.WorldObjectType.Resource, 1.0)
world.addObject(15, 15, pynlm.WorldObjectType.Hazard, 1.0)
world.addObject(25, 25, pynlm.WorldObjectType.Resource, 2.0)

# Create agent with specific learning goals
config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("enable_reward_learning", True)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True, 0.8)
agent.enableCuriosity(True, 0.6)
agent.enableStructuralPlasticity(True, 0.005)
agent.enableDevelopment(True, 0.01)

# Custom goal setting
agent.setCurrentGoal([0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5])  # 8D goal space

# Run learning simulation
print("Starting advanced learning simulation...")
for step in range(5000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Get reward based on action
    action_result = world.applyMotorCommand(action, world.getSimulationTime())
    reward = action_result.reward
    
    # Apply reward with prediction error
    predicted_reward = 0.5  # Simple prediction
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Monitor learning progress
    if step % 500 == 0:
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        dopamine = brain.getDopamine() and brain.getDopamine()->getLevel() or 0.0
        print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, Dopamine={dopamine:.3f}")
```

### Memory Replay and Consolidation
```python
import pynlm

# Create agent with enhanced memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure memory systems for replay
if hasattr(brain, 'getEpisodicMemory'):
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.setReplayInterval(100)
    episodic_memory.setConsolidationStrength(0.7)
    episodic_memory.setReplayDepth(3)

# Enable sleep/rest cycle
brain.enableRestMode(True)

# Run simulation with memory replay
print("Running simulation with memory replay...")
for step in range(3000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for sleep/rest trigger
    if step % 500 == 0:
        curiosity = agent.getCuriosityLevel()
        if curiosity < 0.1:  # Low curiosity triggers sleep
            print(f"Sleep/rest triggered at step {step}")
            # System will automatically enter sleep mode
            break

# Analyze memory after simulation
if hasattr(brain, 'getEpisodicMemory'):
    memory = brain.getEpisodicMemory()
    episodes = memory.getAllEpisodes()
    print(f"Total episodes stored: {len(episodes)}")
    
    # Replay important memories
    important_episodes = []
    for episode in episodes:
        if episode.importance > 0.7:
            important_episodes.append(episode)
    
    print(f"Important episodes (importance > 0.7): {len(important_episodes)}")
    
    # Consolidated memory strength
    avg_consolidation = 0.0
    if episodes:
        avg_consolidation = sum(ep->consolidationStrength for ep in episodes) / len(episodes)
    print(f"Average consolidation strength: {avg_consolidation:.3f}")
```

## Python API Reference

### Core Classes

#### Brain
- `brain.initialize()` - Initialize the brain
- `brain.step(step)` - Process one simulation step
- `brain.receiveSensoryInput(input)` - Process sensory data
- `brain.produceAction()` - Generate motor command
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.getTotalNeuronCount()` - Get neuron count
- `brain.getTotalSynapseCount()` - Get synapse count
- `brain.getTotalSpikeCount()` - Get spike count
- `brain.getFiringNeuronCount()` - Get firing neuron count
- `brain.getAverageFiringRate()` - Get average firing rate

#### AgentBrain
- `agent.processSensoryInput(percept)` - Process environmental input
- `agent.decodeMotorCommand()` - Convert neural activity to action
- `agent.applyRewardModulation(reward, predicted)` - Update based on outcomes
- `agent.updateDevelopment(time)` - Update developmental stage
- `agent.getSensoryInputSize()` - Get expected sensory input size
- `agent.getMotorOutputSize()` - Get expected motor output size
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getNeuromodulationLevel()` - Get neuromodulation level
- `agent.getPredictionError()` - Get prediction error

#### Configuration
- `createDefaultConfig()` - Create default configuration
- `config.set(key, value)` - Set configuration parameter
- `config.getOr(key, default)` - Get configuration value with fallback

### Constants
- `NeuronType.Excitatory` - Excitatory neuron
- `NeuronType.Inhibitory` - Inhibitory neuron
- `NeuronType.Sensory` - Sensory neuron
- `NeuronType.Motor` - Motor neuron
- `NeuronType.Internal` - Internal neuron

- `ActionType.MoveForward` - Move forward action
- `ActionType.MoveBackward` - Move backward action
- `ActionType.TurnLeft` - Turn left action
- `ActionType.TurnRight` - Turn right action
- `ActionType.Look` - Look action
- `ActionType.Interact` - Interact action
- `ActionType.Wait` - Wait action

- `WorldObjectType.Empty` - Empty world object
- `WorldObjectType.Resource` - Resource object
- `WorldObjectType.Hazard` - Hazard object
- `WorldObjectType.Wall` - Wall object
- `WorldObjectType.Marker` - Marker object

## Error Handling and Troubleshooting

### Common Issues and Solutions

#### "Brain not initialized"
```python
# Problem: Calling brain methods before initialization
# Solution:
brain = pynlm.createBrain(config)
brain.initialize()  # Initialize first
brain.step(0)       # Then use
```

#### "Agent not properly configured"
```python
# Problem: Using agent without proper initialization
# Solution:
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Initialize agent with world
```

#### "Memory system not available"
```python
# Problem: Accessing memory systems before they're initialized
# Solution:
# Check if memory systems are available
if brain.getWorkingMemory():
    memory = brain.getWorkingMemory()
    # Use memory systems
else:
    print("Working memory not available")
```

#### "Event queue full"
```python
# Problem: Event queue overflow
# Solution:
# Configure larger event queues in config
config.set("spike_queue_capacity", 65536)
config.set("delayed_queue_capacity", 4096)
```

### Performance Tips

#### Memory Usage
- Use smaller neuron counts for testing (`neuron_count`)
- Configure event queue sizes based on expected spike rates
- Monitor memory usage during long simulations

#### Simulation Speed
- Enable SIMD vectorization (`use_simd`)
- Use appropriate timestep for your use case
- Monitor event queue sizes and processing time

#### Learning Performance
- Adjust curiosity and neuromodulation strengths
- Tune plasticity parameters (`stdp_ltp_weight`, `stdp_ltd_weight`)
- Use proper reward and prediction error signals

## Example Projects

### 1. Random Walk Environment
```python
import pynlm

# Simple environment for testing
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=4, visionHeight=4)
world.setAgentStart(10, 10)

config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable basic learning
agent.enableRewardModulation(True)

# Run random walk
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Random walk completed")
```

### 2. Maze Navigation
```python
import pynlm

# Maze environment with goals and obstacles
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)

# Set up maze with walls
for x in range(0, 50, 2):
    world.addObject(x, 25, pynlm.WorldObjectType.Wall, 0.5)
for y in range(0, 50, 2):
    world.addObject(25, y, pynlm.WorldObjectType.Wall, 0.5)

# Goals
world.addObject(10, 10, pynlm.WorldObjectType.Resource, 10.0)
world.addObject(40, 40, pynlm.WorldObjectType.Resource, 10.0)

# Start position
world.setAgentStart(5, 5)

config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable navigation learning
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Navigate maze
for step in range(5000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    if result.success:
        print(f"Step {step}: Moved to ({world.getAgentBody().x}, {world.getAgentBody().y})")

print("Maze navigation completed")
```

### 3. Multi-Agent System
```python
import pynlm

# Create multiple agents in shared environment
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=8, visionHeight=8)

# Place multiple agents
agent1 = pynlm.createAgentBrain(None)  # Will be set after creation
agent2 = pynlm.createAgentBrain(None)
agent3 = pynlm.createAgentBrain(None)

# Create brains and agents
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)

brain1 = pynlm.createBrain(config)
brain1.initialize()
agent1.setBrain(brain1)
agent1.initialize(world)
agent1.setAgentStart(10, 10)

brain2 = pynlm.createBrain(config)
brain2.initialize()
agent2.setBrain(brain2)
agent2.initialize(world)
agent2.setAgentStart(90, 90)

brain3 = pynlm.createBrain(config)
brain3.initialize()
agent3.setBrain(brain3)
agent3.initialize(world)
agent3.setAgentStart(50, 50)

# Enable social learning
agent1.enableSocialLearning(True)
agent2.enableSocialLearning(True)
agent3.enableSocialLearning(True)

# Run multi-agent simulation
for step in range(3000):
    world.update(0.1)
    
    # Each agent senses and acts
    for agent in [agent1, agent2, agent3]:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent_brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 500 == 0:
        print(f"Step {step}: Agents positions")
        for i, agent in enumerate([agent1, agent2, agent3]):
            body = agent.getAgentBody()
            print(f"  Agent {i+1}: ({body.x}, {body.y})")

print("Multi-agent simulation completed")
```

## Troubleshooting Guide

### "Brain not responding"
- Check if brain is initialized
- Verify timestep values
- Monitor spike system status
- Check for infinite loops in simulation

### "Agent not moving"
- Verify agent initialization with world
- Check sensory input processing
- Examine motor command decoding
- Review action application in world

### "Memory not being stored"
- Check if memory systems are initialized
- Verify memory configuration parameters
- Monitor memory usage statistics
- Check for memory system errors

### "Performance issues"
- Reduce simulation timestep for better stability
- Configure smaller event queue sizes
- Monitor memory usage and CPU utilization
- Optimize for single-threaded performance

## Support

For questions, issues, or feature requests:
- Check documentation for common solutions
- Review API reference for method usage
- Test with minimal reproduction examples
- Monitor system logs for error details

---

**Note:** This is an experimental system. Features are continuously improved. Some components may have limited functionality or require specific configuration for optimal performance.
