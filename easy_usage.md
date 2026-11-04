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

## Advanced Usage: Memory and Learning

### Working Memory Examples

```python
import pynlm

# Create a brain with working memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get working memory (if available)
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active memory traces: {working_memory.getActiveTraces()}")
    
    # Store sensory input in working memory
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Process sensory input (automatically stores in working memory)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Check what's stored
    memory_content = working_memory.getMemoryActivations()
    print(f"Working memory activity levels: {len(memory_content)} traces")
```

### Episodic Memory and Experience Storage

```python
import pynlm

# Create brain with episodic memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()
agent.initialize(world)

# Enable episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episodic_memory.setMaxEpisodes(1000)

# Run simulation and store experiences
for episode in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(episode)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Simulate reward based on position
    agentBody = world.getAgentBody()
    if agentBody:
        dist_to_center = ((agentBody.x - 7.5)**2 + (agentBody.y - 7.5)**2)**0.5
        reward = max(0, 1 - dist_to_center / 7.5)
        agent.applyRewardModulation(reward, 0)

# Retrieve stored experiences
if episodic_memory:
    recent_episodes = episodic_memory.getRecentEpisodes(5)
    print(f"Recent episodes: {len(recent_episodes)}")
    for i, episode in enumerate(recent_episodes):
        print(f"  Episode {i}: reward={episode.reward:.2f}, steps={episode.age}")
```

### Neuromodulation Configuration

```python
import pynlm

# Create configuration with enhanced neuromodulation
config = pynlm.createDefaultConfig()

# Configure dopamine (reward and plasticity)
config.set("neuromod.dopamine_baseline", 0.1)
config.set("neuromod.dopamine_release_rate", 1.0)
config.set("neuromod.dopamine_decay_rate", 0.1)

# Configure acetylcholine (attention and memory)
config.set("neuromod.acetylcholine_baseline", 0.05)
config.set("neuromod.acetylcholine_attention_weight", 0.3)
config.set("neuromod.acetylcholine_memory_consolidation_weight", 0.7)

# Configure norepinephrine (arousal and vigilance)
config.set("neuromod.norepinephrine_baseline", 0.03)
config.set("neuromod.norepinephrine_arousal_weight", 0.4)

# Configure serotonin (mood and social behavior)
config.set("neuromod.serotonin_baseline", 0.02)
config.set("neuromod.serotonin_social_weight", 0.5)

# Create brain with neuromodulation
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with enhanced neuromodulation
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

### Task-Specific Brain Configuration

```python
import pynlm

# Configure brain for different tasks
tasks = {
    "navigation": {
        "neuron_count": 1500,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
    },
    "pattern_learning": {
        "neuron_count": 1000,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
    },
    "social_learning": {
        "neuron_count": 1200,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.013,
        "synaptogenesis_rate": 0.00015,
        "pruning_rate": 0.000015,
    }
}

# Create task-specific configuration
def createTaskConfig(task_name):
    config = pynlm.createDefaultConfig()
    task_params = tasks.get(task_name, tasks["navigation"])
    
    for key, value in task_params.items():
        if isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, int):
            config.set(key, value)
    
    return config

# Use task-specific configuration
nav_config = createTaskConfig("navigation")
navigation_brain = pynlm.createBrain(nav_config)
navigation_brain.initialize()
```

### Simulation Management and Utilities

```python
import pynlm

# Create simulation with context manager
class Simulation:
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
    def initialize(self):
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        self.agent.initialize(self.world)
        self.brain.initialize()
        
    def run_episode(self, steps, task_type="navigation"):
        for step in range(steps):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Task-specific reward
            if task_type == "navigation":
                agent_body = self.world.getAgentBody()
                if agent_body:
                    dist_to_center = ((agent_body.x - 10)**2 + (agent_body.y - 10)**2)**0.5
                    reward = max(0, 1 - dist_to_center / 10)
                    self.agent.applyRewardModulation(reward, 0)
                    
    def get_stats(self):
        stats = {}
        if self.brain:
            stats["neurons"] = self.brain.getTotalNeuronCount()
            stats["synapses"] = self.brain.getTotalSynapseCount()
            stats["spikes"] = self.brain.getTotalSpikeCount()
            stats["learning"] = self.brain.getLearnedPreference()
            
        if self.world:
            stats["world_width"] = self.world.getWidth()
            stats["world_height"] = self.world.getHeight()
            
        return stats

# Use the simulation
sim = Simulation()
sim.initialize()
sim.run_episode(100, "navigation")
stats = sim.get_stats()
print(f"Simulation completed: {stats}")
```

### Batch Processing and Benchmarking

```python
import pynlm

# Create multiple configurations for benchmarking
configs = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("random_seed", i * 100)
    config.set("neuron_count", 500 * (i + 1))
    configs.append(config)

# Run batch simulation
print("Running batch simulation...")
results = []

for i, config in enumerate(configs):
    print(f"  Simulation {i+1}/5...")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward for testing
        reward = 0.5  # Fixed reward for benchmarking
        agent.applyRewardModulation(reward, 0)
    
    # Collect results
    result = {
        "neurons": brain.getTotalNeuronCount(),
        "synapses": brain.getTotalSynapseCount(),
        "spikes": brain.getTotalSpikeCount(),
        "learning": brain.getLearnedPreference(),
        "firing": brain.getFiringNeuronCount()
    }
    results.append(result)

# Print summary
print("\nBatch Results Summary:")
print("=====================")
for i, result in enumerate(results):
    print(f"Simulation {i+1}: {result['neurons']} neurons, "
          f"{result['synapses']} synapses, "
          f"{result['spikes']} spikes, "
          f"learning={result['learning']:.3f}")
```

### Memory Analysis and Debugging

```python
import pynlm

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Run simulation to generate memory content
for step in range(50):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Simple reward
    agent.applyRewardModulation(0.5, 0)

# Analyze memory systems
print("Memory System Analysis:")
print("======================")

# Working memory analysis
working_memory = brain.getWorkingMemory()
if working_memory:
    print("Working Memory:")
    print(f"  Capacity: {working_memory.getCapacity()} traces")
    print(f"  Active traces: {working_memory.getActiveTraces()}")
    print(f"  Memory activity: {working_memory.getMemoryActivity():.3f}")
    
    memory_neurons = working_memory.getMemoryNeurons()
    memory_activations = working_memory.getMemoryActivations()
    print(f"  Stored neurons: {len(memory_neurons)}")
    print(f"  Activation range: {min(memory_activations):.3f} - {max(memory_activations):.3f}")

# Episodic memory analysis
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print("\nEpisodic Memory:")
    print(f"  Episode count: {episodic_memory.getEpisodeCount()}")
    print(f"  Max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"  Replay enabled: {'Yes' if episodic_memory.isReplayEnabled() else 'No'}")
    
    recent_episodes = episodic_memory.getRecentEpisodes(3)
    print(f"  Recent episodes: {len(recent_episodes)}")

# Associative memory analysis
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print("\nAssociative Memory:")
    print(f"  Association count: {associative_memory.getAssociationCount()}")

# Prediction system analysis
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("\nPrediction System:")
    print("  Prediction system active (analysis not yet implemented)")

# Cognition systems analysis
neural_planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
self_model = brain.getSelfModel()

if neural_planner:
    print("\nCognition Systems:")
    print(f"  Neural planner: active (analysis not yet implemented)")
    
if concept_formation:
    concept_count = concept_formation.getConceptCount()
    print(f"  Concept formation: {concept_count} concepts")
    
if self_model:
    print(f"  Self model: active (analysis not yet implemented)")

# Neuromodulation analysis
print("\nNeuromodulation Levels:")
if hasattr(agent, 'getCuriosityLevel'):
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
if hasattr(agent, 'getNoveltyLevel'):
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
if hasattr(agent, 'getPredictionError'):
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
if hasattr(agent, 'getNeuromodulationLevel'):
    print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```
