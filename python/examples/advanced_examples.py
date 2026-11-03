"""Advanced examples demonstrating NLM capabilities"""

import pynlm
import time

print("=== Advanced NLM Examples ===")

# Advanced configuration and learning
print("\n1. Advanced Learning Configuration:")
config = pynlm.createDefaultConfig()

# Configure advanced settings
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.setRandomSeed(42)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation with detailed monitoring
print("   Running advanced simulation with learning enabled...")
start_time = time.time()

for step in range(100):
    world.update(0.05)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent's state
    body = world.getAgentBody()
    reward = body.energy / 100.0  # Normalize energy
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.05)

elapsed = time.time() - start_time
print(f"   Completed in {elapsed:.2f} seconds")
print(f"   Final stats:")
print(f"     - Firing neurons: {brain.getFiringNeuronCount()}")
print(f"     - Total spikes: {brain.getTotalSpikeCount()}")
print(f"     - Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"     - Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"     - Development stage: {brain.getDevelopmentalStage()}")
print(f"     - Prediction error: {agent.getPredictionError():.3f}")
print(f"     - Average firing rate: {brain.getAverageFiringRate():.3f}")

# Demonstrate batch operations
print("\n2. Batch Processing Example:")
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Create multiple percepts for batch processing
print("   Processing multiple percepts in batch...")
percepts = []
for _ in range(10):
    percepts.append(world.getSensoryPercept())
    world.update(0.1)

# Process batch efficiently
for percept in percepts:
    agent.processSensoryInput(percept)
    brain2.step(0)

print(f"   Processed {len(percepts)} percepts in batch")
print(f"   Result: {brain2.getFiringNeuronCount()} neurons firing")

# Demonstrate configuration templates
print("\n3. Configuration Templates:")

def create_research_config(neuron_count=1000):
    """Create a configuration optimized for research scenarios"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", true)
    config.set("plasticity.structural.enable", true)
    config.set("neuromod.curiosity.enable", true)
    config.set("neuromod.novelty.enable", true)
    return config

def create_training_config(base_config=None):
    """Create a configuration optimized for training/learning"""
    if base_config is None:
        config = pynlm.createDefaultConfig()
    else:
        # Create a copy of the base config
        import copy
        config = copy.deepcopy(base_config)
    
    config.set("agent.enable_reward_modulation", True)
    config.set("agent.enable_structural_plasticity", True)
    config.set("agent.enable_development", True)
    config.set("agent.enable_curiosity", True)
    
    # Increase learning rate for training
    config.set("plasticity.stdp.learning_rate", 0.005)
    return config

# Use templates
research_config = create_research_config(500)
training_config = create_training_config(research_config)

print(f"   Research config: {research_config.getOr('brain.neuron_count', 0)} neurons")
print(f"   Training config has {len(training_config.getKeys())} entries")

# Demonstrate advanced checkpointing
print("\n4. Advanced Checkpointing:")
checkpoint_file = "/tmp/nlm_advanced_test.bin"

# Create a scenario where we have meaningful state to save
for step in range(20):
    brain.step(step)

brain.save(checkpoint_file)
print(f"   Saved brain with {brain.getTotalSpikeCount()} spikes")

# Load into new brain and verify state
brain3 = pynlm.createBrain(config)
brain3.initialize()
brain3.load(checkpoint_file)
print(f"   Restored brain with {brain3.getTotalSpikeCount()} spikes")

# Compare key metrics
spike_diff = abs(brain.getTotalSpikeCount() - brain3.getTotalSpikeCount())
print(f"   Spike count difference: {spike_diff} (should be small)")

print("\n=== Advanced examples completed! ===")
