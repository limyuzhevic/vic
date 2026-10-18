# NLM - Getting Started Guide

## Welcome to NLM!

NLM (熙然) is a neural simulation framework that creates artificial brains capable of learning, memory, and development. Think of it as teaching a computer how to think and learn like a brain!

## What Makes NLM Special?

- **Brain-like Intelligence**: Uses spiking neural networks similar to real brains
- **Developmental Learning**: Brains grow and adapt over time
- **Memory Integration**: Multiple memory systems (working, episodic, semantic)
- **Neuromodulation**: Reward, curiosity, and novelty drive learning
- **Environmental Interaction**: Agents sense and act in virtual worlds

## Quick Start: Three Essential Examples

We'll start with the simplest possible NLM program and gradually build up to complete agent simulations.

### Example 1: Silent Brain (Just Thinking)

```python
import pynlm

print("=== Example 1: Silent Brain ===")

# Step 1: Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Start the brain
brain.initialize()

# Step 3: Make it think for 10 steps
for i in range(10):
    brain.step(i)

# Step 4: See what happened
print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Currently firing: {brain.getFiringNeuronCount()}")
```

### Example 2: Brain in a World (Sensing and Acting)

```python
import pynlm

print("\n=== Example 2: Brain in a Simple World ===")

# Step 1: Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Run for 20 steps
for step in range(20):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get what the agent sees
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks
    brain.step(step)
    
    # 5. Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # 6. Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())

print("\nSimulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Example 3: Complete Learning Agent (Full Experience)

```python
import pynlm

print("\n=== Example 3: Complete Learning Agent ===")

def run_complete_simulation():
    """Run a complete agent simulation with learning and development"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Run episode
    for step in range(50):
        # Environment interaction
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Monitor progress
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("\nEpisode complete!")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

# Run the simulation
run_complete_simulation()
```

## Understanding the Three Key Components

NLM works with three main things:

### 1. Brain (The Thinking Part)
- **What it does**: Stores memories, makes decisions, learns from experience
- **How to use it**: 
  - `brain.initialize()` - Start up the brain
  - `brain.step(step)` - Make it think one moment
  - `brain.getFiringNeuronCount()` - Check how many neurons are active

### 2. World (The Environment)
- **What it does**: Contains agents, objects, and rules for interaction
- **How to use it**:
  - `world.configure(width, height, visionWidth, visionHeight)` - Set up the world
  - `world.update(timestep)` - Advance time
  - `world.getSensoryPercept()` - Get what agents see/hear
  - `world.applyMotorCommand(action, time)` - Make an action happen

### 3. Agent (The Bridge)
- **What it does**: Connects brain to world, processes sensory input
- **How to use it**:
  - `agent.initialize(world)` - Connect agent to world
  - `agent.processSensoryInput(percept)` - Give brain sensory information
  - `agent.decodeMotorCommand()` - Get action from brain activity
  - `agent.applyRewardModulation(reward, predicted)` - Learn from results

## Common Patterns and Tips

### Pattern 1: Run a Simulation
```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: monitor progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Use it:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Monitor Brain Statistics
```python
# Add these to your simulation loop:
print("=== Brain Statistics ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Currently firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Pattern 3: Enable Learning Features
```python
# These are optional but recommended for realistic behavior:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# You can also check what's enabled:
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
```

## Understanding the Analogies

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns | `agent.applyRewardModulation(reward, predicted)` |

## Common Troubleshooting

### "My brain isn't doing anything"
**Check**:
- Did you call `brain.initialize()`?
- Are you calling `brain.step()` in a loop?
- Try increasing the number of steps

### "The agent isn't moving"
**Check**:
- Did you call `world.applyMotorCommand()`?
- Is `world.update()` being called?
- Make sure `agent.decodeMotorCommand()` returns a valid action

### "Everything is 0"
**Explanation**: Brains need time to "warm up" - neurons need input to fire
**Solution**: Try more steps, or make sure sensory input is connected

### "Import error: No module named 'pynlm'"
**Solution**: Install the package
```bash
pip install .
```
or
```bash
pip install -e .
```

## What's Next?

### For Beginners:
1. **Master the three examples** above
2. **Try variations** (different world sizes, step counts)
3. **Read the API reference** for complete method listings
4. **Explore advanced features** in the documentation

### For Intermediate Users:
1. **Create custom configurations**
2. **Design experiments** with specific goals
3. **Combine multiple agents**
4. **Add custom world rules**

### For Advanced Users:
1. **Modify the source code**
2. **Create new neural architectures**
3. **Implement custom learning rules**
4. **Contribute to the project**

## Quick Reference

### Essential Commands
```python
# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Main loop
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Common Method Groups

#### Creation
```python
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

#### Initialization
```python
brain.initialize()
agent.initialize(world)
world.reset()
```

#### Main Loop
```python
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### Learning
```python
agent.enableRewardModulation(True)
agent.applyRewardModulation(reward, predicted)
agent.updateDevelopment(0.1)
```

#### Statistics
```python
brain.getFiringNeuronCount()
brain.getTotalSpikeCount()
brain.getAverageFiringRate()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
```

## Ready to Start?

You're now ready to use NLM! Start with the simple brain example and work your way up to complete learning agents.

**Next Steps**:
1. Install NLM: `pip install .`
2. Run Example 1: `python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); brain.step(0); print('Success!')"`
3. Try the full examples
4. Explore the documentation for advanced features

Welcome to the world of artificial brain simulation!
