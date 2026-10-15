# High-Level Python API Examples
# Create comprehensive examples demonstrating the enhanced Pythonic interface

# Basic usage examples

# Example 1: Simple brain simulation
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrainWithDefaultConfig()

# Run a simulation with progress tracking
print("Running simulation...")
pynlm.runSimulation(brain, 100, lambda step: print(f"Step {step}/100"))
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")

# Example 2: Agent-environment integration
world = pynlm.createSimpleWorld()
brain = pynlm.createBrainWithDefaultConfig()
agent_brain = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent_brain.initialize(world)

# Run an episode
print("Running agent episode...")
for step in range(50):
    world.update(0.1)
    agent_brain.processSensoryInput(world.getSensoryPercept())
    agent_brain.updateDevelopment(0.1)

print(f"Episode complete. Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")

# Example 3: Configuration management
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 2000)
config.setDouble("brain.simulation_timestep", 0.002)

brain2 = pynlm.createBrain(config)
print(f"Created brain with {brain2.getTotalNeuronCount()} neurons")

# Example 4: Learning experiment
brain = pynlm.createBrainWithDefaultConfig()
pynlm.runLearningExperiment(brain, 1000)
print(f"Learning experiment completed. Total spikes: {brain.getTotalSpikeCount()}")

# Example 5: Complete agent setup
world, brain, agent_brain = pynlm.createCompleteAgentWorld()
print("Created complete agent world")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Brain neurons: {brain.getTotalNeuronCount()}")

# Example 6: Context manager usage
print("\nUsing context managers...")
with pynlm.createBrain() as brain:
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

print("Context manager example complete")

# Example 7: Batch processing with callbacks
print("\nRunning batch simulation...")
def progress_callback(step):
    if step % 10 == 0:
        print(f"Progress: {step}/100 steps")

pynlm.runSimulation(brain, 100, progress_callback)

# Example 8: Sensory processing pipeline
vision_data = [0.1, 0.5, 0.9, 0.3, 0.7] * 4  # 20 elements for 4x5x1 image
vision = pynlm.Vision(4, 5)
vision.setData(vision_data)
print(f"Vision data validated: {vision.is_valid()}")

# Example 9: Action utilities
action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.3])
print(f"Action: {action.name} with parameters {action.parameters}")
action.reset()

# Example 10: Object management
obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0, 0.5)
world.addObject(obj)
distance = obj.distance_to(world.getAgentBody())
print(f"Distance to agent: {distance:.2f}")

print("\nAll examples completed successfully!")
