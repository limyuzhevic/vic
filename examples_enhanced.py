import pynlm

#=============================================================================
# NLM Python Example Gallery - Enhanced Features Demonstration
#=============================================================================

# Example 1: Basic Brain Creation with Enhanced Features
print("=== Example 1: Enhanced Brain Creation and Simulation ===")

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.neuron_count} neurons")
print(f"Initial development stage: {brain.development_stage}")

# Run enhanced simulation
print("\nRunning enhanced simulation...")
for i in range(5):
    brain.step(i)
    if i % 1 == 0:  # Print every step for demo
        print(f"  Step {i}: {brain.firing_neurons} firing neurons, "
              f"{brain.average_firing_rate:.2f} Hz average rate")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
print(f"Brain status:\n{brain.get_status()}")

print("\n" + "="*60 + "\n")

# Example 2: Complete Agent with Enhanced API Features
print("=== Example 2: Advanced Agent with Enhanced Python API ===")

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Enabled advanced agent subsystems:")
print(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")

# Run enhanced agent episode
print("\nRunning enhanced agent episode...")
for step in range(10):
    # Update world
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation with reward feedback
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    print(f"  Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
          f"Novelty={agent.getNoveltyLevel():.3f}, "
          f"Dev Stage={agent.getDevelopmentalStage()}")

print(f"\nEpisode complete! Agent statistics:\n{agent.get_episode_stats()}")

print("\n" + "="*60 + "\n")

# Example 3: Interactive Brain Exploration
print("=== Example 3: Interactive Brain Exploration with Python Features ===")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use Pythonic properties for quick access
print(f"Initial brain properties:")
print(f"  Neurons: {brain.neuron_count}")
print(f"  Synapses: {brain.synapse_count}")
print(f"  Development Stage: {brain.development_stage}")
print(f"  Average Firing Rate: {brain.average_firing_rate:.2f} Hz")

# Demonstrate simulated behavior with enhanced API
print("\nSimulating brain development...")
for step in range(8):
    brain.step(step)
    
    # Use Pythonic properties for monitoring
    print(f"  Step {step}: {brain.firing_neurons} firing neurons, "
          f"{brain.average_firing_rate:.2f} Hz average")
    
    # Demonstrate property-based access
    if step % 3 == 0:
        print(f"    E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")

print(f"\nFinal brain status:\n{brain.get_status()}")

print("\n" + "="*60 + "\n")

# Example 4: Configuration Management
print("=== Example 4: Configuration Management ===")

config = pynlm.createDefaultConfig()

# Use Pythonic configuration methods
print(f"Initial config has 'brain.neuron_count': {config.has('brain.neuron_count')}")
print(f"Available config keys: {config.getKeys()}")

# Modify configuration
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)

print(f"\nModified configuration:")
print(f"  Neuron count: {config.get('brain.neuron_count')}")
print(f"  Synapse density: {config.get('brain.synapse_density')}")
print(f"  STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")

# Create new brain with modified config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created new brain with {brain.neuron_count} neurons")

print("\n" + "="*60 + "\n")

# Example 5: Sensory Processing Pipeline
print("=== Example 5: Sensory Processing Pipeline ===")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Setting up sensory processing pipeline...")
print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
print(f"Vision dimensions: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")

# Run sensory processing cycle
for step in range(5):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    print(f"\nStep {step}:")
    print(f"  Vision: {len(percept.getVision())} values")
    print(f"  Touch: {len(percept.getTouch())} values")
    print(f"  Internal: {len(percept.getInternal())} values")
    print(f"  Proprioception: {len(percept.getProprioception())} values")
    
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("\nSensory processing pipeline completed successfully!")

print("\n" + "="*60 + "\n")

# Example 6: Error Handling and Exception Management
print("=== Example 6: Enhanced Error Handling ===")

try:
    # Create brain and test error conditions
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Testing error handling...")
    
    # Test normal operations
    brain.step(0)
    print("✓ Basic brain step completed successfully")
    
    # Test property access
    status = brain.get_status()
    print("✓ Status retrieval completed successfully")
    
    # Test simulation
    spike_count = brain.simulate(5, 0.1)
    print(f"✓ Enhanced simulation completed, {spike_count} total spikes")
    
except Exception as e:
    print(f"Error occurred: {e}")
    print("Error handling test completed")

print("\n" + "="*60 + "\n")

print("NLM Python Example Gallery Complete!")
print("All enhanced features demonstrated successfully.")
print(f"Module version: {pynlm.__version__}")
print(f"Module author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")
