# NLM - Comprehensive Usage Guide

## Overview

NLM (Neural Learning Machine) is a computational brain simulator that enables creating virtual brains that can learn, remember things, and make decisions. It provides a complete neuroscience-inspired architecture with neurons, synapses, memory systems, and neuromodulation.

## Quick Start

### Installation
```bash
# Install the package
cmake --build build
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run brain for 100 simulation steps
for step in range(100):
    brain.step(step)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Core Components

### 1. Brain
The central neural processor with spiking neurons, synapses, and all cognitive subsystems.

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Essential brain functions
brain.step(step_number)  # Process one simulation step
brain.receiveSensoryInput(percept)  # Inject sensory data
brain.produceAction()  # Generate motor output
brain.save("brain_state.pkl")  # Save brain state
brain.load("brain_state.pkl")  # Load brain state
```

### 2. World
The environment where the brain interacts, providing sensory input and consequences.

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# World operations
world.update(0.1)  # Advance simulation time
percept = world.getSensoryPercept()  # What the brain perceives
agent_body = world.getAgentBody()  # Physical state
world.applyMotorCommand(action, time)  # Execute action
world.addObject(obj)  # Add object to world
```

### 3. AgentBrain
Connects brain to world, handling sensory transduction and motor decoding.

```python
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Agent operations
agent.processSensoryInput(percept)  # Convert sensory data
agent.decodeMotorCommand()  # Convert neural activity to action
agent.applyRewardModulation(reward, predicted)  # Learning from rewards
```

## Advanced Usage Patterns

### Pattern 1: Complete Agent Simulation

```python
def run_agent_simulation(config_file=None, steps=1000):
    # Setup configuration
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    # Create all components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow/shrink connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Drive exploration
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent perceives
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and provide learning feedback
        reward = world.getLastActionResult().reward
        agent.applyRewardModulation(reward, 0.0)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'final_stats': {
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'neurons_firing': brain.getFiringNeuronCount(),
            'memory_usage': brain.getWorkingMemory().getUsage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
    }
```

### Pattern 2: Brain Development Study

```python
def study_development(config_file=None, duration_minutes=10):
    """Study brain development over time"""
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Track developmental changes
    development_data = []
    
    for step in range(int(duration_minutes * 10)):  # 10 steps per minute
        # Record state before step
        state = {
            'step': step,
            'developmental_stage': brain.getDevelopmentalStage(),
            'plasticity_modifier': brain.getPlasticityModifier(),
            'spike_count': brain.getTotalSpikeCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'memory_usage': brain.getWorkingMemory().getUsage(),
            'cognition_score': brain.getConceptFormation().getCohesionScore()
        }
        
        # Run step
        brain.step(step)
        
        # Record developmental changes
        development_data.append(state)
    
    return development_data
```

### Pattern 3: Memory System Experiment

```python
def memory_experiment(config_file=None, trials=50):
    """Test memory systems"""
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with objects to remember
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable memory systems
    brain.enableEpisodicMemory(True)
    brain.enableWorkingMemory(True)
    
    results = []
    
    for trial in range(trials):
        # Random position in world
        x = config.getRandomGenerator()->uniformReal(0.0f, 1.0f) * 10
        y = config.getRandomGenerator()->uniformReal(0.0f, 1.0f) * 10
        
        # Store initial position as memory
        initial_pos = (x, y)
        brain.getEpisodicMemory()->storeExperience(trial, "start", initial_pos)
        
        # Navigate to random location
        for step in range(20):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(trial * 20 + step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check memory retrieval
        current_pos = (agent.getAgentBody()->x, agent.getAgentBody()->y)
        nearest_memory = brain.getEpisodicMemory()->recallNearest("start")
        
        # Calculate distance from memory
        if nearest_memory:
            dist = ((current_pos[0] - nearest_memory.position.x) ** 2 + 
                   (current_pos[1] - nearest_memory.position.y) ** 2) ** 0.5
        else:
            dist = float('inf')
        
        results.append({
            'trial': trial,
            'initial_pos': initial_pos,
            'final_pos': current_pos,
            'memory_distance': dist,
            'spike_count': brain.getTotalSpikeCount(),
            'working_memory_usage': brain.getWorkingMemory().getUsage()
        })
    
    return results
```

## Advanced Python Functions

### Configuration Management

```python
# Create specialized configurations

# High-performance configuration for complex brains
performance_config = pynlm.createDefaultConfig()
performance_config.set("brain.neuron_count", 10000)
performance_config.set("brain.simulation_speed", 2.0)
performance_config.set("brain.plasticity_enabled", True)

# Learning-focused configuration
learning_config = pynlm.createDefaultConfig()
learning_config.set("brain.reward_modulation", 1.0)
learning_config.set("brain.curiosity", 0.8)
learning_config.set("brain.development_rate", 1.5)

# Minimal configuration for testing
minimal_config = pynlm.createDefaultConfig()
minimal_config.set("brain.neuron_count", 100)
minimal_config.set("brain.simulation_speed", 0.5)

# Save configurations to files
performance_config.saveToFile("performance_config.json")
```

### Brain Manipulation

```python
# Advanced brain manipulation functions

def modify_brain_structure(brain, region_name, target_size):
    """Grow or shrink a brain region"""
    region = brain.getRegionByName(region_name)
    if region:
        current_size = region.getPopulationCount()
        if current_size < target_size:
            # Grow region
            brain.addNeuronsToRegion(region_name, target_size - current_size)
        elif current_size > target_size:
            # Shrink region (remove some neurons)
            brain.removeNeuronsFromRegion(region_name, current_size - target_size)

def enhance_plasticity(brain, factor):
    """Temporarily increase brain plasticity"""
    original_modifier = brain.getPlasticityModifier()
    brain.setPlasticityModifier(factor)
    return original_modifier

def study_connectivity(brain):
    """Analyze brain connectivity patterns"""
    regions = brain.getAllRegions()
    connectivity_data = {}
    
    for region_name, region in regions.items():
        connections = region.getOutgoingConnections()
        total_weight = sum(conn.weight for conn in connections)
        connection_count = len(connections)
        
        connectivity_data[region_name] = {
            'total_weight': total_weight,
            'connection_count': connection_count,
            'avg_weight': total_weight / max(connection_count, 1),
            'type_distribution': region.getConnectionTypeDistribution()
        }
    
    return connectivity_data
```

### Batch Processing

```python
# Batch processing for multiple brain instances

def batch_train_brains(config, num_brains=10, steps=100):
    """Train multiple brains in parallel"""
    brains = []
    results = []
    
    for i in range(num_brains):
        # Create brain with slight variations
        brain_config = pynlm.createDefaultConfig()
        brain_config.copyFrom(config)
        
        # Add small random variations
        brain_config.set("brain.seed", i)
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        brains.append(brain)
    
    # Train all brains
    for step in range(steps):
        for i, brain in enumerate(brains):
            # Random sensory input
            sensory_data = generate_random_sensory_input()
            brain.receiveSensoryInput(sensory_data)
            
            # Process
            brain.step(step)
            
            # Random reward
            reward = brain.getRandomGenerator().uniformReal(0, 1)
            brain.applyRewardModulation(reward, 0.0)
    
    # Collect results
    for i, brain in enumerate(brains):
        results.append({
            'brain_id': i,
            'final_spike_count': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_behavior': brain.getBehaviorSummary()
        })
    
    return results

def generate_random_sensory_input():
    """Generate random sensory input for testing"""
    # Vision (16x16 grid)
    vision_data = []
    for _ in range(16 * 16):
        vision_data.append(brain.getRandomGenerator().uniformReal(0, 1))
    
    # Touch (8 values)
    touch_data = []
    for _ in range(8):
        touch_data.append(1.0 if brain.getRandomGenerator().uniformReal(0, 1) < 0.1 else 0.0)
    
    # Create sensory input
    sensory = pynlm.SensoryInput()
    sensory.setVision(vision_data)
    sensory.setTouch(touch_data)
    
    return sensory
```

## Configuration System

### Advanced Configuration Options

```python
# Configuration keys and their purposes

# Brain configuration
brain.neuron_count = Total number of neurons in the brain
brain.simulation_speed = Simulation time multiplier (1.0 = real-time)
brain.plasticity_enabled = Whether synaptic plasticity is enabled
brain.reward_modulation = Strength of reward-based learning
brain.curiosity = Drive for exploration
brain.development_rate = Rate of developmental changes

# World configuration
world.width = World width in meters
world.height = World height in meters
world.vision_width = Vision sensor width
world.vision_height = Vision sensor height
world.max_energy = Maximum energy for agent
world.energy_decay_rate = Rate of energy depletion

# Agent configuration
agent.reward_modulation_enabled = Enable reward-based learning
agent.structural_plasticity_enabled = Enable connection growth
agent.development_enabled = Enable developmental changes
agent.curiosity_enabled = Enable exploration drive

# Memory configuration
memory.episodic.capacity = Number of episodic memories to store
memory.working.capacity = Working memory capacity
memory.associative.similarity_threshold = Pattern matching threshold

# Performance configuration
performance.num_threads = Number of CPU threads to use
performance.enable_vectorization = Use SIMD instructions
performance.enable_memory_pools = Use memory pooling
```

### Loading and Saving Configurations

```python
# Save brain state
brain.save("brain_checkpoint.pkl")

# Load brain state
brain.load("brain_checkpoint.pkl")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")

# Export for debugging
config.exportToPython("brain_config.py")
```

## Performance Optimization

### Multi-threaded Processing

```python
# Use multiple CPU cores
pynlm.enable_multi_threading(True)
pynlm.set_thread_count(8)

# Memory optimization
brain.enable_memory_pools(True)
brain.set_batch_size(100)

# Performance monitoring
brain.enable_performance_profiling(True)
while brain.is_profiling():
    stats = brain.get_performance_stats()
    print(f"Performance: {stats}")
```

### Large Brain Configuration

```python
# Configure for large-scale simulations
large_config = pynlm.createDefaultConfig()
large_config.set("brain.neuron_count", 100000)
large_config.set("brain.plasticity_enabled", True)
large_config.set("brain.development_enabled", True)

# Memory pools for large brains
large_config.set("memory.pools.enabled", True)
large_config.set("memory.pools.neuron_pool_size", 50000)
large_config.set("memory.pools.synapse_pool_size", 500000)

# Performance optimizations
large_config.set("performance.vectorization", True)
large_config.set("performance.num_threads", 8)
large_config.set("performance.batch_size", 1000)
```

## Error Handling and Debugging

### Error Handling Patterns

```python
def safe_brain_simulation(config_file, steps=100):
    """Simulation with error handling"""
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
        return brain
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        # Return a valid brain for debugging
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
```

### Debugging Tools

```python
# Enable logging
brain.enable_logging(True)
brain.set_log_level("DEBUG")

# Performance profiling
brain.enable_performance_profiling(True)
brain.start_profiling()

# Memory analysis
brain.enable_memory_profiling(True)
memory_stats = brain.get_memory_stats()

# Neuron analysis
brain.enable_neuron_profiling(True)
neuron_stats = brain.get_neuron_stats()

# Save debug information
brain.save_debug_info("debug_output/")
```

## Examples and Tutorials

### Basic Examples
1. **Silent Brain** - Just neurons thinking
2. **Brain Watching a World** - Perception without action
3. **Complete Agent** - Full perception-action loop
4. **Memory Experiment** - Testing memory systems
5. **Development Study** - Studying brain maturation

### Advanced Examples
1. **Multi-agent System** - Multiple brains in shared world
2. **Social Learning** - Brains learning from each other
3. **Adaptive Environment** - World that changes based on brains
4. **Neural Evolution** - Evolving brain configurations
5. **Real-time Control** - Using NLM for control systems

## API Reference

### Core Classes
- `Brain` - Main neural processor
- `SimpleWorld` - Simulation environment
- `AgentBrain` - Interface between brain and world
- `Config` - Configuration system
- `SensoryInput` - Sensory data container
- `Action` - Motor output container

### Key Methods
- `brain.step()` - Process simulation step
- `brain.initialize()` - Initialize brain
- `brain.save()` / `brain.load()` - State persistence
- `world.update()` - Update world state
- `world.getSensoryPercept()` - Get sensory input
- `agent.processSensoryInput()` - Process perception
- `agent.decodeMotorCommand()` - Generate action
- `agent.applyRewardModulation()` - Learn from rewards

### Enumerations
- `NeuronType` - Types of neurons
- `MotorCommand` - Low-level motor commands
- `ActionType` - High-level actions
- `DevelopmentalStage` - Brain development stages
- `WorldObjectType` - Objects in the world

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have sufficient input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are receiving input

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if development is enabled for early growth

**"Memory systems not working"**
- Verify memory systems are enabled
- Check if episodic memory capacity is sufficient
- Ensure reward modulation is working for memory consolidation

**"Performance too slow"**
- Reduce brain size
- Disable unnecessary features
- Increase batch size for processing
- Check memory usage

### Getting Help

For more detailed assistance:
1. Read the `docs/ARCHITECTURE.md` file for system architecture
2. Check `docs/SCIENCE.md` for scientific background
3. Review `examples/` directory for complete examples
4. Join the NLM community for discussion
5. File issues with detailed error messages

## Conclusion

NLM provides a complete neuroscience-inspired framework for creating, testing, and studying artificial brains. Whether you're doing simple simulations or complex research, NLM offers the tools you need to explore the boundaries of artificial intelligence.

The Python bindings make it easy to get started, while the advanced features enable complex research scenarios. The combination of spiking neurons, multiple memory systems, and neuromodulation creates a rich environment for studying brain-like computation.

Start with the basic examples, then gradually explore the advanced features as you become more comfortable with the system.
