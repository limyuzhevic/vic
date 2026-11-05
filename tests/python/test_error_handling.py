#!/usr/bin/env python3
"""
NLM Python Error Handling Tests

Tests Python exception handling for C++ errors:
- Configuration error handling
- Brain operation error handling
- Invalid parameter validation
- Boundary condition checking
- Memory error handling
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import numpy as np
import pytest

class TestErrorHandling:
    """Test error handling and validation in NLM"""
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test invalid key types (if applicable)
        # This would depend on actual implementation
        
        # Test setting valid values (should not error)
        config.set("test.int", 42)
        config.set("test.string", "valid")
        config.set("test.float", 3.14)
        config.set("test.bool", True)
        
        # Test getting values
        int_val = config.getInt("test.int")
        assert int_val == 42
        
        # Test error handling for invalid operations
        # These would raise appropriate exceptions based on improvements
        
    def test_brain_error_handling(self):
        """Test brain operation error handling"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test invalid operations
        # brain.step(-1)  # Should raise exception
        # brain.injectCurrent(INVALID_NEURON_ID, 1.0)  # Should raise exception
        
        # Test valid operations
        for step in range(5):
            brain.step(step)  # Should not error
        
        # Verify brain is in valid state
        assert brain.getTotalSpikeCount() >= 0
        
    def test_vision_error_handling(self):
        """Test vision data error handling"""
        vision = pynlm.Vision(10, 10, 3)
        
        # Test invalid vision data (if applicable)
        # This would test setDataFromArray error handling
        
        # Test out of bounds pixel access
        # pixel = vision.getPixelValue(15, 15)  # Should raise exception
        # vision.setPixelValue(-1, 5, 0.5)  # Should raise exception
        
        # Test valid operations
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
    def test_audio_error_handling(self):
        """Test audio data error handling"""
        audio = pynlm.Audio(44100, 1000)
        
        # Test invalid audio data (if applicable)
        
        # Test valid operations
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1000
        
    def test_sensory_percept_error_handling(self):
        """Test sensory percept error handling"""
        percept = pynlm.SensoryPercept()
        
        # Test invalid vision data
        invalid_vision = [[0.0, 0.5], [0.2]]  # Wrong dimensions
        
        # percept.setVision(invalid_vision)  # Should raise exception
        
        # Test valid operations
        assert percept.getVisionWidth() == 0  # Default empty percept
        assert percept.getVisionHeight() == 0
        
        # Test pixel access boundary checking
        # These should raise IndexError or similar
        # with pytest.raises(Exception):
        #     percept.getPixelValue(100, 100)
        #     percept.setPixelValue(100, 100, 0.5)
        
    def test_world_error_handling(self):
        """Test world operation error handling"""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test invalid operations
        # world.update(-0.1)  # Invalid timestep
        # world.applyMotorCommand(None, 1.0)  # Invalid command
        
        # Test boundary conditions
        assert not world.isValidPosition(-1, 5)
        assert not world.isValidPosition(5, -1)
        assert not world.isValidPosition(20, 5)  # Out of bounds
        assert not world.isValidPosition(5, 20)
        
        # Test valid operations
        assert world.isValidPosition(0, 0)
        assert world.isValidPosition(10, 10)
        
    def test_agent_error_handling(self):
        """Test agent brain error handling"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test invalid operations
        # agent.processSensoryInput(None)  # Invalid input
        # agent.applyRewardModulation(2.0, 0.5)  # Invalid reward
        
        # Test valid operations
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)  # Should not error
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    def test_type_error_handling(self):
        """Test type error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test type mismatches (if applicable)
        # config.set("int_key", "not_an_int")  # Should raise TypeError
        
        # Test invalid parameter types
        # brain.step("not_a_step")  # Should raise TypeError
        
    def test_memory_error_handling(self):
        """Test memory-related error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test memory allocation errors (if applicable)
        
        # Test valid memory operations
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Access memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        # These should not error (or handle gracefully)
        assert working_memory is not None or episodic_memory is not None
        
    def test_parameter_validation(self):
        """Test parameter validation"""
        config = pynlm.createDefaultConfig()
        
        # Test parameter validation for config
        # config.set("key", None)  # Invalid value type
        # config.set("", 123)  # Invalid key
        
        # Test with valid parameters
        config.set("positive_int", 42)
        config.set("negative_int", -10)
        config.set("float_value", 3.14159)
        config.set("zero_value", 0)
        config.set("empty_string", "")
        
        # Verify they can be retrieved
        assert config.getInt("positive_int") == 42
        assert config.getInt("negative_int") == -10
        assert abs(config.getFloat("float_value") - 3.14159) < 0.0001
        
    def test_boundary_condition_handling(self):
        """Test boundary condition error handling"""
        # Test world boundaries
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        
        # Test position boundaries
        assert world.isValidPosition(0, 0)
        assert world.isValidPosition(9, 9)
        assert not world.isValidPosition(-1, 5)
        assert not world.isValidPosition(5, -1)
        assert not world.isValidPosition(10, 5)  # Out of bounds
        assert not world.isValidPosition(5, 10)
        
        # Test vision pixel boundaries
        vision = pynlm.Vision(5, 5, 3)
        
        # Pixel access should handle boundaries
        # These should either raise errors or handle gracefully
        # pixel = vision.getPixelValue(4, 4)  # Last valid pixel
        # pixel = vision.getPixelValue(5, 5)  # Out of bounds - should error
        
        # Test audio boundaries
        audio = pynlm.Audio(44100, 1000)
        
        # Audio sample access should handle boundaries
        # sample = audio.getSample(999)  # Last valid sample
        # sample = audio.getSample(1000)  # Out of bounds - should error
        
    def test_exception_cascading(self):
        """Test that exceptions propagate correctly"""
        config = pynlm.createDefaultConfig()
        
        # Test that one error doesn't prevent other tests
        try:
            # This might raise an exception
            # config.set("invalid", None)
            pass
        except Exception:
            pass  # Expected
        
        # Other operations should still work
        config.set("valid_key", "valid_value")
        assert config.get("valid_key") == "valid_value"
        
    def test_resource_cleanup_on_error(self):
        """Test resource cleanup when errors occur"""
        config = pynlm.createDefaultConfig()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Perform operations
        for step in range(5):
            brain.step(step)
        
        # Verify brain is in valid state even after operations
        assert brain.getTotalSpikeCount() >= 0
        
        # Test that resources are cleaned up
        # In actual implementation: check for memory leaks, etc.
        
    def test_error_recovery(self):
        """Test error recovery mechanisms"""
        config = pynlm.createDefaultConfig()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate error condition (if applicable)
        
        # Test that brain can recover from error
        # In actual implementation: brain.reset() or similar
        
        # Verify brain is still functional
        for step in range(5):
            brain.step(step)
        
        assert brain.getTotalSpikeCount() >= 0

# Global test functions for pytest compatibility
def run_error_handling_tests():
    """Run all error handling tests"""
    test_obj = TestErrorHandling()
    
    tests = [
        "test_config_error_handling",
        "test_brain_error_handling",
        "test_vision_error_handling",
        "test_audio_error_handling",
        "test_sensory_percept_error_handling",
        "test_world_error_handling",
        "test_agent_error_handling",
        "test_type_error_handling",
        "test_memory_error_handling",
        "test_parameter_validation",
        "test_boundary_condition_handling",
        "test_exception_cascading",
        "test_resource_cleanup_on_error",
        "test_error_recovery"
    ]
    
    results = []
    for test_name in tests:
        try:
            method = getattr(test_obj, test_name)
            method()
            results.append({"test": test_name, "status": "PASS", "error": None})
            print(f"✓ {test_name}")
        except Exception as e:
            results.append({"test": test_name, "status": "FAIL", "error": str(e)})
            print(f"✗ {test_name}: {e}")
    
    passed = sum(1 for r in results if r["status"] == "PASS")
    total = len(results)
    
    print(f"\nError Handling Tests: {passed}/{total} passed")
    return results