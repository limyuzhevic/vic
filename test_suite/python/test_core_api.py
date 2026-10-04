"""
Core Python API Tests

Tests for the Enhanced NLM Python API including:
- Basic functionality tests
- Performance tests
- Integration tests
"""
import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import the pynlm module
import pynlm

class TestCorePythonAPI:
    """Test core Python API functionality"""
    
    def test_imports(self):
        """Test that the module can be imported"""
        assert hasattr(pynlm, 'Config')
        assert hasattr(pynlm, 'Brain')
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
    
    def test_config_creation(self):
        """Test Config class creation and basic functionality"""
        config = pynlm.Config()
        assert config is not None
        
        # Test basic config operations
        assert not config.has("nonexistent_key")
        
        # Test get keys
        keys = config.getKeys()
        assert isinstance(keys, list)
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
    
    def test_simple_brain_creation(self):
        """Test Brain class creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        # Test brain properties
        assert brain.getRegionCount() == 0  # No regions added yet
        assert brain.getTotalNeuronCount() == 0
        assert brain.getTotalSynapseCount() == 0
    
    def test_vision_sensory(self):
        """Test Vision sensory input class"""
        vision = pynlm.Vision(100, 100, 3)
        assert vision.getWidth() == 100
        assert vision.getHeight() == 100
        assert vision.getChannels() == 3
        
        # Test data setting
        data = [0.0] * 30000  # 100x100x3
        vision.setData(data)
    
    def test_agent_body(self):
        """Test AgentBody class"""
        body = pynlm.AgentBody()
        assert hasattr(body, 'x')
        assert hasattr(body, 'y')
        assert hasattr(body, 'orientation')
        
        # Test properties
        body.x = 10.5
        body.y = 20.5
        body.orientation = 45.0
        assert body.x == 10.5
        assert body.y == 20.5
        assert body.orientation == 45.0
    
    def test_action_creation(self):
        """Test Action class"""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action.getType() == pynlm.ActionType.MoveForward
        assert action.getName() == "MoveForward"
        
        # Test action with parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        params = action2.getParameters()
        assert len(params) == 3
        assert params[0] == 1.0

class TestPerformanceTests:
    """Performance-related tests"""
    
    def test_memory_efficiency(self):
        """Test memory efficiency with object creation"""
        import sys
        
        # Create many objects to test memory
        objects = []
        for i in range(1000):
            config = pynlm.Config()
            objects.append(config)
        
        # Basic sanity check
        assert len(objects) == 1000
        
        # Clean up
        del objects
    
    def test_large_config_operations(self):
        """Test performance with large configuration"""
        config = pynlm.createDefaultConfig()
        
        # Add many configuration keys
        for i in range(1000):
            config.set(f"key_{i}", f"value_{i}")
        
        # Test that all keys exist
        for i in range(1000):
            assert config.has(f"key_{i}")
        
        keys = config.getKeys()
        assert len(keys) >= 1000  # At least 1000 plus default keys

class TestErrorHandling:
    """Test error handling and edge cases"""
    
    def test_invalid_sensory_input(self):
        """Test handling of invalid sensory input"""
        # This would test error handling for malformed inputs
        # Implementation depends on actual error handling in the bindings
        pass
    
    def test_null_object_handling(self):
        """Test handling of null/invalid objects"""
        # Test that invalid objects are handled gracefully
        pass

# Integration test class for C++ integration
class TestCPPIIntegration:
    """Tests for C++ integration"""
    
    def test_cpp_neuron_properties(self):
        """Test that neurons created in C++ have correct properties"""
        # This would test that Python objects interact correctly with C++ components
        pass
    
    def test_python_cpp_interoperability(self):
        """Test Python and C++ interoperability"""
        # Test that Python code can interact with C++ implementations
        pass

if __name__ == "__main__":
    pytest.main([__file__, "-v"])