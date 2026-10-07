"""
NLM Python Examples and Tutorials

Comprehensive examples demonstrating the improved NLM Python API
"""

# ==============================================================================
# BASIC BRAIN USAGE
# ==============================================================================

import pynlm
from pynlm import Brain, Config, create_brain
from pynlm import Vision, Audio, InternalSignals
from pynlm import Action, ActionType
from pynlm import SimpleWorld, AgentBrain

# Example 1: Simplest Brain
print("=== Example 1: Simplest Brain ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print()

# Example 2: Brain in a Simple World
print("=== Example 2: Brain in a Simple World ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print()

# Example 3: Complete Agent with Learning
print("=== Example 3: Complete Agent with Learning ===")
config = pynlm.createDefaultConfig()
config["brain.regionCount"] = 5
config["brain.plasticityHebbianRate"] = 0.01

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Reward mod: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.2f}")

print("Agent simulation complete!")
print()

# ==============================================================================
# SENSORIAL PROCESSING
# ==============================================================================

print("=== Example 4: Sensory Processing ===")
# Create different sensory inputs
vision = pynlm.createVision(width=64, height=64, channels=3)
# Vision data would be set here (e.g., from camera or image)

audio = pynlm.createAudio(sample_rate=44100, num_samples=1024)

internal = pynlm.createInternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.addSignal(0.3)

# Process sensory input
print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
print(f"Internal signals: {internal.getSignals()}")
print(f"Internal signal mean: {internal.getSignalMean():.3f}")
print(f"Internal signal std: {internal.getSignalStd():.3f}")
print()

# ==============================================================================
# ACTION CREATION AND EXECUTION
# ==============================================================================

print("=== Example 5: Action Creation ===")
# Create different types of actions
actions = [
    pynlm.move_forward(),
    pynlm.move_backward(),
    pynlm.turn_left(),
    pynlm.turn_right(),
    pynlm.interact(),
    pynlm.wait(),
    pynlm.eat(),
    pynlm.drink()
]

for action in actions:
    print(f"{action.getName()}: type={action.getType()}, movement={action.isMovement()}, interaction={action.isInteraction()}")

# Create custom action
parameters = [0.5, 0.8, 0.3]
custom_action = pynlm.createCustomAction(parameters)
print(f"Custom action: {custom_action.getName()} with parameters {custom_action.getParameters()}")
print()

# ==============================================================================
# WORLD INTERACTION
# ==============================================================================

print("=== Example 6: World Interaction ===")
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
world.reset()
world.set_agent_start(20, 20)

# Add objects to world
resource = pynlm.create_resource(x=10, y=10, value=5.0)
world.add_object(resource)

wall = pynlm.create_wall(x=15, y=15, radius=0.8)
world.add_object(wall)

hazard = pynlm.create_hazard(x=25, y=25, value=2.0)
world.add_object(hazard)

# Run agent in world
brain = pynlm.createBrain()
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    if result.is_good():
        print(f"Step {step}: {action.getName()} - {result.message} (Reward: {result.reward:.1f})")

print("World interaction complete!")
print(f"Final position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
print()

# ==============================================================================
# CONFIGURATION MANAGEMENT
# ==============================================================================

print("=== Example 7: Configuration Management ===")
# Create configuration
config = pynlm.createDefaultConfig()

# Set various configuration options
config["brain.regionCount"] = 8
config["brain.plasticityHebbianRate"] = 0.02
config["brain.plasticitySTDPStrength"] = 0.15
config["brain.developmentPlasticityMultiplier"] = 1.5
config["world.width"] = 50
config["world.height"] = 50
config["world.energyDecayRate"] = 0.3

print("Configuration:")
for key in config.getKeys():
    value = config[key]
    print(f"  {key}: {value}")

# Save configuration
config.save_to_file("my_nlm_config.json")
print("Configuration saved to 'my_nlm_config.json'")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.load_from_file("my_nlm_config.json")
print(f"Loaded configuration has {len(new_config.getKeys())} keys")
print()

# ==============================================================================
# BRAIN CONTROL AND MONITORING
# ==============================================================================

print("=== Example 8: Brain Control and Monitoring ===")
brain = pynlm.createBrain()
brain.initialize()

# Add some regions
region1 = brain.addRegion("Visual Cortex")
region2 = brain.addRegion("Motor Cortex")

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getRegionCount()} regions")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"EI ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get brain status
brain.log_status()
print()

# ==============================================================================
# ADVANCED FEATURES
# ==============================================================================

print("=== Example 9: Advanced Features ===")
# Get all neuromodulation levels
brain = pynlm.createBrain()
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(pynlm.createSimpleWorld())

# Enable all features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Enabled features:")
print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")

print("Neuromodulation levels:")
print(f"  Dopamine: {agent.getNeuromodulationLevel():.2f}")
print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
print(f"  Prediction error: {agent.getPredictionError():.2f}")

print("Development stage:", agent.getDevelopmentalStage().name)
print()

print("=== All Examples Completed Successfully! ===")
