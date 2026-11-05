# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental artificial developmental brain** project that simulates complex neural computation. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Key Features:**
- **Real LIF neuron dynamics** with membrane potential integration
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity rules**: STDP (Spike-Timing Dependent Plasticity), Hebbian learning
- **Structural plasticity**: Synaptogenesis and pruning
- **Integrated memory systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Cognition systems**: Neural planning, concept formation, attention
- **Developmental stages**: Brain maturation and learning critical periods
- **Checkpoint persistence**: Save and load brain states

Think of it like this:
- **Neurons** = Brain cells that send signals to each other using electrical spikes
- **Synapses** = Connections between brain cells that transmit signals with delays
- **Learning** = When connections strengthen/weaken based on spike timing (STDP) and correlated activity (Hebbian)
- **Memory** = Working memory (active patterns), episodic memory (stored experiences), associative memory (pattern associations)
- **Development** = Brain maturation and plasticity modulation across different life stages

That's it! NLM simulates all of this with biologically-inspired neural dynamics.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and learns with thousands of neurons and synapses
2. **World** - The environment the brain lives in with sensory input and motor output
3. **Agent** - The bridge connecting brain to world with perception and action processing

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Full Learning Capacity

```python
import pynlm

# Step 1: Make a brain with default configuration (1000 neurons, advanced learning)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on and enable all learning subsystems
brain.initialize()

# Enable advanced learning capabilities
config = brain.getConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)

# Step 3: Make it think for 1000 steps with learning
for i in range(1000):
    # Apply random input to simulate sensory input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
    brain.step(i)  # One step of thinking with learning

print("Done! Your brain learned through", brain.getTotalSpikeCount(), "spikes")
print("Final weight ratio:", brain.getExcitationInhibitionRatio())
print("Development stage:", brain.getDevelopmentalStage())
```

### Example 2: Complete Agent with World Interaction and Memory

```python
import pynlm
import random

# Create and initialize the advanced brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world with 20x20 space and 8x8 vision
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable comprehensive learning capabilities
agent.enableRewardModulation(True)      # Dopamine-based learning
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Run complete simulation loop
episode_reward = 0.0
for step in range(1000):
    # Update world with time passing
    world.update(0.1)
    
    # Get what the agent sees (8x8 vision)
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain (activates sensory neurons)
    agent.processSensoryInput(percept)
    
    # Brain thinks and learns
    brain.step(step)
    
    # Get action from brain's motor neurons
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward for agent's actions
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development (brain ages and changes plasticity)
    agent.updateDevelopment(0.1)
    
    episode_reward += reward
    
    # Log every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: Reward={reward:.2f}, Total Reward={episode_reward:.2f}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("\n=== Simulation Complete ===")
print(f"Total episode reward: {episode_reward}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Brain memory capacity used: {brain.getFiringNeuronCount()}/{brain.getTotalNeuronCount()}")
print(f"E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
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

### Pattern 1: Run a Complete Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with learning."""
    total_reward = 0.0
    for step in range(num_steps):
        # 1. Update world with time passing
        world.update(0.1)
        
        # 2. Get what the agent perceives (8x8 vision by default)
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain (activates sensory neurons)
        agent.processSensoryInput(percept)
        
        # 4. Apply current inputs to simulate external stimuli
        if step % 10 == 0:  # Apply input every 10 steps
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        
        # 5. Brain thinks, learns, and adapts
        brain.step(step)
        
        # 6. Get action from brain's motor activity
        action = agent.decodeMotorCommand()
        
        # 7. Execute action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 8. Apply reward-based learning (dopamine modulation)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 9. Update development system (brain ages and changes)
        agent.updateDevelopment(0.1)
        
        total_reward += reward
    
    return total_reward

# Use it like this:
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

total_reward = run_simulation(brain, world, agent, 1000)
print(f"Simulation complete! Total reward: {total_reward}")
```

### Pattern 2: Monitor Brain Learning

```python
# Add these inside your simulation loop:
print(f"Step {step}: Reward={reward:.2f}, Curiosity={agent.getCuriosityLevel():.3f}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction error: {agent.getPredictionError():.3f}")
print(f"  E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
```

### Pattern 3: Enable Advanced Learning Features

```python
# Enable comprehensive learning capabilities:
agent.enableRewardModulation(True)       # Dopamine-based learning from rewards
agent.enableStructuralPlasticity(True)   # Synaptogenesis and pruning (brain growth)
agent.enableDevelopment(True)            # Brain maturation and developmental stages
agent.enableCuriosity(True)              # Drive to explore novel stimuli

# Configure learning parameters:
config = agent.getBrain().getConfig()
config.set("plasticity.stdp.enable", True)        # Spike-timing dependent plasticity
config.set("plasticity.hebbian.enable", True)    # Hebbian co-activation learning
config.set("plasticity.structural.enable", True) # Structural changes to connectivity
config.set("neuromod.dopamine.scale", 1.0)        # Reward sensitivity
config.set("neuromod.curiosity.threshold", 0.1)   # Novelty detection sensitivity
config.set("development.initial_plasticity", 1.0) # High initial learning rate
```

---
## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with **thousands of neurons and synapses** implementing real LIF neural dynamics. The brain supports:
- **1000+ neurons** by default with **80/20 excitatory/inhibitory balance**
- **150,000+ synapses** for complex connectivity patterns  
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms**: STDP, Hebbian, structural plasticity
- **Integrated memory systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Cognition systems**: Neural planning, concept formation, attention
- **Developmental stages**: Brain maturation and learning critical periods
- **Checkpoint persistence**: Save and load brain states

- `config` = settings for the brain (use `createDefaultConfig()` for advanced setup with learning enabled)

### `brain.initialize()`
Initializes the brain with **biologically realistic parameters**. Always call this before using the brain!

The brain will:
- Create neurons with LIF dynamics (membrane potential integration)
- Establish random connectivity patterns  
- Initialize synaptic weights with biological ranges (-10 to +10)
- Set up plasticity mechanisms with realistic parameters
- Prepare memory systems for experience storage

### `brain.step(step_number)`
Executes **one complete simulation step** where the brain:
- Processes pending spike events from previous steps
- Updates all neuron membrane potentials using LIF equations
- Detects spikes and schedules outgoing spike events with delays
- Updates working memory with active neural patterns
- Applies neuromodulation effects (dopamine, curiosity)
- Executes plasticity rules (STDP, Hebbian learning)
- Stores current experience in episodic memory
- Updates prediction and attention systems
- Applies structural plasticity (slow changes to connectivity)
- Replays important memories during rest periods
- Applies developmental effects on plasticity rates

### `world.update(time)`
Updates the virtual world simulation with **time passing**, handling:
- Physics simulation (agent movement, velocity, acceleration)
- Collision detection and response
- Energy decay and resource management
- Sensory input generation (8x8 vision by default)
- Action execution with reward calculation

### `agent.processSensoryInput(percept)`
Provides **rich sensory information** to the brain, including:
- **Vision**: 8x8 grid of float values (-1.0 to +1.0)
- **Touch**: Position-based sensory signals
- **Internal signals**: Proprioception, energy levels, health
- **Audio**: Simple sound sensor inputs

The brain injects these signals into **sensory neurons** with biologically realistic mapping.

### `agent.decodeMotorCommand()`
Reads the brain's motor neuron activity to decide actions, using:
- **Winner-take-all competition** among motor neurons
- **Neural population coding** for action selection
- **Threshold-based activation** of motor commands
- **Time-varying dynamics** based on neural firing rates

### Configuration System
The `createDefaultConfig()` creates a brain with **advanced learning enabled**:
- **STDP plasticity** enabled with realistic parameters
- **Hebbian learning** for correlated activity
- **Structural plasticity** for connectivity changes
- **Neuromodulation** systems active
- **Development stages** from initial to aging
- **Memory systems** configured for capacity

## Mini Projects

### Project 1: Silent Brain (Just Neurons with Learning)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    # Inject current to stimulate neurons and enable learning
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
    brain.step(i)

print("Silent brain test done!")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Spikes: {brain.getTotalSpikeCount()}")
print(f"  Learning occurred: {brain.getTotalSpikeCount() > 0}")
```

### Project 2: Brain Watching a World (Sensory Integration)

```python
import pynlm

# Setup with advanced learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Apply sensory input to stimulate brain
    if i % 3 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 25.0f)
    
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Learning effect: {agent.getCuriosityLevel():.3f}")
```

### Project 3: Complete Learning Agent (Full Brain Integration)

```python
import pynlm
import random

# Setup complete agent with all learning systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable comprehensive learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning episode
episode_reward = 0.0
for step in range(200):
    world.update(0.1)
    
    # Process visual input from world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Apply random sensory stimulation for learning
    if step % 5 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
    
    brain.step(step)
    
    # Select and execute action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and learn
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    episode_reward += reward
    
    if step % 50 == 0:
        print(f"Step {step}: Reward={reward:.2f}, Total={episode_reward:.2f}")

print("\n=== Learning Agent Complete ===")
print(f"Total reward: {episode_reward:.2f}")
print(f"Spikes generated: {brain.getTotalSpikeCount()}")
print(f"Brain development stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty experienced: {agent.getNoveltyLevel():.3f}")
```

---

## Quick Reference

### Core Creation

| What you want | Code |
|--------------|------|
| Create brain with default advanced configuration | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| Start brain with biological initialization | `brain.initialize()` |
| Make brain think and learn | `brain.step(step)` |
| Create simple 2D world environment | `world = pynlm.createSimpleWorld()` |
| Create agent brain interface | `agent = pynlm.createAgentBrain(brain)` |

### Getting Information

| What you want | Code |
|--------------|------|
| Get total neuron count | `brain.getTotalNeuronCount()` |
| Get total synapse count | `brain.getTotalSynapseCount()` |
| Get currently firing neurons | `brain.getFiringNeuronCount()` |
| Get average firing rate (Hz) | `brain.getAverageFiringRate()` |
| Get excitation/inhibition balance | `brain.getExcitationInhibitionRatio()` |
| Get total spike count | `brain.getTotalSpikeCount()` |
| Get current developmental stage | `brain.getDevelopmentalStage()` |
| Get agent curiosity level | `agent.getCuriosityLevel()` |
| Get agent novelty level | `agent.getNoveltyLevel()` |
| Get agent prediction error | `agent.getPredictionError()` |

### Processing

| What you want | Code |
|--------------|------|
| Get sensory perception | `percept = world.getSensoryPercept()` |
| Process sensory input in brain | `agent.processSensoryInput(percept)` |
| Decode action from brain | `action = agent.decodeMotorCommand()` |
| Execute action in world | `world.applyMotorCommand(action, world.getSimulationTime())` |
| Update world time | `world.update(0.1)` |

### Learning and Development

| What you want | Code |
|--------------|------|
| Enable reward learning | `agent.enableRewardModulation(True)` |
| Enable structural plasticity | `agent.enableStructuralPlasticity(True)` |
| Enable development stages | `agent.enableDevelopment(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |
| Apply reward modulation | `agent.applyRewardModulation(reward, predicted)` |
| Update development | `agent.updateDevelopment(0.1)` |

### Configuration

| What you want | Code |
|--------------|------|
| Create default config with learning | `config = pynlm.createDefaultConfig()` |
| Load from file | `config.loadFromFile("config.json")` |
| Save to file | `config.saveToFile("config.json")` |
| Get config keys | `config.getKeys()` |
| Set custom parameter | `config.set("plasticity.stdp.enable", True)` |

### Advanced Operations

| What you want | Code |
|--------------|------|
| Get region by ID | `region = brain.getRegion(region_id)` |
| Get all regions | `regions = brain.getRegions()` |
| Save brain state | `brain.save("checkpoint.bin")` |
| Load brain state | `brain.load("checkpoint.bin")` |
| Add new region | `region_id = brain.addRegion("cortex")` |
| Get spike system | `spike_system = brain.getSpikeSystem()` |
| Get neuromodulation level | `level = agent.getNeuromodulationLevel()` |

---

## Troubleshooting

**"My brain isn't learning or firing"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps to 1000+
- Apply external current: `brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)`
- Ensure learning is enabled: `agent.enableRewardModulation(True)`

**"The agent isn't learning from rewards"**
- Did you call `agent.applyRewardModulation(reward, predicted_reward)`?
- Check reward values: `reward = world.getSensoryPercept().getInternal()[0]`

**"Everything is 0"**
- Brains need time to "warm up" - try 500+ steps for complex behaviors
- Ensure plasticity is enabled for synaptic changes
- Check configuration: `brain.getConfig().set("plasticity.stdp.enable", True)`

**"Brain state won't save/load"**
- Use absolute file paths: `brain.save("/path/to/checkpoint.bin")`
- Ensure write permissions in target directory
- Check config for checkpoint directory: `config.set("checkpoint_dir", "./checkpoints")`

**"Getting Python errors"**
- Make sure the `pynlm` module is installed: `pip install -e .`
- Check compatibility with your Python version (3.8+)
- Verify all required C++ dependencies are built

---

## Next Steps

When you're comfortable with the basics:

1. **Read HOW_TO_USE.md** for detailed C++ build instructions and advanced configuration options
2. **Read docs/ARCHITECTURE.md** to understand the complete system architecture and integration patterns
3. **Read docs/EXPERIMENTS.md** for experiment descriptions and benchmark results
4. **Experiment with different configurations** - modify `createDefaultConfig()` settings for different behaviors
5. **Build from source** for custom modifications and optimizations
6. **Study Phase 6 integration** in the experiments/ directory for complete system understanding

## Advanced Features Available

Beyond the basics, you can:

- **Custom neural architectures** with multiple brain regions
- **Complex world environments** with terrain, hazards, and resources
- **Advanced learning algorithms** with different neuromodulation strategies
- **Performance optimization** using SIMD vectorization and parallel processing
- **Checkpoint persistence** for long-running learning sessions
- **Ablation studies** to understand component contributions
- **Scaling benchmarks** for different network sizes

That's it! You're now ready to use NLM's **full Phase 6 integrated artificial brain** with comprehensive learning capabilities!
