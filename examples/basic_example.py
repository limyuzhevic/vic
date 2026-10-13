#!/usr/bin/env python3
"""
NLM Python Example: Basic Neural Network Usage

This example demonstrates how to use the NLM Python bindings
to create and interact with a neural learning machine.
"""

import pynlm
import numpy as np

def create_and_run_brain():
    """Create a brain and run a simple simulation."""
    print("=== NLM Python Example: Basic Neural Network ===\n")
    
    # Create a default configuration
    print("1. Creating default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Display configuration summary
    print("2. Configuration summary:")
    print(config.summary())
    
    # Modify some configuration values
    print("\n3. Modifying configuration...")
    config.set("neuron_count", 100)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Create a brain with the custom configuration
    print("\n4. Creating brain...")
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    print("5. Initializing brain...")
    brain.initialize()
    
    # Log brain status
    print("\n6. Brain status:")
    brain.logStatus()
    
    # Run some steps
    print("\n7. Running simulation steps...")
    for step in range(100):
        brain.step(step, step * 0.001)
        
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print("\n8. Final brain metrics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.4f}")
    
    return brain

def demonstrate_config_serialization():
    """Demonstrate JSON configuration serialization."""
    print("\n\n=== Configuration Serialization Example ===\n")
    
    # Create and modify configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("stdp_ltp_weight", 0.02)
    config.set("enable_plasticity", True)
    
    print("Original configuration:")
    print(config.summary())
    
    # Save to JSON file
    print("\nSaving to 'brain_config.json'...")
    if config.saveToFile("brain_config.json"):
        print("✓ Configuration saved successfully")
    else:
        print("✗ Failed to save configuration")
        return
    
    # Load from JSON file
    print("\nLoading from 'brain_config.json'...")
    config2 = pynlm.createDefaultConfig()
    if config2.loadFromFile("brain_config.json"):
        print("✓ Configuration loaded successfully")
        print("Loaded configuration:")
        print(config2.summary())
    else:
        print("✗ Failed to load configuration")
    
    # Compare configurations
    print("\nComparing configurations...")
    keys1 = set(config.getKeys())
    keys2 = set(config2.getKeys())
    
    print(f"Keys in original: {len(keys1)}")
    print(f"Keys in loaded: {len(keys2)}")
    
    if keys1 == keys2:
        print("✓ Configurations have same keys")
    else:
        print("✗ Key mismatch!")
        print(f"Only in original: {keys1 - keys2}")
        print(f"Only in loaded: {keys2 - keys1}")

def demonstrate_agent_usage():
    """Demonstrate using the agent brain interface."""
    print("\n\n=== Agent Brain Interface Example ===\n")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize()
    
    print("Created agent brain interface")
    print(f"Expected sensory input size: {agent.getSensoryInputSize()}")
    print(f"Expected motor output size: {agent.getMotorOutputSize()}")
    
    # Demonstrate reward modulation
    print("\nDemonstrating reward modulation...")
    for step in range(10):
        # Simulate some reward
        reward = 1.0 if step % 3 == 0 else 0.0
        predicted_reward = 0.5  # Dummy prediction
        
        agent.applyRewardModulation(reward, predicted_reward)
        print(f"Step {step}: Reward={reward}, Neuromodulation={agent.getNeuromodulationLevel():.3f}")
    
    # Demonstrate development
    print("\nDemonstrating development update...")
    for step in range(5):
        agent.updateDevelopment(step * 0.001)
        print(f"Step {step}: Developmental stage={agent.getDevelopmentalStage()}")
        
        if agent.getDevelopmentalStage() == pynlm.DevelopmentalStage.Aging:
            print("  (Adult brain reached)")
            break

if __name__ == "__main__":
    try:
        # Run all examples
        brain = create_and_run_brain()
        demonstrate_config_serialization()
        demonstrate_agent_usage()
        
        print("\n\n=== All examples completed successfully! ===")
        
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()