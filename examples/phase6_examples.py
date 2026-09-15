# NLM Phase 6 Integration Examples

## Example 1: Basic Brain Creation

This example shows the simplest way to create and initialize a brain:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Example 2: Running Brain Steps

This example demonstrates running simulation steps and observing plasticity:

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
```

## Example 3: Agent with World Integration

This example shows a complete agent-world integration:

```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Example 4: Save and Load Brain State

This example demonstrates checkpointing functionality:

```python
import pynlm
import os

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for step in range(50):
    brain.step(step)

# Save brain state
checkpoint_file = "nlm_checkpoint.bin"
save_success = brain.save(checkpoint_file)

# Load into new brain
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
load_success = brain2.load(checkpoint_file)

# Clean up
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
```

## Example 5: Developmental Progression

This example shows how development affects plasticity:

```python
import pynlm

# Create agent with development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Development stages progression:")
print(f"Initial stage: {agent.getDevelopmentalStage()}")

# Run simulation across developmental stages
for timestep in range(0, 1000, 100):
    agent.updateDevelopment(timestep)
    print(f"Time {timestep}: {agent.getDevelopmentalStage()}")
```

## Example 6: Complete Integrated Simulation

This example runs a complete simulation with all Phase 6 systems:

```python
import pynlm
import json
import time
import os

def save_results(results, filename="nlm_phase6_results.json"):
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)

# Create and configure brain for Phase 6
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable all systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
results = {"episodes": []}
for episode in range(5):
    world.reset()
    agent.reset()
    
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    episode_data = {
        "episode": episode + 1,
        "reward": 0.0,  # Calculate based on actual rewards
        "stage": agent.getDevelopmentalStage()
    }
    results["episodes"].append(episode_data)

# Save results
save_results(results)
```

## Running the Examples

To run these examples:

```bash
python phase6_examples.py
```

Or run specific examples:

```bash
python -c "import pynlm; config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config); brain.initialize(); print('Brain created!')"
```

## Key Features Demonstrated

1. **Brain Initialization**: Creating and setting up a neural brain
2. **Simulation Steps**: Running the brain through time steps
3. **World Integration**: Connecting brain to environment
4. **Agent Interface**: Using the agent brain interface
5. **Checkpointing**: Saving and loading brain state
6. **Development**: Showing developmental stage progression
7. **Complete Integration**: Running all Phase 6 systems together

## Benefits for Users

- **Simple**: Easy to start with basic functionality
- **Scalable**: Can run from simple tests to complex simulations
- **Flexible**: Can integrate with different world types
- **Robust**: Includes checkpointing for persistence
- **Educational**: Shows developmental progression clearly
- **Practical**: Real-world usage examples

This example set provides users with a comprehensive understanding of how to use NLM Phase 6 features, from basic neural simulation to complex integrated brain-world systems.