#!/usr/bin/env python3
"""
Quick Start Guide for NLM Performance and Debugging Tools

This is a concise guide to using the new performance monitoring and debugging
capabilities in the NLM framework.
"""

import pynlm
import time
import json

def quick_start_performance_monitoring():
    """Quick start example for performance monitoring."""
    print("=== NLM Performance Monitoring Quick Start ===")
    
    # 1. Create a brain with configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create agent and world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 4. Run simulation with monitoring
    print("Running simulation...")
    start_time = time.time()
    
    for step in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    
    # 5. Display results
    print(f"\nSimulation Results:")
    print(f"  Total time: {end_time - start_time:.2f} seconds")
    print(f"  Average TPS: {(end_time - start_time) / 20:.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

def quick_start_configuration():
    """Quick start example for configuration management."""
    print("\n=== Configuration Management Quick Start ===")
    
    # Create and customize configuration
    config = pynlm.createDefaultConfig()
    
    # Set configuration parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    print("✓ Configuration created with custom parameters")
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Configuration summary: {config.summary()}")

def quick_start_error_handling():
    """Quick start example for error handling."""
    print("\n=== Error Handling Quick Start ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Testing error handling...")
    
    # Test normal operation
    try:
        brain.step(0)
        print("✓ Brain step completed successfully")
    except Exception as e:
        print(f"✗ Error in brain step: {e}")
    
    # Test configuration validation
    print("Checking configuration...")
    if config.has("brain.neuron_count"):
        print(f"✓ Neuron count: {config.get('brain.neuron_count')}")
    else:
        print("✗ Neuron count not found in configuration")

def quick_start_data_export():
    """Quick start example for data export."""
    print("\n=== Data Export Quick Start ===")
    
    # Run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(10):
        brain.step(step)
    
    # Export simulation data
    simulation_data = {
        'timestamp': time.time(),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'simulation_steps': 10
    }
    
    # Save to file
    with open('quick_start_export.json', 'w') as f:
        json.dump(simulation_data, f, indent=2)
    
    print(f"✓ Data exported to quick_start_export.json")
    print(f"✓ Sample data: {simulation_data['neuron_count']} neurons, {simulation_data['total_spikes']} spikes")
    
    # Clean up
    import os
    if os.path.exists('quick_start_export.json'):
        os.remove('quick_start_export.json')

def main():
    """Run all quick start examples."""
    print("NLM Performance and Debugging Tools - Quick Start Guide")
    print("=" * 60)
    
    try:
        quick_start_performance_monitoring()
        quick_start_configuration()
        quick_start_error_handling()
        quick_start_data_export()
        
        print("\n" + "=" * 60)
        print("Quick start completed successfully!")
        print("\nKey features demonstrated:")
        print("✓ Performance monitoring")
        print("✓ Configuration management")
        print("✓ Error handling")
        print("✓ Data export")
        print("\nNext steps:")
        print("1. Explore the examples/ directory for more complex examples")
        print("2. Use performance monitoring for optimization")
        print("3. Configure brain parameters for your specific use case")
        print("4. Export data for analysis and visualization")
        
    except Exception as e:
        print(f"\nError during quick start: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
