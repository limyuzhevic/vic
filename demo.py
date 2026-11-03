"""
NLM Demo Script

This script demonstrates the enhanced NLM Python API features.
"""

import pynlm
import time

def demo_basic_simulation():
    """Demonstrate basic brain simulation"""
    print("=== Basic Brain Simulation ===")
    
    # Create brain with minimal configuration
    brain = pynlm.BrainSimulator()
    brain.setupWorld(width=20, height=20, visionWidth=8, visionHeight=8)
    
    print("Starting simulation...")
    start_time = time.time()
    
    brain.run(500)
    
    elapsed = time.time() - start_time
    stats = brain.getStats()
    
    print(f"Simulation completed in {elapsed:.2f} seconds")
    print(f"  Neurons: {stats['neurons_total']}")
    print(f"  Spikes: {stats['spikes_total']}")
    print(f"  Firing rate: {stats['average_firing_rate']:.2f} Hz")
    print()

def demo_learning_experiment():
    """Demonstrate learning experiment with curiosity"""
    print("=== Learning Experiment with Curiosity ===")
    
    # Create learning brain
    brain = pynlm.BrainSimulator()
    brain.setupWorld(width=30, height=30, visionWidth=10, visionHeight=10)
    brain.enableLearning(True, True, True, True)  # Enable all learning subsystems
    
    print("Running learning experiment...")
    print("The agent will explore the world and learn through curiosity-driven exploration")
    print()
    
    brain.run(1000, logProgress=True)
    
    stats = brain.getStats()
    print(f"\nLearning experiment results:")
    print(f"  Curiosity level: {stats['curiosity_level']:.3f}")
    print(f"  Novelty level: {stats['novelty_level']:.3f}")
    print(f"  Prediction error: {stats['prediction_error']:.3f}")
    print(f"  Neuromodulation: {stats['neuromodulation_level']:.3f}")
    print(f"  Total steps completed: 1000")
    print()

def demo_quick_brain():
    """Demonstrate very quick brain test"""
    print("=== Quick Brain Test ===")
    
    print("Running quick brain test (200 steps)...")
    
    # Use the simple helper function
    pynlm.runBrainOnly(200, neuronCount=500)
    
    print("Quick brain test completed!")
    print()

def demo_agent_world():
    """Demonstrate agent-world simulation"""
    print("=== Agent-World Simulation ===")
    
    print("Running agent-world simulation...")
    print("This simulates an agent exploring a 2D world with sensory input and motor output")
    print()
    
    # Use the helper function
    pynlm.runAgentWorld(300, worldWidth=25, worldHeight=25)
    
    print("Agent-world simulation completed!")
    print()

def demo_configuration_presets():
    """Demonstrate configuration presets"""
    print("=== Configuration Presets ===")
    
    # Demonstrate different configuration presets
    print("1. Minimal configuration:")
    config1 = pynlm.createMinimalConfig()
    print(f"   Neurons: {config1.getOr('neuron_count', 'N/A')}")
    
    print("2. Custom brain configuration:")
    config2 = pynlm.createBrainConfig(neuronCount=2000, regionCount=2, connectionProb=0.15)
    print(f"   Neurons: {config2.getOr('neuron_count', 'N/A')}")
    print(f"   Regions: {config2.getOr('region_count', 'N/A')}")
    print(f"   Connection probability: {config2.getOr('connection_probability', 'N/A')}")
    
    print("3. Simulation configuration:")
    config3 = pynlm.createSimulationConfig(steps=5000, timestep=0.001, checkpointDir="./checkpoints")
    print(f"   Timestep: {config3.getOr('simulation_timestep', 'N/A')}")
    print(f"   Checkpoint dir: {config3.getOr('checkpoint_dir', 'N/A')}")
    print()

def demo_statistics():
    """Demonstrate statistics reporting"""
    print("=== Statistics Reporting ===")
    
    brain = pynlm.BrainSimulator()
    brain.setupWorld(width=15, height=15)
    brain.enableLearning(True, True, True, True)
    
    brain.run(500)
    
    stats = brain.getStats()
    print("Simulation statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    print()

def demo_error_handling():
    """Demonstrate improved error handling"""
    print("=== Error Handling Demo ===")
    
    try:
        print("Attempting to create brain without initialization...")
        brain = pynlm.BrainSimulator()
        # This should fail because brain is not initialized
        brain.step(0)
        print("This should not print")
    except Exception as e:
        print(f"Caught error (expected): {e}")
    
    try:
        print("\nAttempting to load non-existent file...")
        brain = pynlm.BrainSimulator()
        brain.load("nonexistent_file.bin")
        print("This should not print")
    except Exception as e:
        print(f"Caught error (expected): {e}")
    
    print("Error handling demo completed successfully!")
    print()

def main():
    """Run all demo functions"""
    print("=" * 60)
    print("NLM Enhanced Python API Demo")
    print("=" * 60)
    print()
    
    demo_quick_brain()
    demo_configuration_presets()
    demo_basic_simulation()
    demo_agent_world()
    demo_learning_experiment()
    demo_statistics()
    demo_error_handling()
    
    print("=" * 60)
    print("All demos completed successfully!")
    print("=" * 60)
    print()
    print("Key improvements demonstrated:")
    print("  ✓ High-level BrainSimulator API")
    print("  ✓ Configuration presets")
    print("  ✓ Common pattern helpers")
    print("  ✓ Enhanced statistics reporting")
    print("  ✓ Improved error handling")
    print("  ✓ Learning subsystems (curiosity, novelty, etc.)")
    print()

if __name__ == "__main__":
    main()
