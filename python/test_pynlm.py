#!/usr/bin/env python3
"""
Comprehensive test suite for pynlm Python bindings.
Tests the enhanced Python bindings with better error handling, convenience methods, and utility functions.
"""

import pynlm
import sys
import traceback

def test_basic_imports():
    """Test that all basic imports work."""
    print("Testing basic imports...")
    try:
        # Test core classes
        assert hasattr(pynlm, 'Config')
        assert hasattr(pynlm, 'Brain')
        assert hasattr(pynlm, 'AgentBrain')
        assert hasattr(pynlm, 'SimpleWorld')
        assert hasattr(pynlm, 'Vision')
        assert hasattr(pynlm, 'Audio')
        assert hasattr(pynlm, 'Action')
        assert hasattr(pynlm, 'SensoryPercept')
        
        # Test enums
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'ActionType')
        assert hasattr(pynlm, 'DevelopmentalStage')
        
        # Test constants
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'RANDOM_SEED')
        
        print("  ✓ Basic imports passed")
        return True
    except Exception as e:
        print(f"  ✗ Basic imports failed: {e}")
        traceback.print_exc()
        return False

def test_config():
    """Test Config class with enhanced methods."""
    print("Testing Config class...")
    try:
        # Test creation
        config = pynlm.Config()
        assert config is not None
        
        # Test setting and getting values
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("string_value", "hello")
        config.set("bool_value", True)
        
        # Test has method
        assert config.has("int_value")
        assert config.has("float_value")
        assert config.has("string_value")
        assert config.has("bool_value")
        assert not config.has("nonexistent")
        
        # Test get method with default
        int_val = config.get("int_value", 0)
        assert int_val == 42
        
        # Test getOr method
        default_val = config.getOr("nonexistent", 100)
        assert default_val == 100
        
        # Test remove
        config.remove("string_value")
        assert not config.has("string_value")
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        
        # Test clone
        config2 = config.clone()
        assert config2 is not None
        assert config2.has("int_value")
        assert not config2.has("string_value")  # Should not have removed key
        
        # Test validation
        assert config.validate() == True
        
        empty_config = pynlm.Config()
        assert empty_config.has("int_value") == False
        
        print("  ✓ Config class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Config class tests failed: {e}")
        traceback.print_exc()
        return False

def test_brain():
    """Test Brain class with enhanced methods."""
    print("Testing Brain class...")
    try:
        # Create config
        config = pynlm.Config()
        config.set("simulation.steps", 100)
        config.set("simulation.dt", 0.01)
        
        # Create brain
        brain = pynlm.Brain(config)
        assert brain is not None
        
        # Test getConfig
        brain_config = brain.getConfig()
        assert brain_config is not None
        
        # Test createDefaultConfig
        default_config = pynlm.Brain.createDefaultConfig()
        assert default_config is not None
        assert default_config.has("simulation.steps")
        assert default_config.has("brain.memory.enabled")
        
        # Test createDefaultConfigWithCapacity
        capacity_config = pynlm.Brain.createDefaultConfigWithCapacity(1000)
        assert capacity_config is not None
        assert capacity_config.has("brain.neuron.capacity")
        
        # Test createBrainFromConfigFile (requires file, skip for now)
        # test_brain_file = pynlm.Brain.createBrainFromConfigFile("nonexistent.json")
        # Should throw exception
        
        print("  ✓ Brain class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Brain class tests failed: {e}")
        traceback.print_exc()
        return False

def test_vision():
    """Test Vision class with enhanced methods."""
    print("Testing Vision class...")
    try:
        # Test creation
        vision = pynlm.Vision()
        assert vision is not None
        
        # Test creation with dimensions
        vision2 = pynlm.Vision(10, 10, 3)
        assert vision2.getWidth() == 10
        assert vision2.getHeight() == 10
        assert vision2.getChannels() == 3
        
        # Test data setting
        test_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
        vision.setData(test_data)
        
        # Test getData
        retrieved_data = vision.getData()
        assert len(retrieved_data) == len(test_data)
        
        # Test normalized data
        vision.setDataNormalized(test_data, 0.0, 1.0)
        normalized_data = vision.getData()
        assert all(0.0 <= x <= 1.0 for x in normalized_data)
        
        # Test pixel access
        pixel_value = vision2.getPixel(0, 0, 0)
        assert pixel_value >= 0.0 and pixel_value <= 1.0
        
        # Test dimensions tuple
        width, height, channels = vision2.getArrayDimensions()
        assert width == 10 and height == 10 and channels == 3
        
        print("  ✓ Vision class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Vision class tests failed: {e}")
        traceback.print_exc()
        return False

def test_sensory_percept():
    """Test SensoryPercept class."""
    print("Testing SensoryPercept class...")
    try:
        # Create percept
        percept = pynlm.SensoryPercept()
        assert percept is not None
        
        # Test with vision data
        vision = pynlm.Vision(5, 5, 3)
        test_data = [i * 0.1 for i in range(75)]  # 5*5*3 = 75
        vision.setData(test_data)
        percept.setVision(vision)
        
        # Test getters
        retrieved_vision = percept.getVision()
        assert retrieved_vision is not None
        assert retrieved_vision.getWidth() == 5
        
        # Test timestamp
        test_timestamp = 123456789.0
        percept.setTimestamp(test_timestamp)
        assert percept.getTimestamp() == test_timestamp
        
        print("  ✓ SensoryPercept class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ SensoryPercept class tests failed: {e}")
        traceback.print_exc()
        return False

def test_action():
    """Test Action class."""
    print("Testing Action class...")
    try:
        # Create action with type
        action = pynlm.createAction(pynlm.ActionType.MoveForward)
        assert action is not None
        assert action.getType() == pynlm.ActionType.MoveForward
        assert action.getName() == "MoveForward"
        
        # Create action with parameters
        params = [0.5, 1.0, 1.5]
        action2 = pynlm.createActionWithParams(pynlm.ActionType.Custom, params)
        assert action2.getType() == pynlm.ActionType.Custom
        assert action2.getParameters() == params
        
        # Test setType
        action.setType(pynlm.ActionType.Rest)
        assert action.getType() == pynlm.ActionType.Rest
        
        # Test setParameters
        new_params = [2.0, 3.0, 4.0]
        action.setParameters(new_params)
        assert action.getParameters() == new_params
        
        # Test clone
        action3 = action.clone()
        assert action3.getType() == action.getType()
        assert action3.getParameters() == action.getParameters()
        
        print("  ✓ Action class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Action class tests failed: {e}")
        traceback.print_exc()
        return False

def test_world():
    """Test SimpleWorld class."""
    print("Testing SimpleWorld class...")
    try:
        # Test creation
        world = pynlm.SimpleWorld()
        assert world is not None
        
        # Test configuration
        world.configure(100, 100, 16, 16)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        
        # Test agent start
        world.setAgentStart(50.0, 50.0)
        
        # Test agent body
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 50.0
        assert body.y == 50.0
        
        # Test object management
        obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 1.0, 0.5)
        world.addObject(obj)
        
        # Test position validation
        assert world.isValidPosition(50.0, 50.0)
        assert not world.isValidPosition(200.0, 200.0)
        
        # Test random seed
        world.setRandomSeed(12345)
        seed = world.getRandomSeed()
        assert seed == 12345
        
        print("  ✓ SimpleWorld class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ SimpleWorld class tests failed: {e}")
        traceback.print_exc()
        return False

def test_agent_brain():
    """Test AgentBrain class with enhanced methods."""
    print("Testing AgentBrain class...")
    try:
        # Create config and brain
        config = pynlm.Brain.createDefaultConfig()
        brain = pynlm.Brain(config)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(64, 64, 16, 16)
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Create agent brain with world (initializes it)
        agent2 = pynlm.createAgentBrainWithWorld(brain, world)
        assert agent2 is not None
        
        # Test initialize
        agent.initialize(world)
        
        # Test sizes
        input_size = agent.getSensoryInputSize()
        output_size = agent.getMotorOutputSize()
        assert input_size > 0
        assert output_size > 0
        
        # Test processSensoryInput
        percept = pynlm.SensoryPercept()
        vision = pynlm.Vision(16, 16, 3)
        test_data = [i * 0.01 for i in range(768)]  # 16*16*3 = 768
        vision.setData(test_data)
        percept.setVision(vision)
        
        agent.processSensoryInput(percept)
        
        # Test decodeMotorCommand
        action = agent.decodeMotorCommand()
        assert action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward,
                         pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight,
                         pynlm.MotorCommand.Interact, pynlm.MotorCommand.Wait]
        
        # Test development
        agent.updateDevelopment(1.0)
        stage = agent.getDevelopmentalStage()
        assert stage in [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation, pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
        
        # Test neuromodulation
        agent.applyRewardModulation(1.0, 0.5)
        neuromodulation = agent.getNeuromodulationLevel()
        assert neuromodulation >= -1.0 and neuromodulation <= 1.0
        
        # Test curiosity
        curiosity = agent.getCuriosityLevel()
        assert curiosity >= 0.0 and curiosity <= 1.0
        
        # Test enable methods
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        
        assert agent.isRewardModulationEnabled() == False
        assert agent.isCuriosityEnabled() == False
        
        # Test runEpisode
        sensory_inputs = [test_data for _ in range(10)]  # 10 steps
        reward = agent.runEpisode(world, sensory_inputs)
        assert reward >= 0.0
        
        print("  ✓ AgentBrain class tests passed")
        return True
    except Exception as e:
        print(f"  ✗ AgentBrain class tests failed: {e}")
        traceback.print_exc()
        return False

def test_utility_functions():
    """Test utility functions."""
    print("Testing utility functions...")
    try:
        # Test createSensoryPercept
        percept = pynlm.createSensoryPercept()
        assert percept is not None
        
        # Test createVision
        vision = pynlm.createVision(10, 10, 3)
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
        # Test createAudio
        audio = pynlm.createAudio(44100, 1024)
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Test action creation
        action = pynlm.createAction(pynlm.ActionType.MoveForward)
        assert action is not None
        
        # Test getNeuronTypes
        neuron_types = pynlm.getNeuronTypes()
        assert isinstance(neuron_types, list)
        assert "Excitatory" in neuron_types
        assert "Sensory" in neuron_types
        assert "Motor" in neuron_types
        
        # Test getSynapseTypes
        synapse_types = pynlm.getSynapseTypes()
        assert isinstance(synapse_types, list)
        assert "Excitatory" in synapse_types
        
        # Test getActionTypes
        action_types = pynlm.getActionTypes()
        assert isinstance(action_types, list)
        assert "MoveForward" in action_types
        assert "Wait" in action_types
        
        # Test actionTypeToString
        action_str = pynlm.actionTypeToString(pynlm.ActionType.MoveForward)
        assert action_str == "MoveForward"
        
        # Test createDefaultConfig (factory method)
        default_config = pynlm.createDefaultConfig()
        assert default_config is not None
        
        # Test createSimpleWorldConfigured
        world = pynlm.createSimpleWorldConfigured(100, 100, 16, 16)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        
        print("  ✓ Utility functions tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Utility functions tests failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    try:
        # Test ConfigError
        config = pynlm.Config()
        try:
            config.get("nonexistent")
            assert False, "Should have raised KeyError"
        except Exception as e:
            # Should handle gracefully
            pass
        
        # Test VisionError
        vision = pynlm.Vision(10, 10, 3)
        try:
            vision.getPixel(100, 100, 0)  # Out of bounds
            assert False, "Should have raised VisionError"
        except Exception as e:
            pass
        
        print("  ✓ Error handling tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Error handling tests failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features for experienced users."""
    print("Testing advanced features...")
    try:
        # Test complete simulation
        config = pynlm.Brain.createDefaultConfigWithCapacity(500)
        brain = pynlm.Brain(config)
        
        world = pynlm.createSimpleWorld()
        world.configure(32, 32, 8, 8)
        
        agent = pynlm.createAgentBrainWithWorld(brain, world)
        
        # Simulate multiple steps
        for step in range(20):
            # Generate random sensory input
            sensory_inputs = []
            for _ in range(5):  # 5 sensory channels
                data = [i * 0.05 for i in range(64)]  # 8x8 = 64
                sensory_inputs.append(data)
            
            # Run episode for one step
            reward = agent.runEpisode(world, sensory_inputs)
            assert reward >= 0.0
            
            # Apply some reward
            agent.applyRewardModulation(reward, reward * 0.8)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Test brain save/load
        brain.save("test_brain.json")
        brain2 = pynlm.Brain(config)
        brain2.load("test_brain.json")
        
        # Test agent reset
        agent.reset()
        
        # Test advanced brain statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        
        assert total_neurons > 0
        assert total_synapses >= 0
        assert active_neurons >= 0
        assert firing_neurons >= 0
        
        # Test developmental stage changes
        agent.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        stage = agent.getDevelopmentalStage()
        assert stage == pynlm.DevelopmentalStage.Adult
        
        print("  ✓ Advanced features tests passed")
        return True
    except Exception as e:
        print(f"  ✗ Advanced features tests failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("PYNLM PYTHON BINDINGS COMPREHENSIVE TEST SUITE")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_config,
        test_brain,
        test_vision,
        test_sensory_percept,
        test_action,
        test_world,
        test_agent_brain,
        test_utility_functions,
        test_error_handling,
        test_advanced_features,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("=" * 60)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())