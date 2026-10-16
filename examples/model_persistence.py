#!/usr/bin/env python3
"""Demonstrate saving and loading brain models.

This example shows how to persist and restore brain states:
- Save brain state to file
- Load brain state from file
- Compare behavior before/after saving
- Use checkpoints for long simulations
"""

import pynlm
import sys
import os
import time

def compare_brains(brain1, brain2, label1="Brain 1", label2="Brain 2"):
    """Compare two brains and print statistics."""
    print(f"\n{label1} vs {label2} comparison:")
    print(f"  {label1}: {brain1.getTotalNeuronCount()} neurons, "
          f"{brain1.getTotalSynapseCount()} synapses")
    print(f"  {label2}: {brain2.getTotalNeuronCount()} neurons, "
          f"{brain2.getTotalSynapseCount()} synapses")
    
    # Compare firing rates
    rate1 = brain1.getAverageFiringRate()
    rate2 = brain2.getAverageFiringRate()
    print(f"  Firing rate: {rate1:.2f} Hz vs {rate2:.2f} Hz")
    
    # Compare total spikes
    spikes1 = brain1.getTotalSpikeCount()
    spikes2 = brain2.getTotalSpikeCount()
    print(f"  Total spikes: {spikes1} vs {spikes2}")
    
    # Check developmental stage
    stage1 = brain1.getDevelopmentalStage()
    stage2 = brain2.getDevelopmentalStage()
    print(f"  Development stage: {stage1} vs {stage2}")
    
    # Compare plasticity (if available)
    if hasattr(brain1, 'getSTDP') and hasattr(brain2, 'getSTDP'):
        stdp1 = brain1.getSTDP()
        stdp2 = brain2.getSTDP()
        print(f"  STDP LTP: {stdp1.getLTPWeight():.3f} vs {stdp2.getLTPWeight():.3f}")
        print(f"  STDP LTD: {stdp1.getLTDWeight():.3f} vs {stdp2.getLTDWeight():.3f}")

def main():
    print("=== NLM Model Persistence Demo ===\n")
    
    # 1. Create initial brain and run some steps
    print("Step 1: Creating and training initial brain")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some steps to establish baseline
    for i in range(20):
        brain1.step(i)
    
    print(f"✓ Initial brain trained: {brain1.getTotalSpikeCount()} spikes")
    print()
    
    # 2. Save brain state
    print("Step 2: Saving brain state")
    
    save_file = "brain_checkpoint_1.bin"
    try:
        brain1.save(save_file)
        print(f"✓ Saved to {save_file}")
        print(f"  File size: {os.path.getsize(save_file)} bytes")
    except Exception as e:
        print(f"✗ Failed to save: {e}")
        return
    
    # Get brain statistics for comparison
    initial_stats = {
        'neurons': brain1.getTotalNeuronCount(),
        'synapses': brain1.getTotalSynapseCount(),
        'spikes': brain1.getTotalSpikeCount(),
        'firing_rate': brain1.getAverageFiringRate(),
        'dev_stage': brain1.getDevelopmentalStage()
    }
    
    print(f"  Statistics saved: {initial_stats}")
    print()
    
    # 3. Load brain state (after further training)
    print("Step 3: Loading saved brain state")
    
    # Train the original brain more
    for i in range(20, 50):
        brain1.step(i)
    
    print(f"✓ Continued training: {brain1.getTotalSpikeCount()} total spikes")
    
    # Create new brain and load the saved state
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.load(save_file)
    
    print(f"✓ Loaded brain has {brain2.getTotalNeuronCount()} neurons")
    print(f"✓ Loaded brain has {brain2.getTotalSynapseCount()} synapses")
    print()
    
    # 4. Compare saved vs loaded
    print("Step 4: Comparing saved and loaded brains")
    
    compare_brains(brain1, brain2, "Trained brain", "Loaded brain")
    
    # 5. Demonstrate multiple checkpoints
    print("Step 5: Multiple checkpoint demonstration")
    
    checkpoints = []
    for checkpoint in range(3):
        checkpoint_file = f"checkpoint_{checkpoint}.bin"
        
        # Train a bit more
        brain1.step(50 + checkpoint * 10)
        
        # Save checkpoint
        brain1.save(checkpoint_file)
        checkpoints.append(checkpoint_file)
        print(f"  ✓ Saved checkpoint {checkpoint} to {checkpoint_file}")
    
    # 6. Restore from intermediate checkpoint
    print("\nStep 6: Restore from intermediate checkpoint")
    
    brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain3.load("checkpoint_1.bin")
    
    print("✓ Restored from checkpoint_1.bin")
    print(f"  Restored brain has {brain3.getTotalNeuronCount()} neurons")
    print(f"  Development stage: {brain3.getDevelopmentalStage()}")
    
    # Compare all three brains
    print("\nStep 7: Comparing all three brain versions")
    compare_brains(brain1, brain2, "Final trained", "First loaded")
    compare_brains(brain2, brain3, "First loaded", "Checkpoint 1 restored")
    
    # 7. Error handling for missing files
    print("\nStep 8: Testing error handling")
    
    brain4 = pynlm.createBrain(pynlm.createDefaultConfig())
    
    try:
        brain4.load("nonexistent_file.bin")
        print("✗ Should have raised an error for missing file")
    except Exception as e:
        print(f"✓ Correctly caught error for missing file: {type(e).__name__}")
    
    # 8. Large brain persistence
    print("\nStep 9: Large brain persistence test")
    
    large_config = pynlm.createDefaultConfig()
    large_config.set("brain.neuron_count", 2000)  # Larger brain
    
    brain5 = pynlm.createBrain(large_config)
    brain5.initialize()
    
    # Save large brain
    large_file = "large_brain.bin"
    brain5.save(large_file)
    print(f"✓ Saved large brain ({brain5.getTotalNeuronCount()} neurons)")
    print(f"  File size: {os.path.getsize(large_file)} bytes")
    
    # Load and verify
    brain6 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain6.load(large_file)
    
    print(f"✓ Loaded large brain: {brain6.getTotalNeuronCount()} neurons")
    
    if brain5.getTotalNeuronCount() == brain6.getTotalNeuronCount():
        print("✓ Neuron count matches!")
    else:
        print("✗ Neuron count mismatch!")
    
    # 9. Cleanup
    print("\nStep 10: Cleanup")
    
    files_to_remove = [
        save_file, "checkpoint_0.bin", "checkpoint_1.bin", "checkpoint_2.bin",
        large_file, "brain_checkpoint.bin"
    ]
    
    removed = 0
    for filename in files_to_remove:
        if os.path.exists(filename):
            os.remove(filename)
            print(f"  ✓ Removed {filename}")
            removed += 1
    
    print(f"\n✓ Removed {removed} files")
    
    print("\n=== Model Persistence Demo Complete ===")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running persistence demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
