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

### Example 3: Advanced Agent with Learning and Development

```python
import pynlm

def run_advanced_agent_simulation():
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5f)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("=== Advanced Agent Simulation ===")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print("Enabled subsystems: Reward, Structural Plasticity, Development, Curiosity")
    print()
    
    # Run simulation with comprehensive monitoring
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent/brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (simulate goal reaching)
        reward = 0.0f
        if world.getAgentBody().x > 25.0 and world.getAgentBody().y > 25.0:
            reward = 10.0f  # Goal reached
        
        agent.applyRewardModulation(reward, 0.0f)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step:3d}: "
                  f"Firing: {brain.getFiringNeuronCount():4d} | "
                  f"Curiosity: {agent.getCuriosityLevel():.2f} | "
                  f"Novelty: {agent.getNoveltyLevel():.2f} | "
                  f"Dopamine: {agent.getNeuromodulationLevel():.2f} | "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Simulation Complete ===")
    print(f"Final brain stats:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 'N/A'}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the advanced simulation
advanced_brain, advanced_agent, advanced_world = run_advanced_agent_simulation()
```

### Example 4: Batch Simulation with Checkpointing

```python
import pynlm
import time

def batch_simulation_with_checkpoints():
    """Run multiple simulations with checkpointing"""
    
    # Create base configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    simulations = []
    
    for sim_id in range(3):
        print(f"\n=== Simulation {sim_id + 1}/3 ===")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation with periodic checkpoints
        start_time = time.time()
        
        for step in range(200):
            world.update(0.1)
            
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simple reward for exploring
            if step % 50 == 0:
                agent.applyRewardModulation(1.0f, 0.0f)
        
        end_time = time.time()
        
        print(f"Simulation completed in {end_time - start_time:.2f}s")
        print(f"Total reward: {brain.getTotalSpikeCount() * 0.01f:.2f}")
        print(f"Final curiosity: {agent.getCuriosityLevel():.2f}")
        
        # Save checkpoint
        checkpoint_file = f"checkpoint_sim_{sim_id + 1}.bin"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
        
        simulations.append((brain, agent, world))
    
    print("\n=== All simulations completed ===")
    return simulations

# Run batch simulation
batch_results = batch_simulation_with_checkpoints()
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

### Example 5: Command-Line Tools and Advanced Usage

```python
import pynlm
import subprocess
import os

def demonstrate_command_line_tools():
    """Demonstrate the advanced command-line tools"""
    
    print("=== Command-Line Tools Demonstration ===\n")
    
    # Example 1: Batch simulation with checkpointing
    print("1. Running batch simulation with command-line arguments...")
    
    # You can run this command in terminal:
    # ./nlm --batch 1000 --save brain_checkpoint.bin --experiment learning
    # Or in Python using subprocess:
    
    print("   Command: nlm --batch 1000 --save brain_checkpoint.bin --experiment learning")
    print("   This runs 1000 simulation steps and saves checkpoint")
    print()
    
    # Example 2: Advanced experiment types
    print("2. Advanced experiment types:")
    experiment_types = [
        ("learning", "Learning and plasticity demonstration"),
        ("neuromodulation", "Neuromodulation system test"),
        ("plasticity", "Plasticity mechanisms test"),
        ("memory", "Memory integration test"),
        ("development", "Development system test"),
        ("full", "Complete integration test")
    ]
    
    for exp_type, description in experiment_types:
        print(f"   nlm --experiment {exp_type:12} # {description}")
    
    print()
    
    # Example 3: Configuration validation
    print("3. Configuration validation:")
    print("   nlm --validate --config my_config.json")
    print("   Validates configuration before running simulation")
    print()
    
    # Example 4: Statistics and profiling
    print("4. Statistics and profiling:")
    print("   nlm --stats --benchmark")
    print("   Shows detailed statistics and runs benchmark tests")
    print()
    
    return True

def demonstrate_python_api_improvements():
    """Demonstrate the new Python API improvements"""
    
    print("=== New Python API Improvements ===\n")
    
    print("1. Enhanced convenience methods:")
    print("   brain.runSteps(1000, 0.001)  # Run multiple steps efficiently")
    print("   agent.runEpisode(world, 500)  # Run complete agent episode")
    print("   world.applyAction(action, time)  # Convenience alias")
    print()
    
    print("2. Advanced system access:")
    print("   spike_system = brain.getSpikeSystem()")
    print("   stdp = brain.getSTDP()")
    print("   curiosity = brain.getCuriosity()")
    print("   working_memory = brain.getWorkingMemory()")
    print()
    
    print("3. Comprehensive statistics:")
    print("   stats = brain.getBrainStats()")
    print("   neuromod_stats = agent.getNeuromodulationStats()")
    print("   development_info = agent.getDevelopmentStats()")
    print()
    
    print("4. System management:")
    print("   agent.enableAllSubsystems()  # Enable everything")
    print("   agent.disableAllSubsystems()  # Disable everything")
    print("   agent.printStatus()  # Print comprehensive status")
    print()
    
    return True

# Run demonstrations
if __name__ == "__main__":
    demonstrate_command_line_tools()
    print("\n" + "="*60 + "\n")
    demonstrate_python_api_improvements()
    
    print("\n" + "="*60)
    print("Enhanced Python API and command-line tools demonstration complete!")
    print("="*60)
```
