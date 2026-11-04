# NLM - Easy Usage Guide for Beginners (Enhanced Edition)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

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

### Example 2: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything with improved convenience
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with enhanced configuration
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent with world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

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
print("Curiosity level:", agent.getCuriosityLevel())
print("Development stage:", brain.getDevelopmentalStage())
```

### Example 3: Advanced Usage with Error Handling

```python
import pynlm

def run_safe_simulation(steps=1000):
    """Run a simulation with proper error handling."""
    try:
        # Create and configure
        config = pynlm.createDefaultConfig()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward from world (placeholder logic)
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
                
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
        return brain, world, agent
        
    except Exception as e:
        print(f"Simulation error: {e}")
        raise

# Run the simulation
brain, world, agent = run_safe_simulation(500)
print("Advanced simulation complete!")
```

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
print("Your brain has", brain.getTotalSynapseCount(), "synapses!")
```

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Monitoring

```python
def run_monitored_simulation(brain, world, agent, num_steps):
    stats = {
        'steps_completed': 0,
        'neurons_firing': [],
        'curiosity_levels': [],
        'rewards': []
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
        
        # 7. Monitor and record
        stats['steps_completed'] += 1
        stats['neurons_firing'].append(brain.getFiringNeuronCount())
        stats['curiosity_levels'].append(agent.getCuriosityLevel())
        
        # 8. Print progress every 10 steps
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"Curiosity: {agent.getCuriosityLevel():.2f}")
    
    return stats

# Use it like this:
stats = run_monitored_simulation(brain, world, agent, 1000)
print(f"Simulation completed: {stats['steps_completed']} steps")
print(f"Average neurons firing: {sum(stats['neurons_firing'])/len(stats['neurons_firing']):.1f}")
```

### Pattern 2: Get Comprehensive Brain Stats

```python
def get_brain_stats(brain, agent=None):
    """Get comprehensive statistics about brain and agent state."""
    stats = {
        'neurons': {
            'total': brain.getTotalNeuronCount(),
            'active': brain.getActiveNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'region_count': brain.getRegionCount(),
            'developmental_stage': str(brain.getDevelopmentalStage())
        }
    }
    
    if agent:
        stats['agent'] = {
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': str(agent.getDevelopmentalStage()),
            'enabled_features': {
                'reward_modulation': agent.isRewardModulationEnabled(),
                'structural_plasticity': agent.isStructuralPlasticityEnabled(),
                'development': agent.isDevelopmentEnabled(),
                'curiosity': agent.isCuriosityEnabled()
            }
        }
    
    return stats

# Example usage:
brain_stats = get_brain_stats(brain, agent)
print(f"Total neurons: {brain_stats['neurons']['total']}")
print(f"Active neurons: {brain_stats['neurons']['active']}")
print(f"Developmental stage: {brain_stats['neurons']['developmental_stage']}")

if agent:
    print(f"Curiosity level: {brain_stats['agent']['curiosity_level']:.2f}")
    print(f"Neuromodulation level: {brain_stats['agent']['neuromodulation_level']:.2f}")
```

### Pattern 3: Advanced Learning Configuration

```python
def configure_advanced_learning(config, agent):
    """Configure advanced learning parameters."""
    # Configure brain
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.v_thresh", -55.0)  # Lower threshold = easier to fire
    config.set("brain.v_rest", -65.0)    # Higher resting potential
    
    # Configure plasticity
    config.set("plasticity.stdp.learning_rate", 0.0015)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    
    # Configure neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced learning configured successfully!")

# Use it:
configure_advanced_learning(config, agent)
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- **Enhanced**: Now accepts configuration objects directly and validates parameters

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- **Enhanced**: Now includes optional validation and progress reporting

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- **Enhanced**: Added support for optional timestamp and logging

### `world.update(time)`
Updates the virtual world by `time` seconds.
- **Enhanced**: Now supports delta-time for more precise control

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.
- **Enhanced**: Now includes validation and preprocessing options

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.
- **Enhanced**: Now includes post-processing and smoothing options

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Neurons created: {brain.getTotalNeuronCount()}")
print(f"Synapses created: {brain.getTotalSynapseCount()}")
```

### Project 2: Brain Watching a World (Enhanced)

```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Enhanced monitoring
    if i % 5 == 0:
        print(f"Step {i}: Firing rate {brain.getAverageFiringRate():.2f}, "
              f"Curiosity {agent.getCuriosityLevel():.2f}")

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Final curiosity: {agent.getCuriosityLevel():.2f}")
```

### Project 3: Complete Agent with Learning (Enhanced)

```python
import pynlm

# Enhanced setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning with custom configuration
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run agent with enhanced monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        stats = get_brain_stats(brain, agent)
        print(f"Step {step}: {stats['neurons']['firing']} neurons firing, "
              f"Curiosity: {stats['agent']['curiosity_level']:.3f}, "
              f"Development: {stats['neurons']['developmental_stage']}")

print("Agent simulation complete!")

# Save final brain state (new feature)
brain.save("final_brain_state.bin")
```

### Project 4: Advanced Error Handling (New)

```python
import pynlm

def robust_simulation_runner(config, world_config, steps=1000):
    """Robust simulation runner with comprehensive error handling."""
    results = {
        'success': False,
        'brain': None,
        'world': None,
        'agent': None,
        'errors': [],
        'stats': {}
    }
    
    try:
        # Step 1: Create brain
        results['brain'] = pynlm.createBrain(config)
        results['brain'].initialize()
        
        # Step 2: Create world
        results['world'] = pynlm.createSimpleWorld()
        results['world'].configure(**world_config)
        results['world'].reset()
        
        # Step 3: Create agent
        results['agent'] = pynlm.createAgentBrain(results['brain'])
        results['agent'].initialize(results['world'])
        
        # Step 4: Run simulation
        for step in range(steps):
            try:
                results['world'].update(0.1)
                
                percept = results['world'].getSensoryPercept()
                results['agent'].processSensoryInput(percept)
                results['brain'].step(step)
                
                action = results['agent'].decodeMotorCommand()
                results['world'].applyMotorCommand(action, results['world'].getSimulationTime())
                
                # Record stats
                if step % 100 == 0:
                    print(f"Progress: Step {step}/{steps}")
                    
            except Exception as e:
                results['errors'].append(f"Step {step} error: {e}")
                # Continue with next step
                continue
        
        results['success'] = True
        results['stats'] = get_brain_stats(results['brain'], results['agent'])
        
    except Exception as e:
        results['errors'].append(f"Setup error: {e}")
    
    return results

# Use the robust runner
world_config = {
    'width': 20,
    'height': 20,
    'visionWidth': 8,
    'visionHeight': 8
}

results = robust_simulation_runner(config, world_config, 500)

if results['success']:
    print("✅ Simulation completed successfully!")
    print(f"Total neurons: {results['stats']['neurons']['total']}")
    print(f"Average firing rate: {results['stats']['neurons']['average_firing_rate']:.2f}")
else:
    print(f"❌ Simulation failed with {len(results['errors'])} errors:")
    for error in results['errors']:
        print(f"  - {error}")
```

---

## Quick Reference (Enhanced)

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

**Enhanced Features:**
- `brain.save(filename)` - Save brain state
- `brain.load(filename)` - Load brain state
- `agent.enableFeature(True/False)` - Enable/disable features
- `config.set(key, value)` - Set configuration values
- `config.get_int(key, default)` - Get typed configuration values
- `world.configure(**kwargs)` - Enhanced world configuration
- `get_brain_stats(brain, agent)` - Get comprehensive statistics

---

## Troubleshooting (Enhanced)

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try enabling curiosity: `agent.enableCuriosity(True)`
- Check world updates: `world.update()` must be called

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has motor neurons: `agent.getMotorOutputSize()`

### "Everything is 0"
- Brains need time to "warm up" - try more steps (at least 100)
- Some neurons need input to fire - make sure sensory input is connected
- Check if development is enabled: `agent.enableDevelopment(True)`

### "Performance is slow"
- Try reducing neuron count: `config.set("brain.neuron_count", 500)`
- Disable curiosity for faster simulation: `agent.enableCuriosity(False)`
- Use batch processing for multiple steps

### "Memory issues"
- Save brain state periodically: `brain.save("checkpoint.bin")`
- Use smaller world dimensions
- Monitor memory usage: `import psutil; psutil.Process().memory_info().rss`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the enhanced examples with error handling and monitoring
5. Create your own advanced projects using the new API features

**New Enhanced Features Available:**
- Comprehensive error handling
- Robust simulation runners
- Statistics collection
- Configuration validation
- Progress monitoring
- State persistence
- Feature enable/disable
- Type-safe configuration access

That's it! You're now ready to use NLM with the enhanced Python bindings. The improved API makes it easier to create complex simulations with better error handling and more control over the simulation behavior.