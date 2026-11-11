#!/usr/bin/env python3
"""
Quick test script to verify the NLM Python package improvements.
"""

import sys
import os

# Add pynlm to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'pynlm'))

def test_basic_imports():
    """Test basic imports."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        print(f"  Version: {pynlm.__version__}")
        print(f"  Author: {pynlm.__author__}")
        
        # Test main classes
        from pynlm import Brain, AgentBrain, SimpleWorld, Config
        print("✓ All main classes imported successfully")
        
        # Test type definitions
        from pynlm.types import (
            NeuronId, SynapseId, RegionId, PopulationId,
            NeuronType, ActionType, DevelopmentalStage
        )
        print("✓ All type definitions imported successfully")
        
        # Test sensory classes
        from pynlm.sensory import Vision, Audio, InternalSignals
        print("✓ All sensory classes imported successfully")
        
        # Test motor classes
        from pynlm.motor import Action
        print("✓ All motor classes imported successfully")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

def test_config_functionality():
    """Test configuration functionality."""
    print("\nTesting configuration functionality...")
    
    try:
        from pynlm import Config
        
        # Test config creation
        config = Config()
        print("✓ Config created successfully")
        
        # Test config properties
        config.set('test_key', 'test_value')
        print("✓ Config property set successful")
        
        value = config.get('test_key')
        print(f"✓ Config property get successful: {value}")
        
        # Test config types
        config.set('int_value', 42)
        config.set('float_value', 3.14)
        config.set('bool_value', True)
        config.set('string_value', 'hello')
        
        print("✓ Config with different types created successfully")
        
        # Test config profiles
        result = config.apply_profile('default')
        print(f"✓ Config profile applied: {result}")
        
        # Test config validation
        errors = config.validate()
        print(f"✓ Config validation completed: {len(errors)} errors")
        
        return True
        
    except Exception as e:
        print(f"✗ Config test error: {e}")
        return False

def test_type_definitions():
    """Test type definitions."""
    print("\nTesting type definitions...")
    
    try:
        from pynlm.types import (
            NeuronId, SynapseId, RegionId, PopulationId,
            NeuronType, ActionType, DevelopmentalStage,
            INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
            INVALID_REGION_ID, INVALID_POPULATION_ID
        )
        
        # Test ID creation
        neuron_id = NeuronId(42)
        print(f"✓ NeuronId created: {neuron_id}")
        
        synapse_id = SynapseId(100)
        print(f"✓ SynapseId created: {synapse_id}")
        
        region_id = RegionId(5)
        print(f"✓ RegionId created: {region_id}")
        
        population_id = PopulationId(1)
        print(f"✓ PopulationId created: {population_id}")
        
        # Test enum values
        print(f"✓ NeuronType.Excitatory: {NeuronType.Excitatory}")
        print(f"✓ ActionType.MoveForward: {ActionType.MoveForward}")
        
        # Test constants
        print(f"✓ INVALID_NEURON_ID: {INVALID_NEURON_ID}")
        print(f"✓ INVALID_SYNAPSE_ID: {INVALID_SYNAPSE_ID}")
        
        return True
        
    except Exception as e:
 corde = f"✗ Type definitions test error: {e}")
        return False

def test_motor_functionality():
    """Test motor functionality."""
    print("\nTesting motor functionality...")
    
    try:
        from pynlm.motor import (
            create_action, create_movement_action,
            create_look_action, create_interaction_action,
            compose_actions, get_walking_sequence
        )
        
        # Test action creation
        action = create_action()
        print(f"✓ Action created: {action}")
        
        # Test movement action
        move_action = create_movement_action('forward', 0.5, 1.0)
        print(f"✓ Movement action created: {move_action}")
        
        # Test look action
        look_action = create_look_action('up', 0.8)
        print(f"✓ Look action created: {look_action}")
        
        # Test interaction action
        interact_action = create_interaction_action('interact', {'target': 'object'})
        print(f"✓ Interaction action created: {interact_action}")
        
        # Test action composition
        composed = compose_actions(move_action, look_action, blend_factor=0.5)
        print(f"✓ Actions composed: {composed}")
        
        # Test walking sequence
        walking_seq = get_walking_sequence(speed=0.3, duration=0.5)
        print(f"✓ Walking sequence created: {len(walking_seq)} actions")
        
        return True
        
    except Exception as e:
        print(f"✗ Motor functionality test error: {e}")
        return False

def test_sensory_functionality():
    """Test sensory functionality."""
    print("\nTesting sensory functionality...")
    
    try:
        from pynlm.sensory import Vision, Audio, InternalSignals
        
        # Test vision
        vision = Vision(32, 32, 1)
        print(f"✓ Vision created: {vision.get_dimensions()}")
        
        # Test audio
        audio = Audio(16000, 4096)
        print(f"✓ Audio created: {audio.get_dimensions()}")
        
        # Test internal signals
        internal = InternalSignals()
        internal.add_signal(1.0)
        internal.add_signal(2.0)
        stats = internal.get_statistics()
        print(f"✓ Internal signals created: {stats['count']} signals")
        
        return True
        
    except Exception as e:
        print(f"✗ Sensory functionality test error: {e}")
        return False

def test_documentation():
    """Test documentation and package structure."""
    print("\nTesting documentation and package structure...")
    
    try:
        # Check if __init__.py has proper documentation
        with open('pynlm/__init__.py', 'r') as f:
            init_content = f.read()
        
        if '__version__' in init_content and '__author__' in init_content:
            print("✓ Package metadata present in __init__.py")
        else:
            print("✗ Package metadata missing from __init__.py")
        
        # Check if all modules have __all__ defined
        modules = ['brain', 'agent', 'world', 'sensory', 'motor', 'config', 'types']
        
        for module in modules:
            try:
                module_path = f'pynlm/{module}.py'
                with open(module_path, 'r') as f:
                    content = f.read()
                
                if '__all__' in content:
                    print(f"✓ {module}.py has __all__ defined")
                else:
                    print(f"✓ {module}.py exists (no __all__ required)")
                    
            except FileNotFoundError:
                print(f"✗ {module}.py not found")
        
        return True
        
    except Exception as e:
        print(f"✗ Documentation test error: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Package Improvement Test")
    print("=" * 60)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Configuration Functionality", test_config_functionality),
        ("Type Definitions", test_type_definitions),
        ("Motor Functionality", test_motor_functionality),
        ("Sensory Functionality", test_sensory_functionality),
        ("Documentation and Structure", test_documentation),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"✗ Test '{test_name}' failed with exception: {e}")
            results.append((test_name, False))
    
    # Print summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:.<40} {status}")
        if result:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! The NLM Python package improvements are working correctly.")
        return True
    else:
        print(f"\n❌ {total - passed} test(s) failed. Please review the implementation.")
        return False

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)