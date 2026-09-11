# NLM Python API Best Practices
# Practical Guidelines for Using NLM in Python

## Introduction

This document provides best practices and practical advice for using the NLM Python API effectively. It covers common patterns, performance considerations, and advanced usage techniques.

## Basic Usage Patterns

### Pattern 1: Simple Simulation

**Do:** Use minimal configuration when not needed
```python
import pynlm

# Simple setup - uses all defaults
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

**Don't:** Over-configure unnecessarily
```python
# Too many custom settings
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("connection_probability", 0.1)
config.set("stdp_ltp_weight", 0.01)
config.set("stdp_ltd_weight", 0.012)
config.set("stdp_tau", 20.0)
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)
# ... many more
```

### Pattern 2: Environment-Agent Interaction

**Do:** Create reusable world and agent objects
```python
def create_agent_world(config):
    """Factory function for creating agent-world setup"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, world, agent

# Reuse the factory
for episode in range(10):
    brain, world, agent = create_agent_world(pynlm.createDefaultConfig())
    # ... run episode
```

**Don't:** Create new objects for every simulation step
```python
# Bad pattern - creates new objects each step
for step in range(1000):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())  # New brain every step!
    brain.initialize()  # Re-initialize
    # ...
```

### Pattern 3: Memory Management

**Do:** Save brain states for long simulations
```python
def save_checkpoint(brain, step, max_checkpoints=10):
    """Save periodic checkpoints"""
    if step % (1000 * max_checkpoints) == 0:
        brain.save(f"checkpoint_{step:08d}.bin")
        print(f"Saved checkpoint at step {step}")

# Use in simulation
for step in range(total_steps):
    brain.step(step)
    save_checkpoint(brain, step)
```

**Don't:** Save every step in long simulations
```python
# Bad pattern - saves every step
save_every_step = False
for step in range(100000):
    brain.step(step)
    if save_every_step:
        brain.save(f"checkpoint_{step}.bin")  # Creates thousands of files
```

## Performance Optimizations

### CPU Performance

**Do:** Use appropriate configuration for your use case
```python
# For small experiments - faster startup
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Small brain
config.set("region_count", 1)

# For research - more neurons
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)  # Larger brain
config.set("region_count", 4)    # Multiple regions
```

**Don't:** Use large brains for quick tests
```python
# Bad pattern - slow startup for quick test
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)  # 10K neurons for 10 steps
```

### Memory Performance

**Do:** Use memory pools for repeated simulations
```python
import pynlm

# Create a factory that reuses objects
class BrainPool:
    def __init__(self, config):
        self.config = config
        self.brain_pool = []
    
    def get_brain(self):
        if self.brain_pool:
            return self.brain_pool.pop()
        return pynlm.createBrain(self.config)
    
    def return_brain(self, brain):
        brain.reset()
        self.brain_pool.append(brain)

# Use in multiple simulations
pool = BrainPool(pynlm.createDefaultConfig())
for episode in range(10):
    brain = pool.get_brain()
    brain.initialize()
    # ... run episode
    pool.return_brain(brain)
```

**Don't:** Create new brain objects for every simulation
```python
# Bad pattern - memory allocation overhead
for episode in range(100):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)  # New allocation
    brain.initialize()
    # ... run episode
    # brain is never returned to pool
```

### I/O Performance

**Do:** Use appropriate file formats and locations
```python
# Save to fast local filesystem
brain.save("/tmp/brain_checkpoint.bin")
brain.load("/tmp/brain_checkpoint.bin")

# Avoid network paths for frequent I/O
# Don't use: brain.save("s3://bucket/brain.bin")
```

**Don't:** Use slow I/O for frequent operations
```python
# Bad pattern - slow for frequent operations
for step in range(10000):
    brain.step(step)
    brain.save(f"checkpoint_{step}.bin")  # Frequent I/O is slow
```

## Advanced Features

### Pattern 1: Curiosity-Driven Exploration

**Do:** Enable curiosity for adaptive behavior
```python
def run_curiosity_driven_simulation(brain, agent, world, steps):
    """Run simulation with curiosity-driven exploration"""
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if agent is exploring due to curiosity
        curiosity_level = agent.getCuriosityLevel()
        if curiosity_level > 0.5:
            print(f"Step {step}: High curiosity ({curiosity_level:.2f}) - "
                  f"exploring new areas")
```

**Don't:** Always use default action selection
```python
# Bad pattern - never explore
for step in range(1000):
    action = agent.decodeMotorCommand()  # Always uses default policy
    # No curiosity-driven exploration
```

### Pattern 2: Development Tracking

**Do:** Monitor developmental stages
```python
def track_development(brain, agent, check_interval=100):
    """Track brain development through stages"""
    stages = []
    
    for step in range(5000):
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        if step % check_interval == 0:
            stage = brain.getDevelopmentalStage()
            stages.append((step, stage))
            print(f"Step {step}: {stage}")
            
            # React to developmental changes
            if stage == pynlm.DevelopmentalStage.Adult:
                print("Brain has reached adulthood - stabilizing behavior")
                agent.enableCuriosity(False)
            elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
                print("Critical period - high learning capacity")
    
    return stages
```

**Don't:** Ignore developmental state
```python
# Bad pattern - never check development
for step in range(1000):
    brain.step(step)
    # Never check developmental stage or update development
    # Brain doesn't mature or change over time
```

### Pattern 3: Memory Integration

**Do:** Use integrated memory systems
```python
def run_memory_integration_experiment(brain, agent):
    """Demonstrate working with memory systems"""
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    
    # Working memory - transient information
    print(f"Working memory capacity: {working_mem.getCapacity()}")
    print(f"Active traces: {working_mem.getActiveTraces()}")
    
    # Episodic memory - experience storage
    print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")
    print(f"Current episodes: {episodic_mem.getEpisodeCount()}")
    
    # Store example episode
    if episodic_mem:
        episode = episodic_mem.getCurrentEpisode()
        if episode:
            print(f"Last episode at step {episode.timestamp}")
            print(f"Active neurons: {len(episode.activeNeurons)}")
```

**Don't:** Ignore memory systems
```python
# Bad pattern - never use memory
for step in range(1000):
    brain.step(step)
    # Never access or use working memory, episodic memory, or associative memory
    # Memory is not being utilized
```

## Error Handling

### Pattern 1: Graceful Degradation

**Do:** Handle missing features gracefully
```python
def safe_simulation_step(brain, agent, world, step):
    """Run simulation with error handling"""
    try:
        # Update world
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for errors and handle gracefully
        if not result.success:
            print(f"Action failed at step {step}: {result.message}")
            # Use fallback action
            fallback_action = pynlm.MotorCommand.Wait
            world.applyMotorCommand(fallback_action, world.getSimulationTime())
        
        return True
        
    except Exception as e:
        print(f"Error at step {step}: {e}")
        return False
```

**Don't:** Let errors crash the simulation
```python
# Bad pattern - no error handling
for step in range(1000):
    world.update(0.1)  # Might crash
    agent.processSensoryInput(world.getSensoryPercept())  # Might crash
    brain.step(step)  # Might crash
    # No error handling
```

### Pattern 2: Validation

**Do:** Validate inputs before use
```python
def validate_simulation_setup(brain, agent, world):
    """Validate simulation setup"""
    errors = []
    
    # Check brain
    if not brain:
        errors.append("Brain is None")
    elif not brain.getTotalNeuronCount():
        errors.append("Brain has no neurons")
    
    # Check agent
    if not agent:
        errors.append("Agent is None")
    elif not agent.getBrain():
        errors.append("Agent has no brain")
    
    # Check world
    if not world:
        errors.append("World is None")
    elif not world.getWidth() or not world.getHeight():
        errors.append("World has invalid dimensions")
    
    if errors:
        print("Validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    print("Simulation setup is valid")
    return True
```

**Don't:** Assume everything is set up correctly
```python
# Bad pattern - no validation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
# ... no validation
```

## Documentation and Examples

### Pattern 1: Add Docstrings

**Do:** Document your code
```python
def run_specific_experiment(config, steps, enable_features):
    """
    Run a specific NLM experiment.
    
    Args:
        config: Brain configuration
        steps: Number of simulation steps
        enable_features: Dictionary of features to enable
        
    Returns:
        dict: Results including performance metrics
        
    Example:
        >>> results = run_specific_experiment(
        ...     pynlm.createDefaultConfig(), 100, 
        ...     {"curiosity": True, "reward": True}
        ... )
        >>> print(f"Final performance: {results['avg_performance']}")
    """
    # Implementation
```

**Don't:** Document only in comments
```python
# Bad pattern - only comments, no docstring
def run_experiment(brain):  # No docstring
    # Comment explaining what it does
    for step in range(100):
        brain.step(step)
```

### Pattern 2: Create Examples

**Do:** Create reusable example scripts
```python
# examples/tutorial_basic.py
import pynlm

def tutorial_1_basic_brain():
    """Tutorial 1: Basic brain simulation"""
    print("Tutorial 1: Basic Brain Simulation")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    print(f"Done! Total spikes: {brain.getTotalSpikeCount()}")

def tutorial_2_agent_world():
    """Tutorial 2: Agent in world"""
    print("Tutorial 2: Agent in World")
    print("-" * 30)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Agent simulation complete!")

if __name__ == "__main__":
    tutorial_1_basic_brain()
    tutorial_2_agent_world()
```

**Don't:** Write everything in a single monolithic script
```python
# Bad pattern - everything in one file
def main():
    # Tutorial 1
    # Tutorial 2  
    # Tutorial 3
    # Tutorial 4
    # etc.
```

## Common Issues and Solutions

### Issue 1: Brain Not Learning

**Problem:** Brain weights aren't changing

**Solution:** Check plasticity settings
```python
# Check plasticity is enabled
brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity systems
stdp = brain.getSTDP()
if stdp:
    print(f"STDP LTP weight: {stdp.getLTPWeight()}")
    print(f"STDP LTD weight: {stdp.getLTDWeight()}")

# Enable plasticity flags
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

### Issue 2: Agent Not Moving

**Problem:** Agent doesn't change position

**Solution:** Check action selection and world
```python
# Debug action selection
brain.step(step)
action = agent.decodeMotorCommand()
print(f"Selected action: {action}")

# Check motor neuron activity
if hasattr(brain, 'getMotorNeurons'):
    motor_neurons = brain.getMotorNeurons()
    print(f"Motor neuron count: {len(motor_neurons)}")

# Check world state
print(f"Agent position: {world.getAgentBody().x}, {world.getAgentBody().y}")
```

### Issue 3: Memory Not Being Stored

**Problem:** Memory systems not working

**Solution:** Check memory system setup
```python
# Check memory systems
brain = pynlm.createBrain(config)
brain.initialize()

working_mem = brain.getWorkingMemory()
if working_mem:
    print(f"Working memory capacity: {working_mem.getCapacity()}")
    print(f"Available storage: {working_mem.getAvailableStorage()}")

episodic_mem = brain.getEpisodicMemory()
if episodic_mem:
    print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")
    print(f"Current episodes: {episodic_mem.getEpisodeCount()}")
```

## Migration Guide

### From C++ to Python

**Do:** Use Python for prototyping and research
```python
# Python - quick research and prototyping
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

**Don't:** Use Python for production systems requiring maximum performance
```python
# Bad pattern - use C++ for production
# Python has overhead for very frequent operations
```

### From Old NLM to Phase 6

**Do:** Update to Phase 6 features
```python
# Old API
brain = pynlm.createBrain(config)

# New Phase 6 API with integrated systems
brain = pynlm.createBrain(config)
brain.initialize()

# Access all Phase 6 systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()
planner = brain.getPlanner()
attention = brain.getAttention()
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
```

## Future Improvements

### Pattern 1: Extend API

**Do:** Add Python-specific convenience functions
```python
# Add convenience function
def run_standard_experiment(config, steps=1000, save_checkpoint=True):
    """Standard experiment with all best practices"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable common features
    # (Implementation would go here)
    
    for step in range(steps):
        brain.step(step)
        
        if save_checkpoint and step % 1000 == 0:
            brain.save(f"exp_{step:08d}.bin")
    
    return brain
```

**Don't:** Implement everything in core API
```python
# Bad pattern - all functionality in core
# Python-specific features should be separate
```

### Pattern 2: Add Visualization

**Do:** Provide Python visualization tools
```python
import pynlm
import matplotlib.pyplot as plt

def plot_simulation_results(brain, agent, world, steps):
    """Plot simulation results"""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Spike count over time
    spikes_over_time = [brain.getTotalSpikeCount()]
    axes[0,0].plot(spikes_over_time)
    axes[0,0].set_title("Total Spikes")
    axes[0,0].set_xlabel("Step")
    axes[0,0].set_ylabel("Spikes")
    
    # Plot 2: Developmental stage
    stages = [brain.getDevelopmentalStage()]
    axes[0,1].plot(stages)
    axes[0,1].set_title("Developmental Stage")
    axes[0,1].set_xlabel("Step")
    axes[0,1].set_ylabel("Stage")
    
    # Plot 3: Curiosity level
    curiosity_levels = [agent.getCuriosityLevel()]
    axes[1,0].plot(curiosity_levels)
    axes[1,0].set_title("Curiosity Level")
    axes[1,0].set_xlabel("Step")
    axes[1,0].set_ylabel("Curiosity")
    
    # Plot 4: Novelty level
    novelty_levels = [agent.getNoveltyLevel()]
    axes[1,1].plot(novelty_levels)
    axes[1,1].set_title("Novelty Level")
    axes[1,1].set_xlabel("Step")
    axes[1,1].set_ylabel("Novelty")
    
    plt.tight_layout()
    plt.show()
```

**Don't:** Provide no visualization tools
```python
# Bad pattern - no plotting tools
# Users have to implement their own visualization
```

## Summary

This document provides best practices for using the NLM Python API effectively. Key takeaways:

1. **Simplicity First**: Start with minimal configuration and add features as needed
2. **Error Handling**: Always handle errors gracefully
3. **Performance**: Optimize for your specific use case
4. **Documentation**: Document your code and create examples
5. **Validation**: Validate inputs and configurations
6. **Memory Management**: Manage memory efficiently for repeated simulations
7. **Testing**: Test thoroughly with various scenarios

By following these patterns, you'll be able to develop robust, efficient, and maintainable NLM Python applications that leverage the full power of Phase 6's integrated artificial brain systems.