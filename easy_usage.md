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

## Advanced Examples (Phase 6 Integration)

### Project 4: Agent with Integrated Memory and Learning

```python
import pynlm

def run_integrated_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with ALL integrated systems."""
    
    # 1. Create configuration with Phase 6 systems
    config = pynlm.createDefaultConfig()
    
    # Enable all integrated subsystems
    config.set("brain.development.enable", True)           # Development stages
    config.set("brain.memory.episodic.enable", True)      # Memory integration
    config.set("brain.memory.working.enable", True)       # Working memory
    config.set("brain.neuromodulation.dopamine.enable", True)  # Dopamine
    config.set("brain.neuromodulation.curiosity.enable", True)  # Curiosity
    config.set("brain.neuromodulation.novelty.enable", True)   # Novelty
    config.set("brain.prediction.enable", True)           # Prediction system
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL integrated subsystems
    agent.enableRewardModulation(True)      # Dopamine learning
    agent.enableCuriosity(True)             # Exploration drive
    agent.enableDevelopment(True)           # Developmental stages
    agent.enableStructuralPlasticity(True)  # Neural growth
    
    # 7. Run simulation loop with integrated systems
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (with working memory)
        agent.processSensoryInput(percept)
        
        # Run brain step (integrates ALL systems)
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (dopamine)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (plasticity stages)
        agent.updateDevelopment(0.1)
        
        # Periodically display integrated system status
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Working Memory: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
            print(f"  Episodic Memory: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
            print(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
            print(f"  Avg Firing Rate: {brain.getAverageFiringRate():.3f}")
    
    # 8. Save checkpoint (persistence)
    brain.save("integrated_agent_checkpoint.bin")
    
    print("\n=== Integrated Agent Simulation Complete ===")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_integrated_agent_simulation(2000)
```

### Project 5: Environment Interaction with Prediction

```python
import pynlm

def run_prediction_agent_simulation(num_steps=1500):
    """Run simulation focusing on prediction and learning."""
    
    # Create brain with prediction system enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.prediction.enable", True)
    config.set("brain.prediction.history_size", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with prediction interface
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableCuriosity(True)  # Use prediction error for exploration
    
    # Store experience for prediction learning
    experiences = []
    
    # Run simulation
    for step in range(num_steps):
        # Get observation
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Store experience for prediction learning
        experiences.append({
            'step': step,
            'vision': percept.getVision(),
            'reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
            'action': action
        })
        
        # Get prediction error for monitoring
        prediction_error = agent.getPredictionError()
        
        if step % 300 == 0:
            print(f"Step {step}: Prediction Error = {prediction_error:.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            
            # Show prediction system status if available
            if hasattr(brain, 'getPredictionSystem') and brain.getPredictionSystem():
                print(f"  Predictions learned: {len(brain.getPredictionSystem().getHistory())}")
    
    print("\n=== Prediction Agent Simulation Complete ===")
    print(f"Total experiences stored: {len(experiences)}")
    return brain, agent, world, experiences

brain, agent, world, experiences = run_prediction_agent_simulation(1500)
```

### Project 6: Development Stages Affecting Behavior

```python
import pynlm

def run_development_simulation(num_steps=3000):
    """Run simulation showing how development affects behavior."""
    
    # Create brain with full development system
    config = pynlm.createDefaultConfig()
    config.set("brain.development.enable", True)
    config.set("brain.development.critical_period_length", 500)
    config.set("brain.development.maturation_length", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent in world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("=== Development Stage Simulation ===")
    print("Watch how development stages affect learning and behavior")
    print("Initial stage: CRITICAL PERIOD - high plasticity")
    print("Middle stage: MATURATION - balanced learning")
    print("Final stage: ADULT - stable behavior\n")
    
    # Run simulation and track development progression
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Display development stage changes
        current_stage = brain.getDevelopmentalStage()
        if step % 500 == 0:
            print(f"Step {step:4d} - Stage: {current_stage.name:12s} | "
                  f"Curiosity: {agent.getCuriosityLevel():.3f} | "
                  f"Novelty: {agent.getNoveltyLevel():.3f} | "
                  f"Firing: {brain.getFiringNeuronCount():4d}")
            
            # Show behavioral changes based on stage
            if current_stage.name == "CriticalPeriod":
                print("     -> High learning phase - exploring environment")
            elif current_stage.name == "Maturation":
                print("     -> Refining behaviors - memory consolidation")
            elif current_stage.name == "Adult":
                print("     -> Stable behavior - maintaining skills")
            elif current_stage.name == "Aging":
                print("     -> Declining plasticity - preserving knowledge")
    
    print("\n=== Development Simulation Complete ===")
    print("The brain progressed through all developmental stages")
    return brain, agent, world

brain, agent, world = run_development_simulation(3000)
```

### Project 7: Checkpointing and Replay

```python
import pynlm
import os

def run_checkpoint_simulation(num_steps=2000):
    """Run simulation demonstrating checkpointing and replay."""
    
    # Create brain with persistence enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.memory.episodic.enable", True)
    config.set("brain.memory.replay.enable", True)
    
    # First run: learn and create experiences
    print("=== Phase 1: Learning and Experience ===")
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    agent1 = pynlm.createAgentBrain(brain1)
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world1.reset()
    
    agent1.initialize(world1)
    agent1.enableCuriosity(True)
    
    # Run learning phase
    for step in range(num_steps // 2):
        world1.update(0.1)
        agent1.processSensoryInput(world1.getSensoryPercept())
        brain1.step(step)
        action = agent1.decodeMotorCommand()
        world1.applyMotorCommand(action, world1.getSimulationTime())
        
        if step % 400 == 0:
            print(f"Learning phase step {step}: "
                  f"Episodes: {brain1.getEpisodicMemory().getEpisodeCount() if brain1.getEpisodicMemory() else 0}")
    
    # Save checkpoint
    checkpoint_path = "/tmp/nlm_agent_checkpoint.bin"
    brain1.save(checkpoint_path)
    print(f"\nCheckpoint saved: {checkpoint_path}")
    
    # Display episodes stored
    if brain1.getEpisodicMemory():
        print(f"Episodic memory episodes: {brain1.getEpisodicMemory().getEpisodeCount()}")
    
    # Second run: load from checkpoint and replay
    print("\n=== Phase 2: Resume from Checkpoint ===")
    
    # Create new brain and load checkpoint
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(checkpoint_path):
        print("Successfully loaded checkpoint!")
        print(f"Resumed with {brain2.getTotalSpikeCount()} spikes from previous session")
        
        # Create agent for second session
        agent2 = pynlm.createAgentBrain(brain2)
        world2 = pynlm.createSimpleWorld()
        world2.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world2.reset()
        
        agent2.initialize(world2)
        
        # Run additional steps
        for step in range(num_steps // 2, num_steps):
            world2.update(0.1)
            agent2.processSensoryInput(world2.getSensoryPercept())
            brain2.step(step)
            action = agent2.decodeMotorCommand()
            world2.applyMotorCommand(action, world2.getSimulationTime())
            
            if step % 400 == 0:
                print(f"Resume phase step {step}: "
                      f"Curiosity: {agent2.getCuriosityLevel():.3f}")
    
    # Demonstrate replay mechanism
    print("\n=== Phase 3: Memory Replay ===")
    if brain2.getEpisodicMemory() and brain2.getEpisodicMemory().getEpisodeCount() > 0:
        print("Replay episodes for consolidation:")
        episodes_to_replay = brain2.getEpisodicMemory().getEpisodesForReplay(3)
        print(f"Selected {len(episodes_to_replay)} episodes for replay")
        
        # Simulate replay by processing stored episodes
        for i, episode in enumerate(episodes_to_replay):
            print(f"  Episode {i+1}: {episode.activeNeurons.size()} neurons, "
                  f"reward: {episode.reward:.3f}")
    
    # Clean up
    if os.path.exists(checkpoint_path):
        os.remove(checkpoint_path)
        print(f"\nCleaned up checkpoint: {checkpoint_path}")
    
    print("\n=== Checkpoint Replay Simulation Complete ===")
    return brain1, brain2, agent1, agent2, world1, world2

brain1, brain2, agent1, agent2, world1, world2 = run_checkpoint_simulation(2000)
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
