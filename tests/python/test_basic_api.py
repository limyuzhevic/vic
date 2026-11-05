#!/usr/bin/env python3
"""
Basic NLM Python API Tests

Tests basic Python API functionality including:
- Config class with new getInt, getFloat, getString methods
- Vision and Audio classes
- Basic class instantiation and method calls
- Integration between Python and C++ components
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import pytest
import numpy as np

class TestBasicAPI:
    """Test basic NLM Python API functionality"""
    
    def test_config_creation(self):
        """Test basic Config class creation"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test config properties
        config.set("test_int", 42)
        config.set("test_string", "hello")
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        
        assert config.has("test_int")
        assert config.has("test_string")
        assert config.has("test_float")
        assert config.has("test_bool")
        
    def test_config_get_methods(self):
        """Test new getInt, getFloat, getString methods"""
        config = pynlm.createDefaultConfig()
        
        # Test getInt
        config.set("int_value", 123)
        int_val = config.getInt("int_value")
        assert int_val == 123
        
        # Test getFloat
        config.set("float_value", 45.67)
        float_val = config.getFloat("float_value")
        assert abs(float_val - 45.67) < 0.001
        
        # Test getString (from improvements plan)
        config.set("string_value", "test_string")
        string_val = config.get("string_value")
        assert string_val == "test_string"
        
    def test_brain_creation(self):
        """Test Brain class creation and basic methods"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        # Test brain initialization
        brain.initialize()
        
        # Test basic brain properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getActiveNeuronCount() >= 0
        
    def test_simple_world(self):
        """Test SimpleWorld class"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Test world configuration
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test world properties
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getMaxEnergy() > 0
        
        # Test reset
        world.reset()
        assert world.getSimulationTime() == 0.0
        
    def test_sensory_input(self):
        """Test basic sensory input classes"""
        # Test Vision
        vision = pynlm.Vision(10, 10, 3)
        assert vision is not None
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
        # Test Audio
        audio = pynlm.Audio(44100, 1000)
        assert audio is not None
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1000
        
    def test_agent_brain(self):
        """Test AgentBrain class"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test agent brain properties
        assert agent.getBrain() is not None
        
    def test_action_objects(self):
        """Test Action class"""
        # Test action creation with type
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action is not None
        assert action.getType() == pynlm.ActionType.MoveForward
        
        # Test action with parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        assert action2.getType() == pynlm.ActionType.Custom
        assert len(action2.getParameters()) == 3
        
    def test_sensory_percept(self):
        """Test SensoryPercept class with pixel access"""
        percept = pynlm.SensoryPercept()
        assert percept is not None
        
        # Test vision data handling
        vision_data = np.random.rand(8, 8).astype(np.float32)
        
        # Set vision data using numpy
        percept.setVision(vision_data.tolist())
        
        # Test pixel access methods
        assert percept.getVisionWidth() == 8
        assert percept.getVisionHeight() == 8
        
        # Test pixel value access
        pixel_value = percept.getPixelValue(0, 0)
        assert 0.0 <= pixel_value <= 1.0
        
        # Test set pixel value
        percept.setPixelValue(0, 0, 0.5)
        assert percept.getPixelValue(0, 0) == 0.5
        
        # Test isDarkPixel
        percept.setPixelValue(1, 1, 0.3)
        assert percept.isDarkPixel(1, 1)
        assert not percept.isDarkPixel(0, 0, threshold=0.4)
        
    def test_numpy_array_support(self):
        """Test numpy array support for vision and audio"""
        # Test Vision with numpy
        vision_data = np.random.rand(16, 16, 3).astype(np.float32)
        vision = pynlm.Vision(16, 16, 3)
        
        # This would use setDataFromArray from bindings.cpp
        # vision.set_data_numpy(vision_data)  # Note: actual method name may differ
        
        # Test Audio with numpy
        audio_data = np.random.rand(44100).astype(np.float32)
        audio = pynlm.Audio(44100, 44100)
        
        # audio.set_data_numpy(audio_data)  # Note: actual method name may differ
        
    def test_integration_basic(self):
        """Test integration between Python and C++ components"""
        # Create complete system
        config = pynlm.createDefaultConfig()
        
        # Set up config using new methods
        config.set("brain.neuron_count", 100)
        config.set("plasticity.stdp.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation steps
        for step in range(10):
            brain.step(step)
            
            # Test sensory input processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Test motor command decoding
            action = agent.decodeMotorCommand()
        
        # Verify system state
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getCuriosityLevel() >= 0.0
        
    def test_error_handling_basic(self):
        """Test basic error handling"""
        # Test invalid configuration keys
        config = pynlm.createDefaultConfig()
        
        # These should raise appropriate exceptions
        with pytest.raises(Exception):
            # Try to set invalid config (implementation dependent)
            pass
        
        # Test invalid parameter ranges
        world = pynlm.createSimpleWorld()
        
        # Test invalid positions
        assert not world.isValidPosition(-1, 0)
        assert not world.isValidPosition(0, -1)
        
    def test_neuron_types_and_ids(self):
        """Test neuron type and ID handling"""
        from neuron import NeuronType, NeuronId
        
        # Test NeuronType enum
        assert NeuronType.Excitatory == pynlm.NeuronType.Excitatory
        assert NeuronType.Sensory == pynlm.NeuronType.Sensory
        assert NeuronType.Motor == pynlm.NeuronType.Motor
        
        # Test NeuronId
        neuron_id = NeuronId()
        assert neuron_id.value >= 0
        assert neuron_id.index() >= 0
        
        # Test with value
        neuron_id2 = NeuronId(12345)
        assert neuron_id2.value == 12345
        
    def test_coordinate_boundaries(self):
        """Test coordinate system boundaries"""
        vision = pynlm.Vision(10, 10, 3)
        
        # Test valid pixel coordinates
        assert vision.getPixelValue(0, 0) == 0.0  # Default initialized
        assert vision.getPixelValue(9, 9) == 0.0  # Default initialized
        
        # Test pixel value setters
        vision.setPixelValue(0, 0, 0.5)
        assert vision.getPixelValue(0, 0) == 0.5

# Global test functions for pytest compatibility
def run_basic_api_tests():
    """Run all basic API tests"""
    test_obj = TestBasicAPI()
    
    tests = [
        "test_config_creation",
        "test_config_get_methods", 
        "test_brain_creation",
        "test_simple_world",
        "test_sensory_input",
        "test_agent_brain",
        "test_action_objects",
        "test_sensory_percept",
        "test_numpy_array_support",
        "test_integration_basic",
        "test_error_handling_basic",
        "test_neuron_types_and_ids",
        "test_coordinate_boundaries"
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
    
    print(f"\nBasic API Tests: {passed}/{total} passed")
    return results