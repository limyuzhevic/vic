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

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Advanced Usage Examples

### 1. Advanced Brain Configuration

#### Custom Neural Architecture

```python
import pynlm

# Create custom configuration for specific brain architecture
config = pynlm.createDefaultConfig()

# Configure for cognitive tasks
config.set("brain.neuron_count", 3000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.15)
config.set("brain.initial_weight_mean", 0.6)

# Configure specific brain regions
config.set("region.0.name", "sensory_cortex")
config.set("region.0.neuron_type_distribution.excitatory", 0.7)
config.set("region.0.neuron_type_distribution.inhibitory", 0.3)

config.set("region.1.name", "motor_cortex")
config.set("region.1.neuron_type_distribution.excitatory", 0.6)
config.set("region.1.neuron_type_distribution.inhibitory", 0.4)

config.set("region.2.name", "association_area")
config.set("region.2.neuron_type_distribution.excitatory", 0.5)
config.set("region.2.neuron_type_distribution.inhibitory", 0.5)

# Configure memory systems
config.set("memory.working_capacity", 500)
config.set("memory.episodic_max_episodes", 5000)
config.set("memory.associative_strength", 0.8)

# Configure plasticity rules
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Configure development
config.set("development.critical_period", 100.0)
config.set("development.maturation_rate", 0.01)

# Create and initialize brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Advanced brain created: {brain.getTotalNeuronCount()} neurons in {brain.getRegionCount()} regions")
```

#### Specialized Agent for Learning Tasks

```python
import pynlm

# Configure for complex learning and adaptation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 5)
config.set("memory.episodic_max_episodes", 10000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.structural.enable", True)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all subsystems enabled
agent = pynlm.createAgentBrain(brain)

# Configure world for learning tasks
world = pynlm.createSimpleWorld()
world.configure(
    width=50, 
    height=50, 
    visionWidth=16, 
    visionHeight=16
)
world.setRandomSeed(12345)  # Reproducible experiments
world.reset()

agent.initialize(world)

# Enable all advanced subsystems for learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning experiment
print("Starting learning experiment...")
learning_neuralities = []
for step in range(2000):
    world.update(0.05)  # Larger timestep for faster simulation
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on task performance
    reward = 0.0
    # Example: reward for approaching target
    body = world.getAgentBody()
    if body and body.x > 40 and body.y > 40:  # Near goal
        reward = 10.0
    
    # Check if agent is stuck
    if step % 100 == 0:
        prev_pos = getattr(agent, 'last_position', None)
        if prev_pos:
            current_pos = (body.x, body.y) if body else (0, 0)
            distance_moved = ((current_pos[0] - prev_pos[0])**2 + (current_pos[1] - prev_pos[1])**2)**0.5
            if distance_moved < 0.1:
                reward = -5.0  # Penalize inactivity
        if body:
            agent.last_position = (body.x, body.y)
    
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.05)
    
    if step % 200 == 0:
        learning_neuralities.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'dopamine': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })

print(f"Learning complete. Final firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### 2. Multi-Region Brain Configuration

#### Three-Region Architecture (Sensory-Motor-Cognitive)

```python
import pynlm

# Create multi-region brain for different cognitive functions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure 3 distinct regions with different functions
# Region 0: Sensory processing (high E/I ratio for amplification)
region0_id = brain.addRegion("Sensory_Cortex")
brain.setRegionProperty(region0_id, "type", "sensory")
brain.setRegionProperty(region0_id, "excitation_inhibition_ratio", 3.0)
brain.setRegionProperty(region0_id, "plasticity_rate", 2.0)

# Region 1: Motor control (strong recurrent connections)
region1_id = brain.addRegion("Motor_Cortex")
brain.setRegionProperty(region1_id, "type", "motor")
brain.setRegionProperty(region1_id, "recurrent_connection_density", 0.3)
brain.setRegionProperty(region1_id, "action_selection", True)

# Region 2: Association and working memory (balanced)
region2_id = brain.addRegion("Association_Cortex")
brain.setRegionProperty(region2_id, "type", "association")
brain.setRegionProperty(region2_id, "working_memory_capacity", 1000)
brain.setRegionProperty(region2_id, "concept_formation", True)

# Configure inter-region connections for information flow
# Sensory -> Association (sensory input processing)
brain.addInterRegionConnection(region0_id, region2_id, 0.5, 1)

# Association -> Motor (action selection)
brain.addInterRegionConnection(region2_id, region1_id, 1.2, 2)

# Sensory -> Motor (reflex pathway)
brain.addInterRegionConnection(region0_id, region1_id, 0.3, 0)

# Configure working memory with multiple memory systems
working_mem = brain.getWorkingMemory()
working_mem.setCapacity(2000)  # Large working memory
episodic_mem = brain.getEpisodicMemory()
episodic_mem.setMaxEpisodes(5000)  # Store long-term memories
associative_mem = brain.getAssociativeMemory()

# Run simulation with multi-region brain
agent = pynlm.createAgentBrain(brain)

# Create complex world with varied stimuli
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=12, visionHeight=12)
world.setRandomSeed(42)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Simulation loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

# Analyze multi-region activity
print("Multi-region brain analysis:")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"  Region {region_id.index()}: {region.getTotalNeuronCount()} neurons, "
          f"{region.getActiveNeuronCount()} active, avg weight {region.getAverageSynapticWeight():.3f}")
```

### 3. Complex World Interaction

#### Resource Collection Environment

```python
import pynlm

# Configure brain for resource collection task
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 4000)
config.set("memory.episodic_max_episodes", 3000)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create resource-rich world with obstacles and rewards
world = pynlm.createSimpleWorld()
world.configure(width=60, height=60, visionWidth=10, visionHeight=10)

# Place resources
resources = [
    (10, 10, 1.0),  (50, 10, 2.0), (20, 40, 1.5),
    (55, 45, 3.0), (30, 25, 0.5), (5, 35, 2.5),
]
for x, y, value in resources:
    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=value))

# Place hazards
for x, y in [(35, 20), (40, 20), (45, 20), (40, 35)]:
    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-1.0))

# Place walls (boundary)
for x in range(0, 60):
    world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, value=0.0))
    world.addObject(pynlm.WorldObject(x, 59, pynlm.WorldObjectType.Wall, value=0.0))
for y in range(1, 59):
    world.addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall, value=0.0))
    world.addObject(pynlm.WorldObject(59, y, pynlm.WorldObjectType.Wall, value=0.0))

world.reset()
agent.initialize(world)

# Enable advanced world interaction features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Tracking variables
resources_collected = 0
energy_level = 100.0
collection_log = []

# Resource collection simulation
print("Starting resource collection simulation...")
for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for resource collection
    body = world.getAgentBody()
    if body:
        # Check if we're at a resource location
        for obj in world.getAllObjects():
            if obj.type == pynlm.WorldObjectType.Resource and obj.value > 0:
                dist = ((obj.x - body.x)**2 + (obj.y - body.y)**2)**0.5
                if dist < 2.0:  # Collection radius
                    resources_collected += 1
                    energy_level += obj.value * 0.1  # Energy gain
                    # Remove collected resource
                    world.removeObject(obj.x, obj.y)
                    collection_log.append({
                        'step': step,
                        'position': (body.x, body.y),
                        'energy': energy_level
                    })
                    break
    
    # Calculate reward based on performance
    reward = 0.0
    if resources_collected > 0:
        reward += resources_collected * 0.5
    
    # Energy penalty (simulating metabolic cost)
    energy_level -= 0.1
    if energy_level <= 0:
        reward -= 10.0  # Severe penalty for starvation
    
    # Novelty bonus for exploring new areas
    if step % 100 == 0:
        reward += agent.getNoveltyLevel() * 2.0
    
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 300 == 0:
        print(f"Step {step}: Resources {resources_collected}, Energy {energy_level:.1f}, "
              f"Curiosity {agent.getCuriosityLevel():.3f}")

print(f"\nResource collection complete!")
print(f"Total resources collected: {resources_collected}")
print(f"Final energy level: {energy_level:.1f}")
```

### 4. Learning and Development Simulation

#### Developmental Process Modeling

```python
import pynlm

# Configure brain for studying developmental processes
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("development.critical_period", 150.0)
config.set("development.maturation_rate", 0.02)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create learning environment with changing difficulty
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
world.setRandomSeed(123)
world.reset()

agent.initialize(world)
agent.enableDevelopment(True)
agent.enableRewardModulation(True)

# Track developmental progression
developmental_metrics = {
    'stage_changes': [],
    'plasticity_changes': [],
    'learning_metrics': []
}

print("Simulating developmental process...")
print("Time (steps) | Stage | Plasticity | Learning | Firing Rate")
print("------------|-------|------------|----------|------------")

for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide learning reward
    reward = 0.5 + agent.getCuriosityLevel() * 0.5  # Reward exploration
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Record developmental changes
    if step % 100 == 0:
        stage = brain.getDevelopmentalStage()
        plasticity = agent.getNeuromodulationLevel()
        
        # Record stage change if different from previous
        if not developmental_metrics['stage_changes'] or \
           developmental_metrics['stage_changes'][-1]['stage'] != stage:
            developmental_metrics['stage_changes'].append({
                'step': step,
                'stage': stage,
                'age': agent.getNeuromodulationLevel()
            })
        
        # Record plasticity changes
        developmental_metrics['plasticity_changes'].append({
            'step': step,
            'plasticity': plasticity,
            'learning_rate': brain.getAverageFiringRate()
        })
        
        print(f"{step:10d} | {stage:6s} | {plasticity:10.3f} | {agent.getCuriosityLevel():8.3f} | {brain.getAverageFiringRate():10.3f}")

# Analyze developmental progression
print("\nDevelopmental Analysis:")
print(f"Developmental stages reached: {len(developmental_metrics['stage_changes'])}")
for change in developmental_metrics['stage_changes']:
    print(f"  Stage {change['stage']} at step {change['step']} (age: {change['age']:.1f})")

final_stage = brain.getDevelopmentalStage()
plasticity_modifier = 1.0 - agent.getNeuromodulationLevel()  # Reverse for readability

print(f"\nFinal developmental stage: {final_stage}")
print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

### 5. Checkpointing and Persistence

#### Save/Load Brain State

```python
import pynlm
import os

# Configure and create brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("checkpoint_dir", "./checkpoints")

brain = pynlm.createBrain(config)

print("Initializing brain...")
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Simulation with checkpointing
print("Running simulation...")
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply small reward
    reward = 0.1
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: Firing rate {brain.getAverageFiringRate():.3f}, "
              f"Curiosity {agent.getCuriosityLevel():.3f}")

# Save checkpoint
print("\nSaving checkpoint...")
save_path = brain.save("brain_checkpoint")
if save_path:
    print(f"✓ Brain state saved successfully")
    print(f"  Checkpoint size: {os.path.getsize(save_path) / 1024:.1f} KB")
else:
    print("✗ Failed to save checkpoint")

# Simulate brain sleep/rest period
print("Simulating rest and consolidation...")
for step in range(200):
    brain.step(step)

# Restore from checkpoint
print("\nRestoring from checkpoint...")
restored = brain.load("brain_checkpoint")
if restored:
    print("✓ Brain state restored successfully")
    
    # Continue simulation
    print("Continuing simulation after restore...")
    for step in range(500, 800):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = 0.05
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Restored state firing rate {brain.getAverageFiringRate():.3f}")
else:
    print("✗ Failed to restore from checkpoint")

print("\nCheckpointing demonstration complete!")
```

### 6. Performance Monitoring

#### Real-time Brain Performance Metrics

```python
import pynlm
import time

# Configure brain for performance testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 2)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Performance monitoring enabled")
print("Step | Time(ms) | Neurons | Spikes | FiringRate | E/I | Curiosity | Novelty | DevStage")
print("-----|----------|---------|--------|------------|-----|-----------|---------|---------")

performance_data = []
start_time = time.time()

for step in range(1000):
    loop_start = time.time()
    
    world.update(0.05)  # Fast timestep for performance testing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = 0.0
    if step % 50 == 0:
        reward = 1.0  # Occasional reward
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.05)
    
    loop_time = (time.time() - loop_start) * 1000  # Convert to ms
    
    # Record metrics
    metrics = {
        'step': step,
        'time_ms': loop_time,
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'dev_stage': brain.getDevelopmentalStage()
    }
    performance_data.append(metrics)
    
    if step % 100 == 0:
        print(f"{step:4d} | {loop_time:8.1f} | {metrics['neurons']:6d} | "
              f"{metrics['spikes']:5d} | {metrics['firing_rate']:10.3f} | "
              f"{metrics['e_i_ratio']:4.2f} | {metrics['curiosity']:8.3f} | "
              f"{metrics['novelty']:6.3f} | {metrics['dev_stage']:8s}")

# Performance analysis
end_time = time.time()
total_time = end_time - start_time
avg_loop_time = sum(d['time_ms'] for d in performance_data) / len(performance_data)

print(f"\nPerformance Analysis (total time: {total_time:.2f}s)")
print(f"Average simulation step: {avg_loop_time:.2f} ms")
print(f"Simulation rate: {1000 / (avg_loop_time/1000):.1f} steps/second")

# Calculate stability metrics
firing_rates = [d['firing_rate'] for d in performance_data]
e_i_ratios = [d['e_i_ratio'] for d in performance_data if d['e_i_ratio'] != float('inf')]

print(f"\nStability Metrics:")
print(f"  Firing rate (mean/std): {sum(firing_rates)/len(firing_rates):.3f} ± {((sum((x - sum(firing_rates)/len(firing_rates))**2 for x in firing_rates)/len(firing_rates))**0.5):.3f}")
print(f"  E/I ratio (mean/std): {sum(e_i_ratios)/len(e_i_ratios):.3f} ± {((sum((x - sum(e_i_ratios)/len(e_i_ratios))**2 for x in e_i_ratios)/len(e_i_ratios))**0.5):.3f}")

# Find spikes in activity (learning events)
active_steps = [d for d in performance_data if d['curiosity'] > 0.7]
print(f"  High curiosity steps: {len(active_steps)}/{len(performance_data)} ({len(active_steps)/len(performance_data)*100:.1f}%)")

print("\nPerformance monitoring complete!")
```

---
