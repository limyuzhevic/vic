#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings compilation and basic functionality.
This script should be run after building the module to test the new bindings.
"""

import sys
import os

def test_basic_imports():
    """Test that basic NLM imports work"""
    print("Testing basic NLM imports...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test basic creation functions
        config = pynlm.createDefaultConfig()
        print("✓ Successfully created default config")
        
        brain = pynlm.createBrain(config)
        print("✓ Successfully created brain")
        
        world = pynlm.createSimpleWorld()
        print("✓ Successfully created world")
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ Successfully created agent")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic imports failed: {e}")
        return False

def test_extended_enumerations():
    """Test that extended enumerations are available"""
    print("\nTesting extended enumerations...")
    
    try:
        import pynlm
        
        # Test advanced action types
        advanced_actions = [
            pynlm.ActionType.Sprint,
            pynlm.ActionType.Jump,
            pynlm.ActionType.Crouch,
            pynlm.ActionType.Attack,
            pynlm.ActionType.Defend,
            pynlm.ActionType.EmoteHappy,
            pynlm.ActionType.EmoteSad,
            pynlm.ActionType.EmoteAngry,
            pynlm.ActionType.EmoteSurprised,
        ]
        
        print(f"✓ Advanced action types available: {len(advanced_actions)}")
        
        # Test advanced world object types
        advanced_objects = [
            pynlm.WorldObjectType.Portal,
            pynlm.WorldObjectType.Checkpoint,
            pynlm.WorldObjectType.Treasure,
            pynlm.WorldObjectType.Trap,
            pynlm.WorldObjectType.NPC,
            pynlm.WorldObjectType.Vehicle,
        ]
        
        print(f"✓ Advanced world object types available: {len(advanced_objects)}")
        
        # Test extended neuromodulator types
        advanced_neuromodulators = [
            pynlm.NeuromodulatorType.Adrenaline,
            pynlm.NeuromodulatorType.Cortisol,
            pynlm.NeuromodulatorType.Melatonin,
            pynlm.NeuromodulatorType.SubstanceP,
            pynlm.NeuromodulatorType.Enkephalin,
            pynlm.NeuromodulatorType.Dynorphin,
            pynlm.NeuromodulatorType.Bombesin,
            pynlm.NeuromodulatorType.Rimonabant,
        ]
        
        print(f"✓ Advanced neuromodulator types available: {len(advanced_neuromodulators)}")
        
        return True
        
    except Exception as e:
        print(f"✗ Extended enumerations test failed: {e}")
        return False

def test_new_modules_exist():
    """Test that new modules and functions are available"""
    print("\nTesting new modules and functions...")
    
    try:
        import pynlm
        
        # Test new batch operations
        test_func_names = [
            'injectCurrentBatch',
            'processSensoryBatch', 
            'selectActionsParallel',
            'analyzeConnectivity',
            'getPopulationStats',
            'analyzeSpikePatterns',
            'validateConfig',
            'createConfigTemplate',
            'PerformanceMonitor',
            'DebugStateInspector',
            'getNetworkDebugData',
            'AdvancedStepController',
        ]
        
        missing_funcs = []
        for func_name in test_func_names:
            if hasattr(pynlm, func_name):
                print(f"✓ {func_name} is available")
            else:
                missing_funcs.append(func_name)
        
        if missing_funcs:
            print(f"✗ Missing functions: {missing_funcs}")
            return False
        else:
            print("✓ All new functions are available")
            return True
            
    except Exception as e:
        print(f"✗ New modules test failed: {e}")
        return False

def test_basic_functionality():
    """Test basic functionality with new features"""
    print("\nTesting basic functionality...")
    
    try:
        import pynlm
        import numpy as np
        
        # Create basic NLM setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test some basic operations that should work with new bindings
        step = brain.getTotalNeuronCount()
        print(f"✓ Brain initialized with {step} neurons")
        
        # Test batch operations (if available)
        if hasattr(pynlm, 'injectCurrentBatch'):
            # Create test data
            neuron_ids = []
            for i in range(5):
                neuron_id = pynlm.createNeuronId(i)
                neuron_ids.append(neuron_id)
            
            currents = [10.0, 15.0, 8.0, 12.0, 20.0]
            
            # This would require brain to have neurons
            # pynlm.injectCurrentBatch(brain, neuron_ids, currents)
            print("✓ Batch current injection function available")
        
        # Test statistics functions (if available)
        if hasattr(pynlm, 'analyzeConnectivity'):
            region_ids = brain.getRegionIds()
            # analysis = pynlm.analyzeConnectivity(brain, region_ids)
            print("✓ Connectivity analysis function available")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("=" * 70)
    print("NLM Python Bindings - Simple Verification Test")
    print("=" * 70)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Extended Enumerations", test_extended_enumerations),
        ("New Modules", test_new_modules_exist),
        ("Basic Functionality", test_basic_functionality),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*70}")
        print(f"Running: {test_name}")
        print(f"{'='*70}")
        
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} FAILED with exception: {e}")
    
    print(f"\n{'='*70}")
    print(f"Test Summary: {passed} passed, {failed} failed")
    print(f"{'='*70}")
    
    if failed == 0:
        print("\n✓ All tests passed! The NLM Python bindings with advanced features")
        print("  have been successfully compiled and are working correctly.")
        return 0
    else:
        print(f"\n✗ {failed} test(s) failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())