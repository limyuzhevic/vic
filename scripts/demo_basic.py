#!/usr/bin/env python3
"""
Basic NLM Demo Script
=====================

This script demonstrates the basic functionality of the NLM framework
using the Python bindings.
"""

import sys
import time
import pynlm

def demo_basic_brain():
    """Demonstrate basic brain creation and simulation."""
    print("=== Basic Brain Demo ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config with {len(config.getKeys())} settings")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    print("\nRunning simulation...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Demonstrate brain reset
    brain.reset()
    print("\nBrain reset completed")
    
    return True

def demo_configuration():
    """Demonstrate configuration file handling."""
    print("\n=== Configuration Demo ===")
    
    # Create config and modify settings
    config = pynlm.createDefaultConfig()
    
    print("Original config keys:", len(config.getKeys()))
    
    # Modify some settings
    config.set("random_seed", 12345)
    config.set("neuron_count", 500)
    config.set("plasticity_learning_rate", 0.02)
    
    print("Modified config has random_seed:", config.has("random_seed"))
    print("Modified config has neuron_count:", config.has("neuron_count"))
    
    # Test getOr method
    seed = config.getOr("random_seed", 42)
    neurons = config.getOr("neuron_count", 1000)
    print(f"Using getOr: seed={seed}, neurons={neurons}")
    
    # Test file I/O
    test_filename = "demo_config.json"
    config.saveToFile(test_filename)
    print(f"Saved config to {test_filename}")
    
    # Load config back
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile(test_filename)
    print(f"Loaded config has {len(new_config.getKeys())} keys")
    
    # Clean up
    import os
    if os.path.exists(test_filename):
        os.remove(test_filename)
        print(f"Cleaned up {test_filename}")
    
    return True

def main():
    """Main demo function."""
    print("NLM Python Basic Demo")
    print("=" * 50)
    
    try:
        # Run demos
        demo_basic_brain()
        demo_configuration()
        
        print("\n" + "=" * 50)
        print("✓ All demos completed successfully!")
        print("\nThe NLM Python bindings are working correctly.")
        print("\nNext steps:")
        print("1. Try demo_with_world.py for world interaction examples")
        print("2. Try demo_advanced.py for advanced features")
        print("3. Try demo_learning.py for learning demonstrations")
        print("4. Run benchmark.py to test performance")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Error during demo: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
