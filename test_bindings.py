#!/usr/bin/env python3
"""
Simple test to verify the improved NLM Python bindings work correctly.
This script tests basic functionality without actually compiling/building.
"""

import sys
import traceback

def test_id_classes():
    """Test that ID classes work correctly"""
    print("Testing ID classes...")
    
    # Test that we can import the module
    try:
        import pynlm
        print("✓ Module imported successfully")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    
    # Test ID constants
    try:
        print(f"INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
        print(f"INVALID_SYNAPSE_ID: {pynlm.INVALID_SYNAPSE_ID}")
        print(f"INVALID_REGION_ID: {pynlm.INVALID_REGION_ID}")
        print(f"INVALID_POPULATION_ID: {pynlm.INVALID_POPULATION_ID}")
        print("✓ Invalid ID constants are accessible")
    except Exception as e:
        print(f"✗ Failed to access invalid ID constants: {e}")
        return False
    
    # Test NeuronId
    try:
        nid = pynlm.NeuronId()
        print(f"✓ Default NeuronId created: {nid}")
        
        nid2 = pynlm.NeuronId(42)
        print(f"✓ NeuronId with value created: {nid2}")
        print(f"✓ NeuronId.value: {nid2.value}")
        print(f"✓ NeuronId.index: {nid2.index()}")
        print(f"✓ NeuronId.is_valid: {nid2.is_valid()}")
        
        # Test equality
        nid3 = pynlm.NeuronId(42)
        assert nid2 == nid3, "IDs with same value should be equal"
        assert nid2 != nid, "Different IDs should not be equal"
        print("✓ NeuronId equality operators work")
        
        # Test hash
        ids = {nid2, nid3}
        assert len(ids) == 1, "Equal IDs should produce same hash"
        print("✓ NeuronId hash works")
        
    except Exception as e:
        print(f"✗ NeuronId test failed: {e}")
        traceback.print_exc()
        return False
    
    # Test SynapseId
    try:
        sid = pynlm.SynapseId(100)
        print(f"✓ SynapseId created: {sid}")
        assert sid.value == 100
        assert sid.index() == 100
        print("✓ SynapseId properties work")
    except Exception as e:
        print(f"✗ SynapseId test failed: {e}")
        traceback.print_exc()
        return False
    
    # Test RegionId
    try:
        rid = pynlm.RegionId(999)
        print(f"✓ RegionId created: {rid}")
        assert rid.value == 999
        print("✓ RegionId properties work")
    except Exception as e:
        print(f"✗ RegionId test failed: {e}")
        traceback.print_exc()
        return False
    
    # Test PopulationId
    try:
        pid = pynlm.PopulationId(7)
        print(f"✓ PopulationId created: {pid}")
        assert pid.value == 7
        print("✓ PopulationId properties work")
    except Exception as e:
        print(f"✗ PopulationId test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_enums():
    """Test that enum types work correctly"""
    print("\nTesting enum types...")
    
    try:
        import pynlm
        
        # Test NeuronType
        assert pynlm.NeuronType.Excitatory != pynlm.NeuronType.Inhibitory
        print("✓ NeuronType values distinct")
        
        # Test SynapseType
        assert pynlm.SynapseType.Excitatory != pynlm.SynapseType.Inhibitory
        print("✓ SynapseType values distinct")
        
        # Test other enums
        assert pynlm.DevelopmentalStage.Initial != pynlm.DevelopmentalStage.Adult
        print("✓ DevelopmentalStage values distinct")
        
        assert pynlm.ActionType.MoveForward != pynlm.ActionType.MoveBackward
        print("✓ ActionType values distinct")
        
        return True
        
    except Exception as e:
        print(f"✗ Enum test failed: {e}")
        traceback.print_exc()
        return False

def test_utility_functions():
    """Test utility functions"""
    print("\nTesting utility functions...")
    
    try:
        import pynlm
        
        # Test generateRandomId
        rand_id = pynlm.generateRandomId()
        print(f"✓ Random ID generated: {rand_id}")
        
        # Test createIdFromString
        str_id = pynlm.createIdFromString("test")
        print(f"✓ String-based ID created: {str_id}")
        
        # Test isValidId
        assert pynlm.isValidId(100) == True
        assert pynlm.isValidId(2**64 - 1) == False  # UINT64_MAX is invalid
        print("✓ isValidId works")
        
        # Test normalize/denormalize
        params = [-1.0, 0.0, 1.0]
        norm = pynlm.normalizeActionParameters(params)
        print(f"✓ Normalized: {norm}")
        
        denorm = pynlm.denormalizeActionParameters(norm)
        print(f"✓ Denormalized: {denorm}")
        
        # Check round-trip
        for i in range(3):
            assert abs(params[i] - denorm[i]) < 0.0001
        print("✓ Normalize/denormalize round-trip works")
        
        return True
        
    except Exception as e:
        print(f"✗ Utility function test failed: {e}")
        traceback.print_exc()
        return False

def test_factory_functions():
    """Test factory functions"""
    print("\nTesting factory functions...")
    
    try:
        import pynlm
        
        # Test createDefaultConfig
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created: {config}")
        
        # Test createSimpleWorld
        world = pynlm.createSimpleWorld()
        print(f"✓ Simple world created: {world}")
        
        # Test createBrain
        brain = pynlm.createBrain(config)
        print(f"✓ Brain created: {brain}")
        
        # Test createAgentBrain
        agent_brain = pynlm.createAgentBrain(brain)
        print(f"✓ Agent brain created: {agent_brain}")
        
        return True
        
    except Exception as e:
        print(f"✗ Factory function test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling"""
    print("\nTesting error handling...")
    
    try:
        import pynlm
        
        # Test invalid serialized ID
        try:
            invalid = b"short"
            pynlm.deserializeId(invalid)
            print("✗ Should have thrown error for invalid serialized ID")
            return False
        except Exception:
            print("✓ Invalid serialized ID correctly throws error")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def main():
    print("=" * 60)
    print("NLM Python Bindings Test")
    print("=" * 60)
    
    tests = [
        test_id_classes,
        test_enums,
        test_utility_functions,
        test_factory_functions,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())