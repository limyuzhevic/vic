# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NEW IN VERSION 2.0: Enhanced Python API with Advanced Monitoring and Analysis**

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (enhanced with monitoring)
2. **World** - The environment the brain lives in (enhanced with analysis)
3. **Agent** - The bridge connecting brain to world (enhanced with learning)

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Enhanced Monitoring

```python
import pynlm

# Step 1: Make a brain with enhanced monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps with monitoring
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Average firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStageString())

# Enhanced monitoring and analysis
status = brain.getStatus()
print("Brain Status:")
print(status)
```

### Example 2: Enhanced Agent with Learning and Development

```python
import pynlm

# Create complete agent with enhanced features
brain, world, agent = pynlm.create_learning_agent(
    neuronCount=2000,
    width=30,
    height=30
)

# Initialize with enhanced monitoring
agent.initialize(world)

# Run enhanced simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with enhanced learning
    agent.processSensoryInputWithLearning(percept, learningRate=0.02)
    
    # Brain thinks
    brain.step(step)
    
    # Get motor command with exploration
    action = agent.decodeMotorCommandWithExploration(explorationRate=0.15)
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulationAdvanced(reward, 0.0)
    
    # Update development
    agent.updateDevelopmentAdvanced(0.1, plasticityRate=1.2)
    
    # Print progress every 50 steps
    if step % 50 == 0:
        print(f"Step {step}: Firing rate {brain.getAverageFiringRate():.2f}, "
              f"Curiosity {agent.getCuriosityLevel():.2f}, "
              f"Novelty {agent.getNoveltyLevel():.2f}")

print("Enhanced simulation complete!")
```

### Example 3: Performance Benchmarking

```python
import pynlm
import time

# Create enhanced brain for benchmarking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain_wrapper = pynlm.BrainWrapper(brain)  # Use enhanced wrapper

brain_wrapper.initialize()

# Run benchmark with detailed metrics
start_time = time.time()
metrics = pynlm.benchmark_experiment(brain_wrapper, 2000, timestep=0.001)
end_time = time.time()

print("=== PERFORMANCE BENCHMARK ===")
print(f"Total execution time: {end_time - start_time:.2f} seconds")
print(f"Average step time: {metrics['average_step_time']*1000:.2f} ms")
print(f"Steps per second: {metrics['steps_per_second']:.0f}")
print(f"Total spikes generated: {metrics['total_spikes_generated']}")
print(f"Spikes per second: {metrics['spikes_per_second']:.0f}")
print(f"Average firing rate: {metrics['firing_rate_average']:.2f}")
```

### Example 4: Learning Curve Analysis

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain for learning analysis
brain, world, agent = pynlm.createCompleteAgent(neuronCount=1500)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)

# Analyze learning curves
learning_curve = pynlm.analyze_neural_activity(brain, 800, interval=100)

# Create learning curve plots
fig, axes = plt.subplots(2, 2, figsize=(15, 10))

# Firing rate curve
axes[0, 0].plot(learning_curve['steps'], learning_curve['firing_rates'])
axes[0, 0].set_title('Learning Curve - Average Firing Rate')
axes[0, 0].set_xlabel('Simulation Step')
axes[0, 0].set_ylabel('Firing Rate')
axes[0, 0].grid(True)

# Spike count curve
axes[0, 1].plot(learning_curve['steps'], learning_curve['spike_counts'])
axes[0, 1].set_title('Learning Curve - Cumulative Spikes')
axes[0, 1].set_xlabel('Simulation Step')
axes[0, 1].set_ylabel('Cumulative Spikes')
axes[0, 1].grid(True)

# E/I ratio curve
axes[1, 0].plot(learning_curve['steps'], learning_curve['e_i_ratios'])
axes[1, 0].set_title('Learning Curve - E/I Ratio')
axes[1, 0].set_xlabel('Simulation Step')
axes[1, 0].set_ylabel('E/I Ratio')
axes[1, 0].grid(True)

# Combined activity
axes[1, 1].plot(learning_curve['steps'], learning_curve['firing_rates'], label='Firing Rate')
axes[1, 1].plot(learning_curve['steps'], 
               [x / max(learning_curve['spike_counts']) for x in learning_curve['spike_counts']],
               label='Normalized Spike Activity')
axes[1, 1].set_title('Combined Activity Measures')
axes[1, 1].set_xlabel('Simulation Step')
axes[1, 1].set_ylabel('Activity')
axes[1, 1].legend()
axes[1, 1].grid(True)

plt.tight_layout()
plt.show()

print("Learning curve analysis complete!")
```

---

## Enhanced Wrapper Classes

### BrainWrapper

The enhanced BrainWrapper provides comprehensive monitoring and analysis:

```python
import pynlm

# Create enhanced brain wrapper
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain_wrapper = pynlm.BrainWrapper(brain)

brain_wrapper.initialize()

# Get system status
status = brain_wrapper.getStatus()
print(status)

# Get configuration summary
summary = brain_wrapper.getConfigurationSummary()
print("Configuration summary:", summary)

# Performance benchmarking
metrics = brain_wrapper.benchmark(1000)
print("Benchmark metrics:", metrics)

# Development information
print("Development stage:", brain_wrapper.getDevelopmentalStageString())
print("Working memory traces:", brain_wrapper.getWorkingMemoryActiveTraces())
print("Episodic memory episodes:", brain_wrapper.getEpisodicMemoryEpisodeCount())

# Neuromodulation levels
print("Dopamine level:", brain_wrapper.getDopamineLevel())
print("Curiosity level:", brain_wrapper.getCuriosityLevel())
print("Novelty level:", brain_wrapper.getNoveltyLevel())
```

### AgentWrapper

The enhanced AgentWrapper provides intelligent processing with learning:

```python
import pynlm

# Create complete agent
brain, world, agent = pynlm.createLearningAgent()
agent_wrapper = pynlm.AgentWrapper(agent)

# Initialize
agent_wrapper.initialize(world)

# Enhanced sensory processing with learning
percept = world.getSensoryPercept()
agent_wrapper.processSensoryInputWithLearning(
    percept, 
    learningRate=0.05, 
    noveltyThreshold=0.3
)

# Enhanced motor command with exploration
action = agent_wrapper.decodeMotorCommandWithExploration(explorationRate=0.2)

# Advanced reward modulation
reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
agent_wrapper.applyRewardModulationAdvanced(
    reward, 
    0.0,
    learningRate=1.5,
    enableCuriosity=True,
    enableNovelty=True
)

# Advanced development
agent_wrapper.updateDevelopmentAdvanced(0.1, plasticityRate=1.1)

# Enable advanced features
agent_wrapper.enableAdvancedFeatures(
    reward=True,
    structural=True,
    development=True,
    curiosity=True,
    prediction=False
)
```

### WorldWrapper

The enhanced WorldWrapper provides advanced world analysis:

```python
import pynlm

# Create world wrapper
world = pynlm.createSimpleWorld()
world_wrapper = pynlm.WorldWrapper(world)

# Configure with advanced parameters
world_wrapper.configure_advanced(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    energyDecayRate=0.98,
    maxEnergy=200,
    curiosityThreshold=0.4
)

# Get world state
percept = world_wrapper.getSensoryPercept()
agent_body = world_wrapper.getAgentBody()

# World analysis
width, height = world_wrapper.getWidth(), world_wrapper.getHeight()
density = world_wrapper.getAverageObjectDensity()
print(f"World size: {width}x{height}")
print(f"Object density: {density:.3f}")
```

---

## Simple Analogy (Enhanced)

| Real World | Enhanced NLM Code |
|------------|-------------------|
| You see something | `percept = world_wrapper.getSensoryPercept()` |
| Your brain processes with learning | `agent_wrapper.processSensoryInputWithLearning(percept)` |
| You decide to act with exploration | `action = agent_wrapper.decodeMotorCommandWithExploration()` |
| You do the action | `world_wrapper.update(action)` |
| Your brain learns from results with development | `agent_wrapper.applyRewardModulationAdvanced()` |
| You monitor learning progress | `metrics = brain_wrapper.benchmark()` |

---

## Common Patterns (Enhanced)

### Pattern 1: Enhanced Simulation with Monitoring

```python
def run_enhanced_simulation(brain_wrapper, world_wrapper, agent_wrapper, num_steps):
    for step in range(num_steps):
        # Update world
        world_wrapper.update(0.1)
        
        # Get sensory input
        percept = world_wrapper.getSensoryPercept()
        
        # Enhanced processing with learning
        agent_wrapper.processSensoryInputWithLearning(percept)
        
        # Brain thinks and learns
        brain_wrapper.step(step)
        
        # Get action with exploration
        action = agent_wrapper.decodeMotorCommandWithExploration()
        
        # Apply action
        world_wrapper.applyMotorCommand(action)
        
        # Enhanced reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent_wrapper.applyRewardModulationAdvanced(reward, 0.0)
        
        # Update development
        agent_wrapper.updateDevelopmentAdvanced(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {brain_wrapper.getAverageFiringRate():.2f}")
            print(f"  Curiosity: {agent_wrapper.getCuriosityLevel():.2f}")
            print(f"  Novelty: {agent_wrapper.getNoveltyLevel():.2f}")
            print(f"  Development: {agent_wrapper.getDevelopmentalStageString()}")

# Use it
run_enhanced_simulation(brain_wrapper, world_wrapper, agent_wrapper, 2000)
```

### Pattern 2: Performance Analysis

```python
def analyze_performance(brain_wrapper, world_wrapper, agent_wrapper, analysis_steps):
    """Analyze brain performance over time"""
    
    # Run simulation while collecting data
    firing_rates = []
    curiosity_levels = []
    novelty_levels = []
    development_stages = []
    
    for step in range(analysis_steps):
        world_wrapper.update(0.1)
        agent_wrapper.processSensoryInput(world_wrapper.getSensoryPercept())
        brain_wrapper.step(step)
        
        # Collect metrics
        firing_rates.append(brain_wrapper.getAverageFiringRate())
        curiosity_levels.append(agent_wrapper.getCuriosityLevel())
        novelty_levels.append(agent_wrapper.getNoveltyLevel())
        development_stages.append(agent_wrapper.getDevelopmentalStageString())
        
        # Apply learning
        reward = world_wrapper.getSensoryPercept().getInternal()[0] if world_wrapper.getSensoryPercept().getInternal() else 0.0
        agent_wrapper.applyRewardModulationAdvanced(reward, 0.0)
        agent_wrapper.updateDevelopmentAdvanced(0.1)
    
    # Calculate statistics
    avg_firing = sum(firing_rates) / len(firing_rates)
    max_curiosity = max(curiosity_levels)
    avg_novelty = sum(novelty_levels) / len(novelty_levels)
    
    print(f"Performance Analysis:")
    print(f"  Average firing rate: {avg_firing:.2f}")
    print(f"  Maximum curiosity: {max_curiosity:.2f}")
    print(f"  Average novelty: {avg_novelty:.2f}")
    print(f"  Development progression: {set(development_stages)}")
    
    return {
        'firing_rates': firing_rates,
        'curiosity_levels': curiosity_levels,
        'novelty_levels': novelty_levels,
        'development_stages': development_stages,
        'avg_firing': avg_firing,
        'max_curiosity': max_curiosity,
        'avg_novelty': avg_novelty
    }
```

### Pattern 3: Enhanced Configuration Management

```python
def configure_experiment(neuron_count=2000, world_size=50, learning_enabled=True):
    """Configure complete experiment with enhanced settings"""
    
    # Create enhanced agent with learning
    brain, world, agent = pynlm.createLearningAgent(
        neuronCount=neuron_count,
        width=world_size,
        height=world_size
    )
    
    # Create enhanced wrappers
    brain_wrapper = pynlm.BrainWrapper(brain)
    world_wrapper = pynlm.WorldWrapper(world)
    agent_wrapper = pynlm.AgentWrapper(agent)
    
    # Initialize
    agent_wrapper.initialize(world_wrapper)
    
    # Enable advanced features
    agent_wrapper.enableAdvancedFeatures(
        reward=True,
        structural=True,
        development=True,
        curiosity=True,
        prediction=True
    )
    
    # Configure world with enhanced parameters
    world_wrapper.configure_advanced(
        width=world_size,
        height=world_size,
        visionWidth=world_size // 3,
        visionHeight=world_size // 3,
        energyDecayRate=0.99,
        maxEnergy=100,
        curiosityThreshold=0.3
    )
    
    print(f"Enhanced experiment configured:")
    print(f"  Neurons: {neuron_count}")
    print(f"  World size: {world_size}x{world_size}")
    print(f"  Learning enabled: {learning_enabled}")
    
    return brain_wrapper, world_wrapper, agent_wrapper
```

---

## Enhanced Quick Reference

| What you want | Enhanced Code |
|--------------|---------------|
| Create enhanced brain | `brain = pynlm.createBrain(config); brain_wrapper = pynlm.BrainWrapper(brain)` |
| Start brain | `brain_wrapper.initialize()` |
| Make brain think | `brain_wrapper.step(step)` |
| Enhanced monitoring | `brain_wrapper.getStatus()` |
| Create complete agent | `brain, world, agent = pynlm.create_complete_agent()` |
| Create learning agent | `brain, world, agent = pynlm.create_learning_agent()` |
| Get brain stats | `brain_wrapper.getAverageFiringRate()` |
| Enhanced processing | `agent_wrapper.processSensoryInputWithLearning(percept)` |
| See world | `percept = world_wrapper.getSensoryPercept()` |
| Enhanced action | `action = agent_wrapper.decodeMotorCommandWithExploration()` |
| Performance benchmark | `metrics = brain_wrapper.benchmark(1000)` |
| Learning analysis | `curve = pynlm.analyze_neural_activity(brain_wrapper, 1000)` |
| Get development stage | `brain_wrapper.getDevelopmentalStageString()` |

---

## Troubleshooting (Enhanced)

### "My brain isn't doing anything"
- Did you call `brain_wrapper.initialize()`?
- Try increasing the number of steps
- Check that enhanced monitoring is working: `brain_wrapper.getStatus()`

### "The agent isn't learning"
- Did you enable learning features? `agent_wrapper.enableAdvancedFeatures(reward=True, curiosity=True)`
- Check that curiosity and novelty systems are working
- Verify that development is enabled: `agent_wrapper.updateDevelopmentAdvanced(0.1)`

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Check learning rates: `agent_wrapper.processSensoryInputWithLearning(percept, learningRate=0.05)`
- Enhanced monitoring should help: `print(brain_wrapper.getStatus())`

### "Performance is slow"
- Use performance benchmarking: `metrics = brain_wrapper.benchmark(500)`
- Check step timing in enhanced metrics
- Consider reducing simulation parameters

---

## Next Steps

When you're comfortable:

1. **Explore Advanced Features**:
   - Read `HOW_TO_USE.md` for advanced usage examples
   - Read `docs/ARCHITECTURE.md` to understand enhanced architecture
   - Experiment with different configurations!

2. **Advanced Python Usage**:
   - Use `brain_wrapper.benchmark()` for performance analysis
   - Use `pynlm.analyze_neural_activity()` for learning curves
   - Use enhanced wrapper classes for comprehensive monitoring

3. **Enhanced Configuration**:
   - Experiment with advanced world parameters
   - Configure learning rates and development stages
   - Use factory functions for common scenarios

4. **Research Applications**:
   - Use enhanced monitoring for scientific experiments
   - Apply learning analysis to study neural dynamics
   - Use performance benchmarking for optimization

---

**That's it! You're now ready to use NLM with enhanced features and monitoring!**

The enhanced Python API provides comprehensive monitoring, analysis, and learning capabilities while maintaining full backward compatibility. All existing code will continue to work, while new features enable advanced research and development scenarios.