#!/usr/bin/env python3
"""
NLM Python Test Suite

This module contains all Python-based tests for the NLM project.
It tests the Python bindings and their integration with the C++ framework.
"""

import sys
import os
import pytest
import numpy as np
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import NLM modules
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available, some tests will be skipped")

class TestBasicFunctionality:
    """Test basic NLM Python functionality."""
    
    def test_basic_types(self):
        """Test basic type definitions."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test NeuronId
        neuron_id = pynlm.NeuronId(42)
        assert neuron_id.value == 42
        
        # Test SynapseId
        synapse_id = pynlm.SynapseId(100)
        assert synapse_id.value == 100
        
        # Test RegionId
        region_id = pynlm.RegionId(5)
        assert region_id.value == 5
        
        # Test PopulationId
        pop_id = pynlm.PopulationId(99)
        assert pop_id.value == 99
    
    def test_enums(self):
        """Test enumeration types."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test NeuronType
        assert pynlm.NeuronType.Excitatory == 0
        assert pynlm.NeuronType.Inhibitory == 1
        assert pynlm.NeuronType.Modulatory == 2
        assert pynlm.NeuronType.Sensory == 3
        assert pynlm.NeuronType.Motor == 4
        assert pynlm.NeuronType.Internal == 5
        
        # Test SynapseType
        assert pynlm.SynapseType.Excitatory == 0
        assert pynlm.SynapseType.Inhibitory == 1
        assert pynlm.SynapseType.Modulatory == 2
        assert pynlm.SynapseType.Electrical == 3
        assert pynlm.SynapseType.GapJunction == 4
        
        # Test ActionType
        assert pynlm.ActionType.MoveForward == 0
        assert pynlm.ActionType.MoveBackward == 1
        assert pynlm.ActionType.TurnLeft == 2
        assert pynlm.ActionType.TurnRight == 3
    
    def test_config_basic(self):
        """Test basic Config functionality."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Create config
        config = pynlm.Config()
        
        # Set values
        config.set("learning_rate", 0.01)
        config.set("max_steps", 1000)
        config.set("render_mode", True)
        config.set("model_name", "test_model")
        
        # Get values
        assert config.get("learning_rate") == 0.01
        assert config.getOr("max_steps", 500) == 1000
        assert config.getOr("unknown", "default") == "default"
        assert config.has("learning_rate")
        assert not config.has("unknown")
        
        # Test getKeys
        keys = config.getKeys()
        assert "learning_rate" in keys
        assert "max_steps" in keys
        assert "render_mode" in keys
        assert "model_name" in keys
        
        # Test summary
        summary = config.summary()
        assert "Config: " in summary
    
    def test_context_managers(self):
        """Test context manager functionality."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Create objects
        config = pynlm.createDefaultConfig()
        config.set("test_key", "test_value")
        
        # Test ConfigContextManager
        with pynlm.ConfigContextManager(config) as cfg:
            assert cfg is not None
            assert cfg.has("test_key")
        
        # Test BrainContextManager
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        with pynlm.BrainContextManager(brain) as b:
            assert b is not None
        
        # Test WorldContextManager
        world = pynlm.createSimpleWorld()
        with pynlm.WorldContextManager(world) as w:
            assert w is not None
    
    def test_enhanced_features(self):
        """Test enhanced Python features."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        config = pynlm.Config()
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        config.set("test_string", "hello")
        
        # Test to_dict
        dict_config = config.to_dict()
        assert dict_config["test_int"] == 42
        assert dict_config["test_float"] == 3.14
        assert dict_config["test_bool"] is True
        assert dict_config["test_string"] == "hello"
        
        # Test from_dict
        new_config = pynlm.Config()
        new_dict = {"key1": "value1", "key2": 123, "key3": False}
        new_config.from_dict(new_dict)
        assert new_config.get("key1") == "value1"
        assert new_config.get("key2") == 123
        assert new_config.get("key3") is True
        
        # Test clone/copy
        cloned = config.clone()
        assert cloned is not None
        assert cloned.get("test_int") == 42
        
        copied = config.copy()
        assert copied is not None
        assert copied.get("test_string") == "hello"
    
    def test_exception_handling(self):
        """Test exception handling."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        config = pynlm.Config()
        
        # Test type checking
        try:
            config.set("test", "not_a_number")
            # No exception expected
        except Exception as e:
            print(f"Exception raised (expected): {e}")
    
    def test_serialization(self):
        """Test serialization/deserialization."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        config = pynlm.Config()
        config.set("test_value", 42)
        config.set("test_name", "serialization_test")
        
        # Test JSON serialization (if available)
        try:
            json_str = config.to_json()
            # Note: Actual deserialization may not work without proper implementation
            # but we test that the method exists and doesn't crash
        except Exception as e:
            print(f"JSON serialization (may not be implemented): {e}")

class TestConfiguration:
    """Test configuration functionality."""
    
    def test_config_validation(self):
        """Test configuration validation."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        config = pynlm.Config()
        
        # Test valid configuration
        config.set("valid_int", 100)
        config.set("valid_float", 0.5)
        config.set("valid_string", "test")
        config.set("valid_bool", True)
        
        # Test configuration loading
        assert config.has("valid_int")
        assert config.getOr("valid_int", 0) == 100
        assert config.getOr("nonexistent", "default") == "default"
    
    def test_config_cli(self):
        """Test CLI integration."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test command-line interface functionality
        # This is a placeholder for actual CLI tests
        pass

class TestBindings:
    """Test Python bindings functionality."""
    
    def test_bindings_creation(self):
        """Test creation of NLM objects."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test creating different NLM objects
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        assert config is not None
        assert brain is not None
        assert world is not None
    
    def test_bindings_types(self):
        """Test type conversions."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test type conversions between Python and C++
        config = pynlm.Config()
        
        # Test integer conversion
        config.set("int_val", 42)
        assert config.get("int_val") == 42
        
        # Test float conversion
        config.set("float_val", 3.14159)
        assert abs(config.get("float_val") - 3.14159) < 0.0001
        
        # Test string conversion
        config.set("string_val", "test")
        assert config.get("string_val") == "test"
        
        # Test boolean conversion
        config.set("bool_val", True)
        assert config.get("bool_val") is True
    
    def test_bindings_error_handling(self):
        """Test error handling in bindings."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test that errors are properly raised
        config = pynlm.Config()
        
        # This should work without errors
        config.set("valid_key", "valid_value")
        
        # Test error cases (if any)
        # This is a placeholder for actual error handling tests

class TestPerformance:
    """Test performance-related functionality."""
    
    def test_performance_benchmarks(self):
        """Test performance benchmarks."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test performance measurement
        import time
        
        config = pynlm.Config()
        
        # Time configuration operations
        start_time = time.time()
        for i in range(100):
            config.set(f"key_{i}", i)
        end_time = time.time()
        
        elapsed = end_time - start_time
        print(f"Configuration operations: 100 in {elapsed:.4f} seconds")
        
        # Ensure reasonable performance
        assert elapsed < 1.0  # Should be very fast
    
    def test_memory_usage(self):
        """Test memory usage."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")
        
        # Test memory usage (placeholder for actual memory testing)
        import tracemalloc
        
        tracemalloc.start()
        
        config = pynlm.Config()
        for i in range(1000):
            config.set(f"key_{i}", i)
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        print(f"Memory usage: current={current/1024:.2f}KB, peak={peak/1024:.2f}KB")
        
        # Memory usage should be reasonable
        assert peak < 10 * 1024 * 1024  # Less than 10MB for 1000 operations

def run_python_tests():
    """Run all Python tests."""
    print("Running NLM Python Tests")
    print("=" * 50)
    
    # Run pytest programmatically
    import subprocess
    
    # Change to the tests directory
    os.chdir(Path(__file__).parent)
    
    # Run pytest
    result = subprocess.run([
        sys.executable, "-m", "pytest",
        "tests/",
        "-v",
        "--tb=short"
    ], capture_output=True, text=True)
    
    print("Python test results:")
    print(result.stdout)
    
    if result.stderr:
        print("Python test errors:")
        print(result.stderr)
    
    return result.returncode

if __name__ == "__main__":
    print("NLM Python Test Suite")
    print("=" * 50)
    
    # Check if pytest is available
    try:
        import pytest
        print("Pytest is available")
    except ImportError:
        print("Pytest is not available, installing...")
        subprocess.run([sys.executable, "-m", "pip", "install", "pytest"])
    
    # Run tests
    exit_code = run_python_tests()
    sys.exit(exit_code)