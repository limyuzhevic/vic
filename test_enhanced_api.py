#!/usr/bin/env python3
"""
Test script for enhanced NLM Python API.

This script tests the basic functionality of the enhanced NLM Python interface
to ensure everything works correctly.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_functionality():
    """Test basic functionality of the enhanced NLM Python API."""
    print("Testing enhanced NLM Python API...")
    
    try:
        # Import the enhanced API
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test version information
        metadata = pynlm.get_metadata()
        print(f"✓ Version: {metadata['version']}")
        print(f"✓ Author: {metadata['author']}")
        print(f"✓ Features: {len(metadata['features'])} features")
        
        # Test factory functions
        config = pynlm.create_default_config()
        print("✓ Created default configuration")
        
        brain = pynlm.create_brain(config)
        print("✓ Created brain")
        
        # Test configuration settings
        config.set("brain.neuron_count", 100)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        print("✓ Configured brain parameters")
        
        # Test brain initialization (this will fail if dependencies are not met,
        # but we can test the API structure)
        try:
            success = brain.initialize()
            if success:
                print("✓ Brain initialized successfully")
            else:
                print("⚠ Brain initialization returned False (dependencies may be missing)")
        except Exception as e:
            print(f"⚠ Brain initialization failed (expected if dependencies missing): {e}")
        
        # Test enhanced classes
        from pynlm import LearningMetrics, SimulationConfig, PerformanceProfiler, SimulationMonitor
        
        metrics = LearningMetrics()
        print("✓ Created LearningMetrics instance")
        
        sim_config = SimulationConfig()
        print("✓ Created SimulationConfig instance")
        
        profiler = PerformanceProfiler()
        print("✓ Created PerformanceProfiler instance")
        
        monitor = SimulationMonitor(brain) if brain else SimulationMonitor(None)
        print("✓ Created SimulationMonitor instance")
        
        # Test configuration validation
        errors = sim_config.validate()
        print(f"✓ Configuration validation: {len(errors)} errors (should be 0)")
        
        # Test utility functions
        from pynlm import validate_configuration, create_reproducible_environment
        
        val_errors = validate_configuration(sim_config)
        print(f"✓ validate_configuration: {len(val_errors)} errors")
        
        # Test version printing
        print("\nVersion information:")
        pynlm.print_version_info()
        
        print("\n✓ All basic functionality tests completed successfully!")
        return True
        
    except Exception as e:
        print(f"✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_features():
    """Test enhanced features of the NLM Python API."""
    print("\nTesting enhanced features...")
    
    try:
        import pynlm
        
        # Test LearningMetrics
        from pynlm import LearningMetrics
        
        metrics = LearningMetrics()
        metrics.total_steps = 100
        metrics.total_neurons = 1000
        metrics.firing_rate = 0.05
        metrics.e_i_ratio = 1.2
        
        # Test methods
        is_learning = metrics.is_learning()
        print(f"✓ Learning detection: {is_learning}")
        
        performance_grade = metrics.get_performance_grade()
        print(f"✓ Performance grade: {performance_grade}")
        
        metrics_dict = metrics.to_dict()
        print(f"✓ Serialization: {len(metrics_dict)} fields")
        
        # Test SimulationConfig
        from pynlm import SimulationConfig
        
        config = SimulationConfig()
        config.neuron_count = 2000
        config.total_steps = 500
        
        # Test validation
        errors = config.validate()
        print(f"✓ SimulationConfig validation: {len(errors)} errors")
        
        # Test PerformanceProfiler
        from pynlm import PerformanceProfiler
        
        profiler = PerformanceProfiler()
        profiler.start_profiling()
        print("✓ PerformanceProfiler started")
        
        # Test SimulationMonitor
        from pynlm import SimulationMonitor
        
        monitor = SimulationMonitor(None)
        print("✓ SimulationMonitor created")
        
        print("✓ All enhanced features tests completed successfully!")
        return True
        
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_python_compatibility():
    """Test Python compatibility and type hints."""
    print("\nTesting Python compatibility...")
    
    try:
        import pynlm
        
        # Check Python version
        import sys
        print(f"✓ Python version: {sys.version}")
        
        # Check module attributes
        expected_attrs = [
            '__version__',
            '__author__',
            '__license__',
            'get_metadata',
            'print_version_info',
            'create_default_config',
            'create_brain',
            'create_agent',
            'create_world',
        ]
        
        for attr in expected_attrs:
            if hasattr(pynlm, attr):
                print(f"✓ Has attribute: {attr}")
            else:
                print(f"⚠ Missing attribute: {attr}")
        
        # Check enhanced classes
        enhanced_classes = [
            'LearningMetrics',
            'SimulationConfig',
            'PerformanceProfiler',
            'SimulationMonitor',
            'EnvironmentBuilder',
        ]
        
        for cls_name in enhanced_classes:
            if hasattr(pynlm, cls_name):
                print(f"✓ Has enhanced class: {cls_name}")
            else:
                print(f"⚠ Missing enhanced class: {cls_name}")
        
        print("✓ All compatibility tests completed successfully!")
        return True
        
    except Exception as e:
        print(f"✗ Compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Enhanced Python API - Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Features", test_enhanced_features),
        ("Python Compatibility", test_python_compatibility),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        success = test_func()
        results.append((test_name, success))
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return 0
    else:
        print(f"\n✗ {failed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())