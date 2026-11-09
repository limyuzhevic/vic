#!/usr/bin/env python3
"""
Simple test to verify NLM enhanced bindings can be imported
and basic functionality works.
"""

import sys

def test_basic_import():
    """Test that the enhanced bindings can be imported"""
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Check version
        if hasattr(pynlm, 'version'):
            print(f"✓ Version: {pynlm.version}")
        
        # Check configuration presets
        if hasattr(pynlm, 'createMiniBrainConfig'):
            print("✓ createMiniBrainConfig available")
        
        if hasattr(pynlm, 'createStandardBrainConfig'):
            print("✓ createStandardBrainConfig available")
            
        if hasattr(pynlm, 'createAdvancedBrainConfig'):
            print("✓ createAdvancedBrainConfig available")
            
        # Check performance monitoring
        if hasattr(pynlm, 'PerformanceMonitor'):
            print("✓ PerformanceMonitor available")
            
        # Check context manager
        if hasattr(pynlm, 'createSimulationContext'):
            print("✓ createSimulationContext available")
            
        # Check network analysis
        if hasattr(pynlm, 'VisualizationHelper'):
            print("✓ VisualizationHelper available")
            
        # Check experimental features
        if hasattr(pynlm, 'experimental'):
            print("✓ experimental submodule available")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False

def test_basic_functionality():
    """Test basic NLM functionality"""
    try:
        import pynlm
        
        # Create default brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Brain functionality: {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Spike count: {brain.getTotalSpikeCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality failed: {e}")
        return False

def test_configuration_presets():
    """Test configuration presets"""
    try:
        import pynlm
        
        # Test each preset
        presets = [
            ("Mini", pynlm.createMiniBrainConfig),
            ("Standard", pynlm.createStandardBrainConfig),
            ("Advanced", pynlm.createAdvancedBrainConfig)
        ]
        
        for name, preset_func in presets:
            config = preset_func()
            print(f"✓ {name} preset created")
            
        return True
        
    except Exception as e:
        print(f"✗ Configuration presets failed: {e}")
        return False

def test_enhanced_features():
    """Test enhanced features availability"""
    try:
        import pynlm
        
        # Check for new classes and functions
        required_features = [
            ('PerformanceMonitor', hasattr(pynlm, 'PerformanceMonitor')),
            ('createSimulationContext', hasattr(pynlm, 'createSimulationContext')),
            ('VisualizationHelper', hasattr(pynlm, 'VisualizationHelper')),
            ('experimental', hasattr(pynlm, 'experimental')),
        ]
        
        all_good = True
        for feature_name, available in required_features:
            if available:
                print(f"✓ {feature_name} available")
            else:
                print(f"✗ {feature_name} NOT available")
                all_good = False
                
        return all_good
        
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("NLM Enhanced Bindings - Test Suite")
    print("=" * 50)
    print()
    
    tests = [
        ("Basic Import", test_basic_import),
        ("Basic Functionality", test_basic_functionality),
        ("Configuration Presets", test_configuration_presets),
        ("Enhanced Features", test_enhanced_features),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"Running: {test_name}")
        print("-" * 30)
        
        if test_func():
            passed += 1
            print("✓ PASSED")
        else:
            print("✗ FAILED")
        
        print()
    
    print("=" * 50)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! Enhanced bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())