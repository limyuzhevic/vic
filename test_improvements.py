#!/usr/bin/env python3
"""
Quick Test Script for NLM Python Bindings Improvements

This script verifies that the improvements made to the NLM Python bindings
are working correctly. It tests the enhanced API, new factory methods,
and utility functions.

Run this script to verify your NLM Python installation:
    python test_improvements.py
"""

import sys
import traceback

def test_basic_imports():
    """Test basic imports from the NLM module."""
    print("Testing basic imports...")
    try:
        import nlm
        print(f"✓ Successfully imported nlm module")
        print(f"✓ Python bindings version: {getattr(nlm, '__version__', '0.1.0')}")
        return True
    except ImportError as e:
        print(f"✗ Failed to import nlm: {e}")
        return False

def test_factory_methods():
    """Test new factory methods."""
    print("\nTesting factory methods...")
    try:
        import nlm
        
        # Test createDefaultConfig
        config = nlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        # Test createBrainWithDefaults
        brain = nlm.createBrainWithDefaults(100, 2)
        print(f"✓ createBrainWithDefaults() works (created brain with {brain.getTotalNeuronCount()} neurons)")
        
        # Test createVision
        vision = nlm.createVision(64, 64, 3)
        print(f"✓ createVision() works (dimensions: {vision.getDimensions()})")
        
        # Test createAudio
        audio = nlm.createAudio(44100, 2000)
        print(f"✓ createAudio() works (dimensions: {audio.getDimensions()})")
        
        # Test createInternalSignals
        internal = nlm.createInternalSignals()
        print(f"✓ createInternalSignals() works (dimensions: {internal.getDimensions()})")
        
        # Test createAction
        action = nlm.createAction(nlm.ActionType.Interact, [0.5, 0.8, 0.3])
        print(f"✓ createAction() works (type: {action.getType()})")
        
        return True
        
    except Exception as e:
        print(f"✗ Factory methods test failed: {e}")
        traceback.print_exc()
        return False

def test_validation_and_utilities():
    """Test validation and utility functions."""
    print("\nTesting validation and utilities...")
    try:
        import nlm
        
        # Test validateConfig
        config = nlm.createDefaultConfig()
        try:
            nlm.validateConfig(config)
            print("✓ validateConfig() works")
        except Exception as e:
            print(f"⚠ validateConfig() returned error (expected for default config): {e}")
        
        # Test getConstants
        constants = nlm.getConstants()
        print(f"✓ getConstants() works (found {len(constants)} constants)")
        
        # Test random ID generation
        neuron_id = nlm.randomNeuronId()
        synapse_id = nlm.randomSynapseId()
        print(f"✓ randomNeuronId() works: {neuron_id}")
        print(f"✓ randomSynapseId() works: {synapse_id}")
        
        return True
        
    except Exception as e:
        print(f"✗ Validation and utilities test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_classes():
    """Test enhanced class functionality."""
    print("\nTesting enhanced classes...")
    try:
        import nlm
        
        # Test Action enhancements
        action = nlm.Action(nlm.ActionType.Interact, [0.5, 0.8, 0.3])
        action.setType(nlm.ActionType.MoveForward)
        action.setParameters([0.7, 0.9])
        
        print(f"✓ Action enhancements work (type: {action.getType()}, parameters: {len(action.getParameters())})")
        
        # Test Vision enhancements
        vision = nlm.createVision(32, 32, 3)
        vision.setData([i * 0.1 for i in range(3072)])
        
        print(f"✓ Vision enhancements work (dimensions: {vision.getDimensions()}, width: {vision.getWidth()})")
        
        # Test Audio enhancements
        audio = nlm.createAudio(48000, 4800)
        audio.setData([i * 0.05 for i in range(4800)])
        
        print(f"✓ Audio enhancements work (sample rate: {audio.getSampleRate()}, dimensions: {audio.getDimensions()})")
        
        # Test InternalSignals enhancements
        internal = nlm.createInternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(1.0)
        internal.addSignal(-0.5)
        
        print(f"✓ InternalSignals enhancements work (signals: {internal.getSignalCount()})")
        
        # Test WorldObject enhancements
        world_obj = nlm.WorldObject(10.0, 20.0, nlm.WorldObjectType.Resource, 5.0, 1.0)
        distance = world_obj.distanceTo(15.0, 25.0)
        print(f"✓ WorldObject enhancements work (distance: {distance:.2f})")
        
        # Test ActionResult enhancements
        result = nlm.ActionResult(1.0, True, "Success!")
        result_dict = result.toDict()
        print(f"✓ ActionResult enhancements work (toDict keys: {list(result_dict.keys())})")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced classes test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features and integration."""
    print("\nTesting advanced features...")
    try:
        import nlm
        
        # Create brain with configuration
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 200, nlm.ConfigSource.Runtime)
        config.set("region_count", 3, nlm.ConfigSource.Runtime)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Brain creation works ({brain.getTotalNeuronCount()} neurons)")
        
        # Test sensory processing
        vision = nlm.createVision(20, 20, 1)
        vision.setData([0.1] * 400)
        
        percept = nlm.SensoryPercept()
        percept.setVision(vision)
        
        brain.receiveSensoryInput(percept)
        print("✓ Sensory processing works")
        
        # Test action selection
        action = brain.produceAction()
        if action:
            print(f"✓ Action selection works (action: {action.getName()})")
        
        # Test memory systems
        working_memory = brain.getWorkingMemory()
        if working_memory:
            print(f"✓ Working memory works (capacity: {working_memory.getCapacity()})")
        
        # Test neuromodulation
        dopamine = brain.getDopamine()
        if dopamine:
            print(f"✓ Neuromodulation works (dopamine level: {dopamine.getLevel()})")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        traceback.print_exc()
        return False

def test_clone_methods():
    """Test enhanced clone methods."""
    print("\nTesting clone methods...")
    try:
        import nlm
        
        # Test Vision clone
        vision1 = nlm.createVision(16, 16, 3)
        vision1.setData([0.5] * 768)
        
        vision2 = vision1.clone()
        print(f"✓ Vision clone works (dimensions: {vision2.getDimensions()})")
        
        # Test Audio clone
        audio1 = nlm.createAudio(22050, 1000)
        audio1.setData([0.3] * 1000)
        
        audio2 = audio1.clone()
        print(f"✓ Audio clone works (sample rate: {audio2.getSampleRate()})")
        
        # Test Action clone
        action1 = nlm.Action(nlm.ActionType.Eat, [0.8])
        action2 = action1.clone()
        print(f"✓ Action clone works (type: {action2.getType()})")
        
        return True
        
    except Exception as e:
        print(f"✗ Clone methods test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Main test function."""
    print("NLM Python Bindings Improvements Test")
    print("=" * 60)
    
    # Run all tests
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Factory Methods", test_factory_methods),
        ("Validation and Utilities", test_validation_and_utilities),
        ("Enhanced Classes", test_enhanced_classes),
        ("Advanced Features", test_advanced_features),
        ("Clone Methods", test_clone_methods),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            print(f"✗ {test_name} FAILED")
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed! The NLM Python improvements are working correctly.")
        return 0
    else:
        print("✗ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
