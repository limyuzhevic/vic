# NLM - Enhanced Usage Guide for Advanced Users

## Quick Reference (Copy & Paste)

### Example 1: Basic Brain Setup
```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Complete Agent Setup
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain and world
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete simulation for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Advanced Agent with Debug Monitoring
```python
import pynlm
import time

# Setup with custom configuration
config = pynlm.createDefaultConfig()
# You can modify config parameters here:
# config.set("neuron_count", 2000)  # More neurons
# config.set("region_count", 2)     # Multiple regions

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Monitoring and statistics collection
stats = {
    'steps': 0,
    'total_reward': 0.0,
    'max_firing': 0,
    'novelty_events': 0,
    'curiosity_events': 0
}

# Run simulation with monitoring
start_time = time.time()
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect stats
    stats['steps'] += 1
    stats['total_reward'] += world.computeReward(agent.getBrain()->getRegions()[0].get())
    stats['max_firing'] = max(stats['max_firing'], brain.getFiringNeuronCount())
    if agent.getNoveltyLevel() > 0.5:
        stats['novelty_events'] += 1
    if agent.getCuriosityLevel() > 0.3:
        stats['curiosity_events'] += 1

end_time = time.time()

# Print detailed results
print("=== Advanced Agent Simulation Complete ===")
print(f"Simulation time: {end_time - start_time:.2f}s")
print(f"Steps executed: {stats['steps']}")
print(f"Average reward: {stats['total_reward']/stats['steps']:.3f}")
print(f"Max firing neurons: {stats['max_firing']}")
print(f"Novelty events: {stats['novelty_events']}")
print(f"Curiosity events: {stats['curiosity_events']}")
print(f"Brain developmental stage: {brain.getDevelopmentalStage()}")
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 4: Checkpoint and Save/Load
```python
import pynlm
import os

# Create and train brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for some time
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Save checkpoint for later recovery
checkpoint_path = "./nlm_checkpoint.bin"
if brain.save(checkpoint_path):
    print("Checkpoint saved successfully!")
    
    # Create new brain and load saved state
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    
    if brain2.load(checkpoint_path):
        print("Checkpoint loaded successfully!")
        print("Loaded brain has:", brain2.getTotalNeuronCount(), "neurons")
        print("Loaded brain has:", brain2.getTotalSynapseCount(), "synapses")
    else:
        print("Failed to load checkpoint")
else:
    print("Failed to save checkpoint")

# Cleanup
if os.path.exists(checkpoint_path):
    os.remove(checkpoint_path)
```

## Common Patterns

### Pattern 1: Run a Simulation with Monitoring
```python
def run_simulation_with_monitoring(brain, world, agent, num_steps):
    """Run simulation and collect performance metrics"""
    metrics = {
        'rewards': [],
        'firing_rates': [],
        'novelty_levels': [],
        'curiosity_levels': [],
        'developmental_stages': []
    }
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        metrics['rewards'].append(world.computeReward(agent.getBrain()->getRegions()[0].get()))
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['developmental_stages'].append(brain.getDevelopmentalStage())
    
    # Calculate statistics
    return {
        'avg_reward': sum(metrics['rewards']) / len(metrics['rewards']),
        'avg_firing_rate': sum(metrics['firing_rates']) / len(metrics['firing_rates']),
        'avg_novelty': sum(metrics['novelty_levels']) / len(metrics['novelty_levels']),
        'avg_curiosity': sum(metrics['curiosity_levels']) / len(metrics['curiosity_levels']),
        'final_development_stage': metrics['developmental_stages'][-1],
        'total_steps': num_steps
    }

# Use it like this:
# config = pynlm.createDefaultConfig()
# brain = pynlm.createBrain(config)
# brain.initialize()
# world = pynlm.createSimpleWorld()
# agent = pynlm.createAgentBrain(brain)
# results = run_simulation_with_monitoring(brain, world, agent, 1000)
# print(f"Average reward: {results['avg_reward']:.3f}")
```

### Pattern 2: Get Brain Stats
```python
# Add these inside your simulation loop:
def collect_brain_stats(brain, agent):
    """Collect and return comprehensive brain statistics"""
    stats = {
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'active_neurons': brain.getActiveNeuronCount(),
        'total_neurons': brain.getTotalNeuronCount(),
        'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'dopamine_level': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    }
    return stats

# Use it like this:
# stats = collect_brain_stats(brain, agent)
# print(f"Firing neurons: {stats['neurons_firing']}")
# print(f"Total spikes: {stats['total_spikes']}")
# print(f"Avg firing rate: {stats['avg_firing_rate']:.3f}")
```

### Pattern 3: Enable Brain Features
```python
# These are all optional features you can enable:

def setup_agent_features(agent, enable_everything=False):
    """Setup agent with all desired features enabled"""
    if enable_everything:
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    else:
        # Enable only core features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Uncomment for advanced features
        # agent.enableStructuralPlasticity(True)
        # agent.enableDevelopment(True)
    
    return agent

# Use it like this:
# agent = pynlm.createAgentBrain(brain)
# agent = setup_agent_features(agent, enable_everything=True)
```

### Pattern 4: Create Custom World
```python
def create_custom_world(config, width=10, height=10, vision_width=8, vision_height=8):
    """Create a world with custom settings"""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    
    # You can modify world properties here:
    # world.setGravity(9.8)  # If gravity is supported
    # world.addResource(5, 5, 1.0)  # Add resource at position
    # world.addHazard(8, 8, -0.5)  # Add hazard at position
    
    world.reset()
    return world

# Use it like this:
# world = create_custom_world(config, width=15, height=15, vision_width=16, vision_height=16)
```

## Quick Reference Table

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Save brain state | `brain.save("./checkpoint.bin")` |
| Load brain state | `brain.load("./checkpoint.bin")` |

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (Always needed!)
- Try increasing the number of steps (more time for neurons to fire)

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`? (See your simulation loop)
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that you have at least one region with neurons

**"Brain seems stuck"**
- Check your simulation loop order
- Make sure you're injecting sensory input properly
- Try enabling curiosity for more exploration

**"Checkpoint save/load issues"**
- Ensure you have write permissions in the target directory
- Try using absolute paths like `os.path.abspath("./checkpoint.bin")`

## Advanced Configuration Options

For advanced users who want to customize their NLM brains:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("neuron_count", 2000)  # More neurons = more complex brain
config.set("region_count", 2)     # Multiple regions = different brain areas
config.set("connection_probability", 0.15)  # More connections
config.set("stdp_ltp_weight", 0.02)  # Stronger LTP
config.set("stdp_ltd_weight", 0.018)  # Stronger LTD
config.set("synaptogenesis_rate", 0.0002)  # More structural growth
config.set("pruning_rate", 0.00005)  # Less pruning

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Brain is now ready with your custom settings!
```

## Next Steps

When you're comfortable with NLM:

1. **Read `HOW_TO_USE.md`** for more technical details
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. **Check out the Phase 6 experiments** in `src/experiments/` for advanced examples
5. **Join the NLM community** to share your experiences and get help

That's it! You're now ready to use NLM like a pro. The enhanced examples show you how to set up everything correctly, monitor your agent's performance, save progress, and create custom simulations.

Happy experimenting with your artificial brain! 🧠✨
