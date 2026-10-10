# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## New Improved API

This guide now uses the **improved NLM Python API** which provides a more user-friendly interface with enhanced features like statistics collection, advanced monitoring, and simplified configuration.

The improved API includes:
- High-level brain simulation methods
- Built-in statistics and monitoring
- Enhanced world interaction
- Advanced agent capabilities
- Comprehensive error handling
- Development stage tracking

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Using New API)

```python
import pynlm

# Step 1: Make a brain using improved API
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Enable statistics collection
brain.enable_stats_collection()

# Step 3: Turn it on and make it think for 100 steps
brain.initialize()
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Statistics:", brain.get_stats().get_summary())
```

### Example 2: Brain in a Simple World (Using Enhanced API)

```python
import pynlm

# Create everything using improved API
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("World status:", world.getAgentStatus())
```

### Example 2b: Complete Agent with Advanced Features

```python
import pynlm

# Setup with advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run with automatic monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Advanced agent simulation complete!")
```

### Example 3: Silent Brain (Just Neurons) - Using Improved API

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Statistics: {brain.get_stats().get_summary()}")
```

### Example 4: Complete Agent with Monitoring

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.enable_stats_collection()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
print(f"Final statistics: {brain.get_stats().get_summary()}")
```

### Example 5: Developmental Stage Monitoring

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()

print("Developmental stage monitoring:")
print("=" * 50)

for i in range(20):
    brain.step(i)
    
    # Get developmental stage info
    dev_info = brain.get_developmental_stage_info()
    print(f"Step {i}: {dev_info['stage_name']}")
    print(f"  Progress: {dev_info['progress']:.1%}")
    print(f"  Description: {dev_info['description']}")
    
    # Log stage changes
    if dev_info['stage_changed']:
        print(f"  >>> STAGE CHANGED! <<<")
```

### Example 6: Advanced Agent with Episode Control

```python
import pynlm

# Setup advanced agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run multiple episodes
for episode in range(3):
    print(f"\n--- Episode {episode + 1} ---")
    
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}: Reward={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")

print("\nEpisode monitoring complete!")
print(f"Total statistics: {brain.get_stats().get_summary()}")
```

### Example 7: Quick Reference (New API)

| What you want | New Code |
|--------------|----------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Enable stats | `brain.enable_stats_collection()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.get_stats().get_summary()` |
| See world status | `world.get_agent_status()` |
| Make action | `action = agent.decodeMotorCommand()` |
| Run complete simulation | `pynlm.runAdvancedAgentSimulation(3, 500)` |

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation with Statistics

```python
def run_simulation(brain, world, agent, num_steps):
    # Enable statistics collection
    brain.enable_stats_collection()
    
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
        
        # 7. Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain.get_stats().get_summary() if brain.get_stats() else None

# Use it like this:
# brain = pynlm.createBrain(config)
# brain.enable_stats_collection()
# world = pynlm.createSimpleWorld()
# world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
# agent = pynlm.createAgentBrain(brain)
# stats = run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Enhanced Brain Statistics

```python
def monitor_brain(brain, interval=10):
    """Monitor brain statistics at specified intervals."""
    if not brain.get_stats():
        brain.enable_stats_collection()
    
    def collect_stats(brain_obj, step_result):
        if step_result['step_number'] % interval == 0:
            print(f"Step {step_result['step_number']}:")
            print(f"  Firing neurons: {step_result['firing_neuron_count']}")
            print(f"  Total spikes: {step_result['spike_count']}")
            print(f"  Firing rate: {step_result['firing_rate_hz']:.3f} Hz")
            print(f"  Developmental stage: {step_result['current_developmental_stage'].name}")
            print(f"  Stage progress: {step_result['development_progress']:.1%}")
    
    return collect_stats

# Use it like this:
# brain = pynlm.createBrain(config)
# brain.enable_stats_collection()
# brain.initialize()
# monitor = monitor_brain(brain, interval=20)
# for i in range(100):
#     brain.step(i)
#     monitor(brain, brain.step(i, i * 0.001))
```

### Pattern 3: Enable Advanced Features and Monitoring

```python
def setup_advanced_agent(brain, world, enable_all_features=True):
    """Setup agent with advanced features and monitoring."""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_all_features:
        # Enable all learning capabilities
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # Add development monitoring
    developmental_stages = brain.get_developmental_stage_info()
    
    return agent, developmental_stages

# Use it like this:
# brain = pynlm.createBrain(config)
# brain.enable_stats_collection()
# world = pynlm.createSimpleWorld()
# agent, stages = setup_advanced_agent(brain, world)
# print(f"Developmental stages: {[s.name for s in stages.keys()]}")
```

### Pattern 4: Run Complete Episodes with Monitoring

```python
def run_episode_with_monitoring(agent, brain, world, max_steps=500):
    """Run a complete episode with detailed monitoring."""
    print("Starting episode...")
    
    episode_stats = {
        'steps_completed': 0,
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'max_novelty': 0.0,
        'stage_changes': 0,
        'world_states': [],
    }
    
    for step in range(max_steps):
        # Update world and get status
        world_status = world.update(0.1)
        episode_stats['world_states'].append(world_status)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinking
        brain.step(step)
        
        # Motor control
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation
        reward = world_status.get('agent_energy', 0.0) / 10.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track statistics
        episode_stats['total_reward'] += reward
        episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], agent.getCuriosityLevel())
        episode_stats['max_novelty'] = max(episode_stats['max_novelty'], agent.getNoveltyLevel())
        episode_stats['steps_completed'] = step + 1
        
        # Log progress
        if step % 50 == 0:
            print(f"  Step {step}: Reward={reward:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print(f"Episode completed: {episode_stats['steps_completed']} steps, "
          f"{episode_stats['total_reward']:.3f} total reward")
    
    return episode_stats

# Use it like this:
# brain = pynlm.createBrain(config)
# brain.enable_stats_collection()
# world = pynlm.createSimpleWorld()
# agent = pynlm.createAgentBrain(brain)
# agent.initialize(world)
# episode_stats = run_episode_with_monitoring(agent, brain, world, 200)
```

### Pattern 5: Developmental Stage Monitoring

```python
def monitor_development(brain, check_interval=5):
    """Monitor developmental stages over time."""
    developmental_info = brain.get_developmental_stage_info()
    
    def on_step(brain_obj, step_result):
        if step_result['step_number'] % check_interval == 0:
            dev_info = brain_obj.get_developmental_stage_info()
            stage = dev_info['stage']
            progress = dev_info['progress']
            
            print(f"  Development: {stage.name} ({progress:.1%})")
            print(f"    Description: {dev_info['description']}")
            print(f"    Critical period: {dev_info['is_critical_period']}")
            print(f"    Maturation: {dev_info['is_maturation']}")
            
            if step_result['stage_changed']:
                print(f"    >>> STAGE CHANGED! <<<")
    
    return on_step

# Use it like this:
# brain = pynlm.createBrain(config)
# brain.enable_stats_collection()
# monitor = monitor_development(brain, check_interval=10)
# for i in range(50):
#     brain.step(i)
#     monitor(brain, brain.step(i))
```

---

## What Each Part Does

### `brain.enable_stats_collection()`
Enable detailed statistics collection for monitoring brain performance.

### `brain.get_stats()`
Get the brain statistics collector.

### `brain.get_developmental_stage_info()`
Get comprehensive information about current developmental stage.

### `brain.get_status()`
Get comprehensive brain status information including statistics and development progress.

### `brain.configure(**kwargs)`
Configure brain parameters with keyword arguments.

### `brain.step(step_number, timestep=None)`
Execute a simulation step with enhanced error handling and statistics.

### `brain.run_steps(num_steps, timestep=None, callbacks=None)`
Run multiple simulation steps with optional callbacks.

### `brain.reset()`
Reset brain to initial state with statistics reset.

### Enhanced World Methods

#### `world.configure(**kwargs)`
Configure world parameters with validation and method chaining.

#### `world.update(timestep=None)`
Update world state and return comprehensive status information.

#### `world.get_sensory_percept()`
Get enhanced sensory percept with additional metadata and validation.

#### `world.get_agent_status()`
Get detailed agent status information.

#### `world.add_object(x, y, obj_type, value=1.0, radius=0.5)`
Add a world object with validation.

#### Enhanced Agent Methods

#### `agent.run_episode(max_steps=1000, enable_automatic_control=True)`
Run a complete agent episode with automatic monitoring.

#### `agent.run_multiple_episodes(num_episodes, max_steps_per_episode=1000)`
Run multiple episodes with progress reporting.

#### `agent.apply_reward_modulation(reward, predicted_reward=0.0)`
Apply reward modulation with enhanced tracking.

### Quick Reference (New API)

| What you want | New Code |
|--------------|----------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Enable stats | `brain.enable_stats_collection()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.get_stats().get_summary()` |
| See world status | `world.get_agent_status()` |
| Make action | `action = agent.decodeMotorCommand()` |
| Run complete simulation | `pynlm.runAdvancedAgentSimulation(3, 500)` |
| Monitor development | `brain.get_developmental_stage_info()` |
| Run episode | `agent.run_episode(100)` |

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.configure(**kwargs)`
Configure brain parameters using keyword arguments for enhanced flexibility.

### `brain.enable_stats_collection()`
Enable comprehensive statistics collection for brain monitoring.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number, timestep=None)`
Makes the brain process one moment in time with enhanced error handling and statistics collection.

### `world.configure(**kwargs)`
Configures world parameters with validation and method chaining support.

### `world.update(timestep)`
Updates the virtual world by `timestep` seconds with enhanced status reporting.

### `world.get_sensory_percept()`
Gets sensory information (vision, touch, etc.) with additional metadata and validation.

### `agent.processSensoryInput(percept)`
Gives sensory information to the brain with enhanced processing.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `brain.get_stats()`
Get the brain statistics collector for detailed monitoring.

### `brain.get_developmental_stage_info()`
Get comprehensive information about the current developmental stage including progress and description.

### `agent.run_episode(max_steps)`
Run a complete agent episode with automatic monitoring and statistics collection.

## Mini Projects

### Project 1: Silent Brain (Just Neurons) - Using New API

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Statistics: {brain.get_stats().get_summary()}")
```

### Project 2: Brain Watching a World - Using Enhanced API

```python
import pynlm

# Setup with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.enable_stats_collection()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps with monitoring
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("World status:", world.getAgentStatus())
print("Brain statistics:", brain.get_stats().get_summary())
```

### Project 3: Complete Agent with Advanced Features

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.enable_stats_collection()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with advanced monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
print(f"Final statistics: {brain.get_stats().get_summary()}")
```

---

## Quick Reference

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

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
