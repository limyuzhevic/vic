#!/usr/bin/env python3
"""
Demo script showing improved NLM Python bindings features.

This script demonstrates the enhanced Python API with:
- Better documentation and error handling
- Convenience functions
- Standard configuration creation
- Performance benchmarking
"""

import pynlm
import time

def demo_basic_usage():
    """Demonstrate basic NLM usage with improved API."""
    print("=== Basic NLM Usage Demo ===")
    
    # Create a brain using standard configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize the brain (required before use)
    brain.initialize()
    
    # Run a simple simulation
    for i in range(100):
        brain.step(i)
    
    # Display brain statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Save brain state
    brain.save("demo_brain.bin")
    print("Brain state saved to demo_brain.bin")

def demo_standard_config():
    """Demonstrate standard configuration creation."""
    print("\n=== Standard Configuration Demo ===")
    
    config = pynlm.createStandardConfig()
    print("Standard configuration created with sensible defaults:")
    print(config.summary())
    
    # Verify key settings
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    assert config.getOr("plasticity.stdp.enable", False)
    
    print("✓ Configuration contains expected settings")

def demo_complete_simulation():
    """Demonstrate complete simulation with brain-world-agent integration."""
    print("\n=== Complete Simulation Demo ===")
    
    # Create standard configuration and brain
    config = pynlm.createStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running complete agent simulation...")
    
    # Run simulation using convenience function
    pynlm.runCompleteSimulation(brain, world, agent, 200)
    
    # Display final statistics
    print(f"\nSimulation completed:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")

def demo_agent_episode():
    """Demonstrate agent episode running."""
    print("\n=== Agent Episode Demo ===")
    
    # Create agent, brain, and world
    config = pynlm.createStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run episode with verbose output
    print("Running agent episode (verbose mode)...")
    pynlm.runAgentEpisode(agent, world, maxSteps=300, verbose=True)

def demo_brain_from_config_file():
    """Demonstrate creating brain from configuration file."""
    print("\n=== Brain from Config File Demo ===")
    
    # Create a sample config file
    sample_config = {
        "brain": {
            "neuron_count": 500,
            "synapse_density": 0.15,
            "v_thresh": -45.0
        },
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.002}
        }
    }
    
    import json
    with open("sample_config.json", "w") as f:
        json.dump(sample_config, f, indent=2)
    
    print("Created sample_config.json")
    
    # Create brain from config file
    brain = pynlm.createBrainWithConfig("sample_config.json")
    brain.initialize()
    
    print(f"Brain created from config file:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Spikes in first 50 steps: ", end="")
    
    initial_spikes = brain.getTotalSpikeCount()
    for i in range(50):
        brain.step(i)
    
    print(f"{brain.getTotalSpikeCount() - initial_spikes} new spikes")

def demo_performance_benchmark():
    """Demonstrate brain performance benchmarking."""
    print("\n=== Performance Benchmark Demo ===")
    
    config = pynlm.createStandardConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Benchmarking brain performance...")
    
    # Run benchmark
    result = pynlm.benchmarkBrainPerformance(brain, numSteps=1000, warmupSteps=200)
    
    total_spikes, avg_firing_rate, avg_ms_per_step, steps_per_sec = result
    
    print(f"Performance results:")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"  Average time per step: {avg_ms_per_step:.2f} ms")
    print(f"  Steps per second: {steps_per_sec:.1f}")

def demo_error_handling():
    """Demonstrate improved error handling."""
    print("\n=== Error Handling Demo ===")
    
    try:
        # Try to load non-existent config file
        brain = pynlm.createBrainWithConfig("nonexistent_config.json")
    except Exception as e:
        print(f"✓ Properly caught error: {type(e).__name__}")
        print(f"  Error message: {str(e)}")

if __name__ == "__main__":
    print("NLM Python Bindings - Enhanced Features Demo")
    print("=" * 50)
    
    try:
        demo_basic_usage()
        demo_standard_config()
        demo_complete_simulation()
        demo_agent_episode()
        demo_brain_from_config_file()
        demo_performance_benchmark()
        demo_error_handling()
        
        print("\n" + "=" * 50)
        print("✓ All demos completed successfully!")
        print("\nKey improvements demonstrated:")
        print("• Enhanced documentation and type safety")
        print("• Convenience functions for common patterns")
        print("• Standard configuration creation")
        print("• Complete simulation loops")
        print("• Performance benchmarking")
        print("• Improved error handling")
        
    except Exception as e:
        print(f"\n❌ Demo failed with error: {e}")
        import traceback
        traceback.print_exc()