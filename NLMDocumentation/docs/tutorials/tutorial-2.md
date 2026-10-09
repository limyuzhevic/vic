# Tutorial 2: Brain in a World

**Learning Objectives**: Create a complete NLM setup with brain, world, and agent components.

**Prerequisites**: Tutorial 1 (Silent Brain) completed.

**Difficulty**: Beginner

**Estimated Time**: 30 minutes

### Overview

In this tutorial, you'll extend your basic brain simulation to include a world and an agent. This creates a complete closed-loop system where:

- The **world** provides sensory input to the **agent**
- The **agent** processes sensory input and sends commands to the **brain**
- The **brain** learns from outcomes and adapts its behavior
- The **world** updates based on the brain's actions

This represents the fundamental architecture of NLM and is essential for understanding how intelligence can emerge from neural computation.

### What You'll Learn

- How to create and configure a world
- How to set up an agent brain interface
- How to run complete agent-world simulations
- Basic sensory-motor loop concepts
- How to observe and interact with the simulated environment

### Step 1: Create a Complete World Setup

Let's start by creating a complete world setup:

```python
import pynlm

print("=== Tutorial 2: Brain in a World ===")

# 1. Create the brain
print("1. Creating brain...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create the world
print("2. Creating world...")
world = pynlm.createSimpleWorld()

# Configure world parameters
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

print("   World configured:")
print(f"   Size: {world.getWidth()} x {world.getHeight()}")
print(f"   Vision: {world.getSensoryInputSize()}x{world.getSensoryInputSize()}")

# 3. Create the agent brain interface
print("3. Creating agent brain interface...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"   Agent initialized with brain: {brain.getTotalNeuronCount()} neurons")

# 4. Enable some learning features
print("4. Enabling learning features...")
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time

# Enable structural plasticity for growth
agent.enableStructuralPlasticity(True)

print("   Learning features enabled:")
print("   - Reward modulation: ON")
print("   - Structural plasticity: ON")
print("   - Development: ON")
print("   - Curiosity: ON")
```

### Step 2: Run a Simple World Simulation

Now let's run a basic simulation loop:

```python
print("5. Running simulation...")
print("   (Watch the agent explore the world and learn)")

# Run simulation for a number of steps
num_steps = 100
for step in range(num_steps):
    # Update world (advance time)
    world.update(timestep=0.1)
    
    # Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks and makes decisions
    brain.step(step)
    
    # Get action from brain activity
    action = agent.decodeMotorCommand()
    
    # Agent takes action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation based on outcome
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print status every 20 steps
    if step % 20 == 0:
        print(f"\n   Step {step}:")
        print(f"     Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"     Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"     Total spikes: {brain.getTotalSpikeCount()}")
        print(f"     Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"     Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"     Development stage: {brain.getDevelopmentalStage()}")
        print(f"     Agent energy: {world.getAgentBody().energy:.1f}")
        print(f"     Agent health: {world.getAgentBody().health:.1f}")
```

### Step 3: Create a Reusable Simulation Function

Let's create a reusable function that encapsulates the simulation loop:

```python
def run_simulation(brain, world, agent, num_steps, enable_learning=True):
    """Run a complete agent simulation with brain and world."""
    
    print(f"Running simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if enabled
        if enable_learning:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 25 == 0:
            print(f"  Step {step:4d}: "
                  f"Pos({world.getAgentBody().x:5.1f}, {world.getAgentBody().y:5.1f}) "
                  f"Firing({brain.getFiringNeuronCount():4d}) "
                  f"Curiosity({agent.getCuriosityLevel():.2f})")
    
    print(f"Simulation completed!")

# Use the function
print("\n6. Using reusable simulation function...")
run_simulation(brain, world, agent, 100)
```

### Step 4: Create Your First Experiment

Now let's create a meaningful experiment with a goal:

```python
import pynlm

print("\n=== Tutorial 2: First Experiment ===")

def foraging_experiment(world_size=15, target_food=5, max_steps=200):
    """Create a foraging experiment where agent seeks food."""
    
    print(f"Starting foraging experiment:")
    print(f"  World size: {world_size}x{world_size}")
    print(f"  Target food collected: {target_food}")
    print(f"  Maximum steps: {max_steps}")
    
    # Setup brain with learning
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=6,
        visionHeight=6
    )
    world.reset()
    world.setMaxEnergy(100.0)  # High energy for longer survival
    
    # Setup agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track experiment metrics
    food_collected = 0
    steps_without_food = 0
    max_food_collected = 0
    
    # Run simulation
    for step in range(max_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Check for food collection
        world_objects = []  # Would need actual world object access
        # Simplified: assume agent collects food on contact
        if world.getAgentBody().energy > 80:
            food_collected += 1
            max_food_collected = max(max_food_collected, food_collected)
            steps_without_food = 0
        else:
            steps_without_food += 1
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Log progress every 50 steps
        if step % 50 == 0:
            print(f"    Step {step:3d}: Food={food_collected}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Energy={world.getAgentBody().energy:.1f}")
        
        # Check completion
        if food_collected >= target_food or steps_without_food > 100:
            print(f"  Experiment completed at step {step}")
            break
    
    # Experiment results
    print(f"\nExperiment Results:")
    print(f"  Total steps completed: {step + 1}")
    print(f"  Food collected: {food_collected}")
    print(f"  Maximum food collected in episode: {max_food_collected}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return {
        'steps': step + 1,
        'food_collected': food_collected,
        'max_food_collected': max_food_collected,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_stage': brain.getDevelopmentalStage(),
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'spikes': brain.getTotalSpikeCount()
    }

# Run the experiment
results = foraging_experiment(world_size=20, target_food=10)
```

### Step 5: Your First Exercise

**Exercise**: Modify the world simulation to experiment with:

1. **Different world sizes**: Try worlds with sizes 10x10, 20x20, and 30x30
2. **Different agent starting positions**: Try different starting positions
3. **Different learning settings**: Disable curiosity or reward modulation
4. **Different simulation lengths**: Run simulations for 50, 200, or 500 steps

**What to observe**:
- How does world size affect agent behavior?
- What starting positions lead to better exploration?
- How do different learning settings affect performance?
- Can you detect patterns in the agent's movement?

**Create a comparison chart** showing how different settings affect:
- Agent survival time
- Food collection rate
- Curiosity level changes
- Neural activity patterns

### Key Concepts

#### Agent-Brain-World Architecture

The NLM agent-brain-world system creates a closed-loop:

1. **World**: Provides sensory input and rewards
2. **Sensory Processing**: Agent converts world percepts to neural signals
3. **Brain Processing**: Brain processes neural inputs and produces motor commands
4. **Motor Output**: Agent converts neural activity to actions
5. **World Effects**: Actions change world state and produce rewards
6. **Learning**: Brain adapts based on rewards and prediction errors

#### Key Components

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals for learning
- **Development**: Updates developmental stage over time
- **Attention**: Selective processing through competition

#### Brain Statistics

- **Firing neurons**: Neurons currently generating spikes
- **Active neurons**: Neurons currently processing information
- **Total spikes**: Cumulative learning signals
- **Firing rate**: Information processing rate
- **Development stage**: Current phase of brain maturation

#### Agent Behavior

- **Movement**: Position and orientation in world
- **Energy**: Resources for survival
- **Health**: Condition affecting performance
- **Age**: Developmental progression
- **Curiosity**: Drive for exploration and learning

### Common Issues and Solutions

**Problem 1: Agent doesn't move or explore**
- **Solution**: Check that `agent.decodeMotorCommand()` is returning actions
- **Solution**: Verify world is being updated with `world.update()`
- **Solution**: Ensure agent has energy (set high initially)

**Problem 2: Agent gets stuck in one location**
- **Solution**: Enable curiosity for exploration
- **Solution**: Add movement incentives
- **Solution**: Try different starting positions

**Problem 3: Too much or too little learning**
- **Solution**: Adjust reward modulation strength
- **Solution**: Enable/disable curiosity and development
- **Solution**: Modify brain configuration parameters

**Problem 4: Simulation too slow**
- **Solution**: Reduce world size or simulation steps
- **Solution**: Disable episodic memory if enabled
- **Solution**: Use simpler world configurations

### What's Next?

You've completed Tutorial 2! Now you're ready for Tutorial 3:

## Tutorial 3: Advanced Learning and Development

In Tutorial 3, you'll explore:
- Advanced learning mechanisms (prediction, attention)
- Developmental processes and maturation
- Memory systems (working memory, episodic memory)
- Complex cognitive tasks
- Performance analysis and optimization

Ready to continue? Move on to Tutorial 3 when you're ready!

---

*Tutorial Version*: 1.0
*Last Updated*: 2026-10-09
*Difficulty*: Beginner

**Need Help?** 
- Check the Troubleshooting Guide for common issues
- Look at the Python API Reference for function details
- Search for specific errors in the documentation