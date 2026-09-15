"""NLM Brain - Neural Learning Machine

A computational brain simulation framework that implements artificial developmental
brain systems with neurons, synapses, memory, learning, and perception.

## Core Components

1. **Brain** - The neural processing system with neurons and synapses
2. **World** - The environment where the brain operates  
3. **AgentBrain** - Interface connecting brain to world

## Quick Examples

### Create a Simple Brain
```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run the brain for 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"Brain thought {brain.getTotalSpikeCount()} times!")
```

### Brain in a World
```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a complete simulation loop
for step in range(50):
    world.update(0.1)                    # Update world state
    percept = world.getSensoryPercept()  # Get sensory data
    agent.processSensoryInput(percept)   # Feed to brain
    brain.step(step)                      # Brain processes
    action = agent.decodeMotorCommand()   # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete! Neurons firing: {brain.getFiringNeuronCount()}")
```

### Complete Learning Agent
```python
import pynlm

# Setup brain with world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Learning simulation complete!")
```

## Understanding the Components

### Brain Methods
- `brain.step(step)` - Process one time step
- `brain.initialize()` - Setup the neural network
- `brain.reset()` - Reset brain state
- `brain.getTotalSpikeCount()` - Get number of neural spikes
- `brain.getFiringNeuronCount()` - Get currently active neurons
- `brain.getAverageFiringRate()` - Get average neural activity
- `brain.getConfig()` - Get brain configuration

### World Methods
- `world.configure(width, height, visionWidth, visionHeight)` - Setup world size
- `world.update(dt)` - Advance world by time dt
- `world.reset()` - Reset world to initial state
- `world.getSensoryPercept()` - Get what the agent perceives
- `world.getAgentBody()` - Get agent's position and state
- `world.applyMotorCommand(cmd, time)` - Execute an action

### AgentBrain Methods
- `agent.processSensoryInput(percept)` - Feed perception to brain
- `agent.decodeMotorCommand()` - Get action from brain
- `agent.applyRewardModulation(reward, predicted)` - Learn from rewards
- `agent.updateDevelopment(dt)` - Brain development over time
- `agent.getNeuromodulationLevel()` - Get reward prediction error
- `agent.getCuriosityLevel()` - Get exploration drive
- `agent.getNoveltyLevel()` - Get change detection
- `agent.getPredictionError()` - Get prediction accuracy

## Learning Features

The NLM brain includes several advanced learning mechanisms:

1. **Reward Modulation** - Learn from positive/negative outcomes
2. **Curiosity** - Explore novel situations
3. **Structural Plasticity** - Form new connections over time
4. **Development** - Brain matures and changes capability
5. **Prediction Error** - Compare expectations with reality

## Configuration

Use `pynlm.createDefaultConfig()` for simple setups, or create custom configs:
```python
config = pynlm.createDefaultConfig()
# Configure with custom settings...
brain = pynlm.createBrain(config)
```

## Get Started

1. **Install**: `pip install pynlm`
2. **Start**: Run the examples above
3. **Explore**: Try different world sizes and brain configurations
4. **Learn**: Enable different learning features to see their effects

The NLM framework provides a complete brain-in-environment simulation platform
for research and education in computational neuroscience and artificial intelligence.
