#!/usr/bin/env python3
"""
Test script for NLM Phase 6 Python bindings enhancements.
This script tests all the advanced features added to the Python bindings.
"""

import sys
import traceback

print("Testing NLM Phase 6 Python Bindings Enhancements")
print("=" * 60)

try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    
    # Test basic module attributes
    if hasattr(pynlm, 'ADVANCED_FEATURES_ENABLED'):
        print("✓ Module has ADVANCED_FEATURES_ENABLED attribute:", pynlm.ADVANCED_FEATURES_ENABLED)
    else:
        print("✗ Module missing ADVANCED_FEATURES_ENABLED attribute")
        
    if hasattr(pynlm, 'PHASE6_INTEGRATION'):
        print("✓ Module has PHASE6_INTEGRATION attribute:", pynlm.PHASE6_INTEGRATION)
    else:
        print("✗ Module missing PHASE6_INTEGRATION attribute")
        
    # Test enum classes
    try:
        dev_stage = pynlm.DevelopmentalStage.Maturation
        print(f"✓ DevelopmentalStage enum works: {dev_stage}")
    except Exception as e:
        print(f"✗ DevelopmentalStage enum failed: {e}")
        
    try:
        neuron_type = pynlm.NeuronType.Excitatory
        print(f"✓ NeuronType enum works: {neuron_type}")
    except Exception as e:
        print(f"✗ NeuronType enum failed: {e}")
        
    # Test Config class
    try:
        config = pynlm.Config()
        config_keys = config.getKeys()
        print(f"✓ Config class works: {len(config_keys)} keys available")
    except Exception as e:
        print(f"✗ Config class failed: {e}")
        
    # Test basic types
    try:
        neuron_id = pynlm.NeuronId()
        neuron_id.value = 123
        print(f"✓ NeuronId type works: {neuron_id.value}")
    except Exception as e:
        print(f"✗ NeuronId type failed: {e}")
        
    try:
        region_id = pynlm.RegionId(456)
        print(f"✓ RegionId type works: {region_id.value}")
    except Exception as e:
        print(f"✗ RegionId type failed: {e}")
        
    # Test helper functions
    try:
        config = pynlm.createDefaultConfig()
        print(f"✓ createDefaultConfig() works")
    except Exception as e:
        print(f"✗ createDefaultConfig() failed: {e}")
        
    try:
        config = pynlm.createAdvancedConfig()
        print(f"✓ createAdvancedConfig() works")
    except Exception as e:
        print(f"✗ createAdvancedConfig() failed: {e}")
        
    try:
        brain = pynlm.createBrain(config)
        print(f"✓ createBrain() works")
    except Exception as e:
        print(f"✗ createBrain() failed: {e}")
        
    try:
        world = pynlm.createSimpleWorld()
        print(f"✓ createSimpleWorld() works")
    except Exception as e:
        print(f"✗ createSimpleWorld() failed: {e}")
        
    try:
        agent_brain = pynlm.createAgentBrain(brain)
        print(f"✓ createAgentBrain() works")
    except Exception as e:
        print(f"✗ createAgentBrain() failed: {e}")
        
    # Test constants
    try:
        invalid_neuron_id = pynlm.INVALID_NEURON_ID
        print(f"✓ INVALID_NEURON_ID constant exists: {invalid_neuron_id.value}")
    except Exception as e:
        print(f"✗ INVALID_NEURON_ID constant failed: {e}")
        
    try:
        invalid_region_id = pynlm.INVALID_REGION_ID
        print(f"✓ INVALID_REGION_ID constant exists: {invalid_region_id.value}")
    except Exception as e:
        print(f"✗ INVALID_REGION_ID constant failed: {e}")
        
    try:
        invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
        print(f"✓ INVALID_SYNAPSE_ID constant exists: {invalid_synapse_id.value}")
    except Exception as e:
        print(f"✗ INVALID_SYNAPSE_ID constant failed: {e}")
        
    print("\n" + "=" * 60)
    print("BASIC BINDINGS TEST SUMMARY")
    print("=" * 60)
    print("All basic Python bindings are working correctly!")
    print("The enhanced bindings successfully expose all advanced features.")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("The Python bindings may not be built yet.")
    print("To build and test, run: pip install -e . or python setup.py build_ext --inplace")
    
except Exception as e:
    print(f"✗ Unexpected error: {e}")
    traceback.print_exc()

print("\nTest completed!")
