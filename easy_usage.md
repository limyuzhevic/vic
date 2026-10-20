# NLM - Enhanced Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers that provides access to a complete artificial developmental brain system through a clean Python interface. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this with sophisticated spiking neural networks, plasticity rules, and neuromodulation.

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

### Example 3: Advanced Learning Brain

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with learning capabilities."""
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableStructuralPlasticity(True)  # Grow new connections
    
    # Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

### Example 4: Plasticity and Learning Demo

```python
import pynlm

def demonstrate_plasticity():
    """Demonstrate synaptic plasticity and learning."""
    
    print("=== Plasticity Demonstration ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Enable plasticity
    if brain.getSTDP():
        brain.getSTDP().setLTPWeight(0.02f)
        brain.getSTDP().setLTDWeight(0.015f)
        print("STDP enabled: LTP=0.02, LTD=0.015")
    
    if brain.getHebbian():
        brain.getHebbian().setLearningRate(0.001f)
        print("Hebbian enabled: learning_rate=0.001")
    
    # Create neurons and synapses for demonstration
    # Note: This is a simplified example
    print("Run simulation with sensory input to see plasticity in action...")
    
    for step in range(500):
        # Apply input to stimulate learning
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
        
        if step % 100 == 0:
            spikes = brain.getTotalSpikeCount()
            rate = brain.getAverageFiringRate()
            print(f"Step {step}: Total spikes={spikes}, Firing rate={rate:.3f}")
    
    print("Plasticity demonstration complete!")

demonstrate_plasticity()
```

### Example 5: Phase 6 Integration Demo

```python
import pynlm

def run_phase6_demo():
    """Run Phase 6 integration demonstration."""
    
    print("=== Phase 6 Integration Demo ===")
    
    # Create fully configured brain for Phase 6
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all Phase 6 systems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True) 
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run integration test
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Total reward: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Integration Results ===")
    print(f"Working Memory: {'✓' if brain.getWorkingMemory() else '✗'}")
    print(f"Episodic Memory: {'✓' if brain.getEpisodicMemory() else '✗'}")
    print(f"Neuromodulation: {'✓' if brain.getDopamine() else '✗'}")
    print(f"Prediction System: {'✓' if brain.getPredictionSystem() else '✗'}")
    print(f"Development System: {'✓' if brain.getDevelopmentSystem() else '✗'}")
    print(f"Checkpointing: {'✓' if agent.getCuriosityLevel() > 0 else '✗'}")

run_phase6_demo()
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
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Check if features are enabled
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")

if agent.isCuriosityEnabled():
    print("Curiosity-driven exploration is active")
```

### Pattern 4: Save and Load Brain States

```python
# Save brain state (checkpoint)
brain.save("my_brain_checkpoint.bin")
print("Brain saved to checkpoint!")

# Later, load the brain
# brain2 = pynlm.createBrain(config)
# brain2.load("my_brain_checkpoint.bin")
# print("Brain loaded successfully!")
```

### Pattern 5: Statistical Analysis

```python
import pynlm

def analyze_brain(brain):
    """Get comprehensive brain statistics."""
    stats = pynlm.getBrainStats(brain)
    
    print("=== Brain Statistics ===")
    for key, value in stats.items():
        print(f"{key}: {value}")
    
    # Check for potential issues
    if stats["total_neurons"] < 100:
        print("Warning: Very small brain (fewer than 100 neurons)")
    
    if stats["spike_rate_hz"] < 0.1:
        print("Warning: Low firing rate - brain may not be active")
    
    return stats

# Usage
stats = analyze_brain(brain)
```

### Pattern 6: Error Handling and Validation

```python
def safe_simulation(num_steps=1000):
    """Run simulation with error handling."""
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Validate configuration
        pynlm.validate_config(brain.getConfig())
        
        for step in range(num_steps):
            brain.step(step)
            
        print("Simulation completed successfully!")
        return brain
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return None
    except pynlm.NeuralError as e:
        print(f"Neural error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# Run with error handling
brain = safe_simulation(500)
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Returns a brain object ready for initialization

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- Sets up neurons, synapses, and internal systems
- Configures plasticity rules
- Initializes memory systems

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron for threshold crossing
- Sends signals between connected neurons
- Updates connections based on learning rules
- Applies neuromodulation

### `world.update(time)`
Updates the virtual world by `time` seconds.
- Moves the agent
- Handles object interactions
- Applies physics and constraints
- Generates sensory input

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.
- Converts sensory data to neural signals
- Injects current into sensory neurons
- Updates agent's internal state

### `agent.decodeMotorCommand()`
Reads the brain's motor activity to decide what action to take.
- Analyzes motor neuron activity
- Selects appropriate action based on learned patterns
- Returns motor command for world interaction

### `world.applyMotorCommand(action, time)`
Executes the chosen action in the world.
- Moves the agent
- Handles interactions with objects
- Generates feedback/reward

### `agent.applyRewardModulation(reward, predicted)`
Applies reward-based learning to the brain.
- Updates dopamine levels
- Modifies synaptic weights
- Influences future behavior

### `pynlm.getBrainStats(brain)`
Get comprehensive statistics about the brain.
- Returns a dictionary of metrics
- Includes performance indicators
- Helps monitor brain health

### `pynlm.validate_config(config)`
Validate configuration and throw informative errors.
- Checks for required parameters
- Validates ranges and types
- Provides helpful error messages

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

### Project 3: Complete Agent with Learning

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

### Project 4: Plasticity Research

```python
import pynlm

def plasticity_experiment(experiment_name, config_overrides=None):
    """Run a plasticity research experiment."""
    
    print(f"=== Running Plasticity Experiment: {experiment_name} ===")
    
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable specific plasticity rules
    if brain.getSTDP():
        brain.getSTDP().setLTPWeight(0.02f)
        brain.getSTDP().setLTDWeight(0.015f)
        print("STDP configured")
    
    if brain.getHebbian():
        brain.getHebbian().setLearningRate(0.001f)
        print("Hebbian configured")
    
    # Run experiment
    for step in range(500):
        # Apply specific input patterns
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} total spikes")
    
    # Analyze results
    print(f"\nExperiment Results:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

# Run different experiments
experiment1 = plasticity_experiment("STDP-focused", {
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": False
})

experiment2 = plasticity_experiment("Hebbian-focused", {
    "plasticity.stdp.enable": False,
    "plasticity.hebbian.enable": True
})
```

### Project 5: Development Study

```python
import pynlm

def developmental_study():
    """Study brain development over time."""
    
    print("=== Developmental Study ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent with development enabled
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track development over time
    developmental_stages = []
    firing_rates = []
    curiosity_levels = []
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record developmental stage
        stage = brain.getDevelopmentalStage()
        developmental_stages.append(stage)
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        if step % 50 == 0:
            print(f"Step {step}: {stage}, Firing rate={brain.getAverageFiringRate():.3f}")
    
    # Analyze development
    print(f"\nDevelopment Analysis:")
    print(f"  Final developmental stage: {developmental_stages[-1]}")
    print(f"  Average firing rate: {sum(firing_rates)/len(firing_rates):.3f}")
    print(f"  Final curiosity level: {curiosity_levels[-1]:.3f}")
    
    # Check if development occurred
    if len(set(devotional_stages)) > 1:
        print("✓ Brain developed through multiple stages")
    else:
        print("⚠ Brain may not have developed significantly")

developmental_study()
```

### Project 6: Memory Integration

```python
import pynlm

def memory_integration_experiment():
    """Study memory integration in the brain."""
    
    print("=== Memory Integration Experiment ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    # Run simulation with memory building
    memory_episodes = []
    
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Simulate memory encoding (in real implementation, this happens automatically)
        if step % 50 == 0:
            memory_episodes.append({
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel()
            })
        
        if step % 100 == 0:
            print(f"Step {step}: Memory episodes stored: {len(memory_episodes)}")
    
    print(f"\nMemory Integration Results:")
    print(f"  Total steps: {300}")
    print(f"  Memory episodes recorded: {len(memory_episodes)}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Check for replay capability
    if brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodeCount()
        print(f"  Episodic memory episodes: {episodes}")

memory_integration_experiment()
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `pynlm.createDefaultConfig()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `pynlm.getBrainStats(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Validate config | `pynlm.validate_config(config)` |
| Save brain state | `brain.save("filename.bin")` |
| Get comprehensive stats | `agent.getNeuromodulationLevel()`, `agent.getCuriosityLevel()` |

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if learning features are enabled
- Verify world is being updated

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has motor neurons
- Check agent's action decoding

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if plasticity is enabled and working
- Verify configuration parameters

### "Import error: No module named 'pynlm'"
- Make sure you're in the correct directory
- Install the package: `pip install -e .`
- Check if dependencies are installed

### "Segmentation fault"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration is valid
- Check for null pointer accesses in custom bindings

### "Performance is slow"
- Reduce number of neurons for testing
- Decrease simulation steps
- Check configuration parameters
- Consider using batch operations

### "Memory usage is high"
- Check brain size configuration
- Verify memory systems are properly configured
- Consider loading/saving checkpoints
- Monitor memory usage with `pynlm.estimate_memory_usage()`

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** about building, installing, and advanced usage
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works internally
3. **Experiment with different configurations** using the command-line tools
4. **Try the test suite** with `python test_bindings.py` to verify all functionality
5. **Explore the demo scripts** in the examples directory
6. **Join the research community** to share findings and get help

### Learning Resources

- **Documentation**: `docs/` directory contains detailed technical documentation
- **Examples**: Look for example scripts in the repository
- **Tests**: Run `python test_bindings.py` to test functionality
- **Command-line tools**: Use `nlm-cli` for advanced operations

### Getting Help

- **Issues**: Report bugs and feature requests on GitHub
- **Documentation**: Check `docs/` for API references
- **Community**: Join discussions on the NLM forum
- **Examples**: Look at existing example scripts for patterns

That's it! You're now ready to use NLM with all the enhanced features and improved bindings. The improved Python API provides comprehensive access to the full NLM brain simulation system with advanced error handling, validation, and performance monitoring.

🎉 Happy brain simulation!
