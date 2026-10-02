# NLM - Enhanced Usage Guide for Advanced Users

## What is NLM?

NLM (熙然) is an advanced brain simulation framework with full integration of all neural systems. At **Phase 6: FINAL INTEGRATION**, NLM provides:

- Complete integration of memory, neuromodulation, and prediction systems
- Real neuroscience-inspired models (dopamine, curiosity, novelty)
- JSON/YAML configuration management
- Advanced Python bindings with comprehensive error handling
- Memory persistence and replay capabilities
- Development-aware plasticity

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Plasticity** = Learning mechanisms (STDP, Hebbian, reward-modulated)
- **Neuromodulation** = Chemical signals that regulate learning (dopamine, curiosity)
- **Memory** = Working memory, episodic memory, associative memory
- **Development** = Age-dependent learning and maturation

## Quick Start (Copy & Paste)

### Example 1: Basic Brain with Neuromodulation

```python
import pynlm

# Create a brain with advanced configuration
config = pynlm.createDefaultConfig()

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain and enable advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Enable learning subsystems
agent = pynlm.createAgentBrain(brain)

# Run simulation with learning
for step in range(100):
    brain.step(step)
    
    # Check neuromodulation stats
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    pred_error = agent.getPredictionError()
    
    if step % 10 == 0:
        print(f"Step {step}: Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
```

### Example 2: Complete Integration Test

```python
import pynlm
import time

def run_integration_test():
    """Run a complete Phase 6 integration test."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("enable_checkpointing", True)
    config.set("enable_replay", True)
    config.set("enable_development", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run integration test
    start_time = time.time()
    
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log stats periodically
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    
    print("\n=== INTEGRATION TEST RESULTS ===")
    print(f"Total wall clock time: {end_time - start_time:.2f}s")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    
    # Check integration
    memory_working_memory = brain.getWorkingMemoryIntegration()
    memory_episodic = brain.getEpisodicMemoryIntegration()
    neuromodulation = brain.getNeuromodulationIntegration()
    prediction = brain.getPredictionIntegration()
    development = brain.getDevelopmentIntegration()
    
    print(f"\n=== INTEGRATION STATUS ===")
    print(f"Working Memory: {'CONNECTED' if memory_working_memory else 'DISCONNECTED'}")
    print(f"Episodic Memory: {'CONNECTED' if memory_episodic else 'DISCONNECTED'}")
    print(f"Neuromodulation: {'CONNECTED' if neuromodulation else 'DISCONNECTED'}")
    print(f"Prediction: {'CONNECTED' if prediction else 'DISCONNECTED'}")
    print(f"Development: {'CONNECTED' if development else 'DISCONNECTED'}")

# Run the integration test
run_integration_test()
```

### Example 3: Memory System Access

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
memory = brain.getMemorySystem()

# Working memory: temporary storage for current information
print("Working Memory:")
print(f"  Size: {memory.getWorkingMemorySize()}")
print(f"  Content: {memory.getWorkingMemoryContent()}")

# Episodic memory: long-term storage of experiences
print("\nEpisodic Memory:")
print(f"  Number of episodes: {memory.getEpisodicMemoryCount()}")
print(f"  Recent episodes: {memory.getRecentEpisodes(5)}")

# Associative memory: pattern-based associations
print("\nAssociative Memory:")
print(f"  Number of associations: {memory.getAssociativeMemoryCount()}")
print(f"  Concept stability: {memory.getConceptStability()}")

# Memory management
print("\nMemory Management:")
print("  Clearing working memory...")
memory.clearWorkingMemory()

print("  Compressing episodic memory...")
memory.compressEpisodicMemory()

print("  Consolidating associations...")
memory.consolidateAssociations()

print("  Replay memory...")
memory.replayMemory()
```

### Example 4: Advanced Configuration

```python
import pynlm
import json

# Create configuration with JSON-like structure
config = pynlm.createDefaultConfig()

# Set various parameters using different data types
config.set("brain.neuron_count", 1000)           # Integer
config.set("brain.synapse_density", 0.15)         # Float
config.set("brain.connection_probability", 0.08) # Float
config.set("brain.initial_weight_mean", 0.6)      # Float
config.set("brain.v_thresh", -55.0)              # Float
config.set("brain.v_rest", -70.0)                # Float
config.set("brain.tau_mem", 20.0)                # Float
config.set("brain.tau_ref", 2.0)                 # Float

# Enable plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)

config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.novelty_weight", 0.7)
config.set("neuromod.curiosity.prediction_error_weight", 0.3)
config.set("neuromod.novelty.enable", True)

# Configure development
config.set("development.synaptogenesis.rate", 0.05)
config.set("development.pruning.threshold", 0.3)
config.set("development.maturation.rate", 0.1)
config.set("development.critical_period.start", 10.0)
config.set("development.critical_period.end", 100.0)

# Configure prediction system
config.set("prediction.horizon", 3)
config.set("prediction.accuracy_threshold", 0.8)

# Save configuration to file
config.saveToFile("advanced_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")

print("Configuration saved and loaded successfully!")
print(f"Loaded neuron count: {config2.getOr('brain.neuron_count', 0)}")
print(f"Loaded curiosity enabled: {config2.getOr('neuromod.curiosity.enable', False)}")
```

### Example 5: Error Handling and Validation

```python
import pynlm

# Create brain with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Try to access non-existent memory system
    memory = brain.getMemorySystem()
    
    # This will raise an error if memory system not properly initialized
    if not memory:
        raise pynlm.NLMException("Memory system not properly initialized")
        
except pynlm.NLMException as e:
    print(f"Error: {e}")
    print("Please check your configuration and initialization steps")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    
except pynlm.InitializationError as e:
    print(f"Initialization error: {e}")
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    print("Please check your system configuration")
```

### Example 6: Batch Processing

```python
import pynlm

def run_batch_simulation(config, num_steps, num_repeats=1):
    """Run batch simulation with multiple repetitions."""
    
    results = []
    
    for repeat in range(num_repeats):
        print(f"Running batch simulation {repeat + 1}/{num_repeats}")
        
        # Create brain for this batch
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Initialize agent
        agent.initialize(world)
        
        # Run simulation
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        result = {
            'repeat': repeat + 1,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neutrons': brain.getFiringNeuronCount(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        results.append(result)
        
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Curiosity level: {result['curiosity_level']:.3f}")
        print(f"  Prediction error: {result['prediction_error']:.3f}")
    
    return results

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 300)
config.set("enable_development", True)
config.set("enable_curiosity", True)

# Run batch simulation
batch_results = run_batch_simulation(config, 100, num_repeats=5)

# Analyze results
print("\n=== BATCH SIMULATION RESULTS ===")
for result in batch_results:
    print(f"Repeat {result['repeat']}: {result['total_spikes']} spikes, "
          f"curiosity={result['curiosity_level']:.3f}, "
          f"error={result['prediction_error']:.3f}")

# Calculate statistics
total_spikes = sum(r['total_spikes'] for r in batch_results)
avg_curiosity = sum(r['curiosity_level'] for r in batch_results) / len(batch_results)
avg_error = sum(r['prediction_error'] for r in batch_results) / len(batch_results)

print(f"\nAverage total spikes: {total_spikes / len(batch_results):.1f}")
print(f"Average curiosity level: {avg_curiosity:.3f}")
print(f"Average prediction error: {avg_error:.3f}")
```

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Apply reward and update development
        if step % 10 == 0:
            agent.updateDevelopment(0.1)

# Usage
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
print("Prediction error:", agent.getPredictionError())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

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
- Applies neuromodulation effects

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `agent.applyRewardModulation(reward, predicted)`
Applies reward signals to modulate learning.

### `agent.updateDevelopment(timestep)`
Updates the brain's development system.

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

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neuromodulation is enabled

**"The agent isn't learning"**
- Did you enable reward modulation?
- Did you call `agent.updateDevelopment()`?
- Check curiosity and novelty levels

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if learning subsystems are enabled

**"Integration test failing"**
- Verify all systems are properly initialized
- Check configuration parameters
- Ensure memory systems are connected

## Next Steps

When you're comfortable:
1. **Advanced Configuration**: Read `HOW_TO_USE.md` for more details
2. **Architecture**: Read `docs/ARCHITECTURE.md` to understand how it all works
3. **Science**: Read `docs/SCIENCE.md` to understand the biological basis
4. **Experiments**: Read `docs/EXPERIMENTS.md` for experimental protocols
5. **Real Experiments**: Run `./nlm_phase6_demo` for the full integration test
6. **Custom Experiments**: Create your own experiments with custom configurations

That's it! You're now ready to use NLM with all its advanced features and Phase 6 integration capabilities.

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
