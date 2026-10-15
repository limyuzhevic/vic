#!/usr/bin/env python3
"""
Test script for NLM Python bindings serialization helpers
"""

import numpy as np
import tempfile
import os
import json
import sys

# Add the python directory to the path
sys.path.insert(0, '/workspace/app/python')

try:
    import pynlm
    print("Successfully imported pynlm")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    sys.exit(1)

def test_config_serialization():
    """Test configuration serialization"""
    print("\n=== Testing Config Serialization ===")
    
    # Create config
    config = pynlm.Config()
    config.set("neuron_count", 1000)
    config.set("connection_probability", 0.1)
    config.set("learning_rate", 0.01)
    config.set("max_steps", 1000)
    config.set("simulation_timestep", 0.01)
    config.set("enableSTDP", True)
    config.set("enableHebbian", True)
    
    print("Created config with sample values")
    
    # Test JSON serialization
    try:
        json_str = config.toJson() if hasattr(config, 'toJson') else "toJson not available"
        print(f"JSON serialization: {json_str[:100]}..." if len(json_str) > 100 else f"JSON serialization: {json_str}")
    except AttributeError as e:
        print(f"JSON serialization not available: {e}")
    
    # Test file serialization
    try:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.config', delete=False) as f:
            config_file = f.name
        
        if hasattr(config, 'saveToFile'):
            config.saveToFile(config_file)
            print(f"Saved config to file: {config_file}")
            
            # Load from file
            new_config = pynlm.Config()
            if hasattr(new_config, 'loadFromFile'):
                new_config.loadFromFile(config_file)
                print(f"Loaded config from file successfully")
                
                # Verify some values
                if hasattr(new_config, 'getOr'):
                    neuron_count = new_config.getOr("neuron_count", 0)
                    print(f"Verified neuron_count: {neuron_count}")
            else:
                print("loadFromFile not available")
        else:
            print("saveToFile not available")
            
        os.unlink(config_file)
    except Exception as e:
        print(f"File serialization test failed: {e}")
    
    # Test convenience methods
    try:
        if hasattr(config, 'getKeys'):
            keys = config.getKeys()
            print(f"Config keys: {keys}")
        
        if hasattr(config, 'summary'):
            summary = config.summary()
            print(f"Config summary: {summary[:200]}..." if len(summary) > 200 else f"Config summary: {summary}")
            
    except Exception as e:
        print(f"Convenience methods test failed: {e}")

def test_brain_serialization():
    """Test brain serialization"""
    print("\n=== Testing Brain Serialization ===")
    
    # Create config
    config = pynlm.Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    # Create brain
    brain = pynlm.Brain(config)
    print("Created brain")
    
    # Initialize brain
    if hasattr(brain, 'initialize'):
        brain.initialize()
        print("Initialized brain")
    else:
        print("initialize not available")
    
    # Test basic serialization if available
    try:
        if hasattr(brain, 'getTotalNeuronCount'):
            neuron_count = brain.getTotalNeuronCount()
            print(f"Brain has {neuron_count} neurons")
            
        if hasattr(brain, 'getTotalSynapseCount'):
            synapse_count = brain.getTotalSynapseCount()
            print(f"Brain has {synapse_count} synapses")
            
        # Test serialization methods
        if hasattr(brain, 'toBytes'):
            data = brain.toBytes()
            print(f"Brain serialized to bytes: {len(data)} bytes")
            
            # Test deserialization
            new_brain = pynlm.Brain(config)
            if hasattr(new_brain, 'fromBytes'):
                success = new_brain.fromBytes(data)
                print(f"Brain deserialization: {'Success' if success else 'Failed'}")
        
        if hasattr(brain, 'toJson'):
            json_str = brain.toJson()
            print(f"Brain serialized to JSON: {len(json_str)} characters")
            
    except Exception as e:
        print(f"Brain serialization test failed: {e}")

def test_compression_serialization():
    """Test compression serialization"""
    print("\n=== Testing Compression Serialization ===")
    
    # Create config
    config = pynlm.Config()
    config.set("neuron_count", 50)
    config.set("learning_rate", 0.001)
    
    # Test compression methods if available
    try:
        if hasattr(config, 'toBytes'):
            data = config.toBytes()
            print(f"Original data size: {len(data)} bytes")
            
            # Test compression
            compressed = config.toBytesCompressed()
            print(f"Compressed data size: {len(compressed)} bytes")
            print(f"Compression ratio: {len(compressed)/len(data):.2%}")
            
            # Test decompression
            decompressed = config.fromBytesCompressed(compressed)
            print(f"Decompressed data size: {len(decompressed)} bytes")
            
    except Exception as e:
        print(f"Compression serialization test failed: {e}")

def test_validation_and_metadata():
    """Test validation and metadata methods"""
    print("\n=== Testing Validation and Metadata ===")
    
    # Create config
    config = pynlm.Config()
    config.set("neuron_count", 10)
    
    try:
        # Test validation
        if hasattr(config, 'validateConfig'):
            issues = config.validateConfig()
            print(f"Validation issues: {len(issues)} found")
            
        # Test metadata
        if hasattr(config, 'getSummary'):
            summary = config.getSummary()
            print(f"Config summary: {len(summary)} keys")
            
        # Test checksum
        if hasattr(config, 'getChecksum'):
            checksum = config.getChecksum()
            print(f"Checksum: {checksum}")
            
    except Exception as e:
        print(f"Validation and metadata test failed: {e}")

def test_world_serialization():
    """Test world serialization"""
    print("\n=== Testing World Serialization ===")
    
    # Create world
    world = pynlm.SimpleWorld()
    print("Created world")
    
    # Test basic methods
    try:
        if hasattr(world, 'getWidth') and hasattr(world, 'getHeight'):
            width = world.getWidth()
            height = world.getHeight()
            print(f"World dimensions: {width}x{height}")
            
        if hasattr(world, 'getSimulationTime'):
            sim_time = world.getSimulationTime()
            print(f"Simulation time: {sim_time}")
            
        # Test serialization if available
        if hasattr(world, 'toJson'):
            json_str = world.toJson()
            print(f"World serialized to JSON: {len(json_str)} characters")
            
    except Exception as e:
        print(f"World serialization test failed: {e}")

def test_agent_brain_serialization():
    """Test agent brain serialization"""
    print("\n=== Testing AgentBrain Serialization ===")
    
    # Create config and brain
    config = pynlm.Config()
    config.set("neuron_count", 100)
    
    brain = pynlm.Brain(config)
    if hasattr(brain, 'initialize'):
        brain.initialize()
    
    # Create agent brain
    agent_brain = pynlm.AgentBrain(brain)
    print("Created agent brain")
    
    # Test basic methods
    try:
        if hasattr(agent_brain, 'getNeuromodulationLevel'):
            neuromod = agent_brain.getNeuromodulationLevel()
            print(f"Neuromodulation level: {neuromod}")
            
        if hasattr(agent_brain, 'getCuriosityLevel'):
            curiosity = agent_brain.getCuriosityLevel()
            print(f"Curiosity level: {curiosity}")
            
        # Test serialization if available
        if hasattr(agent_brain, 'toJson'):
            json_str = agent_brain.toJson()
            print(f"AgentBrain serialized to JSON: {len(json_str)} characters")
            
    except Exception as e:
        print(f"AgentBrain serialization test failed: {e}")

def test_batch_serialization():
    """Test batch serialization"""
    print("\n=== Testing Batch Serialization ===")
    
    try:
        # Create multiple brains
        config1 = pynlm.Config()
        config1.set("neuron_count", 50)
        
        config2 = pynlm.Config()
        config2.set("neuron_count", 100)
        
        brain1 = pynlm.Brain(config1)
        brain2 = pynlm.Brain(config2)
        
        if hasattr(brain1, 'initialize'):
            brain1.initialize()
        if hasattr(brain2, 'initialize'):
            brain2.initialize()
        
        # Test batch serialization
        if hasattr(brain1, 'batchSave'):
            brains = [brain1, brain2]
            with tempfile.NamedTemporaryFile(mode='w', suffix='.batch', delete=False) as f:
                batch_file = f.name
            
            success = brain1.batchSave(brains, batch_file)
            print(f"Batch save: {'Success' if success else 'Failed'}")
            
            # Test batch load
            if success and hasattr(brain1, 'batchLoad'):
                loaded_brains = brain1.batchLoad(batch_file, 2)
                print(f"Batch load: {len(loaded_brains)} brains loaded")
                
            os.unlink(batch_file)
        
    except Exception as e:
        print(f"Batch serialization test failed: {e}")

def test_backward_compatibility():
    """Test backward compatibility with existing save/load methods"""
    print("\n=== Testing Backward Compatibility ===")
    
    # Create and save brain using existing save method
    config = pynlm.Config()
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    
    brain = pynlm.Brain(config)
    if hasattr(brain, 'initialize'):
        brain.initialize()
    
    try:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.checkpoint', delete=False) as f:
            checkpoint_file = f.name
        
        # Use existing save method
        if hasattr(brain, 'save'):
            brain.save(checkpoint_file)
            print(f"Saved brain using existing save method: {checkpoint_file}")
            
            # Use existing load method
            new_brain = pynlm.Brain(config)
            if hasattr(new_brain, 'load'):
                new_brain.load(checkpoint_file)
                print("Loaded brain using existing load method successfully")
                
                # Verify basic properties
                if hasattr(new_brain, 'getTotalNeuronCount'):
                    neuron_count = new_brain.getTotalNeuronCount()
                    print(f"Loaded brain neuron count: {neuron_count}")
            
            os.unlink(checkpoint_file)
        
    except Exception as e:
        print(f"Backward compatibility test failed: {e}")

def main():
    """Run all tests"""
    print("NLM Python Bindings Serialization Test Suite")
    print("=" * 50)
    
    # Run all test functions
    test_config_serialization()
    test_brain_serialization()
    test_compression_serialization()
    test_validation_and_metadata()
    test_world_serialization()
    test_agent_brain_serialization()
    test_batch_serialization()
    test_backward_compatibility()
    
    print("\n" + "=" * 50)
    print("Test suite completed!")
    print("\nNote: Some tests may not run if serialization methods")
    print("are not yet implemented in the Python bindings.")

if __name__ == "__main__":
    main()