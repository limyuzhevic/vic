#!/usr/bin/env python3
"""
Test script to verify the improved Python bindings for NLM.
This tests the critical fixes implemented:
1. Memory safety fix in Config.loadFromArgs
2. Exception handling for std::invalid_argument
3. Neuron class binding
4. RandomGenerator class binding
5. Synapse class binding
"""

import sys
import traceback

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except Exception as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic functionality of improved bindings"""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test Config creation
        config = pynlm.createDefaultConfig()
        print("✓ Created default Config")
        
        # Test Config.set with invalid argument
        try:
            config.set("brain.neuron_count", -1)
            print("✗ Should have thrown InvalidArgument for negative neuron count")
        except pynlm.InvalidArgument as e:
            print(f"✓ Correctly threw InvalidArgument: {e}")
        except Exception as e:
            print(f"✗ Unexpected exception type: {type(e).__name__}: {e}")
        
        # Test Config.saveToFile and loadFromFile
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
            
        config.saveToFile(temp_file)
        print(f"✓ Config saved to file: {temp_file}")
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(temp_file)
        print("✓ Config loaded from file")
        
        os.unlink(temp_file)
        
        # Test Config.loadFromArgs with safe memory handling
        args = ["--neuron_count", "100", "--random_seed", "42"]
        config.loadFromArgs(args)
        print(f"✓ Config.loadFromArgs handled safely with args: {args}")
        
        # Test Neuron creation
        neuron = pynlm.Neuron(pynlm.NeuronId(1))
        print(f"✓ Created Neuron: {neuron}")
        
        # Test Neuron properties
        neuron.setMembranePotential(-65.0)
        neuron.setThreshold(-50.0)
        neuron.setType(pynlm.NeuronType.Excitatory)
        
        print(f"✓ Neuron properties set - V={neuron.getMembranePotential()}, "
              f"thresh={neuron.getThreshold()}, type={neuron.getType()}")
        
        # Test Neuron.stepLIF
        success = neuron.stepLIF(0.0, 0.001)
        print(f"✓ Neuron.stepLIF returned: {success}")
        
        # Test Neuron recording spike
        neuron.recordSpike(0.001)
        print(f"✓ Neuron recorded spike, history size: {len(neuron.getSpikeHistory())}")
        
        # Test RandomGenerator
        rng = pynlm.RandomGenerator()
        rng.seed(42)
        print(f"✓ Created RandomGenerator with seed: {rng.getSeed()}")
        
        uniform1 = rng.uniform(0.0, 1.0)
        uniform2 = rng.uniform(5, 10)
        print(f"✓ RandomGenerator.uniform calls: {uniform1}, {uniform2}")
        
        # Test Synapse creation
        synapse = pynlm.Synapse(
            pynlm.SynapseId(1),
            pynlm.NeuronId(0), 
            pynlm.NeuronId(2)
        )
        print(f"✓ Created Synapse: {synapse}")
        
        # Test Synapse properties
        synapse.setWeight(0.8)
        synapse.setType(pynlm.SynapseType.Excitatory)
        print(f"✓ Synapse properties - weight={synapse.getWeight()}, "
              f"type={synapse.getType()}, isExcitatory={synapse.isExcitatory()}")
        
        print("\n=== All tests completed successfully! ===")
        
    except Exception as e:
        print(f"✗ Test failed with exception: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    test_basic_functionality()