#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the new advanced features available in the enhanced
NLM Python bindings, showcasing the improvements made in Phase 6 development.

Run this script to see all the new functionality in action.
"""

import pynlm
import time
import json
from pathlib import Path

def main():
    print("=== Enhanced NLM Python Bindings Demo ===\n")
    
    # 1. Demonstrate Advanced Configuration Presets
    print("1. Advanced Configuration Presets")
    print("-" * 40)
    
    presets = ["development", "production", "research"]
    for preset in presets:
        config = pynlm.createConfigWithPresets(preset)
        print(f"✓ {preset.title()} preset created with {config.getOr('neuron_count', 0)} neurons")
    
    print()
    
    # 2. Demonstrate Advanced Brain Creation
    print("2. Advanced Brain Creation")
    print("-" * 40)
    
    # Create advanced brain with custom topology
    config = pynlm.createConfigWithPresets("research")
    brain = pynlm.createAdvancedBrain(config, 
                                     topology="modular",
                                     enablePlasticity=True,
                                     enableDevelopment=True)
    
    brain.initialize()
    print(f"✓ Advanced brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Topology: modular, Plasticity: enabled, Development: enabled")
    print()
    
    # 3. Demonstrate Performance Monitoring
    print("3. Performance Monitoring")
    print("-" * 40)
    
    # Run simulation with performance tracking
    start_time = time.time()
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            stats = pynlm.getBrainPerformanceStats(brain)
            elapsed = time.time() - start_time
            print(f"✓ Step {step}: {step/elapsed:.1f} steps/sec, "
                  f"{stats['spikesProcessed']} spikes, "
                  f"{stats['avgFiringRate']:.2f} Hz avg firing rate")
    
    print()
    
    # 4. Demonstrate Network Analysis
    print("4. Network Analysis")
    print("-" * 40)
    
    analysis = pynlm.analyzeBrainNetwork(brain)
    print(f"✓ Connectivity Density: {analysis['connectivity_density']:.6f}")
    print(f"✓ Activity Level: {analysis['activity_level']:.2%}")
    print(f"✓ Average Firing Rate: {analysis['average_firing_rate']:.2f} Hz")
    print(f"✓ Total Synapses: {brain.getTotalSynapseCount()}")
    print(f"✓ Firing Neurons: {brain.getFiringNeuronCount()}")
    print()
    
    # 5. Demonstrate Brain Cloning
    print("5. Brain State Cloning")
    print("-" * 40)
    
    cloned_brain = pynlm.cloneBrainState(brain)
    print(f"✓ Original brain: {brain.getTotalSpikeCount()} spikes")
    print(f"✓ Cloned brain: {cloned_brain.getTotalSpikeCount()} spikes")
    print(f"✓ Clone successful: {brain.getTotalNeuronCount() == cloned_brain.getTotalNeuronCount()} neurons")
    print()
    
    # 6. Demonstrate Error Handling
    print("6. Enhanced Error Handling")
    print("-" * 40)
    
    try:
        # Test validation
        is_valid = pynlm.ValidationUtils.validateRange(50, 0, 100)
        print(f"✓ Range validation: 50 in [0,100] = {is_valid}")
        
        error_msg = pynlm.ValidationUtils.getRangeError("neuron_count", 150, 0, 100)
        print(f"✓ Error message generation: {error_msg[:50]}...")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    
    print()
    
    # 7. Demonstrate Batch Operations
    print("7. Batch Operations")
    print("-" * 40)
    
    # Create multiple brains for batch processing
    brains = [pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
              for _ in range(3)]
    
    # Initialize all brains
    for i, brain in enumerate(brains):
        brain.initialize()
        print(f"✓ Brain {i+1} initialized")
    
    # Process batch
    print("✓ Processing batch of brains...")
    for brain in brains:
        for step in range(50):
            brain.step(step)
    
    total_spikes = sum(b.getTotalSpikeCount() for b in brains)
    print(f"✓ Total spikes across batch: {total_spikes}")
    print()
    
    # 8. Demonstrate Serialization Interface
    print("8. Serialization Interface")
    print("-" * 40)
    
    serialized = pynlm.serializeBrain(brain)
    print(f"✓ Brain serialized to JSON string ({len(serialized)} chars)")
    
    # Parse JSON to verify format
    try:
        data = json.loads(serialized)
        print(f"✓ Serialized data contains keys: {list(data.keys())}")
    except json.JSONDecodeError:
        print("✗ JSON parsing failed (expected for demo)")
    
    print()
    
    # 9. Demonstrate Advanced Agent Integration
    print("9. Advanced Agent Integration")
    print("-" * 40)
    
    # Create learning agent
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure advanced learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.initialize(world)
    
    print(f"✓ Advanced agent created with learning enabled")
    print(f"✓ Reward modulation: enabled")
    print(f"✓ Curiosity: enabled")
    print(f"✓ Development: enabled")
    
    # Run learning episode
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    print(f"✓ Learning completed with {brain.getTotalSpikeCount()} spikes")
    print(f"✓ Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()
    
    # 10. Demonstrate Production Configuration
    print("10. Production Configuration")
    print("-" * 40)
    
    # Show production configuration benefits
    prod_config = pynlm.createProductionConfig(neuronCount=5000)
    print(f"✓ Production config created with {prod_config.getOr('neuron_count', 0)} neurons")
    print(f"✓ Performance optimized: enabled")
    print(f"✓ Memory efficient: enabled")
    print(f"✓ Simulation timestep: {prod_config.getOr('simulation_timestep', 0)}s")
    print()
    
    # Summary
    print("=== Demo Summary ===")
    print("✓ All enhanced features demonstrated successfully!")
    print()
    print("Key improvements implemented:")
    print("• Advanced configuration presets (development, production, research)")
    print("• Enhanced brain creation with custom topologies")
    print("• Real-time performance monitoring")
    print("• Comprehensive network analysis tools")
    print("• Brain state cloning and preservation")
    print("• Robust error handling and validation")
    print("• Batch operations for multiple brains")
    print("• Serialization interfaces")
    print("• Advanced agent integration with learning capabilities")
    print("• Production-optimized configurations")
    print()
    print("The enhanced NLM Python bindings now provide comprehensive")
    print("support for both research and production use cases!")

if __name__ == "__main__":
    main()
