# Tutorial 3: Advanced Usage and Expert Features

## Overview
This tutorial covers advanced features of the NLM Python API, including monitoring, callbacks, parallel processing, and expert-level functionality.

## Why Advanced Features Matter
While the basic NLM API is sufficient for many scenarios, the advanced features unlock the full power of the Neural Learning Machine framework. These features are essential for:

- Complex simulations with custom behavior
- Performance monitoring and debugging
- Multi-agent coordination
- Research and experimentation
- Production deployments

## Advanced Feature Categories

### 1. Monitoring and Callbacks
### 2. Parallel Processing
### 3. Custom Simulation Control
### 4. Neural Analysis and Visualization
### 5. Multi-Agent Coordination

## Feature 1: Monitoring and Callbacks

### Why Monitoring?
Monitoring is crucial for:
- Understanding simulation behavior
- Debugging issues
- Performance optimization
- Research analysis

### Setup Monitoring

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Setup monitoring
nlm.setup_monitoring(frequency_seconds=1.0)
```

### Custom Callbacks

```python
import pynlm

# Define custom callback functions
def on_step_complete(brain, step):
    """Called after each brain step"""
    if step % 100 == 0:
        stats = nlm.get_simulation_stats(brain, None)
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")

def on_reward_received(agent, reward, predicted):
    """Called when reward is received"""
    print(f"Reward: {reward:.3f} (predicted: {predicted:.3f})")

def on_brain_state_change(brain):
    """Called when brain state changes significantly"""
    stats = nlm.get_simulation_stats(brain, None)
    if stats['firing_neurons'] > stats['total_neurons'] * 0.1:  # 10% threshold
        print("High activity detected!")

# Create simulation with callbacks
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(15, 15, 6, 6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with custom callbacks
callbacks = {
    "on_step_complete": on_step_complete,
    "on_reward_received": on_reward_received,
    "on_brain_state_change": on_brain_state_change
}

# This would use the advanced simulation function
# nlm.run_advanced_simulation(brain, world, agent, callbacks)
```

### Performance Monitoring

```python
import pynlm
import time

# Create brain
config = pynlm.createHighPerformanceConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile different brain sizes
for neuron_count in [100, 1000, 5000, 10000]:
    config.set("brain.neuron_count", neuron_count)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile performance
    profile = nlm.profile_brain_simulation(brain, 1000)
    
    print(f"Neuron count: {neuron_count:,}")
    print(f"  Steps per second: {profile['steps_per_second']:.0f}")
    print(f"  Average step time: {profile['average_step_time_ms']:.2f} ms")
    print(f"  Total time: {profile['total_time']:.2f} seconds")
```

## Feature 2: Parallel Processing

### Why Parallel Processing?
Parallel processing enables:
- Multi-agent simulations
- Large-scale neural networks
- Concurrent testing
- Research experiments

### Creating Agent Groups

```python
import pynlm
import threading
import time

# Create multiple brains with different configurations
configs = [
    nlm.createDevelopmentConfig(),
    nlm.createStandardConfig(),
    nlm.createHighPerformanceConfig()
]

brains = []
for i, config in enumerate(configs):
    config.set("brain.neuron_count", 500 + i * 500)  # Different sizes
    brain = nlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Create agent group
world = nlm.createSimpleWorld()
world.configure(25, 25, 10, 10)
world.reset()

agents = []
for brain in brains:
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agents.append(agent)

# Test agent group info
group_info = nlm.create_agent_group(brains, [world], agents)
print("Agent Group Information:")
for key, value in group_info.items():
    print(f"  {key}: {value}")
```

### Parallel Simulation Example

```python
import nlm
import threading
import queue

def run_agent_simulation(agent_id, brain_config, world_config, results_queue):
    """Run a single agent simulation in parallel"""
    try:
        # Create brain
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        # Create world
        world = nlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        # Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        nlm.run_episode(brain, world, agent, 500)
        
        # Get results
        stats = nlm.get_simulation_stats(brain, world)
        results_queue.put({
            "agent_id": agent_id,
            "status": "success",
            "stats": stats,
            "brain_neuron_count": stats['total_neurons'],
            "final_firing_rate": stats['average_firing_rate']
        })
        
    except Exception as e:
        results_queue.put({
            "agent_id": agent_id,
            "status": "error",
            "error": str(e)
        })

# Simulation parameters
world_config = {"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8}

# Create brain configurations for different agents
brain_configs = [
    nlm.createDevelopmentConfig(),  # Learning-focused
    nlm.createStandardConfig(),     # Balanced
    nlm.createHighPerformanceConfig() # Speed-focused
]

# Run parallel simulations
results_queue = queue.Queue()
threads = []

for i, brain_config in enumerate(brain_configs):
    thread = threading.Thread(
        target=run_agent_simulation,
        args=(i, brain_config, world_config, results_queue)
    )
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

# Collect results
results = []
while not results_queue.empty():
    results.append(results_queue.get())

# Display results
print("Parallel Simulation Results:")
for result in results:
    if result["status"] == "success":
        print(f"  Agent {result['agent_id']}:")
        print(f"    Neurons: {result['brain_neuron_count']:,}")
        print(f"    Final firing rate: {result['final_firing_rate']:.3f} Hz")
    else:
        print(f"  Agent {result['agent_id']}: ERROR - {result['error']}")
```

## Feature 3: Custom Simulation Control

### Advanced Simulation Control

```python
import nlm

# Create development-focused simulation
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run custom simulation with progress tracking
print("Starting advanced simulation...")
start_time = time.time()

for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Progress reporting
    if step % 100 == 0:
        elapsed = time.time() - start_time
        progress = (step / 1000) * 100
        stats = nlm.get_simulation_stats(brain, world)
        
        print(f"Progress: {progress:.1f}% (Step {step}, {elapsed:.1f}s)")
        print(f"  Firing neurons: {stats['firing_neurons']}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print()

# Final statistics
final_stats = nlm.get_simulation_stats(brain, world)
print("Simulation completed!")
print(f"Total reward: {final_stats['total_spikes']}")
print(f"Average firing rate: {final_stats['average_firing_rate']:.2f} Hz")
print(f"Simulation time: {final_stats['simulation_time']:.1f}s")
```

## Feature 4: Neural Analysis and Visualization

### Neural Connectivity Analysis

```python
import nlm
import matplotlib.pyplot as plt
import numpy as np

# Create and run a brain
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run for a while to establish connections
for step in range(500):
    brain.step(step)

# Analyze neural connectivity
connectivity = nlm.analyze_connectivity(brain)

print("Neural Connectivity Analysis:")
print(f"  Total neurons: {connectivity['total_neurons']:,}")
print(f"  Total synapses: {connectivity['total_synapses']:,}")
print(f"  Average connections per neuron: {connectivity['average_connections_per_neuron']:.2f}")

# Create connectivity matrix
matrix = connectivity['regions']
print(f"  Region connectivity matrix: {len(matrix)}x{len(matrix[0])}")

# Simple visualization (you'd need to install matplotlib)
try:
    plt.figure(figsize=(10, 8))
    plt.imshow(matrix, cmap='hot', interpolation='nearest')
    plt.colorbar(label='Connection density')
    plt.title('Neural Connectivity Matrix')
    plt.xlabel('Target Region')
    plt.ylabel('Source Region')
    plt.savefig('connectivity_matrix.png')
    print("Connectivity matrix saved as 'connectivity_matrix.png'")
except ImportError:
    print("Matplotlib not available for visualization")
```

### Neural Activity Inspection

```python
import nlm
import json

# Create and run a brain
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)

# Inspect neural activity at different thresholds
print("Neural Activity Inspection:")
for threshold in [0.1, 0.3, 0.5, 0.7, 0.9]:
    activity = nlm.inspect_neural_activity(brain, threshold)
    print(f"  Threshold {threshold}: {activity['firing_neurons']} firing neurons "
          f"({activity['firing_neurons']/activity['total_neurons']*100:.1f}%)")

# Save detailed activity data
activity = nlm.inspect_neural_activity(brain, 0.5)
with open('neural_activity.json', 'w') as f:
    json.dump(activity, f, indent=2)

print("Detailed activity data saved to 'neural_activity.json'")
```

## Feature 5: Multi-Agent Coordination

### Coordinated Agent Group

```python
import nlm

# Create a group of agents with different roles
print("Creating coordinated agent group...")

# Agent 1: Explorer (high curiosity)
explorer_config = nlm.createDevelopmentConfig()
explorer_config.set("neuromod.curiosity.scale", 2.0)  # High curiosity
explorer_brain = nlm.createBrain(explorer_config)
explorer_brain.initialize()

explorer_world = nlm.createSimpleWorld()
explorer_world.configure(25, 25, 10, 10)
explorer_world.reset()
explorer_agent = nlm.createAgentBrain(explorer_brain)
explorer_agent.initialize(explorer_world)
explorer_agent.enableCuriosity(True)

# Agent 2: Navigator (high prediction)
navigator_config = nlm.createStandardConfig()
navigator_config.set("neuromod.novelty.scale", 1.5)  # High novelty detection
navigator_brain = nlm.createBrain(navigator_config)
navigator_brain.initialize()

navigator_world = nlm.createSimpleWorld()
navigator_world.configure(25, 25, 10, 10)
navigator_world.reset()
navigator_agent = nlm.createAgentBrain(navigator_brain)
navigator_agent.initialize(navigator_world)

# Agent 3: Learner (high reward modulation)
learner_config = nlm.createDevelopmentConfig()
learner_config.set("neuromod.dopamine.scale", 1.5)  # High reward sensitivity
learner_brain = nlm.createBrain(learner_config)
learner_brain.initialize()

learner_world = nlm.createSimpleWorld()
learner_world.configure(25, 25, 10, 10)
learner_world.reset()
learner_agent = nlm.createAgentBrain(learner_brain)
learner_agent.initialize(learner_world)
learner_agent.enableRewardModulation(True)

# Coordinate agents (example: share information)
agents = [explorer_agent, navigator_agent, learner_agent]
worlds = [explorer_world, navigator_world, learner_world]

# Create coordinated simulation
for step in range(300):
    for i, (agent, world) in enumerate(zip(agents, worlds)):
        # Each agent explores its own environment
        world.update(0.1)
        
        # Agent-specific behavior
        if i == 0:  # Explorer: focus on novel areas
            agent.enableCuriosity(True)
        elif i == 1:  # Navigator: focus on prediction
            # Custom navigation logic would go here
            pass
        elif i == 2:  # Learner: focus on reward
            agent.enableRewardModulation(True)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        agent.getBrain().step(step)
        
        # Coordinated action (agents could coordinate here)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print("Coordinated multi-agent simulation completed!")

# Get group performance
print("\nGroup Performance Summary:")
total_neurons = sum(agent.getBrain().getTotalNeuronCount() for agent in agents)
total_spikes = sum(agent.getBrain().getTotalSpikeCount() for agent in agents)
avg_firing_rate = sum(agent.getBrain().getAverageFiringRate() for agent in agents) / len(agents)

print(f"  Total agents: {len(agents)}")
print(f"  Total neurons: {total_neurons:,}")
print(f"  Total spikes: {total_spikes:,}")
print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
```

## Best Practices for Advanced Usage

### 1. Profile Before Optimizing
```python
import nlm

# Profile different configurations
configs = [
    ("Minimal", nlm.createMinimalConfig()),
    ("Standard", nlm.createStandardConfig()),
    ("Development", nlm.createDevelopmentConfig()),
    ("High Performance", nlm.createHighPerformanceConfig())
]

print("Performance Comparison:")
for name, config in configs:
    brain = nlm.createBrain(config)
    brain.initialize()
    
    profile = nlm.profile_brain_simulation(brain, 1000)
    print(f"  {name}:")
    print(f"    Steps/second: {profile['steps_per_second']:.0f}")
    print(f"    Avg step time: {profile['average_step_time_ms']:.2f} ms")
```

### 2. Monitor Memory Usage
```python
import nlm
import psutil
import os

def get_memory_usage():
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024  # MB

# Monitor memory during long simulation
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

initial_memory = get_memory_usage()
print(f"Initial memory usage: {initial_memory:.1f} MB")

# Run simulation with memory monitoring
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        current_memory = get_memory_usage()
        memory_increase = current_memory - initial_memory
        print(f"Step {step}: Memory usage: {current_memory:.1f} MB (+{memory_increase:.1f} MB)")
        
        # Clean up if memory usage is too high
        if memory_increase > 500:  # 500 MB threshold
            nlm.cleanup_memory()
            initial_memory = get_memory_usage()
            print("  Memory cleaned up")
```

### 3. Error Handling and Recovery
```python
import nlm

# Create brain with error handling
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run with error recovery
for step in range(1000):
    try:
        success = nlm.safe_brain_step(brain, step)
        if not success:
            print(f"Step {step} failed - attempting recovery")
            # Recovery logic would go here
            brain.reset()
    except Exception as e:
        print(f"Error at step {step}: {e}")
        # Error recovery logic
        brain.reset()
```

### 4. Data Management
```python
import nlm
import json
import os
from datetime import datetime

# Create simulation with data management
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Create data directory
os.makedirs("simulation_data", exist_ok=True)

# Run simulation with data collection
for step in range(500):
    brain.step(step)
    
    # Collect data
    stats = nlm.get_simulation_stats(brain, None)
    activity = nlm.inspect_neural_activity(brain, 0.5)
    
    # Save data periodically
    if step % 100 == 0:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save stats
        with open(f"simulation_data/stats_step_{step}_{timestamp}.json", "w") as f:
            json.dump(stats, f, indent=2)
        
        # Save activity
        with open(f"simulation_data/activity_step_{step}_{timestamp}.json", "w") as f:
            json.dump(activity, f, indent=2)
        
        print(f"Data saved for step {step}")

print("Data collection completed!")
```

## Troubleshooting Common Advanced Issues

### Issue: Performance Degradation
**Solution**: Profile and optimize
```python
# Profile to find bottlenecks
profile = nlm.profile_brain_simulation(brain, 1000)
if profile['average_step_time_ms'] > threshold:
    print("Performance issue detected - consider optimizations")
```

### Issue: Memory Usage Too High
**Solution**: Cleanup and optimize
```python
# Monitor and cleanup memory
nlm.cleanup_memory()
```

### Issue: Complex Error Messages
**Solution**: Add better error handling
```python
# Add comprehensive error handling
try:
    success = nlm.safe_brain_step(brain, step)
except Exception as e:
    print(f"Simulation error: {e}")
    # Recovery logic
    brain.reset()
```

### Issue: Visualization Dependencies
**Solution**: Handle missing libraries gracefully
```python
try:
    import matplotlib.pyplot as plt
    # Create plots
    plt.savefig('results.png')
except ImportError:
    print("Matplotlib not available - skipping visualization")
```

## Key Takeaways

### Advanced Usage Benefits
1. **Monitoring**: Track simulation behavior and performance
2. **Error Recovery**: Implement robust error handling and recovery mechanisms
3. **Parallel Processing**: Scale simulations with multiple agents
4. **Neural Analysis**: Deep dive into brain connectivity and activity
5. **Custom Control**: Fine-tune simulation behavior for specific needs

### Performance Considerations
1. **Profile**: Always measure before optimizing
2. **Monitor**: Track performance and memory usage
3. **Cleanup**: Use built-in memory management functions
4. **Validate**: Use configuration validation to catch issues early
5. **Document**: Save data and results for analysis

### Best Practices
1. **Start Simple**: Begin with basic configurations and gradually add complexity
2. **Error Handling**: Implement robust error handling for production systems
3. **Memory Management**: Monitor and manage memory usage
4. **Data Collection**: Save results for analysis and debugging
5. **Performance Optimization**: Profile and optimize based on real measurements

## Next Steps

1. **Tutorial 4: Performance Optimization** - Advanced optimization techniques
2. **Tutorial 5: Research Experiments** - Experimental design and analysis
3. **Tutorial 6: Production Deployment** - Deployment and monitoring in production

## Additional Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [Performance Guide](docs/PERFORMANCE.md)
- [Research Methods](docs/RESEARCH_METHODS.md)
- [Advanced Examples](examples/advanced/)

Advanced NLM usage requires a deeper understanding of neural simulation and system architecture. Start with the basics, gradually add complexity, and always profile and monitor your simulations.

The advanced features provide powerful capabilities for research, experimentation, and production deployments. Use them wisely to build sophisticated neural simulations that meet your specific requirements.

Key points to remember:
1. **Start simple** and gradually add complexity
2. **Profile** before optimizing
3. **Monitor** performance and memory usage
4. **Handle errors** gracefully
5. **Document results** for analysis and debugging

Advanced NLM usage opens up many possibilities for neural simulation research and applications. Use these tools effectively to build powerful neural systems.