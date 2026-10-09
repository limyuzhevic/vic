import pynlm
import numpy as np

"""
Basic Neural Network Example

This example demonstrates the basic usage of NLM's enhanced Python API.
It creates a simple neural network and runs a learning simulation.
"""

print("=== Basic NLM Neural Network Example ===\n")

# Create brain with default configuration
print("1. Creating brain with default configuration...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"   and {brain.getTotalSynapseCount()} synapses")

# Run initial simulation
print("\n2. Running initial simulation...")
for step in range(50):
    brain.step(step)

print(f"   Simulation completed with {brain.getTotalSpikeCount()} spikes")
print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")

# Demonstrate enhanced API
print("\n3. Demonstrating enhanced Python API...")

# Enable neuromodulation
brain.enable_curiosity(True)
brain.enable_novelty(True)
brain.enable_reward_modulation(True)

print("   - Enabled curiosity and novelty-driven exploration")
print("   - Enabled reward modulation")

# Get enhanced statistics
print("\n4. Enhanced statistics...")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
print(f"   Active neurons: {brain.getActiveNeuronCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"   Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Developmental stage
print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
print(f"   Plasticity modulation: {brain.get_plasticity_modulation():.3f}")

# Get neuromodulation levels
if hasattr(brain, 'get_curiosity_level'):
    print(f"   Curiosity level: {brain.get_curiosity_level():.3f}")
if hasattr(brain, 'get_novelty_level'):
    print(f"   Novelty level: {brain.get_novelty_level():.3f}")
if hasattr(brain, 'get_neuromodulation_level'):
    print(f"   Neuromodulation level: {brain.get_neuromodulation_level():.3f}")

print("\n5. Running learning simulation with neuromodulation...")

# Run learning simulation
for step in range(500):
    # Inject sensory input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    
    # Run brain step
    brain.step(step)
    
    # Apply neuromodulation effects
    reward = np.random.uniform(0, 1) if np.random.random() > 0.7 else 0.0
    if hasattr(brain, 'apply_reward_modulation'):
        brain.apply_reward_modulation(reward, 0.0)
    
    # Update development
    if hasattr(brain, 'update_development'):
        brain.update_development(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"\nLearning simulation completed!")
print(f"   Final spike count: {brain.getTotalSpikeCount()}")
print(f"   Final developmental stage: {brain.getDevelopmentalStage()}")

print("\n=== Example Complete ===")