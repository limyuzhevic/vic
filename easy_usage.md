# NLM - Enhanced Easy Usage Guide

## What's New in This Version

This version introduces significant improvements to the NLM Python API, including:

- **Advanced factory functions** for creating complete learning environments
- **Memory persistence utilities** for saving and loading brain states
- **Comprehensive configuration tools** for programmatic setup
- **Version tracking** and enhanced error handling
- **Production-ready examples** with advanced usage patterns

## Installation

The installation process remains the same, but now you can experience the enhanced Python bindings:

```bash
pip install -e .
```

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Advanced Factory Function)

```python
import pynlm

# Create a brain with specific neuron count and connection probability
brain = pynlm.createBrainWithNeurons(500, 0.15)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Initialize and run
brain.initialize()
for i in range(100):
    brain.step(i)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Learning Environment (New Utility)

```python
import pynlm

# Create a complete learning environment with all systems
brain, world, agent = pynlm.createLearningEnvironment(
    worldSize=20,
    visionRange=8,
    enableCuriosity=True,
    enableRewardModulation=True
)

print(f"Setup complete: {brain.getTotalNeuronCount()} neurons")
print(f"Agent enabled: Curiosity={agent.isCuriosityEnabled()}, Reward={agent.isRewardModulationEnabled()}")
```

### Example 3: Save/Load Brain State (Production Ready)

```python
import pynlm

# Create and train a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some training
for step in range(500):
    brain.step(step)

# Save the learned brain state
save_success = pynlm.saveBrainState(brain, "my_brain.pkl")
print(f"Brain saved: {save_success}")

# Create a fresh brain and load the saved state
fresh_brain = pynlm.loadBrainState("my_brain.pkl")
print(f"Loaded brain has {fresh_brain.getTotalNeuronCount()} neurons")

# Continue training from saved state
for step in range(100):
    fresh_brain.step(step + 500)

print(f"Total spikes after load: {fresh_brain.getTotalSpikeCount()}")
```

### Example 4: Advanced Configuration (Programmatic Setup)

```python
import pynlm

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.12)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Get configuration summary
print(config.summary())
```

## The 3 Things You Need to Know (Enhanced)

1. **Brain** - The virtual brain that thinks (now with configurable creation functions)
2. **World** - The environment the brain lives in (with enhanced methods)
3. **Agent** - The bridge connecting brain to world (with persistence utilities)

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Learning

```python
def run_learning_simulation(world_size=15, enable_curiosity=True, enable_reward=True):
    """Run a complete learning simulation."""
    # Create the learning environment
    brain, world, agent = pynlm.createLearningEnvironment(
        worldSize=world_size,
        enableCuriosity=enable_curiosity,
        enableRewardModulation=enable_reward
    )
    
    # Run simulation
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and think
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act in world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent
```

### Pattern 2: Advanced Brain Statistics

```python
def analyzeBrain(brain, world, agent):
    """Get comprehensive brain statistics."""
    stats = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "development_stage": brain.getDevelopmentalStage(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError(),
        "neuromodulation_level": agent.getNeuromodulationLevel()
    }
    
    print("=== Brain Analysis ===")
    for key, value in stats.items():
        print(f"{key:25s}: {value}")
    
    return stats
```

### Pattern 3: Enable Advanced Features

```python
def setupAdvancedFeatures(brain, world, agent):
    """Enable all available advanced features."""
    # Enable all learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Configure brain for development
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    brain.setLearnedPreference(0.5)        # Set initial preference
    
    return agent
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with configuration.
- **New**: `pynlm.createBrainWithNeurons(count, probability)` for quick creation
- **New**: `pynlm.createCompleteAgent(brain, world)` for pre-initialized agents

### `pynlm.createLearningEnvironment(worldSize, visionRange, enableCuriosity, enableRewardModulation)`
Creates a complete learning setup with all systems enabled.
- **All-in-one**: Brain, world, and agent all configured for learning

### `pynlm.saveBrainState(brain, filepath)` / `pynlm.loadBrainState(filepath)`
Save and load brain states for persistent learning.
- **Production ready**: Use to save trained models and continue learning

### `pynlm.createDefaultConfig()`
Creates a default configuration object.
- **Enhanced**: Can be modified programmatically for custom scenarios

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain with Custom Configuration

```python
import pynlm

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.08)

brain = pynlm.createBrain(config)
brain.initialize()

# Run and analyze
for i in range(20):
    brain.step(i)

print(f"Silent brain test done! {brain.getTotalNeuronCount()} neurons")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Project 2: Brain Watching a World with Persistence

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
print(f"Watched world for 50 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")

# Save the experienced brain
save_success = pynlm.saveBrainState(brain, "watched_brain.pkl")
print(f"Brain saved: {save_success}")
```

### Project 3: Complete Production Agent

```python
import pynlm

def createProductionAgent(world_size=30, learning_episodes=100):
    """Create a production-ready agent with persistent memory."""
    # Create initial brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(world_size, world_size, 8, 8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    setupAdvancedFeatures(brain, world, agent)
    
    # Training episodes
    for episode in range(learning_episodes):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(episode)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if episode % 50 == 0:
            print(f"Episode {episode}: {brain.getFiringNeuronCount()} neurons firing")
            
            # Save progress periodically
            if episode % 200 == 0:
                pynlm.saveBrainState(brain, f"agent_progress_episode_{episode}.pkl")
    
    return brain, world, agent

# Create and train production agent
brain, world, agent = createProductionAgent(world_size=25, learning_episodes=500)

# Final analysis
analyzeBrain(brain, world, agent)

# Save final model
pynlm.saveBrainState(brain, "production_agent_final.pkl")
print("Production agent trained and saved!")
```

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create simple brain | `pynlm.createBrain(config)` |
| Create brain with specific neurons | `pynlm.createBrainWithNeurons(count, prob)` |
| Create complete learning setup | `pynlm.createLearningEnvironment(size)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Save brain state | `pynlm.saveBrainState(brain, "file.pkl")` |
| Load brain state | `pynlm.loadBrainState("file.pkl")` |

---

## Advanced Usage Tips

### 1. Working with Different World Sizes

```python
# Small world for testing
brain, world, agent = pynlm.createLearningEnvironment(worldSize=10)

# Large world for production
brain, world, agent = pynlm.createLearningEnvironment(worldSize=50)
```

### 2. Custom Learning Scenarios

```python
# Curiosity-focused learning
brain, world, agent = pynlm.createLearningEnvironment(
    worldSize=20,
    enableCuriosity=True,
    enableRewardModulation=False
)

# Reward-focused learning  
brain, world, agent = pynlm.createLearningEnvironment(
    worldSize=20,
    enableCuriosity=False,
    enableRewardModulation=True
)
```

### 3. Combining Manual and Automatic Setup

```python
# Start with a complete learning environment
brain, world, agent = pynlm.createLearningEnvironment(worldSize=20)

# Then customize specific configurations
config = brain.getConfig()
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.learning_rate", 0.002)
```

---

## Troubleshooting (Enhanced)

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try using `createLearningEnvironment()` for proper setup
- Check if sensory input is being processed

### "The agent isn't learning"
- Did you call `agent.enableRewardModulation(True)`?
- Make sure `world.update()` and `world.applyMotorCommand()` are called
- Verify you are calling `agent.applyRewardModulation()` with reward values

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Use `createLearningEnvironment()` for properly connected neurons
- Start with a larger world size for better connectivity

### "Save/Load fails"
- Make sure you have write permissions for the target directory
- Check if you are using absolute paths
- Verify the file extension (.pkl) is appropriate for your system

---

## Next Steps

When you're ready for more advanced usage:

1. **Read `HOW_TO_USE.md`** for comprehensive technical details
2. **Read `docs/ARCHITECTURE.md`** to understand the system design
3. **Explore Phase 6 demos** for advanced integration examples
4. **Experiment with different configurations** to discover new capabilities

The enhanced NLM Python API now provides everything you need for both simple demonstrations and production-grade neural learning systems!

---

## Version Information

Python bindings version: `pynlm.__version__ = "0.1.0"`

Enhanced features:
- ✅ Advanced factory functions
- ✅ Learning environment utilities
- ✅ Memory persistence
- ✅ Configuration tools
- ✅ Production-ready examples
- ✅ Enhanced documentation

That's it! You're now ready to use the enhanced NLM for neural learning! 🚀
