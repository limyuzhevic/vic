# Advanced Python API Examples
# More sophisticated usage patterns and advanced features

import pynlm
import math

# Example 1: Advanced brain configuration
print("=== Advanced Brain Configuration ===")

config = pynlm.Config()
config.setInt("brain.neuron_count", 5000)
config.setDouble("brain.simulation_timestep", 0.0005)
config.setFloat("brain.connection_probability", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Simulation timestep: {brain.getConfig().getDouble('brain.simulation_timestep', 0.001)}")

# Example 2: Region-based brain exploration
print("\n=== Region-Based Brain Exploration ===")

# Create multiple regions
region1_id = brain.addRegion("SensoryRegion")
region2_id = brain.addRegion("MotorRegion")
region3_id = brain.addRegion("AssociationRegion")

print(f"Created regions: {region1_id}, {region2_id}, {region3_id}")

# Get region information
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    if region:
        neuron_count = region.getAllNeurons().size()
        synapse_count = region.getSynapses().size()
        print(f"Region {region_id}: {neuron_count} neurons, {synapse_count} synapses")

# Example 3: Sensory pipeline processing
print("\n=== Sensory Pipeline Processing ===")

world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Process multiple sensory inputs over time
for step in range(20):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    
    # Analyze sensory data
    vision = percept.getVision()
    if vision.is_valid():
        print(f"Step {step}: Vision {vision.getWidth()}x{vision.getHeight()} processed")
        
        # Normalize vision data
        normalized = vision.get_normalized_data()
        mean_intensity = sum(normalized) / len(normalized)
        print(f"  Mean intensity: {mean_intensity:.3f}")
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    agent_brain.updateDevelopment(0.1)

print(f"Final neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
print(f"Final curiosity level: {agent_brain.getCuriosityLevel():.3f}")

# Example 4: Action selection and decoding
print("\n=== Action Selection and Decoding ===")

# Create various actions
actions = [
    pynlm.Action(pynlm.ActionType.MoveForward, [0.8]),
    pynlm.Action(pynlm.ActionType.TurnLeft, [0.5]),
    pynlm.Action(pynlm.ActionType.Interact, [0.3, 0.7]),
    pynlm.Action(pynlm.ActionType.Wait, [])
]

for i, action in enumerate(actions):
    print(f"Action {i}: {action.name}")
    print(f"  Parameters: {action.parameters}")
    print(f"  Is empty: {action.is_empty()}")

# Decode motor commands from neural activity
motor_command = agent_brain.decodeMotorCommand()
print(f"Decoded motor command: {motor_command}")

# Example 5: Object interaction and world management
print("\n=== Object Interaction and World Management ===")

world = pynlm.createSimpleWorld()
world.configure(200, 150, 30, 30)

# Add various objects to the world
objects = [
    pynlm.WorldObject(50.0, 50.0, pynlm.WorldObjectType.Resource, 1.0, 0.3),
    pynlm.WorldObject(120.0, 80.0, pynlm.WorldObjectType.Hazard, 0.5, 0.4),
    pynlm.WorldObject(180.0, 120.0, pynlm.WorldObjectType.Wall, 1.0, 0.6),
    pynlm.WorldObject(30.0, 100.0, pynlm.WorldObjectType.Marker, 0.1, 0.2)
]

for obj in objects:
    world.addObject(obj)
    print(f"Added {obj.type} at ({obj.x}, {obj.y})")

# Check distances and interactions
agent = world.getAgentBody()
for obj in objects:
    distance = agent.distance_to(obj)
    is_near = agent.is_near(obj, 2.0)
    print(f"Object at ({obj.x}, {obj.y}) is {distance:.1f} units away, "
          f"{'near' if is_near else 'far'}")

# Example 6: Memory and learning simulation
print("\n=== Memory and Learning Simulation ===")

brain = pynlm.createBrainWithDefaultConfig()

# Enable plasticity for learning
brain.enablePlasticity(True, hebbian=True, stdp=True)

# Run learning simulation with progress tracking
def learning_progress(step):
    if step % 200 == 0:
        print(f"Learning step {step}/1000")
        print(f"  Spikes: {brain.getTotalSpikeCount()}, "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Ratio: {brain.getExcitationInhibitionRatio():.3f}")

print("Running learning simulation...")
pynlm.runSimulation(brain, 1000, learning_progress)

print(f"Learning complete. Total spikes: {brain.getTotalSpikeCount()}")
print(f"Final excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Example 7: Batch processing and performance optimization
print("\n=== Batch Processing and Performance ===")

# Create multiple brains for batch processing
brains = [pynlm.createBrainWithDefaultConfig() for _ in range(5)]

print("Initializing batch of brains...")
for i, brain in enumerate(brains):
    brain.initialize()
    print(f"Brain {i+1}: {brain.getTotalNeuronCount()} neurons")

# Run batch simulation with shared progress callback
def batch_progress(step):
    if step % 50 == 0:
        print(f"Batch step {step}/500")

print("Running batch simulation...")
for brain in brains:
    pynlm.runSimulation(brain, 100, batch_progress)

print(f"Batch simulation complete. Average spikes: {sum(b.getTotalSpikeCount() for b in brains) / len(brains):.1f}")

# Example 8: Type-safe configuration management
print("\n=== Type-Safe Configuration Management ===")

config = pynlm.createDefaultConfig()

# Type-safe getters with defaults
neuron_count = config.getInt("brain.neuron_count", 1000)
timestep = config.getDouble("brain.simulation_timestep", 0.001)
probability = config.getFloat("brain.connection_probability", 0.1)

print(f"Configuration:")
print(f"  Neuron count: {neuron_count}")
print(f"  Simulation timestep: {timestep}")
print(f"  Connection probability: {probability}")

# Set values with type safety
config.setInt("brain.neuron_count", 3000)
config.setDouble("brain.simulation_timestep", 0.0005)

print(f"Updated configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Simulation timestep: {config.getDouble('brain.simulation_timestep')}")

# Example 9: Context manager usage for resource management
print("\n=== Context Manager Usage ===")

with pynlm.createBrain() as brain1:
    brain1.initialize()
    print(f"Brain 1 initialized with {brain1.getTotalNeuronCount()} neurons")
    
    # Use the brain
    for i in range(20):
        brain1.step(i, i * 0.001)
    
    print(f"Brain 1 simulation complete. Spikes: {brain1.getTotalSpikeCount()}")

print("Brain 1 automatically reset and resources released")

# Example 10: Error handling and validation
print("\n=== Error Handling and Validation ===")

config = pynlm.Config()
config.setInt("test.value", 42)

# Try to get non-existent key with default
default_value = config.getInt("non.existent.key", 100)
print(f"Default value for non-existent key: {default_value}")

# Validate configuration
errors = config.validate()
if errors:
    print(f"Configuration validation errors: {errors}")
else:
    print("Configuration is valid")

# Example 11: Advanced simulation utilities
print("\n=== Advanced Simulation Utilities ===")

brain = pynlm.createBrainWithDefaultConfig()
brain.enablePlasticity(True)

# Run learning experiment with custom timestep
print("Running enhanced learning experiment...")
pynlm.runLearningExperiment(brain, 500)

print(f"Enhanced experiment completed. Total spikes: {brain.getTotalSpikeCount()}")

# Example 12: Agent world creation utility
print("\n=== Agent World Creation Utility ===")

world, brain, agent_brain = pynlm.createCompleteAgentWorld()

# Set agent start position
world.setAgentStart(50, 50)

# Run episode with action logging
for step in range(30):
    world.update(0.1)
    
    agent_brain.processSensoryInput(world.getSensoryPercept())
    action = agent_brain.decodeMotorCommand()
    
    print(f"Step {step}: Action {action} (Neuromod: {agent_brain.getNeuromodulationLevel():.3f})")

print("Episode complete with action logging")

print("\n=== All Advanced Examples Completed Successfully ===")
