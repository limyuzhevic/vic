#!/usr/bin/env python3
"""
Comprehensive Test Suite for Improved NLM Python Bindings

This test suite validates all the improvements made to the NLM Python bindings:
1. Type hints and comprehensive documentation
2. Error handling and exception support
3. Pythonic interfaces (properties)
4. Enhanced action and world object types
5. New factory functions and utilities
"""

import pytest
import sys
import os
import tempfile
import json

# Import the pynlm module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))
import pynlm

class TestNLMTypeHints:
    """Test type hints and basic functionality"""
    
    def test_config_creation_and_access(self):
        """Test Config class creation and value access"""
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        config.set("test_string", "hello")
        
        # Test value retrieval
        assert config.has("test_int")
        assert not config.has("nonexistent_key")
        
        # Test Pythonic getter methods
        assert config.getInt("test_int", 0) == 42
        assert config.getFloat("test_float", 0.0) == 3.14
        assert config.getBool("test_bool", False) is True
        assert config.getString("test_string", "") == "hello"
        
    def test_config_file_operations(self):
        """Test configuration file operations"""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.set("learning_rate", 0.01)
        config.set("batch_size", 32)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        try:
            # Load from file
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(temp_file)
            
            # Verify values were saved and loaded correctly
            assert config2.get("learning_rate", 0.0) == 0.01
            assert config2.get("batch_size", 0) == 32
        finally:
            os.unlink(temp_file)
    
    def test_world_object_types(self):
        """Test extended world object types"""
        # Test new world object types
        objects = [
            pynlm.WorldObject(0, 0, pynlm.WorldObjectType.Food, 10.0),
            pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Water, 5.0),
            pynlm.WorldObject(20, 20, pynlm.WorldObjectType.Shelter, 20.0),
            pynlm.WorldObject(30, 30, pynlm.WorldObjectType.Weapon, 15.0),
        ]
        
        assert len(objects) == 4
        assert objects[0].type == pynlm.WorldObjectType.Food
        assert objects[1].type == pynlm.WorldObjectType.Water
        assert objects[2].type == pynlm.WorldObjectType.Shelter
        assert objects[3].type == pynlm.WorldObjectType.Weapon
        
        # Test object properties
        assert objects[0].value == 10.0
        assert objects[0].active is True
        assert objects[0].radius == 0.5  # default radius

class TestActionTypes:
    """Test enhanced action types"""
    
    def test_action_creation(self):
        """Test action creation with new types"""
        actions = [
            pynlm.Action(pynlm.ActionType.MoveForward),
            pynlm.Action(pynlm.ActionType.Grab, [1.0, 0.5]),
            pynlm.Action(pynlm.ActionType.Eat, [0.8]),
            pynlm.Action(pynlm.ActionType.Search, []),
            pynlm.Action(pynlm.ActionType.Defend, [0.5, 0.3]),
        ]
        
        assert len(actions) == 5
        assert actions[0].getType() == pynlm.ActionType.MoveForward
        assert actions[1].getType() == pynlm.ActionType.Grab
        assert actions[2].getType() == pynlm.ActionType.Eat
        
        # Test action parameters
        assert actions[1].getParameters() == [1.0, 0.5]
        assert actions[3].getParameters() == []
    
    def test_action_properties(self):
        """Test action properties and methods"""
        action = pynlm.Action(pynlm.ActionType.Interact, [0.5, 0.5])
        
        # Test property access
        assert action.getType() == pynlm.ActionType.Interact
        assert action.getParameters() == [0.5, 0.5]
        assert "Interact" in action.getName()
        
        # Test cloning
        action2 = action.clone()
        assert action2.getType() == action.getType()
        assert action2.getParameters() == action.getParameters()
        
        # Test modification
        action.setType(pynlm.ActionType.Defend)
        assert action.getType() == pynlm.ActionType.Defend

class TestSensoryInput:
    """Test sensory input classes"""
    
    def test_vision(self):
        """Test Vision sensory input"""
        vision = pynlm.Vision(10, 10, 3)
        vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6] * 20)  # 10*10*3 = 300 values
        
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        assert len(vision.getData()) == 300
    
    def test_audio(self):
        """Test Audio sensory input"""
        audio = pynlm.Audio(44100, 44100)  # 1 second of audio
        audio.setData([0.1, -0.2, 0.3, -0.4] * 11025)
        
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 44100
        assert len(audio.getData()) == 44100

class TestErrorHandling:
    """Test error handling and exception support"""
    
    def test_condition_check(self):
        """Test condition checking functionality"""
        config = pynlm.createDefaultConfig()
        
        # This should not raise an error
        pynlm.check_condition(config.has("nonexistent_key") == False, 
                            "Key should not exist")
        
        # This should raise an error
        with pytest.raises(Exception) as exc_info:
            pynlm.check_condition(
                config.has("nonexistent_key"),
                "Key should exist but doesn't"
            )
        
        assert "Key should exist" in str(exc_info.value)
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        
        # Test successful validation
        pynlm.check_condition(config.get("nonexistent_key", None) is None,
                            "Validation should pass")

class TestPythonicInterfaces:
    """Test Pythonic property-style interfaces"""
    
    def test_agent_body_properties(self):
        """Test AgentBody property access"""
        agent = pynlm.AgentBody()
        
        # Test property-style access
        agent.x = 5.0
        agent.y = 3.0
        agent.orientation = 1.57
        agent.velocityX = 0.5
        agent.velocityY = 0.0
        agent.energy = 75.0
        agent.health = 100.0
        agent.age = 10
        
        assert agent.x == 5.0
        assert agent.y == 3.0
        assert agent.orientation == 1.57
        assert agent.velocityX == 0.5
        assert agent.velocityY == 0.0
        assert agent.energy == 75.0
        assert agent.health == 100.0
        assert agent.age == 10
        
        # Test reset method
        agent.reset()
        assert agent.energy == 100.0  # Should be reset to full
        assert agent.health == 100.0  # Should be reset to full
    
    def test_world_object_properties(self):
        """Test WorldObject property access"""
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 20.0)
        
        # Test property-style access
        obj.x = 10.0
        obj.y = 10.0
        obj.radius = 1.0
        obj.value = 30.0
        obj.active = False
        
        assert obj.x == 10.0
        assert obj.y == 10.0
        assert obj.radius == 1.0
        assert obj.value == 30.0
        assert obj.active is False

class TestExampleScripts:
    """Test example scripts"""
    
    def test_quickstart_example(self):
        """Test that quickstart.py can be imported and run basic tests"""
        # Import the quickstart module (without running it)
        import importlib.util
        spec = importlib.util.spec_from_file_location("quickstart", "quickstart.py")
        assert spec is not None
        
        # Test that the module has the expected functions
        assert hasattr(spec, 'createBrain')
        assert hasattr(spec, 'createDefaultConfig')
    
    def test_advanced_usage_structure(self):
        """Test that advanced_usage.py has the expected structure"""
        import importlib.util
        spec = importlib.util.spec_from_file_location("advanced_usage", "advanced_usage.py")
        assert spec is not None

class TestEnumFunctionality:
    """Test enumeration functionality"""
    
    def test_action_type_enum(self):
        """Test ActionType enumeration"""
        action_types = list(pynlm.ActionType)
        
        # Check that important action types exist
        assert pynlm.ActionType.MoveForward in action_types
        assert pynlm.ActionType.Look in action_types
        assert pynlm.ActionType.Eat in action_types
        assert pynlm.ActionType.Custom in action_types
        
        # Test iteration
        for action_type in pynlm.ActionType:
            assert hasattr(action_type, 'name')
            assert isinstance(action_type.name, str)
    
    def test_world_object_type_enum(self):
        """Test WorldObjectType enumeration"""
        object_types = list(pynlm.WorldObjectType)
        
        # Check that important world object types exist
        assert pynlm.WorldObjectType.Empty in object_types
        assert pynlm.WorldObjectType.Resource in object_types
        assert pynlm.WorldObjectType.Hazard in object_types
        assert pynlm.WorldObjectType.Wall in object_types
        assert pynlm.WorldObjectType.Food in object_types
        assert pynlm.WorldObjectType.Shelter in object_types
class TestIntegration:
    """Test integration scenarios"""
    
    def test_complete_agent_world_loop(self):
        """Test a complete agent-world interaction loop"""
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Add a resource
        resource = pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Food, 10.0)
        world.addObject(resource)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a few simulation steps
        for step in range(10):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify system is still working
        assert brain.getTotalNeuronCount() > 0
        assert world.getWidth() == 20
        assert world.getHeight() == 20

class TestDocumentation:
    """Test that documentation is properly integrated"""
    
    def test_class_docstrings(self):
        """Test that key classes have docstrings"""
        assert pynlm.Config.__doc__ is not None
        assert pynlm.Brain.__doc__ is not None
        assert pynlm.AgentBrain.__doc__ is not None
        assert pynlm.SimpleWorld.__doc__ is not None
        assert pynlm.Action.__doc__ is not None
        assert pynlm.WorldObject.__doc__ is not None
    
    def test_module_docstring(self):
        """Test that the module has a docstring"""
        assert pynlm.__doc__ is not None
        assert "NLM" in pynlm.__doc__
        assert "Python Bindings" in pynlm.__doc__

if __name__ == "__main__":
    # Run all tests
    test_classes = [
        TestNLMTypeHints,
        TestActionTypes,
        TestSensoryInput,
        TestErrorHandling,
        TestPythonicInterfaces,
        TestExampleScripts,
        TestEnumFunctionality,
        TestIntegration,
        TestDocumentation,
    ]
    
    print("🧪 Running NLM Python Bindings Test Suite")
    print("=" * 50)
    
    total_tests = 0
    passed_tests = 0
    
    for test_class in test_classes:
        print(f"\n📋 Running {test_class.__name__}...")
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) if method.startswith('test_')]
        
        for test_method in test_methods:
            total_tests += 1
            try:
                getattr(test_instance, test_method)()
                passed_tests += 1
                print(f"  ✓ {test_method}")
            except Exception as e:
                print(f"  ✗ {test_method}: {e}")
    
    print("\n" + "=" * 50)
    print(f"📊 Test Results: {passed_tests}/{total_tests} tests passed")
    
    if passed_tests == total_tests:
        print("🎉 All tests passed!")
        sys.exit(0)
    else:
        print(f"❌ {total_tests - passed_tests} tests failed")
        sys.exit(1)