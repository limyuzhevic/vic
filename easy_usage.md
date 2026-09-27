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

## Advanced Features (Phase 6.5)

### Quick Start with Advanced Features

```python
import pynlm
from pynlm_advanced import (
    SimulationPreset, BrainVisualizer, AgentTrainer,
    SimulationProfiler, create_preset_brain,
    create_agent_world_setup, setup_debug_monitor
)

# Step 1: Create brain using a preset
print("Creating brain with LEARNING_EXPERIMENT preset...")
brain_config = create_preset_brain(SimulationPreset.LEARNING_EXPERIMENT)
brain = brain_config['brain']
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Step 2: Set up complete agent-world system
print("\nSetting up agent-world system...")
system = create_agent_world_setup(vision_width=16, vision_height=16)
brain = system['brain']
world = system['world']
agent = system['agent']
print(f"System configured: {system['vision_size']} vision pixels")

# Step 3: Set up comprehensive monitoring
print("\nSetting up debug monitoring...")
monitor = setup_debug_monitor(
    brain=brain, world=world, agent=agent,
    log_level="INFO", enable_profiling=True
)

# Step 4: Run advanced simulation with activity tracking
print("\nRunning enhanced simulation...")
visualizer = BrainVisualizer(brain)

for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect activity data periodically
    if step % 100 == 0:
        visualizer.collect_activity_data(500, 100)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Step 5: Analyze and visualize results
print("\nAnalyzing simulation results...")
activity_data = visualizer.collect_activity_data(500, 100)
analysis = visualizer.analyze_patterns(activity_data)

print(f"\n=== SIMULATION ANALYSIS ===")
print(f"Average firing rate: {analysis['firing_rate_stats']['mean']:.3f}")
print(f"Firing rate trend: {analysis['firing_rate_stats']['trend']:.4f}")
print(f"Firing rate stability: {analysis.get('stability', {}).get('coefficient_of_variation', 'N/A'):.3f}")
print(f"Detected patterns: {list(analysis['patterns'].keys())}")
if 'oscillations' in analysis['patterns']:
    osc = analysis['patterns']['oscillations']
    print(f"Oscillation frequency: {1/osc['average_interval']:.3f} Hz")

# Step 6: Performance profiling (if enabled)
if monitor['profiler']:
    print("\n=== PERFORMANCE ANALYSIS ===")
    # You can add performance analysis here if needed
    pass
```

## Advanced Pattern 1: Training with Custom Rewards

```python
def curiosity_reward(world, agent, step):
    """Custom reward that encourages exploration and curiosity."""
    novelty = agent.getNoveltyLevel()
    curiosity = agent.getCuriosityLevel()
    prediction_error = abs(agent.getPredictionError())
    
    # Reward for high curiosity and novelty
    reward = novelty * 0.5 + curiosity * 0.3 + prediction_error * 0.2
    
    # Bonus for discovering new areas
    if hasattr(world, 'getSensoryPercept'):
        percept = world.getSensoryPercept()
        if hasattr(percept, 'getVision'):
            vision = percept.getVision()
            if vision:
                unique_areas = len(set(v[0] for v in vision if v[0] > 0.1))
                reward += unique_areas * 0.1
    
    return reward

# Create system and train with custom reward
system = create_agent_world_setup(vision_width=8, vision_height=8)
trainer = AgentTrainer(system['brain'], system['world'], system['agent'])

# Train with custom reward function
results = trainer.train_with_custom_reward(
    reward_function=curiosity_reward,
    num_episodes=50,
    exploration_factor=1.5
)

print(f"Training complete! Mean reward: {results['mean_reward']:.3f}")
print(f"Reward std: {results['std_reward']:.3f}")
print(f"Average episode length: {results['mean_length']:.1f} steps")
```

## Advanced Pattern 2: Curriculum Learning

```python
def create_learning_curriculum():
    """Create a curriculum for progressive learning."""
    return [
        {
            'name': 'Sensory Foundation',
            'episodes': 5,
            'steps': 200,
            'world_config': {'visionWidth': 4, 'visionHeight': 4},
            'features': {'RewardModulation': True, 'Curiosity': True}
        },
        {
            'name': 'Motor Coordination',
            'episodes': 8,
            'steps': 400,
            'world_config': {'visionWidth': 8, 'visionHeight': 8},
            'features': {'RewardModulation': True, 'Curiosity': True, 'StructuralPlasticity': True}
        },
        {
            'name': 'Complex Navigation',
            'episodes': 10,
            'steps': 600,
            'world_config': {'visionWidth': 16, 'visionHeight': 16},
            'features': {'RewardModulation': True, 'Curiosity': True, 'Development': True}
        }
    ]

# Run curriculum training
curriculum = create_learning_curriculum()
system = create_agent_world_setup(vision_width=16, vision_height=16)
trainer = AgentTrainer(system['brain'], system['world'], system['agent'])

print("Starting curriculum learning...")
curriculum_results = trainer.train_with_curriculum(curriculum, base_steps=1000)

print(f"\n=== CURRICULUM RESULTS ===")
print(f"Total episodes: {curriculum_results['total_episodes']}")
print(f"Final mean reward: {curriculum_results['final_performance']['mean_reward']:.3f}")
improvement = curriculum_results['reward_improvement']
print(f"Improvement rate: {improvement['rate']}")
print(f"Best stage: {improvement['best_stage']}")
```

## Advanced Pattern 3: Simulation Profiling

```python
import time

# Set up system with profiler
system = create_agent_world_setup(vision_width=8, vision_height=8)
brain = system['brain']
world = system['world']
agent = system['agent']

# Create profiler and run simulation with profiling
profiler = SimulationProfiler(brain, world, agent)

print("Running simulation with profiling...")
start_time = time.time()

for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}/200")

total_time = time.time() - start_time
print(f"Simulation completed in {total_time:.2f} seconds")

# Profile the simulation
profile_data = profiler.profile_simulation(brain, world, agent, 200, log_interval=50)

# Generate performance report
report = profiler.generate_performance_report(profile_data)
print("\n=== PERFORMANCE REPORT ===")
print(report)
```

## Quick Reference for Advanced Features

| Advanced Feature | Module | Main Class/Function | Description |
|------------------|--------|-------------------|-------------|
| **Presets** | `pynlm_advanced` | `create_preset_brain()` | Pre-configured brains for different research goals |
| **System Setup** | `pynlm_advanced` | `create_agent_world_setup()` | Complete agent-world system creation |
| **Activity Analysis** | `pynlm_advanced` | `BrainVisualizer` | Pattern detection and statistical analysis |
| **Training** | `pynlm_advanced` | `AgentTrainer` | Custom reward functions and curriculum learning |
| **Performance** | `pynlm_advanced` | `SimulationProfiler` | Simulation timing and resource analysis |
| **Debug Setup** | `pynlm_advanced` | `setup_debug_monitor()` | Comprehensive logging and debugging tools |

## Next Steps

When you're comfortable with the basics:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

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
