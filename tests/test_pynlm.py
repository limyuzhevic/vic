"""
Python binding tests for NLM (Neural Learning Machine)

This module contains comprehensive tests for the Python bindings,
validating that the C++ functionality is properly exposed and works correctly.
"""

import pytest
import pynlm
import tempfile
import os
from pathlib import Path

def test_module_metadata():
    """Test that the module has proper metadata."""
    # Check version attribute
    assert hasattr(pynlm, '__version__')
    assert isinstance(pynlm.__version__, str)
    assert pynlm.__version__ == "0.1.0"
    
    # Check constants
    assert hasattr(pynlm, 'VERSION')
    assert pynlm.VERSION == "0.1.0"
    
    # Check module docstring
    assert pynlm.__doc__ is not None
    assert "NLM" in pynlm.__doc__
    assert "Neural Learning Machine" in pynlm.__doc__

def test_constants():
    """Test that constants are properly exposed."""
    # Check basic constants
    assert hasattr(pynlm, 'DEFAULT_WORLD_WIDTH')
    assert hasattr(pynlm, 'DEFAULT_WORLD_HEIGHT')
    assert hasattr(pynlm, 'DEFAULT_VISION_WIDTH')
    assert hasattr(pynlm, 'DEFAULT_VISION_HEIGHT')
    
    # Check neuron type constants
    assert hasattr(pynlm, 'EXCITATORY_NEURON')
    assert hasattr(pynlm, 'INHIBITORY_NEURON')
    assert hasattr(pynlm, 'SENSORY_NEURON')
    assert hasattr(pynlm, 'MOTOR_NEURON')
    
    # Check invalid IDs
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')

def test_id_classes():
    """Test NeuronId, SynapseId, RegionId, and PopulationId classes."""
    # Test NeuronId
    neuron_id = pynlm.NeuronId()
    assert neuron_id.value == 0
    
    neuron_id2 = pynlm.NeuronId(42)
    assert neuron_id2.value == 42
    assert neuron_id2.index() == 42
    
    # Test equality
    assert neuron_id == pynlm.NeuronId(0)
    assert neuron_id != neuron_id2
    
    # Test string representation
    assert str(neuron_id).startswith("NeuronId")
    assert str(neuron_id2).startswith("NeuronId")
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(100)
    assert synapse_id.value == 100
    assert synapse_id.index() == 100
    
    # Test RegionId
    region_id = pynlm.RegionId(200)
    assert region_id.value == 200
    assert region_id.index() == 200
    
    # Test PopulationId
    pop_id = pynlm.PopulationId(300)
    assert pop_id.value == 300
    assert pop_id.index() == 300

def test_enumerations():
    """Test enumeration types."""
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory.value == 0
    assert pynlm.NeuronType.Inhibitory.value == 1
    assert pynlm.NeuronType.Sensory.value == 4
    assert pynlm.NeuronType.Motor.value == 5
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory.value == 0
    assert pynlm.SynapseType.Inhibitory.value == 1
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward.value == 0
    assert pynlm.ActionType.TurnLeft.value == 5
    assert pynlm.ActionType.Interact.value == 11
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial.value == 0
    assert pynlm.DevelopmentalStage.CriticalPeriod.value == 1
    assert pynlm.DevelopmentalStage.Adult.value == 4

def test_config_class():
    """Test the Config class."""
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Test basic properties
    assert isinstance(config, pynlm.Config)
    assert "brain.neuron_count" in config.getKeys()
    
    # Test get/set methods
    assert config.get("brain.neuron_count") == "1000"
    config.set("brain.neuron_count", "2000")
    assert config.get("brain.neuron_count") == "2000"
    
    # Test integer getter
    neuron_count = config.getInt("brain.neuron_count")
    assert neuron_count == 2000
    
    # Test boolean getter
    plasticity_enabled = config.getBool("plasticity.stdp.enable")
    assert plasticity_enabled == True
    
    # Test string representation
    config_str = str(config)
    assert "Config" in config_str
    assert "2000" in config_str
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        
        # Load back
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(f.name)
        
        assert config2.get("brain.neuron_count") == "2000"
        
        os.unlink(f.name)

def test_action_class():
    """Test the Action class."""
    # Create action from type
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward
    assert action1.getName() == "MoveForward"
    
    # Create action with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8, 0.3])
    assert action2.getType() == pynlm.ActionType.Custom
    params = action2.getParameters()
    assert len(params) == 3
    assert params[0] == 0.5
    
    # Test cloning
    action3 = action2.clone()
    assert action3.getType() == action2.getType()
    assert action3.getParameters() == action2.getParameters()
    
    # Test modification
    action1.setType(pynlm.ActionType.TurnLeft)
    assert action1.getType() == pynlm.ActionType.TurnLeft

def test_world_object_class():
    """Test the WorldObject class."""
    # Create default
    obj1 = pynlm.WorldObject()
    assert obj1.x == 0.0
    assert obj1.y == 0.0
    assert obj1.type == pynlm.WorldObjectType.Empty
    assert obj1.active == False
    
    # Create with parameters
    obj2 = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 1.0)
    assert obj2.x == 10.0
    assert obj2.y == 20.0
    assert obj2.type == pynlm.WorldObjectType.Resource
    assert obj2.value == 5.0
    assert obj2.radius == 1.0
    
    # Test string representation
    obj_str = str(obj2)
    assert "WorldObject" in obj_str

def test_agent_body_class():
    """Test the AgentBody class."""
    body = pynlm.AgentBody()
    
    # Test default values
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.velocityX == 0.0
    assert body.velocityY == 0.0
    assert body.energy == 0.0
    assert body.health == 100.0  # Default health
    assert body.age == 0.0
    
    # Test modification
    body.x = 15.0
    body.y = 25.0
    body.orientation = 90.0
    body.energy = 50.0
    body.health = 75.0
    body.age = 10.0
    
    assert body.x == 15.0
    assert body.y == 25.0
    assert body.orientation == 90.0
    assert body.energy == 50.0
    assert body.health == 75.0
    assert body.age == 10.0
    
    # Test distance method
    distance = body.distanceTo(0.0, 0.0)
    assert distance > 0.0

def test_sensory_percept_class():
    """Test the SensoryPercept class."""
    percept = pynlm.SensoryPercept()
    
    # Test default state
    assert percept.getVisionWidth() == 0
    assert percept.getVisionHeight() == 0
    assert len(percept.getAllSignals()) == 0
    
    # Test vision access
    vision_data = [0.1, 0.2, 0.3, 0.4]
    vision = pynlm.Vision(2, 2)
    vision.setData(vision_data)
    
    percept.setVision(vision)
    assert percept.getVisionWidth() == 2
    assert percept.getVisionHeight() == 2
    
    # Test internal signals
    percept.addInternalSignal(0.5)
    percept.addInternalSignal(0.8)
    assert len(percept.getInternal()) == 2

def test_factory_functions():
    """Test factory functions."""
    # Test config factories
    config1 = pynlm.createDefaultConfig()
    assert isinstance(config1, pynlm.Config)
    assert config1.getInt("brain.neuron_count") == 1000
    
    config2 = pynlm.createMinimalConfig()
    assert config2.getInt("brain.neuron_count") == 100
    
    config3 = pynlm.createHighPerformanceConfig()
    assert config3.getInt("brain.neuron_count") == 5000
    assert config3.getBool("plasticity.stdp.enable") == False
    
    # Test complete agent setup
    config = pynlm.createDefaultConfig()
    brain, world, agent = pynlm.createCompleteAgent(config, 10, 10, 4, 4)
    
    assert isinstance(brain, pynlm.Brain)
    assert isinstance(world, pynlm.SimpleWorld)
    assert isinstance(agent, pynlm.AgentBrain)
    
    # Test that systems are enabled
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
def test_constants_access():
    """Test that constants are accessible and have correct values."""
    # Test version constants
    assert pynlm.VERSION == "0.1.0"
    
    # Test world size constants
    assert pynlm.DEFAULT_WORLD_WIDTH == 20
    assert pynlm.DEFAULT_WORLD_HEIGHT == 20
    assert pynlm.DEFAULT_VISION_WIDTH == 8
    assert pynlm.DEFAULT_VISION_HEIGHT == 8
    
    # Test neuron type constants
    assert pynlm.EXCITATORY_NEURON == pynlm.NeuronType.Excitatory
    assert pynlm.INHIBITORY_NEURON == pynlm.NeuronType.Inhibitory
    assert pynlm.SENSORY_NEURON == pynlm.NeuronType.Sensory
    assert pynlm.MOTOR_NEURON == pynlm.NeuronType.Motor

def test_error_handling():
    """Test error handling and validation."""
    import pytest
    
    config = pynlm.createDefaultConfig()
    
    # Test invalid key access (should return empty string)
    invalid_value = config.get("invalid.key")
    assert invalid_value == ""
    
    # Test type conversion errors (should raise ValueError)
    with pytest.raises(pynlm.RuntimeError):
        # This should work
        config.getInt("brain.neuron_count")
    
    # Test that invalid operations raise appropriate errors
    # (Specific error tests depend on actual error handling in C++ code)

def test_module_docstring():
    """Test that module has proper documentation."""
    # Check that main module docstring is accessible
    assert pynlm.__doc__ is not None
    assert len(pynlm.__doc__.strip()) > 0
    
    # The docstring should contain key information
    docstring = pynlm.__doc__.upper()
    assert "NLM" in docstring
    assert "NEURAL" in docstring
    assert "BINDINGS" in docstring

def run_all_tests():
    """Run all tests and report results."""
    tests = [
        test_module_metadata,
        test_constants,
        test_id_classes,
        test_enumerations,
        test_config_class,
        test_action_class,
        test_world_object_class,
        test_agent_body_class,
        test_sensory_percept_class,
        test_factory_functions,
        test_constants_access,
        test_error_handling,
        test_module_docstring,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            print(f"✓ {test.__name__} PASSED")
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} FAILED: {e}")
            failed += 1
    
    print(f"\nTest Results: {passed} passed, {failed} failed")
    return passed, failed

if __name__ == "__main__":
    # Run tests
    passed, failed = run_all_tests()
    
    # Exit with appropriate code
    if failed > 0:
        exit(1)
    else:
        print("\nAll tests passed!")
        exit(0)
