# NLM Practical Examples and Tutorials

This document provides practical examples for getting started with NLM and demonstrates advanced usage patterns.

## Table of Contents

1. [Basic Brain Setup](#basic-brain-setup)
2. [Agent in World Interaction](#agent-in-world-interaction)
3. [Memory Systems Demonstration](#memory-systems-demonstration)
4. [Learning and Training Examples](#learning-and-training-examples)
5. [Advanced Configuration](#advanced-configuration)
6. [Debugging and Monitoring](#debugging-and-monitoring)
7. [Performance Optimization](#performance-optimization)

---

## Basic Brain Setup

### Example 1: Simple Brain with Default Configuration

```python
import pynlm

# Create a basic brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Example 2: Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain created with {brain.getTotalNeuronCount()} neurons")
```

---

## Agent in World Interaction

### Example 3: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(steps=500):
    """Run a complete agent simulation with sensory-motor loop."""
    
    # 1. Create and initialize components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 3. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentBody().x}, {world.getAgentBody().y}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Simulation completed!")

# Run the simulation
run_agent_simulation(500)
```

### Example 4: Visual Navigation

```python
import pynlm

def visual_navigation_task(target_x=40, target_y=40, steps=200):
    """Agent learns to navigate to a target position."""
    
    # Create environment with visual features
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Add visual target
    world.addObject(pynlm.WorldObject(target_x, target_y, 
                                      pynlm.WorldObjectType.Marker, value=1.0))
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Run navigation
    for step in range(steps):
        world.update(0.1)
        
        # Get vision
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check distance to target
        agent_body = world.getAgentBody()
        distance = ((agent_body.x - target_x)**2 + (agent_body.y - target_y)**2)**0.5
        
        if distance < 2.0:
            print(f"Target reached at step {step}!")
            break
        
        if step % 50 == 0:
            print(f"Step {step}: Distance to target: {distance:.1f}")

# Run navigation task
visual_navigation_task()
```

---

## Memory Systems Demonstration

### Example 5: Working Memory

```python
import pynlm

def working_memory_demo():
    """Demonstrate working memory capabilities."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access working memory
    working_memory = brain.getWorkingMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Current usage: {working_memory.getUsageCount()}")
    
    # Store patterns
    pattern1 = [0.1f, 0.5f, 0.9f, 0.2f, 0.8f]
    pattern2 = [0.3f, 0.7f, 0.4f, 0.6f, 0.1f]
    
    working_memory.storePattern(pattern1)
    working_memory.storePattern(pattern2)
    
    print(f"After storing patterns: usage = {working_memory.getUsageCount()}")
    
    # Retrieve patterns
    retrieved = working_memory.getCurrentPattern()
    print(f"Retrieved pattern: {retrieved}")
    
    # Clear memory
    working_memory.clear()
    print(f"After clear: usage = {working_memory.getUsageCount()}")

working_memory_demo()
```

### Example 6: Episodic Memory

```python
import pynlm

def episodic_memory_demo():
    """Demonstrate episodic memory capabilities."""
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Record experiences
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Create a simple experience record
        if percept.getVision():
            state = percept.getVision()
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            action_array = [0.1f, 0.9f]  # Simplified action
            agent.getEpisodicMemory().recordExperience(state, reward, action_array)
    
    # Access episodic memory
    episodic_memory = agent.getEpisodicMemory()
    print(f"Episodic memory capacity: {episodic_memory.getCapacity()}")
    print(f"Episodes recorded: {episodic_memory.getEpisodeCount()}")
    print(f"Replay count: {episodic_memory.getReplayCount()}")
    print(f"Consolidation strength: {episodic_memory.getConsolidationStrength()}")

episodic_memory_demo()
```

---

## Learning and Training Examples

### Example 7: Basic Learning

```python
import pynlm

def basic_learning_demo():
    """Demonstrate basic learning capabilities."""
    
    # Create learning environment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableAllSystems()
    
    print("Starting learning demo...")
    print("Enabled systems:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run learning
    for episode in range(10):
        print(f"\nEpisode {episode + 1}:")
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.1 if step % 10 == 0 else 0.0  # Simple reward structure
            agent.applyRewardModulation(reward, 0.0)
        
        # Get learning statistics
        stats = agent.getLearningStatistics()
        print(f"  Curiosity level: {stats['curiosity']:.3f}")
        print(f"  Novelty level: {stats['novelty']:.3f}")
        print(f"  Prediction error: {stats['prediction_error']:.3f}")
    
    # Get final performance metrics
    metrics = agent.getPerformanceMetrics()
    print(f"\nFinal Performance Metrics:")
    for key, value in metrics.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")

basic_learning_demo()
```

### Example 8: Advanced Training with Episodes

```python
import pynlm

def advanced_training_demo():
    """Demonstrate advanced training with episode-based learning."""
    
    # Create components
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=15, visionHeight=15)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllSystems()
    
    # Training parameters
    num_episodes = 5
    steps_per_episode = 100
    
    print(f"Starting advanced training: {num_episodes} episodes, {steps_per_episode} steps each")
    
    # Run training
    for episode in range(num_episodes):
        print(f"\nTraining episode {episode + 1}/{num_episodes}")
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 20 == 0:
                print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, Curiosity={agent.getCuriosityLevel():.3f}")
        
        # Reset for next episode
        agent.reset()
        world.reset()
    
    print("\nTraining completed!")
    
    # Create checkpoint
    agent.createCheckpoint("training_checkpoint.bin")
    print("Checkpoint saved: training_checkpoint.bin")
    
    # Get final statistics
    stats = agent.getLearningStatistics()
    print(f"\nFinal Learning Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")

advanced_training_demo()
```

---

## Advanced Configuration

### Example 9: Development-Focused Configuration

```python
import pynlm

def create_development_config():
    """Create a configuration focused on development and learning."""
    
    config = pynlm.createDevelopmentConfig()
    
    # Override some settings
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.12)
    config.set("development.critical_period_end", 100.0f)
    config.set("development.plasticity_decay_rate", 0.9f)
    config.set("neuromod.curiosity.threshold", 0.2f)
    
    return config

# Use the config
config = create_development_config()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 10: Exploration-Focused Configuration

```python
import pynlm

def create_exploration_config():
    """Create a configuration focused on exploration and novelty."""
    
    config = pynlm.createExplorationConfig()
    
    # Increase exploration parameters
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    
    return config

# Use the config
config = create_exploration_config()
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Debugging and Monitoring

### Example 11: System Monitoring

```python
import pynlm

def monitor_system(agent, brain, world, steps=100):
    """Monitor system performance during simulation."""
    
    print("Starting system monitoring...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor at intervals
        if step % 20 == 0:
            # Get system status
            status = pynlm.getSystemStatus(brain)
            
            print(f"\nStep {step} Status:")
            for key, value in status.items():
                print(f"  {key}: {value}")
            
            # Get learning statistics
            stats = agent.getLearningStatistics()
            print(f"  Learning stats: {stats}")
    
    print("Monitoring completed!")

# Use monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

monitor_system(agent, brain, world, 100)
```

### Example 12: Checkpoint Management

```python
import pynlm

def checkpoint_demo():
    """Demonstrate checkpoint save/load functionality."""
    
    # Create and train a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Train for some steps
    for step in range(50):
        brain.step(step)
    
    # Save checkpoint
    print("Saving checkpoint...")
    success = brain.save("demo_checkpoint.bin")
    if success:
        print("✓ Checkpoint saved successfully")
    else:
        print("✗ Failed to save checkpoint")
    
    # Create a new brain and load checkpoint
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    
    print("Loading checkpoint...")
    success = new_brain.load("demo_checkpoint.bin")
    if success:
        print("✓ Checkpoint loaded successfully")
        print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
    else:
        print("✗ Failed to load checkpoint")

checkpoint_demo()
```

---

## Performance Optimization

### Example 13: Performance Benchmarking

```python
import pynlm
import time

def benchmark_performance(config, steps=1000):
    """Benchmark brain performance."""
    
    print(f"Benchmarking performance for {steps} steps...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        # Record performance at intervals
        if step % 100 == 0:
            # Get metrics
            neurons = brain.getTotalNeuronCount()
            spikes = brain.getTotalSpikeCount()
            firing = brain.getFiringNeuronCount()
            
            # Calculate performance
            time_per_step = (time.time() - start_time) / (step + 1)
            
            print(f"Step {step}: {neurons} neurons, {spikes} spikes, "
                  f"{firing} firing, {time_per_step*1000:.2f} ms/step")
    
    total_time = time.time() - start_time
    steps_per_second = steps / total_time
    
    print(f"\nBenchmark Results:")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Average time per step: {total_time/steps*1000:.2f} ms")

# Benchmark with different configurations
configs = {
    "Small": pynlm.createDevelopmentConfig(),
    "Medium": pynlm.createDefaultConfig(),
    "Large": pynlm.createExplorationConfig()
}

for name, config in configs.items():
    print(f"\n{'='*50}")
    print(f"Benchmarking {name} configuration")
    print(f"{'='*50}")
    benchmark_performance(config, 500)
```

### Example 14: Memory Usage Monitoring

```python
import pynlm
import psutil
import os

def monitor_memory_usage(agent, brain, world, steps=200):
    """Monitor memory usage during simulation."""
    
    print("Monitoring memory usage...")
    
    process = psutil.Process(os.getpid())
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor at intervals
        if step % 50 == 0:
            memory_info = process.memory_info()
            cpu_percent = process.cpu_percent()
            
            print(f"\nStep {step}:")
            print(f"  Memory usage: {memory_info.rss / 1024 / 1024:.1f} MB")
            print(f"  CPU usage: {cpu_percent:.1f}%")
            
            # Get system status
            status = pynlm.getSystemStatus(brain)
            print(f"  System status: {status}")
    
    print("\nMemory monitoring completed!")

# Use memory monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

monitor_memory_usage(agent, brain, world, 100)
```

---

## Common Patterns and Best Practices

### Pattern 1: Complete Agent Setup

```python
def setup_complete_agent(config=None):
    """Setup a complete agent with all components."""
    
    # Create configuration
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable systems
    agent.enableAllSystems()
    
    return agent, brain, world

# Use the pattern
agent, brain, world = setup_complete_agent()
```

### Pattern 2: Training Loop

```python
def training_loop(agent, brain, world, num_episodes, steps_per_episode):
    """Standard training loop pattern."""
    
    for episode in range(num_episodes):
        print(f"Episode {episode + 1}/{num_episodes}")
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Reset episode
        agent.reset()
        world.reset()
    
    print("Training completed!")

# Use the pattern
agent, brain, world = setup_complete_agent()
training_loop(agent, brain, world, 5, 100)
```

### Pattern 3: Evaluation Loop

```python
def evaluation_loop(agent, brain, world, steps):
    """Evaluation loop without learning."""
    
    # Disable learning for evaluation
    agent.enableRewardModulation(False)
    agent.enableCuriosity(False)
    agent.enableDevelopment(False)
    
    # Run evaluation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get performance metrics
    metrics = agent.getPerformanceMetrics()
    
    # Re-enable learning (optional)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return metrics

# Use the pattern
agent, brain, world = setup_complete_agent()
metrics = evaluation_loop(agent, brain, world, 500)
print(f"Evaluation metrics: {metrics}")
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain not doing anything"

**Problem**: Brain seems inactive or not responding.

**Solutions**:
- Ensure `brain.initialize()` is called before using the brain
- Check if sensory input is being provided
- Verify that simulation steps are being called
- Check neuron parameters (threshold, resting potential)

**Debugging**:
```python
# Add debugging
print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
print(f"Configuration: {brain.getConfig().summary()}")

# Check neuron properties
neurons = brain.getRegion(brain.getRegionIds()[0])
if neurons:
    for neuron in neurons.getAllNeurons():
        print(f"Neuron V={neuron.getMembranePotential()}, thresh={neuron.getThreshold()}")
```

#### 2. "Agent not moving"

**Problem**: Agent produces no actions or stays in place.

**Solutions**:
- Ensure `world.update()` is being called
- Check if `agent.decodeMotorCommand()` returns valid actions
- Verify that `world.applyMotorCommand()` is called
- Check action production logic

**Debugging**:
```python
# Debug action production
action = agent.decodeMotorCommand()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")

# Check brain state
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

#### 3. "Learning not happening"

**Problem**: Brain is not learning or adapting.

**Solutions**:
- Ensure learning systems are enabled
- Check reward signals
- Verify neuromodulation is working
- Check plasticity parameters

**Debugging**:
```python
# Check learning system status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")

# Check learning statistics
stats = agent.getLearningStatistics()
print(f"Learning statistics: {stats}")

# Check neuromodulation
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
```

#### 4. "Performance issues"

**Problem**: Simulation is slow or memory-intensive.

**Solutions**:
- Reduce brain size
- Decrease simulation steps
- Optimize configuration
- Monitor memory usage

**Debugging**:
```python
import time
import psutil
import os

process = psutil.Process(os.getpid())

start_time = time.time()
start_memory = process.memory_info().rss

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        current_time = time.time()
        current_memory = process.memory_info().rss
        
        print(f"Step {step}:")
        print(f"  Time per step: {(current_time - start_time) / (step + 1) * 1000:.2f} ms")
        print(f"  Memory per step: {(current_memory - start_memory) / (step + 1) / 1024:.2f} KB")
```

---

## Next Steps

After getting familiar with the basic examples:

1. **Read Advanced Documentation**: Check `docs/ARCHITECTURE.md` for detailed component explanations
2. **Experiment with Configurations**: Try different parameter combinations
3. **Run Phase 6 Demo**: Use the comprehensive Phase 6 demo to see all systems in action
4. **Build Your Own Experiments**: Create custom experiments using the experiment framework
5. **Join the Community**: Contribute to the project and share your findings

## Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **API Reference**: Check the Python module documentation
- **Examples**: More examples in the `examples/` directory
- **Support**: Join the NLM community for support and questions

---

*This documentation is continuously updated. Check for the latest version and examples.*