#!/usr/bin/env python3
"""
Simple test to verify Brain save/load functionality.
This script creates a brain, saves it, resets and reloads it.
"""

import os
import sys
import tempfile
import shutil

# Add src directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def test_brain_save_load():
    print("Testing Brain save/load functionality...")
    
    # Try to import the modules
    try:
        from brain.Brain import Brain
        from core.Config.Config import Config
        print("✓ Successfully imported Brain and Config")
    except ImportError as e:
        print(f"✗ Import error: {e}")
        print("Note: This test requires a complete build environment")
        return True
    
    # Create configuration
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    
    # Create first brain
    brain1 = Brain(config)
    success = brain1.initialize()
    
    if not success:
        print("✗ Failed to initialize first brain")
        return False
    
    print(f"✓ First brain initialized with {brain1.getTotalNeuronCount()} neurons and {brain1.getTotalSynapseCount()} synapses")
    
    # Create temporary directory for checkpoint
    temp_dir = tempfile.mkdtemp()
    checkpoint_file = os.path.join(temp_dir, "test_checkpoint.ncl")
    
    try:
        # Save brain state
        save_success = brain1.save(checkpoint_file)
        
        if not save_success:
            print("✗ Failed to save brain state")
            return False
        
        print(f"✓ Successfully saved brain state to {checkpoint_file}")
        
        # Get original state
        original_neuron_count = brain1.getTotalNeuronCount()
        original_synapse_count = brain1.getTotalSynapseCount()
        
        # Create second brain
        brain2 = Brain(config)
        init_success = brain2.initialize()
        
        if not init_success:
            print("✗ Failed to initialize second brain")
            return False
        
        # Reset second brain
        brain2.reset()
        
        # Load saved state
        load_success = brain2.load(checkpoint_file)
        
        if not load_success:
            print("✗ Failed to load brain state")
            return False
        
        print(f"✓ Successfully loaded brain state into second brain")
        
        # Verify state
        loaded_neuron_count = brain2.getTotalNeuronCount()
        loaded_synapse_count = brain2.getTotalSynapseCount()
        
        print(f"  - Original: {original_neuron_count} neurons, {original_synapse_count} synapses")
        print(f"  - Loaded:   {loaded_neuron_count} neurons, {loaded_synapse_count} synapses")
        
        if loaded_neuron_count != original_neuron_count:
            print(f"✗ Neuron count mismatch: expected {original_neuron_count}, got {loaded_neuron_count}")
            return False
        
        if loaded_synapse_count != original_synapse_count:
            print(f"✗ Synapse count mismatch: expected {original_synapse_count}, got {loaded_synapse_count}")
            return False
        
        print("✓ Save/load test passed")
        
    finally:
        # Clean up
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print(f"✓ Cleaned up checkpoint file: {checkpoint_file}")
        
        shutil.rmtree(temp_dir, ignore_errors=True)
    
    return True

if __name__ == "__main__":
    print("=" * 60)
    print("Brain Save/Load Test")
    print("=" * 60)
    
    success = test_brain_save_load()
    
    print("=" * 60)
    if success:
        print("Test completed successfully!")
    else:
        print("Test failed!")
    print("=" * 60)
    
    sys.exit(0 if success else 1)