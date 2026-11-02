#!/usr/bin/env python3
"""Test script to verify checkpointing functionality"""

import sys
import os
import tempfile
import struct

# Add the build directory to Python path
sys.path.insert(0, '/workspace/app/build')

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please build the Python bindings first: pip install -e .")
    sys.exit(1)

def test_basic_checkpointing():
    """Test basic checkpoint save/load functionality"""
    print("\n=== Testing Basic Checkpointing ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Created brain with {brain.getTotalSynapseCount()} synapses")
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing_rate = brain.getAverageFiringRate()
    
    # Inject some activity
    for i in range(10):
        brain.step(i)
    
    print(f"After 10 steps:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    print(f"\nSaving checkpoint to: {checkpoint_path}")
    save_success = brain.save(checkpoint_path)
    
    if not save_success:
        print("✗ Failed to save checkpoint")
        return False
    
    print("✓ Checkpoint saved successfully")
    
    # Check file size
    file_size = os.path.getsize(checkpoint_path)
    print(f"Checkpoint file size: {file_size} bytes")
    
    # Reset brain
    brain.reset()
    brain.initialize()
    
    print(f"\nReset brain, firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Load checkpoint
    print(f"Loading checkpoint from: {checkpoint_path}")
    load_success = brain.load(checkpoint_path)
    
    if not load_success:
        print("✗ Failed to load checkpoint")
        os.unlink(checkpoint_path)
        return False
    
    print("✓ Checkpoint loaded successfully")
    
    # Verify loaded state
    print(f"\nAfter loading checkpoint:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Compare with initial state
    spikes_restored = brain.getTotalSpikeCount() == initial_spikes
    firing_rate_restored = abs(brain.getAverageFiringRate() - initial_firing_rate) < 0.01
    
    print(f"\nState restoration:")
    print(f"  Spikes restored: {'✓' if spikes_restored else '✗'}")
    print(f"  Firing rate restored: {'✓' if firing_rate_restored else '✗'}")
    
    # Clean up
    os.unlink(checkpoint_path)
    
    return spikes_restored and firing_rate_restored

def test_checkpoint_validation():
    """Test checkpoint validation and error handling"""
    print("\n=== Testing Checkpoint Validation ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Try to load non-existent file
    print("Testing load of non-existent file...")
    result = brain.load("/non/existent/file.bin")
    
    if result:
        print("✗ Should have failed to load non-existent file")
        return False
    
    print("✓ Correctly rejected non-existent file")
    
    # Create a corrupted checkpoint file
    print("\nTesting load of corrupted file...")
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        f.write(b"This is not a valid checkpoint file")
        corrupted_path = f.name
    
    result = brain.load(corrupted_path)
    
    if result:
        print("✗ Should have failed to load corrupted file")
        os.unlink(corrupted_path)
        return False
    
    print("✓ Correctly rejected corrupted file")
    os.unlink(corrupted_path)
    
    return True

def test_multiple_checkpoints():
    """Test saving and loading multiple checkpoints"""
    print("\n=== Testing Multiple Checkpoints ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    checkpoints = []
    
    # Create and save multiple checkpoints at different steps
    for i in range(3):
        brain.step(i * 100)
        
        with tempfile.NamedTemporaryFile(suffix=f'_{i}.bin', delete=False) as f:
            checkpoint_path = f.name
        
        if brain.save(checkpoint_path):
            checkpoints.append(checkpoint_path)
            print(f"✓ Saved checkpoint {i} to {checkpoint_path}")
        else:
            print(f"✗ Failed to save checkpoint {i}")
            return False
    
    # Load checkpoints in reverse order
    print("\nLoading checkpoints in reverse order:")
    for i, checkpoint_path in enumerate(reversed(checkpoints)):
        # Reset and reinitialize brain
        brain.reset()
        brain.initialize()
        
        if brain.load(checkpoint_path):
            print(f"✓ Loaded checkpoint {i}")
        else:
            print(f"✗ Failed to load checkpoint {i}")
            return False
    
    # Clean up
    for path in checkpoints:
        os.unlink(path)
    
    print("✓ All checkpoint tests passed")
    return True

if __name__ == "__main__":
    print("NLM Checkpoint Testing Utility")
    print("=========================")
    
    success = True
    
    success &= test_basic_checkpointing()
    success &= test_checkpoint_validation()
    success &= test_multiple_checkpoints()
    
    print("\n" + "="*50)
    if success:
        print("✓ All checkpoint tests PASSED!")
    else:
        print("✗ Some checkpoint tests FAILED!")
        sys.exit(1)
    
    print("="*50)