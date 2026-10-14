#!/usr/bin/env python3
"""
NLM Python Bindings - Main Demo Script

This script demonstrates the key features of the NLM Python bindings
through a comprehensive example of brain-world interaction.
"""

import sys
import os
import time

# Add the workspace root to the Python path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

print("NLM Python Bindings - Comprehensive Demo")
print("=" * 70)

# Import the main module
import pynlm

print("\n1. Creating a Brain with Custom Configuration")
print("-" * 50)

# Create a custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

print(f"Created config with {len(config.getKeys())} parameters")
print(f"  Neuron count: {config.get('brain.neuron_count')}")
print(f"  Synapse density: {config.get('brain.synapse_density')}")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("\n2. Creating and Configuring a World")
print("-" * 50)

world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=10,
    visionHeight=10
)
world.reset()
world.setAgentStart(15.0, 15.0)

print(f"Created world: {world.getWidth()}x{world.getHeight()}")
print(f"  Agent vision: {world.getAgentBody().getVisionWidth()}x{world.getAgentBody().getVisionHeight()}")
print(f"  Agent start: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")

print("\n3. Creating and Initializing an Agent")
print("-" * 50)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Created agent brain interface")
print(f"  Agent has {agent.getSensoryInputSize()} sensory input locations")
print(f"  Agent has {agent.getMotorOutputSize()} motor output locations")

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("\n4. Running Complete Agent Episode")
print("-" * 50)

print("Running simulation with learning enabled...")

start_time = time.time()

for step in range(50):
    # Update world
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation (simulate reward from world)
    internal_signals = percept.getInternal()
    reward = internal_signals[0] if internal_signals and len(internal_signals) > 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress every 10 steps
    if step % 10 == 0:
        print(f"  Step {step:2d}: "
              f"Firing={brain.getFiringNeuronCount():4d}, "
              f"Curiosity={agent.getCuriosityLevel():5.3f}, "
              f"Novelty={agent.getNoveltyLevel():5.3f}, "
              f"Action={action}")

end_time = time.time()
elapsed = end_time - start_time

print(f"\nEpisode completed in {elapsed:.2f} seconds")
print(f"Average performance: {50/elapsed:.1f} steps/second")

print("\n5. Collecting Brain Statistics")
print("-" * 50)

stats = {
    'Total neurons': brain.getTotalNeuronCount(),
    'Total synapses': brain.getTotalSynapseCount(),
    'Active neurons': brain.getActiveNeuronCount(),
    'Firing neurons': brain.getFiringNeuronCount(),
    'Total spikes': brain.getTotalSpikeCount(),
    'Average firing rate': brain.getAverageFiringRate(),
    'Excitement/Inhibition ratio': brain.getExcitationInhibitionRatio(),
    'Development stage': brain.getDevelopmentalStage(),
    'Agent curiosity': agent.getCuriosityLevel(),
    'Agent novelty': agent.getNoveltyLevel(),
    'Agent prediction error': agent.getPredictionError(),
    'World simulation time': world.getSimulationTime()
}

for key, value in stats.items():
    if isinstance(value, float):
        print(f"  {key:25s}: {value:8.3f}")
    else:
        print(f"  {key:25s}: {value:8d}")

print("\n6. Testing Convenience Functions")
print("-" * 50)

# Test quick_test
print("Testing quick_test()...")
brain2 = pynlm.quick_test()
print(f"✓ Quick test created brain with {brain2.getTotalNeuronCount()} neurons")

# Test create_simulation
print("\nTesting create_simulation()...")
sim = pynlm.create_simulation()
brain3 = sim['brain']
world3 = sim['world']
agent3 = sim['agent']

print(f"✓ Created complete simulation:")
print(f"  - Brain: {brain3.getTotalNeuronCount()} neurons")
print(f"  - World: {world3.getWidth()}x{world3.getHeight()}")
print(f"  - Agent configured")

# Test run_agent_episode
print("\nTesting run_agent_episode()...")
results = pynlm.run_agent_episode(steps=20, enable_features=True)

print(f"✓ Run episode completed:")
print(f"  - Steps: {results['steps_run']}")
print(f"  - Final curiosity: {results['final_stats']['curiosity']:.3f}")
print(f"  - Final firing rate: {results['final_stats']['avg_firing_rate']:.2f} Hz")

print("\n7. Testing Error Handling")
print("-" * 50)

# Test error handling with invalid configuration
print("Testing error handling...")
try:
    # This should raise an error (invalid configuration key)
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("invalid.key", "test")
    print("✗ Expected error for invalid configuration")
except Exception as e:
    print(f"✓ Correctly handled error: {type(e).__name__}")

print("\n" + "=" * 70)
print("Demo completed successfully!")
print("\nKey features demonstrated:")
print("  ✓ Custom configuration and brain creation")
print("  ✓ World setup and agent initialization")
print("  ✓ Complete agent episode with learning")
print("  ✓ Statistics collection and analysis")
print("  ✓ Convenience functions")
print("  ✓ Error handling")
print("\nThe NLM Python bindings provide a comprehensive API for:")
print("  • Neural network simulation")
print("  • Brain development and plasticity")
print("  • Agent perception and action")
print("  • World interaction and control")
print("  • Learning and adaptation")
print("  • Real-time monitoring and statistics")
print("\nThe NLM Python bindings are ready for use!")
