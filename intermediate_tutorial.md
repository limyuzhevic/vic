# NLM Intermediate Tutorial

## Welcome to Intermediate NLM!

You've completed the beginner tutorial and now have a solid foundation. This tutorial will help you explore more advanced features and create more sophisticated simulations.

## What We'll Cover

1. Learning with neuromodulation (reward and curiosity)
2. Working memory and attention systems
3. Building more complex agents
4. Performance monitoring and analysis
5. Advanced configuration and tuning
6. Practical applications

## Step 1: Learning with Neuromodulation

Neuromodulation is how brains learn from experience. NLM has three main neuromodulation systems:

- **Dopamine**: Reward signals
- **Curiosity**: Exploration motivation
- **Novelty**: Detection of new patterns

Let's build a learning agent!

```python
import pynlm

print("Creating learning agent...")
print("-" * 40)

# Setup with learning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("✓ Learning systems enabled")
print("  - Reward modulation: Dopamine-based learning")
print("  - Curiosity: Exploration motivation")
print("  - Development: Brain maturation")
```

## Step 2: Complete Learning Simulation

Now let's run a learning episode where the agent learns from rewards and explores new things.

```python
# Learning simulation
print("\nStarting learning episode...")
print("-" * 40)

# Run learning episode
for step in range(200):
    world.update(0.1)
    
    # Get sensory input and process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Make and execute action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress periodically
    if step % 50 == 0:
        print(f"\nStep {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
        print(f"  Action: {action.name}")
        print(f"  Reward: {reward:.3f}")

print("\n" + "=" * 40)
print("✓ Learning episode complete!")
print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
```

## Step 3: Working Memory

Working memory lets the agent maintain information over time. This is crucial for:

- Remembering recent events
- Maintaining goals
- Sequencing actions

```python
# Working memory example
print("\nSetting up working memory...")
print("-" * 40)

# Get working memory from brain
wm = brain.getWorkingMemory()
if wm:
    print("✓ Working memory available")
    
    # Set capacity and decay rate
    wm.setCapacity(50)           # How many items to remember
    wm.setDecayRate(0.02)        # How quickly memories fade
    
    # Store some information during simulation
    for step in range(100):
        # Store pattern (simplified - store step number as pattern)
        pattern = [float(step % 10) / 10.0] * 3  # 3-dimensional pattern
        wm.store(pattern, strength=1.0)
        
        # Update working memory (maintenance)
        wm.update(0.1)
        
        # Check what's stored
        if step % 25 == 0:
            active = wm.getActiveTraces()
            activity = wm.getMemoryActivity()
            print(f"Step {step}:")
            print(f"  Active memory traces: {active}")
            print(f"  Memory activity level: {activity:.3f}")
            
            # Store a goal
            if step == 50:
                wm.store([1.0, 0.0, 0.0], strength=2.0)  # "Go to goal"
                print("  Stored goal: 'Go to goal'")
    
    print("✓ Working memory simulation complete")
else:
    print("⚠ Working memory not available")
```

## Step 4: Attention System

Attention lets the agent focus on important information among competing inputs.

```python
# Attention system example
print("\nSetting up attention system...")
print("-" * 40)

# Get attention from brain
attn = brain.getAttention()
if attn:
    print("✓ Attention system available")
    
    # Initialize attention (requires brain reference)
    # Note: This would normally be done during agent setup
    # For this example, we'll simulate attention effects
    
    # Simulate competing inputs
    print("Simulating attention competition...")
    print("  - Strong stimuli win competition")
    print("  - Weaker stimuli receive inhibition")
    print("  - Top-down goals bias selection")
    
    # In real simulation, attention would:
    # 1. Process competing sensory inputs
    # 2. Apply lateral inhibition
    # 3. Select winners based on strength and goals
    # 4. Focus processing resources on selected inputs
    
    print("✓ Attention system demonstration complete")
else:
    print("⚠ Attention system not available")
```

## Step 5: Advanced Configuration

Intermediate users should understand how to fine-tune configuration for better performance.

```python
# Advanced configuration
print("\nConfiguring brain for intermediate use...")
print("-" * 40)

# Create brain with custom configuration
config = pynlm.createDefaultConfig()

# Fine-tune parameters for learning
config.set("brain.neuron_count", 500)           # More neurons for complexity
config.set("brain.synapse_density", 0.05)       # Balanced connectivity
config.set("plasticity.stdp.learning_rate", 0.01)  # Higher learning rate
config.set("neuromod.dopamine.scale", 1.0)      # Standard reward scaling
config.set("neuromod.curiosity.enable", True)  # Enable curiosity
config.set("neuromod.novelty.enable", True)     # Enable novelty detection

# Create brain with this configuration
brain = pynlm.createBrain(config)
brain.initialize()

print("✓ Brain configured with:")
print(f"  - {config.get('brain.neuron_count')} neurons")
print(f"  - {config.get('brain.synapse_density') * 100}% synapse density")
print(f"  - STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
```

## Step 6: Performance Monitoring

Track how your brain performs over time.

```python
# Performance monitoring
print("\nSetting up performance monitoring...")
print("-" * 40)

import time

# Performance metrics
metrics = {
    "spike_count": [],
    "firing_rate": [],
    "step_time": [],
    "memory_usage": [],
    "curiosity": [],
    "novelty": []
}

brain.initialize()

# Run monitored simulation
for step in range(100):
    start_time = time.time()
    
    # Run one simulation step
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    end_time = time.time()
    metrics["step_time"].append(end_time - start_time)
    metrics["spike_count"].append(brain.getTotalSpikeCount())
    metrics["firing_rate"].append(brain.getAverageFiringRate())
    metrics["curiosity"].append(agent.getCuriosityLevel())
    metrics["novelty"].append(agent.getNoveltyLevel())
    
    # Record memory usage
    wm = brain.getWorkingMemory()
    if wm:
        metrics["memory_usage"].append({
            "capacity": wm.getCapacity(),
            "active": wm.getActiveTraces(),
            "activity": wm.getMemoryActivity()
        })

# Analyze performance
print("\nPerformance Analysis:")
print("-" * 40)
print(f"Total steps: {len(metrics['step_time'])}")
print(f"Average step time: {sum(metrics['step_time'])/len(metrics['step_time'])*1000:.1f} ms")
print(f"Total spikes: {metrics['spike_count'][-1]}")
print(f"Final firing rate: {metrics['firing_rate'][-1]:.2f} Hz")
print(f"Average curiosity: {sum(m for m in metrics['curiosity'])/len(metrics['curiosity']):.3f}")
print(f"Average novelty: {sum(m for m in metrics['novelty'])/len(metrics['novelty']):.3f}")

if wm:
    print(f"Memory capacity used: {wm.getActiveTraces()}/{wm.getCapacity()}")

print("✓ Performance monitoring complete")
```

## Step 7: Complex Agent Behavior

Create an agent that exhibits more sophisticated behavior.

```python
# Complex agent behavior
print("\nBuilding complex agent behavior...")
print("-" * 40)

# Create sophisticated agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Enable all advanced systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Create a simple goal-seeking agent
print("Creating goal-seeking agent...")
agent.setGoal([1.0, 1.0])  # Goal position

# Run complex behavior
for step in range(150):
    world.update(0.1)
    
    # Sense environment
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Plan and think
    brain.step(step)
    
    # Make decision with goal in mind
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Check if goal reached
    if world.getAgentBody().getPosition()[0] > 15 and world.getAgentBody().getPosition()[1] > 15:
        print(f"✓ Goal approached at step {step}")
        break

print("✓ Complex agent behavior complete")
```

## Step 8: Save and Load Sessions

Save your trained brains for later use.

```python
# Save brain session
print("\nSaving brain session...")
print("-" * 40)

from pynlm import saveBrainSession, loadBrainSession

# Save the current brain
brain.save("trained_brain.bin")

# Save with metadata
metadata = saveBrainSession(
    "trained_brain.bin",
    "goal_seeker_v1",
    brain,
    include_config=True
)

print("✓ Brain saved successfully")
print(f"  - Filename: trained_brain.bin")
print(f"  - Session name: goal_seeker_v1")
print(f"  - Timestamp: {metadata.get('timestamp', 'N/A')}")
print(f"  - Include config: {metadata.get('include_config', 'N/A')}")

# Load the saved brain (uncomment to test)
# loaded_brain, loaded_metadata = loadBrainSession("trained_brain.bin")
# print(f"✓ Brain loaded successfully")
# print(f"  - Session name: {loaded_metadata.get('session_name', 'N/A')}")
```

## Step 9: Experiment Framework

Use NLM's built-in experiment framework for scientific evaluation.

```python
# Experiment framework
print("\nSetting up experiment framework...")
print("-" * 40)

# Create experiment configuration
experiment_config = {
    "name": "GoalSeekingBehavior",
    "brain_template": config,
    "environment_config": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8,
        "reward_decay": 0.9
    },
    "max_steps": 100,
    "metrics_config": {
        "spike_rate": True,
        "memory_retention": True,
        "goal_achievement": True,
        "learning_rate": True
    }
}

print("✓ Experiment configuration created")
print(f"  - Experiment name: {experiment_config['name']}")
print(f"  - Max steps: {experiment_config['max_steps']}")
print(f"  - Environment size: {experiment_config['environment_config']['width']}x{experiment_config['environment_config']['height']}")
print(f"  - Metrics: {list(experiment_config['metrics_config'].keys())}")
```

## Step 10: Common Intermediate Patterns

### Pattern 1: Learning Curve Analysis

```python
def analyze_learning_curve(config, num_episodes=10):
    """Analyze how learning progresses over episodes."""
    import numpy as np
    
    learning_data = []
    
    for episode in range(num_episodes):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run episode
        for step in range(100):
            # (simulation code would go here)
            pass
        
        # Record metrics
        learning_data.append({
            "episode": episode,
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate(),
            "developmental_stage": brain.getDevelopmentalStage().name
        })
    
    return learning_data
```

### Pattern 2: Parameter Optimization

```python
def optimize_parameters(base_config, param_grid):
    """Test different parameter combinations."""
    results = []
    
    for params in param_grid:
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        for key, value in params.items():
            config.set(key, value)
        
        # Test configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        # Record results
        results.append({
            "params": params,
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate()
        })
    
    return results
```

## Step 11: Troubleshooting Intermediate Issues

### "Learning isn't working"
- **Check neuromodulation**: Ensure `enableRewardModulation(True)` is called
- **Verify reward signals**: Check that rewards are being applied
- **Review configuration**: Learning rate might be too low

### "Performance is too slow"
- **Reduce complexity**: Fewer neurons, smaller world
- **Profile with `profileBrain()`**: Identify bottlenecks
- **Check for infinite loops**: Ensure simulations terminate

### "Working memory not updating"
- **Check initialization**: Ensure working memory is initialized with brain reference
- **Verify capacity settings**: Make sure capacity is reasonable
- **Check decay rate**: Decay rate might be too high

### "Attention system not selective"
- **Verify implementation**: Attention might require specific setup
- **Check competition parameters**: Inhibition strength, thresholds
- **Review sensory input**: Weak inputs won't compete well

## Step 12: What's Next?

You've completed the intermediate tutorial! You now know:

1. How to use neuromodulation systems
2. Working memory and attention mechanisms
3. Performance monitoring and analysis
4. Advanced configuration
5. Session management
6. Experiment framework

**Ready for advanced work?** Check out the Advanced Tutorial for:

- Phase 4 cognitive experiments
- Prediction systems
- Social learning
- Episodic memory
- Concept formation
- Planning and control

## Quick Reference for Intermediate Users

| What you want | Code |
|--------------|------|
| Enable reward learning | `agent.enableRewardModulation(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |
| Enable development | `agent.enableDevelopment(True)` |
| Get working memory | `wm = brain.getWorkingMemory()` |
| Get attention | `attn = brain.getAttention()` |
| Profile performance | `metrics = profileBrain(brain, num_steps=100)` |
| Save session | `saveBrainSession("brain.bin", "name", brain)` |
| Load session | `brain, meta = loadBrainSession("brain.bin")` |

Remember: Intermediate work is about combining systems and understanding trade-offs. Start simple, add complexity gradually, and measure everything.

Enjoy your journey into advanced brain simulation!

That's it! You're now an intermediate NLM user with the skills to create sophisticated learning agents and analyze their performance. Start building complex behaviors and keep experimenting with different configurations.

Time to tackle the Advanced Tutorial for the full power of NLM!

Happy simulating! 🚀

## Files Created

- `beginner_tutorial.md`: Complete beginner guide
- `intermediate_tutorial.md`: Complete intermediate guide
- `API_REFERENCE.md`: Comprehensive API documentation
- `README.md`: Project overview
- `easy_usage.md`: Simple usage guide

You now have a complete documentation suite for NLM at different skill levels! 🎉