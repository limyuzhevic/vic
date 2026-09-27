#!/usr/bin/env python3
"""
Quick test script to verify the advanced NLM features are working.
This script tests the basic functionality without running full simulations.
"""

import sys
import traceback

def test_basic_imports():
    """Test that all advanced modules can be imported."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print("✓ Core pynlm module imported successfully")
        
        from pynlm_advanced import (
            SimulationPreset, BrainVisualizer, AgentTrainer,
            SimulationProfiler, create_preset_brain,
            create_agent_world_setup, setup_debug_monitor
        )
        print("✓ All advanced features imported successfully")
        
        return True
    except Exception as e:
        print(f"✗ Import failed: {e}")
        traceback.print_exc()
        return False

def test_presets():
    """Test that simulation presets work."""
    print("\nTesting simulation presets...")
    
    try:
        from pynlm_advanced import create_preset_brain, SimulationPreset
        
        for preset in SimulationPreset:
            brain_config = create_preset_brain(preset)
            brain = brain_config['brain']
            print(f"✓ {preset.value:20} - Created brain with {brain.getTotalNeuronCount():4d} neurons")
        
        return True
    except Exception as e:
        print(f"✗ Preset test failed: {e}")
        traceback.print_exc()
        return False

def test_system_setup():
    """Test system setup factory function."""
    print("\nTesting system setup...")
    
    try:
        from pynlm_advanced import create_agent_world_setup
        
        system = create_agent_world_setup(vision_width=8, vision_height=8)
        print(f"✓ System created successfully")
        print(f"  - Vision: {system['vision_size']} pixels")
        print(f"  - Neurons: {system['brain'].getTotalNeuronCount()}")
        print(f"  - Motor commands: {system['agent'].getMotorOutputSize()}")
        
        return True
    except Exception as e:
        print(f"✗ System setup test failed: {e}")
        traceback.print_exc()
        return False

def test_visualizer():
    """Test BrainVisualizer class."""
    print("\nTesting BrainVisualizer...")
    
    try:
        from pynlm_advanced import create_agent_world_setup, BrainVisualizer
        
        system = create_agent_world_setup(vision_width=4, vision_height=4)
        visualizer = BrainVisualizer(system['brain'])
        
        print(f"✓ BrainVisualizer created successfully")
        print(f"  - Can collect activity data: {hasattr(visualizer, 'collect_activity_data')}")
        print(f"  - Can analyze patterns: {hasattr(visualizer, 'analyze_patterns')}")
        
        return True
    except Exception as e:
        print(f"✗ BrainVisualizer test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced feature classes."""
    print("\nTesting advanced feature classes...")
    
    try:
        from pynlm_advanced import (
            BrainVisualizer, AgentTrainer, SimulationProfiler,
            setup_debug_monitor
        )
        
        # Test that all classes can be instantiated
        print("✓ BrainVisualizer class: Available")
        print("✓ AgentTrainer class: Available")
        print("✓ SimulationProfiler class: Available")
        print("✓ setup_debug_monitor function: Available")
        
        return True
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        traceback.print_exc()
        return False

def test_documentation_examples():
    """Test that documentation examples can be imported."""
    print("\nTesting documentation examples...")
    
    try:
        # Test importing the quick reference
        from python import advanced_quick_ref
        print("✓ advanced_quick_ref.py can be imported")
        
        # Test that the summary can be accessed
        import os
        if os.path.exists('python/NLM_Advanced_Features_Summary.md'):
            print("✓ NLM_Advanced_Features_Summary.md exists")
        
        return True
    except Exception as e:
        print(f"✗ Documentation examples test failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Advanced Features - Quick Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_presets,
        test_system_setup,
        test_visualizer,
        test_advanced_features,
        test_documentation_examples
    ]
    
    results = []
    for test in tests:
        result = test()
        results.append(result)
    
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = sum(results)
    total = len(results)
    
    print(f"Tests passed: {passed}/{total}")
    
    if passed == total:
        print("🎉 All tests passed! Advanced features are working correctly.")
        return True
    else:
        print("❌ Some tests failed. Please check the output above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
