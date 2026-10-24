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

### Complete Working Example: Agent in a World

```python
import pynlm
import time

# === SETUP ===
print("=== NLM Agent Simulation ===")

# 1. Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # 500 neurons
config.set("brain.synapse_density", 0.1)  # 10% connectivity
config.set("plasticity.stdp.enable", True)  # Enable learning
config.set("neuromodulation.curiosity.enable", True)  # Enable exploration

brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create the world
world = pynlm.createSimpleWorld()
world.configure(
    width=30, height=30,  # 30x30 grid
    visionWidth=8, visionHeight=8  # 8x8 vision
)
world.reset()
world.setAgentStart(15.0, 15.0)  # Start in center

# 3. Create agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")

# === MAIN SIMULATION LOOP ===
print("\n=== Running Simulation ===")
start_time = time.time()

for step in range(1000):
    # 1. Update world state
    world.update(0.1)
    
    # 2. Get sensory input from world
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # 4. Brain processes information (thinks)
    brain.step(step)
    
    # 5. Brain decides what to do (motor command)
    action = agent.decodeMotorCommand()
    
    # 6. Apply action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Learning occurs (neuromodulation)
    # Get reward from agent's internal signals
    reward = agent.getNeuromodulationLevel()
    agent.applyRewardModulation(reward, 0.0)
    
    # 8. Update development
    agent.updateDevelopment(0.1)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        brain_stats = {
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "dev_stage": brain.getDevelopmentalStage()
        }
        print(f"Step {step}: {brain_stats}")

# === RESULTS ===
end_time = time.time()
elapsed = end_time - start_time

print("\n=== Simulation Complete ===")
print(f"Total simulation time: {elapsed:.2f} seconds")
print(f"Final brain statistics:")
print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")

print(f"\nAgent statistics:")
print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  - Prediction error: {agent.getPredictionError():.3f}")

print(f"\nWorld statistics:")
print(f"  - Agent position: ({agent.getBrain().getAgentBody().x:.1f}, {agent.getBrain().getAgentBody().y:.1f})")
print(f"  - Agent energy: {agent.getBrain().getAgentBody().energy:.1f}")
print(f"  - Simulation time: {world.getSimulationTime():.1f}s")

print("\n=== Example Complete ===")

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

## Working with Configurations

### Advanced Configuration Setup

```python
import pynlm

# Create a brain with advanced configuration
config = pynlm.createDefaultConfig()

# Brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.05)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.tau_mem", 20.0)

# STDP plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.stdp.ltp_weight", 0.01)
config.set("plasticity.stdp.ltd_weight", 0.012)

# Hebbian learning
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

# Structural plasticity
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0005f)
config.set("plasticity.structural.pruning_rate", 0.00005f)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction_error.scale", 1.0f)

# Development
config.set("development.initial_stage", "CriticalPeriod")
config.set("development.critical_period_duration", 1000)
config.set("development.maturation_rate", 0.01f)
config.set("development.plasticity_modulation", True)

# Memory systems
config.set("memory.working_capacity", 200)
config.set("memory.episodic_max_episodes", 1000)
config.set("memory.associative_strength", 0.5f)
config.set("memory.replay_interval", 100)
config.set("memory.consolidation_threshold", 0.7f)

# Prediction system
config.set("prediction.enable", True)
config.set("prediction.horizon", 50)
config.set("prediction.error_threshold", 0.1f)

# Cognition
config.set("cognition.planner.depth", 5)
config.set("cognition.attention.inhibition_strength", 0.5f)
config.set("cognition.attention.excitation_strength", 1.5f)
config.set("cognition.concept.formation_rate", 0.02f)

# Simulation parameters
config.set("simulation.timestep", 0.001)
config.set("simulation.max_steps", 10000)
config.set("simulation.checkpoint_interval", 1000)
config.set("simulation.replay_enabled", True)

# Random seed for reproducibility
config.set("random_seed", 42)

# Save configuration to file
config.saveToFile("advanced_config.json")

# Load configuration back
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("advanced_config.json")

brain = pynlm.createBrain(new_config)
brain.initialize()
```

### Configuration Validation and Error Handling

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set some values
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)

# Validate configuration
if not config.validate():
    print("Configuration validation failed!")
    # Get keys for debugging
    print("Current keys:", config.getKeys())
    exit(1)

# Check required keys
required_keys = [
    "brain.neuron_count",
    "plasticity.stdp.enable",
    "brain.synapse_density"
]

if not config.hasAllRequired(required_keys):
    print("Missing required configuration keys")
    missing = []
    for key in required_keys:
        if not config.has(key):
            missing.append(key)
    print("Missing keys:", missing)
    # Set defaults for missing keys
    for key in missing:
        if key == "brain.neuron_count":
            config.set(key, 1000)
        elif key == "plasticity.stdp.enable":
            config.set(key, True)
        elif key == "brain.synapse_density":
            config.set(key, 0.1)

# Test getting values with validation
neuron_count = config.getValidated<int>("brain.neuron_count", lambda x: x > 0 and x <= 10000)
if neuron_count.has_value():
    print(f"Validated neuron count: {neuron_count.value()}")
else:
    print("Invalid neuron count, using default")
    config.set("brain.neuron_count", 1000)

# Create brain with validated config
brain = pynlm.createBrain(config)
brain.initialize()
```

### JSON and YAML Configuration Files

Create a configuration file `brain_config.json`:

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.05,
    "synapse_density": 0.1,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0,
      "ltp_weight": 0.01,
      "ltd_weight": 0.012
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.005
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0005,
      "pruning_rate": 0.00005
    }
  },
  
  "neuromod": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true
    },
    "novelty": {
      "enable": true
    }
  }
}
```

```yaml
# brain_config.yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.05
  synapse_density: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
    ltp_weight: 0.01
    ltd_weight: 0.012
  hebbian:
    enable: true
    learning_rate: 0.005
  structural:
    enable: true
    synaptogenesis_rate: 0.0005
    pruning_rate: 0.00005

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true
```

```python
import pynlm

# Load configuration from JSON or YAML
config = pynlm.createDefaultConfig()

# Load from JSON
config.loadFromFile("brain_config.json")

# Or load from YAML
# config.loadFromFile("brain_config.yaml")

# Save in JSON format
config.saveToFile("output_config.json")

# Create brain with loaded configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Advanced Brain Operations

### Brain State Management

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Initial state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")

# Save brain state to checkpoint
brain.save("initial_brain_state.bin")

# Run simulation
for i in range(500):
    brain.step(i)

print(f"After 500 steps: {brain.getTotalSpikeCount()} spikes")

# Load saved state back
brain.load("initial_brain_state.bin")

print(f"After loading checkpoint: {brain.getTotalSpikeCount()} spikes")
```

### Brain Statistics and Monitoring

```python
import pynlm
import time

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Setup monitoring
monitor_interval = 50
start_time = time.time()
last_checkpoint = 0

print("=== Brain Monitoring ===")

for step in range(2000):
    brain.step(step)
    
    # Monitor every interval
    if step % monitor_interval == 0:
        current_time = time.time() - start_time
        
        # Get comprehensive brain statistics
        stats = {
            "step": step,
            "time": current_time,
            "neurons_total": brain.getTotalNeuronCount(),
            "synapses_total": brain.getTotalSynapseCount(),
            "neurons_active": brain.getActiveNeuronCount(),
            "neurons_firing": brain.getFiringNeuronCount(),
            "spikes_total": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "dev_stage": brain.getDevelopmentalStage(),
            "pending_spikes": brain.getPendingSpikeEventCount()
        }
        
        # Print statistics
        print(f"Step {stats['step']:4d} ({stats['time']:6.2f}s): "
              f"Neurons={stats['neurons_total']:4d} "
              f"Active={stats['neurons_active']:3d} "
              f"Firing={stats['neurons_firing']:3d} "
              f"Spikes={stats['spikes_total']:5d} "
              f"E/I={stats['e_i_ratio']:5.2f} "
              f"Dev={stats['dev_stage']}")
        
        # Create checkpoint periodically
        if step % 500 == 0 and step > last_checkpoint:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            if brain.save(checkpoint_file):
                print(f"  Checkpoint saved to {checkpoint_file}")
                last_checkpoint = step
            else:
                print(f"  Failed to save checkpoint")
        
        # Get system component stats
        if brain.getWorkingMemory():
            wm_size = brain.getWorkingMemory().getActiveTraces()
            print(f"  Working memory traces: {wm_size}")
        
        if brain.getEpisodicMemory():
            episodes = brain.getEpisodicMemory().getEpisodeCount()
            print(f"  Episodic memory episodes: {episodes}")
        
        if brain.getDopamine():
            dopamine = brain.getDopamine().getLevel()
            print(f"  Dopamine level: {dopamine:.3f}")
        
        if brain.getCuriosity():
            curiosity = brain.getCuriosity().getLevel()
            print(f"  Curiosity level: {curiosity:.3f}")

print("\n=== Monitoring Complete ===")

# Load latest checkpoint if available
for step in [1500, 1000, 500]:
    checkpoint_file = f"checkpoint_step_{step}.bin"
    try:
        brain.load(checkpoint_file)
        print(f"Loaded checkpoint from {checkpoint_file}")
        break
    except:
        continue
```

### Advanced Agent Operations

```python
import pynlm

# Create advanced agent setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=7, visionHeight=7)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Advanced Agent Operations ===")
print(f"Agent created with sensory input size: {agent.getSensoryInputSize()}")
print(f"Agent motor output size: {agent.getMotorOutputSize()}")

# === BEHAVIORAL EXPERIMENTS ===

# Experiment 1: Random exploration
print("\n--- Experiment 1: Random Exploration ---")
agent.reset()
world.reset()

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Exploration complete. Final position: ({agent.getBrain().getAgentBody().x:.1f}, {agent.getBrain().getAgentBody().y:.1f})")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")

# Experiment 2: Goal-directed behavior
print("\n--- Experiment 2: Goal-Directed Behavior ---")
agent.reset()
world.reset()

# Set goal position
goal_x, goal_y = 45.0, 45.0
agent_target = agent.getBrain().getAgentBody()

goal_reached = False
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check if goal reached
    dist_to_goal = ((agent_target.x - goal_x)**2 + (agent_target.y - goal_y)**2)**0.5
    if dist_to_goal < 2.0:
        goal_reached = True
        print(f"Goal reached at step {step}!")
        break

if not goal_reached:
    dist_to_goal = ((agent_target.x - goal_x)**2 + (agent_target.y - goal_y)**2)**0.5
    print(f"Goal not reached. Final distance to goal: {dist_to_goal:.1f}")

print(f"Learning levels - Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")

# Experiment 3: Memory formation and recall
print("\n--- Experiment 3: Memory Formation ---")
agent.reset()
world.reset()

# Visit several locations to form episodic memories
locations = [(10, 10), (20, 20), (30, 30), (40, 40)]
visited_locations = []

for step in range(500):
    # Choose random location to visit
    target_loc = locations[step % len(locations)]
    
    # Move towards target
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check if we reached target
    agent_body = agent.getBrain().getAgentBody()
    if abs(agent_body.x - target_loc[0]) < 2.0 and abs(agent_body.y - target_loc[1]) < 2.0:
        visited_locations.append(target_loc)
        print(f"Visited location {target_loc} at step {step}")
        
        # Store in episodic memory
        if brain.getEpisodicMemory():
            brain.getEpisodicMemory().storeEpisode(target_loc[0], target_loc[1], step)

print(f"\nVisited locations: {visited_locations}")
if brain.getEpisodicMemory():
    episodes = brain.getEpisodicMemory().getEpisodeCount()
    print(f"Episodic memory contains {episodes} episodes")

# Experiment 4: Development over time
print("\n--- Experiment 4: Development Over Time ---")
agent.reset()
world.reset()

print("Development stages over time:")
for step in range(0, 3000, 500):
    # Update development
    agent.updateDevelopment(0.1)
    
    # Check developmental stage
    dev_stage = brain.getDevelopmentalStage()
    print(f"  Step {step}: {dev_stage}")
    
    # Run a few simulation steps
    for _ in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print(f"\nFinal development stage: {brain.getDevelopmentalStage()}")
print(f"Final plasticity modulation: {agent.getNeuromodulationLevel():.3f}")

print("\n=== All Experiments Complete ===")

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

```python
import pynlm

# Create a simple brain to experiment with
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# === EXPERIMENT: LIF Neuron Dynamics ===
print("=== Experiment: LIF Neuron Dynamics ===")

# Monitor individual neuron activity
for step in range(200):
    brain.step(step)
    
    if step % 50 == 0:
        print(f"\nStep {step}:")
        # Get all neurons and check their states
        neurons = []
        for region in brain.getRegions():
            for pop in region.getPopulations():
                for neuron in pop.getNeurons():
                    neurons.append(neuron)
        
        # Analyze neuron population
        total_neurons = len(neurons)
        active_neurons = sum(1 for n in neurons if n.isFiring())
        firing_rate = brain.getAverageFiringRate()
        
        print(f"  Total neurons: {total_neurons}")
        print(f"  Active neurons: {active_neurons}")
        print(f"  Firing rate: {firing_rate:.3f}")
        
        # Check membrane potential distribution
        potentials = []
        for neuron in neurons:
            state = neuron.getState()
            potentials.append(state.membranePotential)
        
        if potentials:
            avg_potential = sum(potentials) / len(potentials)
            max_potential = max(potentials)
            min_potential = min(potentials)
            
            print(f"  Membrane potential: avg={avg_potential:.1f}, max={max_potential:.1f}, min={min_potential:.1f}")
        
        # Check refractory neurons
        refractory_count = sum(1 for n in neurons if n.getState().firingState == FiringState.Refractory)
        if refractory_count > 0:
            print(f"  Refractory neurons: {refractory_count}")

print("\n=== LIF Dynamics Experiment Complete ===")

# === EXPERIMENT: Synaptic Plasticity ===
print("\n=== Experiment: Synaptic Plasticity ===")

# Create brain with plasticity
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

brain = pynlm.createBrain(config)
brain.initialize()

print("Initial state:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Run plasticity experiment
for step in range(500):
    brain.step(step)
    
    if step % 100 == 99:
        print(f"\nStep {step}:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        # Check STDP system
        stdp = brain.getSTDP()
        if stdp:
            # STDP doesn't have public getters, so we track via spike counts
            pass

print("\n=== Synaptic Plasticity Experiment Complete ===")

# === EXPERIMENT: Memory Systems ===
print("\n=== Experiment: Memory Systems ===")

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("memory.working_capacity", 300)
config.set("memory.episodic_max_episodes", 500)
config.set("memory.associative_strength", 0.7f)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent for interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Initial memory systems:")
if brain.getWorkingMemory():
    wm_capacity = brain.getWorkingMemory().getCapacity()
    print(f"  Working memory capacity: {wm_capacity}")

if brain.getEpisodicMemory():
    max_episodes = brain.getEpisodicMemory().getMaxEpisodes()
    print(f"  Episodic memory max episodes: {max_episodes}")

# Run memory experiment
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 99:
        print(f"\nStep {step}:")
        if brain.getWorkingMemory():
            active_traces = brain.getWorkingMemory().getActiveTraces()
            print(f"  Working memory traces: {active_traces}")
        
        if brain.getEpisodicMemory():
            episodes = brain.getEpisodicMemory().getEpisodeCount()
            print(f"  Episodic memory episodes: {episodes}")

print("\n=== Memory Systems Experiment Complete ===")

# === EXPERIMENT: Neuromodulation ===
print("\n=== Experiment: Neuromodulation ===")

config = pynlm.createDefaultConfig()
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction_error.scale", 1.0f)

brain = pynlm.createBrain(config)
brain.initialize()

print("Initial neuromodulation:")
if brain.getDopamine():
    print(f"  Dopamine scale: {brain.getDopamine().getScale():.3f}")
if brain.getCuriosity():
    print(f"  Curiosity level: {brain.getCuriosity().getLevel():.3f}")
if brain.getNovelty():
    print(f"  Novelty level: {brain.getNovelty().getLevel():.3f}")
if brain.getPredictionError():
    print(f"  Prediction error scale: {brain.getPredictionError().getScale():.3f}")

# Run neuromodulation experiment
for step in range(400):
    brain.step(step)
    
    if step % 100 == 99:
        print(f"\nStep {step}:")
        if brain.getDopamine():
            print(f"  Dopamine level: {brain.getDopamine().getLevel():.3f}")
            print(f"  Dopamine plasticity factor: {brain.getDopamine().getPlasticityFactor():.3f}")
        if brain.getCuriosity():
            print(f"  Curiosity level: {brain.getCuriosity().getLevel():.3f}")
        if brain.getNovelty():
            print(f"  Novelty level: {brain.getNovelty().getLevel():.3f}")
        if brain.getPredictionError():
            print(f"  Prediction error level: {brain.getPredictionErrorSignal().getLevel():.3f}")

print("\n=== Neuromodulation Experiment Complete ===")

print("\n=== All Experiments Complete ===")

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
