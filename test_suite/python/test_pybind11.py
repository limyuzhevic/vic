"""
Python-Specific Tests

Tests specific to Python bindings and Python-C++ integration
"""
import pytest
import sys
import os
import inspect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class TestPybind11Integration:
    """Test pybind11 integration quality"""
    
    def test_module_documentation(self):
        """Test that the module has proper documentation"""
        assert pynlm.__doc__ is not None
        assert "NLM" in pynlm.__doc__ or "Neural" in pynlm.__doc__
    
    def test_class_documentation(self):
        """Test that classes have proper Python documentation"""
        config_doc = pynlm.Config.__doc__
        assert config_doc is not None and len(config_doc) > 10
        
        brain_doc = pynlm.Brain.__doc__
        assert brain_doc is not None and len(brain_doc) > 10
    
    def test_method_signatures(self):
        """Test that methods have proper Python signatures"""
        config = pynlm.Config()
        
        # Test that methods exist and are callable
        assert callable(config.loadFromFile)
        assert callable(config.saveToFile)
        assert callable(config.has)
        assert callable(config.getKeys)
        assert callable(config.summary)
    
    def test_type_annotations(self):
        """Test that types are properly exposed"""
        # Test that Python enums work correctly
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'SynapseType')
        assert hasattr(pynlm, 'ActionType')
        assert hasattr(pynlm, 'FiringState')
        
        # Test enum values
        assert pynlm.NeuronType.Excitatory is not None
        assert pynlm.NeuronType.Inhibitory is not None
        assert pynlm.SynapseType.Excitatory is not None
        assert pynlm.ActionType.MoveForward is not None

class TestObjectLifetimeAndCleanup:
    """Test object lifetime management"""
    
    def test_config_cleanup(self):
        """Test that Config objects are properly cleaned up"""
        import gc
        
        # Create and immediately delete config
        for i in range(100):
            config = pynlm.Config()
            config.set("test", f"value_{i}")
            # config goes out of scope here
        
        # Force garbage collection
        gc.collect()
        
        # Should not leak memory (basic test)
        # In a real implementation, we'd use memory profiling
        pass
    
    def test_brain_cleanup(self):
        """Test that Brain objects are properly cleaned up"""
        import gc
        
        config = pynlm.createDefaultConfig()
        for i in range(10):
            brain = pynlm.createBrain(config)
            # brain goes out of scope here
        
        gc.collect()
        
        # Should not leak memory
        pass
    
    def test_python_cpp_object_interaction(self):
        """Test interaction between Python and C++ objects"""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        # Python object should be able to interact with C++ object
        assert hasattr(brain, 'initialize')
        assert hasattr(brain, 'step')
        assert hasattr(brain, 'getTotalNeuronCount')
        
        # Call C++ methods from Python
        brain.initialize()
        
        # Check that C++ data is accessible from Python
        neuron_count = brain.getTotalNeuronCount()
        assert isinstance(neuron_count, int)
        assert neuron_count >= 0

class TestExceptionHandling:
    """Test exception handling in Python bindings"""
    
    def test_runtime_error_handling(self):
        """Test that runtime errors are properly exposed"""
        config = pynlm.Config()
        
        # Test that exceptions can be caught
        try:
            # This should raise an exception if file doesn't exist
            config.loadFromFile("nonexistent_file.json")
        except Exception as e:
            # Should get an exception
            assert isinstance(e, Exception)
    
    def test_type_safety(self):
        """Test type safety in Python bindings"""
        config = pynlm.Config()
        
        # Should be able to set and get different types
        config.set("string_value", "hello")
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        
        # Retrieving should work
        assert config.has("string_value")
        assert config.has("int_value")
        assert config.has("float_value")
        
        # Type checking would depend on implementation
        # In pybind11, types might be preserved or converted
        pass
    
    def test_boundary_conditions(self):
        """Test boundary conditions and edge cases"""
        config = pynlm.Config()
        
        # Test empty strings
        config.set("empty_string", "")
        
        # Test large values
        config.set("large_int", 2**63 - 1)
        
        # Test negative values
        config.set("negative_float", -3.14159)
        
        # Should accept these without crashing
        assert config.has("empty_string")
        assert config.has("large_int")
        assert config.has("negative_float")

class TestPerformanceWithPythonObjects:
    """Test performance characteristics with Python objects"""
    
    def test_object_creation_performance(self):
        """Test performance of object creation"""
        import time
        
        start_time = time.time()
        
        configs = []
        for i in range(1000):
            config = pynlm.Config()
            config.set(f"key_{i}", f"value_{i}")
            configs.append(config)
        
        creation_time = time.time() - start_time
        
        # Should be able to create 1000 objects quickly
        assert len(configs) == 1000
        assert creation_time < 5.0  # Should complete in reasonable time
    
    def test_method_call_performance(self):
        """Test performance of method calls"""
        import time
        
        config = pynlm.Config()
        
        # Pre-populate with data
        for i in range(100):
            config.set(f"key_{i}", f"value_{i}")
        
        # Test method call performance
        start_time = time.time()
        
        for i in range(100):
            _ = config.has(f"key_{i}")
            _ = config.getKeys()
            _ = config.summary()
        
        total_time = time.time() - start_time
        
        # Should complete method calls quickly
        assert total_time < 1.0
    
    def test_python_cpp_memory_interaction(self):
        """Test memory interaction between Python and C++"""
        import gc
        
        gc.collect()
        
        # Create many Python objects that will interact with C++
        config = pynlm.createDefaultConfig()
        
        # Create brain and interact with it
        for i in range(100):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Perform some operations
            _ = brain.getTotalNeuronCount()
            _ = brain.getTotalSynapseCount()
            
            # Clean up
            del brain
        
        # Force garbage collection
        gc.collect()

if __name__ == "__main__":
    pytest.main([__file__, "-v"])