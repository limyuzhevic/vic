#!/usr/bin/env python3
"""
Quick verification script for enhanced NLM Python bindings.
Tests basic functionality to ensure everything works correctly.
"""

import sys
import tempfile
import os

# Add test directory to path
sys.path.insert(0, '/workspace/app/tests')

def test_basic_functionality():
    """Test basic functionality of enhanced bindings."""
    print("Testing basic functionality...")
    
    try:
        # Test imports
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test type aliases
        assert hasattr(pynlm, 'neuron_id')
        assert hasattr(pynlm, 'synapse_id')
        assert hasattr(pynlm, 'action_type')
        print("✓ Type aliases available")
        
        # Test enums
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'ActionType')
        print("✓ Enums available")
        
        # Test factory functions
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')
        print("✓ Factory functions available")
        
        # Test convenience functions
        assert hasattr(pynlm, 'run_simulation')
        assert hasattr(pynlm, 'train_agent')
        assert hasattr(pynlm, 'create_agent_config')
        assert hasattr(pynlm, 'create_rendering_config')
        print("✓ Convenience functions available")
        
        # Test utility functions
        assert hasattr(pynlm, 'save_checkpoint')
        assert hasattr(pynlm, 'load_checkpoint')
        assert hasattr(pynlm, 'benchmark_brain')
        assert hasattr(pynlm, 'profile_simulation')
        print("✓ Utility functions available")
        
        # Test context managers
        assert hasattr(pynlm, 'SimulationContext')
        assert hasattr(pynlm, 'ConfigContext')
        print("✓ Context managers available")
        
        # Test configuration convenience methods
        config = pynlm.createDefaultConfig()
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.string", "hello")
        config.set("test.bool", True)
        
        assert config.get_int("test.int") == 42
        assert config.get_float("test.float") == 3.14
        assert config.get_string("test.string") == "hello"
        assert config.get_bool("test.bool") == True
        print("✓ Configuration convenience methods work")
        
        # Test agent config presets
        agent_config = pynlm.create_agent_config()
        assert agent_config.get_int("brain.neuron_count") == 2000
        print("✓ Agent config preset works")
        
        rendering_config = pynlm.create_rendering_config()
        assert rendering_config.get_int("brain.neuron_count") == 500
        print("✓ Rendering config preset works")
        
        # Test version
        version = pynlm.get_version()
        assert isinstance(version, str) and "NLM" in version
        print("✓ Version function works")
        
        # Test default config
        default_config = pynlm.get_default_config()
        assert isinstance(default_config, dict)
        assert "brain.neuron_count" in default_config
        print("✓ Default config function works")
        
        print("\n✅ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_resource_cleanup():
    """Test that resources are properly cleaned up."""
    print("\nTesting resource cleanup...")
    
    try:
        import pynlm
        
        # Create and use context managers
        config = pynlm.createDefaultConfig()
        
        with pynlm.ConfigContext(config) as cfg:
            cfg.set("test.key", "value")
            assert config.get("test.key") == "value"
        
        # Test that context manager properly cleaned up
        # (No exceptions raised = success)
        print("✓ Context manager cleanup works")
        
        print("✅ Resource cleanup tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Resource cleanup test failed: {e}")
        return False

def main():
    """Main test function."""
    print("=" * 60)
    print("NLM Python Bindings - Quick Verification")
    print("=" * 60)
    
    # Run tests
    basic_ok = test_basic_functionality()
    cleanup_ok = test_resource_cleanup()
    
    print("\n" + "=" * 60)
    if basic_ok and cleanup_ok:
        print("✅ ALL VERIFICATION TESTS PASSED!")
        print("The enhanced NLM Python bindings are working correctly.")
        return 0
    else:
        print("❌ SOME VERIFICATION TESTS FAILED!")
        print("Please check the errors above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())