# NLM - Easy Usage Guide for Beginners

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

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

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
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
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

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

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

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
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
- Check that sensory input is being injected into the brain

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the agent's motor decoding is working

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that plasticity rules are enabled (STDP, Hebbian, structural)

**"No learning detected"**
- Verify that `agent.enableRewardModulation(True)` is called
- Check that `agent.enableCuriosity(True)` is called if you want exploration
- Ensure development stages are progressing
- Verify that the simulation timestep is appropriate for your scale

**"Checkpoint loading failed"**
- Check that the checkpoint file exists and is not corrupted
- Verify that the brain configuration matches the saved state
- Check file permissions and disk space

---

## Advanced Examples

### Example 4: Learning with Memory Integration

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all learning systems
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)      # Reward-based learning
agent.enableCuriosity(True)              # Exploration drive
agent.enableStructuralPlasticity(True)   # Growing new connections
agent.enableDevelopment(True)            # Brain maturation

print("Running advanced learning simulation with memory integration...")

for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check memory systems integration
    if step % 100 == 0:
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        if working_mem:
            print(f"Step {step}: Working memory active traces: {working_mem.getActiveTraces()}")
        if episodic_mem:
            print(f"Step {step}: Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
        
        # Check prediction system
        prediction = brain.getPredictionSystem()
        if prediction:
            error = prediction.getPredictionError()
            print(f"Step {step}: Prediction error: {error}")
        
        # Check cognitive systems
        planner = brain.getPlanner()
        concept = brain.getConceptFormation()
        attention = brain.getAttention()
        
        if planner:
            print(f"Step {step}: Planner active: {planner.isActive()}")
        if concept:
            print(f"Step {step}: Concepts formed: {concept.getConceptCount()}")
        if attention:
            print(f"Step {step}: Attention focus: {attention.getAttendedRegions()}")
        
        # Check neuromodulation
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        
        if dopamine:
            print(f"Step {step}: Dopamine level: {dopamine.getLevel()}")
        if curiosity:
            print(f"Step {step}: Curiosity level: {curiosity.getLevel()}")
        if novelty:
            print(f"Step {step}: Novelty level: {novelty.getLevel()}")

print("Advanced simulation complete!")
```

### Example 5: Multi-Agent Coordination

```python
import pynlm
import threading
import time

class NLMEnvironment:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        self.world.reset()
        
        # Create multiple agents with different personalities
        self.agents = []
        for i in range(3):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            self.agents.append(agent)
        
        # Give agents different preferences (learned personality)
        for i, agent in enumerate(self.agents):
            agent.getBrain()->setLearnedPreference((i - 1) * 0.5)  # -0.5, 0, 0.5
    
    def step(self):
        for agent in self.agents:
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            agent.getBrain()->step(0)
            action = agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Each agent has different reaction to world feedback
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            predicted = 0.0  # Simple prediction
            agent.applyRewardModulation(reward, predicted)

# Run multi-agent coordination simulation
env = NLMEnvironment()
for step in range(200):
    env.step()
    if step % 50 == 0:
        print(f"Step {step}: World size {env.world.getWidth()}x{env.world.getHeight()}")
        print(f"Agent preferences: {[env.agents[0].getBrain()->getLearnedPreference(), env.agents[1].getBrain()->getLearnedPreference(), env.agents[2].getBrain()->getLearnedPreference()]}")

print("Multi-agent coordination complete!")
```

### Example 6: Configuration Persistence

```python
import pynlm
import json
import os

class ConfigurationManager:
    def __init__(self, config_dir="configs/"):
        self.config_dir = config_dir
        os.makedirs(config_dir, exist_ok=True)
    
    def save_config(self, brain, filename, description=""):
        """Save brain configuration and learned personality to file"""
        config_data = {
            "config": brain.getConfig()->summary(),
            "learned_personality": brain.getLearnedPreference(),
            "stats": {
                "neurons": brain.getTotalNeuronCount(),
                "synapses": brain.getTotalSynapseCount(),
                "spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio()
            },
            "description": description
        }
        
        filepath = os.path.join(self.config_dir, f"{filename}.json")
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        # Also save brain state using pynlm's checkpoint
        brain.save(f"{filepath}.bin")
        print(f"Configuration saved to {filepath}")
    
    def load_config(self, filename):
        """Load configuration and brain state from file"""
        filepath = os.path.join(self.config_dir, f"{filename}.json")
        
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file {filepath} not found")
        
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        # Create brain from saved config
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Load checkpoint if available
        checkpoint_path = f"{filepath}.bin"
        if os.path.exists(checkpoint_path):
            brain.load(checkpoint_path)
        
        return brain

# Usage example
config_manager = ConfigurationManager()

# Create and train a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for a while to develop personality
for i in range(100):
    brain.step(i)

# Save the developed brain
config_manager.save_config(brain, "my_brain_v1", "Brain developed through reinforcement learning")

# Later, load the saved brain
# loaded_brain = config_manager.load_config("my_brain_v1")
```

---

## Advanced API Reference

### Brain System Methods

#### Enhanced Brain Methods

| Method | Description | Return Type |
|--------|-------------|-------------|
| `brain.getWorkingMemory()` | Access working memory system | NeuralWorkingMemory* |
| `brain.getEpisodicMemory()` | Access episodic memory system | NeuralEpisodicMemory* |
| `brain.getAssociativeMemory()` | Access associative memory system | NeuralAssociativeMemory* |
| `brain.getPredictionSystem()` | Access prediction system | PredictionSystem* |
| `brain.getPlanner()` | Access neural planner | NeuralPlanner* |
| `brain.getConceptFormation()` | Access concept formation | ConceptFormation* |
| `brain.getAttention()` | Access attentional selection | AttentionalSelection* |
| `brain.getDevelopmentSystem()` | Access development system | DevelopmentSystem* |
| `brain.getDopamine()` | Access dopamine neuromodulation | Dopamine* |
| `brain.getCuriosity()` | Access curiosity neuromodulation | Curiosity* |
| `brain.getNovelty()` | Access novelty detection | Novelty* |
| `brain.getPredictionErrorSignal()` | Access prediction error | PredictionError* |

#### Brain Statistics and State

| Method | Description | Example Output |
|--------|-------------|----------------|
| `brain.getStats()` | Get comprehensive brain statistics | dict with all metrics |
| `brain.getState()` | Get current brain state as dictionary | dict with runtime state |
| `brain.verifyIntegration()` | Verify all systems are integrated | dict with integration status |

### Config System Methods

#### Advanced Config Operations

| Method | Description |
|--------|-------------|
| `config.loadFromFileWithValidation(filepath)` | Load and validate config file |
| `config.mergeConfig(other_config)` | Merge with another configuration |
| `config.getAllKeys()` | Get all configuration keys |
| `config.getByCategory(category)` | Get configuration by category |
| `config.validateForBrain()` | Validate config for current brain |

### Error Handling and Logging

#### Enhanced Logging

| Level | Description |
|-------|-------------|
| `LogLevel::Debug` | Detailed debugging information |
| `LogLevel::Info` | General informational messages |
| `LogLevel://Warning` | Warnings about potential issues |
| `LogLevel::Error` | Error conditions and failures |

#### Common Error Scenarios

1. **"Brain already initialized"**: Call `brain.reset()` before re-initializing
2. **"Invalid neuron configuration"**: Check `brain.getTotalNeuronCount()` after init
3. **"Memory system not working"**: Verify integration with `brain.verifyIntegration()`
4. **"Checkpoint format mismatch"**: Ensure compatible brain versions

### Performance and Monitoring

#### Monitoring Tools

| Function | Description |
|----------|-------------|
| `brain.profileMemoryUsage()` | Get detailed memory usage statistics |
| `brain.profilePerformance()` | Get timing and performance metrics |
| `brain.getSystemStats()` | Get comprehensive system statistics |
| `brain.startMonitoring()` / `brain.stopMonitoring()` | Start/stop monitoring |

#### Performance Optimization Tips

1. **Batch sensory input**: Use `brain.receiveSensoryInput()` for multiple inputs
2. **Pre-allocate regions**: Configure `brain.addRegion()` before initialization
3. **Use appropriate timestep**: Set `config->set("simulation_timestep", 0.001)` for accuracy
4. **Enable SIMD**: Configure build with SIMD optimizations

---

## Next Steps

When you're comfortable:

1. **Explore Advanced Examples**: Try the examples 4, 5, and 6 for advanced usage patterns
2. **Read HOW_TO_USE.md**: Learn more technical details and advanced configurations
3. **Study docs/ARCHITECTURE.md**: Understand the complete system architecture
4. **Check docs/PHASE6_FINAL_AUDIT.md**: Learn about integration achievements and remaining challenges
5. **Experiment with Different Configurations**: Try varying parameters for interesting behaviors
6. **Explore Advanced Python Features**: Use the enhanced Python bindings for sophisticated applications

## Advanced Usage Resources

- **Phase 6 Integration Demo**: `./nlm_phase6_demo` - Comprehensive integration test
- **Phase 6 Text Demo**: `./nlm_phase6_text_demo` - Text-based interaction demo
- **Performance Benchmark**: `./nlm_benchmark` - Performance testing framework
- **Advanced Examples**: Look for additional examples in the documentation directory

Remember: NLM is not a traditional AI system - it's a developmental brain that learns from experience. The key to successful usage is patience, appropriate configuration, and understanding the integration between different brain systems.

That's it! You're now ready for advanced NLM usage. Happy experimenting with your artificial developmental brain!
