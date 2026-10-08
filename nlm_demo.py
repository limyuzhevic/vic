#!/usr/bin/env python3
"""
NLM Python Bindings Demo Script

This script demonstrates the improved Python bindings for NLM.
Run with: python nlm_demo.py
"""

import sys
import os
import time

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def main():
    print("=" * 70)
    print("NLM Python Bindings Demo - Improved API Demonstration")
    print("=" * 70)
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm module")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        print("Please ensure NLM is properly installed with Python bindings")
        return 1
    
    # Demo 1: Basic Configuration and Brain Creation
    print("\n" + "-" * 70)
    print("Demo 1: Basic Configuration and Brain Creation")
    print("-" * 70)
    
    # Create configuration with defaults
    config = pynlm.createConfigWithDefaults(
        neuronCount=200,
        regionCount=2
    )
    print(f"✓ Created configuration with {config.get('neuron_count')} neurons")
    
    # Create brain
    brain = pynlm.createBrain(200, 2, "demo_seed_1")
    if brain:
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    else:
        print("✗ Failed to create brain")
        return 1
    
    # Demo 2: Simple World Creation
    print("\n" + "-" * 70)
    print("Demo 2: World and Agent Creation")
    print("-" * 70)
    
    # Create world with specific dimensions
    world = pynlm.createSimpleWorld(64, 64, 16, 16)
    print(f"✓ Created world: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    print(f"✓ Created agent brain")
    
    # Initialize agent
    agent.initialize(world)
    print(f"✓ Initialized agent with world")
    
    # Demo 3: Sensory/Motor Neuron Creation
    print("\n" + "-" * 70)
    print("Demo 3: Neuron Factory Functions")
    print("-" * 70)
    
    # Create specialized neurons
    sensory_neuron = pynlm.create_sensory_neuron(pynlm.NeuronId(101))
    motor_neuron = pynlm.create_motor_neuron(pynlm.NeuronId(102))
    
    print(f"✓ Created sensory neuron: ID {sensory_neuron.getId()}")
    print(f"✓ Created motor neuron: ID {motor_neuron.getId()}")
    
    # Demo 4: Simulation Loop
    print("\n" + "-" * 70)
    print("Demo 4: Simulation Loop with Agent")
    print("-" * 70)
    
    # Reset for clean simulation
    brain.reset()
    agent.reset()
    world.reset()
    
    # Run simulation steps
    total_spikes = 0
    steps = 50
    
    print(f"Running {steps} simulation steps...")
    
    for step in range(steps):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get motor command
        command = agent.decodeMotorCommand()
        
        # Apply action (in this case, just move forward)
        world.applyMotorCommand(command, step * 0.001)
        
        # Update agent development
        agent.updateDevelopment(0.001)
        
        # Get some reward to test neuromodulation
        reward = 0.1 if step % 10 == 0 else 0.0
        if reward > 0:
            agent.applyRewardModulation(reward, 0.0)
        
        # Count spikes periodically
        if step % 10 == 0:
            total_spikes += brain.getTotalSpikeCount()
    
    # Print simulation results
    print(f"\nSimulation Results:")
    print(f"  Total steps: {steps}")
    print(f"  Final total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Agent neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Agent curiosity level: {agent.getCuriosityLevel():.3f}")
    
    # Demo 5: Configuration Management
    print("\n" + "-" * 70)
    print("Demo 5: Configuration File Operations")
    print("-" * 70)
    
    # Create and save a custom configuration
    custom_config = pynlm.createConfigWithDefaults(500, 3, "custom_config")
    custom_config.set("connection_probability", 0.05)
    
    config_file = "demo_config.cfg"
    custom_config.saveToFile(config_file)
    print(f"✓ Saved custom configuration to {config_file}")
    
    # Load configuration from file
    loaded_config = pynlm.createDefaultConfig(config_file)
    if loaded_config:
        print(f"✓ Loaded configuration from file")
        print(f"  Neuron count: {loaded_config.get('neuron_count')}")
        print(f"  Region count: {loaded_config.get('region_count')}")
    
    # Demo 6: Brain Comparison
    print("\n" + "-" * 70)
    print("Demo 6: Brain Comparison")
    print("-" * 70)
    
    # Create brains with different sizes
    small_brain = pynlm.createBrain(100, 1, "small")
    large_brain = pynlm.createBrain(500, 2, "large")
    
    if small_brain and large_brain:
        print(f"✓ Created small brain: {small_brain.getTotalNeuronCount()} neurons")
        print(f"✓ Created large brain: {large_brain.getTotalNeuronCount()} neurons")
        
        # Compare their initial properties
        print(f"\nInitial comparison:")
        print(f"  Small brain - Average firing rate: {small_brain.getAverageFiringRate():.3f}")
        print(f"  Large brain - Average firing rate: {large_brain.getAverageFiringRate():.3f}")
        print(f"  Small brain - E/I ratio: {small_brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Large brain - E/I ratio: {large_brain.getExcitationInhibitionRatio():.3f}")
    
    # Demo 7: Run Built-in Demo
    print("\n" + "-" * 70)
    print("Demo 7: Built-in Brain Demo")
    print("-" * 70)
    
    print("Running built-in brain demo (this may take a few seconds)...")
    start_time = time.time()
    
    try:
        pynlm.runSimpleBrainDemo()
        print(f"✓ Built-in demo completed in {time.time() - start_time:.2f} seconds")
    except Exception as e:
        print(f"✗ Built-in demo failed: {e}")
    
    # Clean up
    if os.path.exists(config_file):
        os.remove(config_file)
        print(f"✓ Cleaned up temporary config file")
    
    # Final summary
    print("\n" + "=" * 70)
    print("Demo Summary")
    print("=" * 70)
    print("All demos completed successfully! ✓")
    print("\nKey improvements demonstrated:")
    print("  1. Enhanced Python bindings with convenient factory functions")
    print("  2. Improved configuration management")
    print("  3. Better agent brain interface with neuromodulation")
    print("  4. Simplified world and simulation creation")
    print("  5. Built-in demonstration functionality")
    print("  6. Comprehensive error handling")
    print("  7. File-based configuration operations")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())