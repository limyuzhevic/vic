#!/usr/bin/env python3
"""
Test suite for NLM Python bindings
Comprehensive tests for the improved Python API
"""

import sys
import tempfile
import os
from pathlib import Path

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available, skipping tests")

def test_basic_imports():
    """Test that all major classes can be imported"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        # Test basic imports
        assert hasattr(pynlm, 'Config')
        assert hasattr(pynlm, 'Brain')
        assert hasattr(pynlm, 'SimpleWorld')
        assert hasattr(pynlm, 'AgentBrain')
        assert hasattr(pynlm, 'AgentBody')
        assert hasattr(pynlm, 'Action')
        assert hasattr(pynlm, 'WorldObject')
        assert hasattr(pynlm, 'Vision')
        assert hasattr(pynlm, 'Audio')
        
        # Test neuromodulation classes
        assert hasattr(pynlm, 'Dopamine')
        assert hasattr(pynlm, 'Curiosity')
        assert hasattr(pynlm, 'Novelty')
        assert hasattr(pynlm, 'PredictionError')
        assert hasattr(pynlm, 'Reward')
        
        # Test memory classes
        assert hasattr(pynlm, 'NeuralWorkingMemory')
        assert hasattr(pynlm, 'NeuralEpisodicMemory')
        assert hasattr(pynlm, 'NeuralAssociativeMemory')
        
        # Test cognition classes
        assert hasattr(pynlm, 'NeuralPlanner')
        assert hasattr(pynlm, 'ConceptFormation')
        
        # Test development classes
        assert hasattr(pynlm, 'DevelopmentSystem')
        assert hasattr(pynlm, 'Synaptogenesis')
        assert hasattr(pynlm, 'Pruning')
        assert hasattr(pynlm, 'Maturation')
        
        # Test prediction classes
        assert hasattr(pynlm, 'PredictionSystem')
        assert hasattr(pynlm, 'NeuralPrediction')
        
        return True
    except Exception as e:
        print(f"Import test failed: {e}")
        return False

def test_config_dict_interface():
    """Test Pythonic dict-like interface for Config class"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        config = pynlm.Config()
        
        # Test dict-like access
        config['random_seed'] = 42
        config['neuron_count'] = 1000
        config['simulation_timestep'] = 0.001
        
        assert config['random_seed'] == 42
        assert config['neuron_count'] == 1000
        assert config['simulation_timestep'] == 0.001
        
        # Test __contains__
        assert 'random_seed' in config
        assert 'neuron_count' in config
        assert 'non_existent_key' not in config
        
        # Test get_or method
        assert config.get_or('random_seed', 0) == 42
        assert config.get_or('non_existent', 99) == 99
        
        # Test to_dict
        config_dict = config.to_dict()
        assert isinstance(config_dict, dict)
        assert 'random_seed' in config_dict
        
        # Test from_dict
        config2 = pynlm.Config()
        config2.from_dict({'test_key': 'test_value', 'test_number': 123})
        assert config2['test_key'] == 'test_value'
        assert config2['test_number'] == 123
        
        # Test copy_from and merge
        config3 = pynlm.Config()
        config3.copy_from(config)
        config3.merge(config2)
        
        return True
    except Exception as e:
        print(f"Config dict interface test failed: {e}")
        return False

def test_config_serialization():
    """Test Config serialization to/from JSON and dict"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        config = pynlm.Config()
        config.set('random_seed', 42)
        config.set('neuron_count', 1000)
        config.set('simulation_timestep', 0.001)
        config.set('log_level', 'INFO')
        
        # Test to_dict
        config_dict = config.to_dict()
        assert isinstance(config_dict, dict)
        assert config_dict['random_seed'] == 42
        assert config_dict['neuron_count'] == 1000
        
        # Test from_dict
        config2 = pynlm.Config()
        config2.from_dict(config_dict)
        assert config2['random_seed'] == 42
        assert config2['neuron_count'] == 1000
        
        # Test to_json (basic functionality)
        json_str = config.to_json()
        assert isinstance(json_str, str)
        assert 'random_seed' in json_str
        assert '42' in json_str
        
        return True
    except Exception as e:
        print(f"Config serialization test failed: {e}")
        return False

def test_convenience_methods():
    """Test convenience methods and factory functions"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        # Test create_default_config
        default_config = pynlm.create_default_config()
        assert isinstance(default_config, pynlm.Config)
        
        # Test create_minimal_config
        minimal_config = pynlm.create_minimal_config()
        assert isinstance(minimal_config, pynlm.Config)
        
        # Test create_large_scale_config
        large_config = pynlm.create_large_scale_config()
        assert isinstance(large_config, pynlm.Config)
        
        # Test create_config_from_dict
        config_dict = {'test': 123, 'example': 'value'}
        dict_config = pynlm.load_config_from_dict(config_dict)
        assert isinstance(dict_config, pynlm.Config)
        assert dict_config['test'] == 123
        
        # Test clone_config
        cloned_config = pynlm.clone_config(default_config)
        assert isinstance(cloned_config, pynlm.Config)
        
        # Test merge_configs
        merged_config = pynlm.merge_configs(default_config, minimal_config)
        assert isinstance(merged_config, pynlm.Config)
        
        return True
    except Exception as e:
        print(f"Convenience methods test failed: {e}")
        return False

def test_error_handling():
    """Test error handling with custom NLM exceptions"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        config = pynlm.Config()
        
        # Test MissingConfigError
        try:
            # Access non-existent key
            _ = config['non_existent_key']
            assert False, "Should have raised MissingConfigError"
        except pynlm.MissingConfigError:
            pass  # Expected
        
        # Test InvalidConfigError
        try:
            config.set_with_validation('', 'value')  # Empty key should raise error
            assert False, "Should have raised InvalidConfigError"
        except pynlm.InvalidConfigError:
            pass  # Expected
        
        # Test get_with_validation
        try:
            _ = config.get_with_validation('non_existent')
            assert False, "Should have raised MissingConfigError"
        except pynlm.MissingConfigError:
            pass  # Expected
        
        # Test try_get
        found, value = config.try_get('non_existent')
        assert not found
        assert value is None or value is pynlm.py_none
        
        return True
    except Exception as e:
        print(f"Error handling test failed: {e}")
        return False

def test_world_and_sensory_wrappers():
    """Test Pythonic wrappers for WorldObject and Sensory processing"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        # Test WorldObject wrappers
        world_obj = pynlm.create_world_object(1.0, 2.0, pynlm.WorldObjectType.Resource, 10.0)
        assert world_obj.x == 1.0
        assert world_obj.y == 2.0
        assert world_obj.type == pynlm.WorldObjectType.Resource
        assert world_obj.value == 10.0
        
        # Test WorldObject methods
        pos = world_obj.get_position()
        assert isinstance(pos, tuple)
        assert len(pos) == 2
        
        within_range = world_obj.is_within_range(1.0, 2.0, 0.1)
        assert within_range
        
        # Test movement
        world_obj.move(1.0, 1.0)
        assert world_obj.x == 2.0
        assert world_obj.y == 3.0
        
        # Test AgentBody wrappers
        agent_body = pynlm.create_agent_body()
        agent_body.x = 5.0
        agent_body.y = 10.0
        agent_body.velocityX = 1.0
        agent_body.velocityY = 1.0
        
        velocity_magnitude = agent_body.get_velocity_magnitude()
        assert abs(velocity_magnitude - 1.41421356237) < 0.001
        
        angle = agent_body.get_angle()
        assert abs(angle - 0.78539816339) < 0.001
        
        distance = agent_body.get_distance_to(0.0, 0.0)
        assert abs(distance - 11.18033988749) < 0.001
        
        # Test SensoryPercept wrappers
        percept = pynlm.SensoryPercept()
        
        # Test sensory checks
        assert not percept.has_vision()
        assert not percept.has_touch()
        assert not percept.has_internal()
        assert not percept.has_proprioception()
        assert not percept.has_audio()
        
        # Test with data
        percept.setVision([0.1, 0.2, 0.3])
        assert percept.has_vision()
        vision_data = percept.get_vision_data()
        assert len(vision_data) == 3
        assert abs(vision_data[0] - 0.1) < 0.001
        
        # Test all sensory data
        all_sensory = percept.get_all_sensory_data()
        assert isinstance(all_sensory, dict)
        assert 'vision' in all_sensory
        
        # Test sensory summary
        summary = percept.get_sensory_summary()
        assert isinstance(summary, str)
        assert 'Vision' in summary
        
        return True
    except Exception as e:
        print(f"World and sensory wrappers test failed: {e}")
        return False

def test_agent_integration():
    """Test agent integration features"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        # Create basic setup
        config = pynlm.create_default_config()
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        world.setAgentStart(0.0, 0.0)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test agent state
        state = agent_brain.get_agent_state()
        assert isinstance(state, dict)
        
        # Test reset
        agent_brain.reset()
        world.reset()
        world.setAgentStart(0.0, 0.0)
        
        # Test development system
        dev_system = brain.getDevelopmentSystem()
        if dev_system:
            stage = dev_system.getStage()
            assert stage in [
                pynlm.DevelopmentalStage.Initial,
                pynlm.DevelopmentalStage.CriticalPeriod,
                pynlm.DevelopmentalStage.Maturation,
                pynlm.DevelopmentalStage.Adult,
                pynlm.DevelopmentalStage.Aging
            ]
        
        # Test neuromodulation system
        neuromodulation = brain.get_neuromodulation_system()
        assert isinstance(neuromulation, dict)
        
        # Test memory system
        memory_status = brain.get_memory_system_status()
        assert isinstance(memory_status, dict)
        
        # Test performance stats
        stats = brain.get_performance_stats()
        assert isinstance(stats, dict)
        
        # Test system integrity
        integrity = brain.get_system_integrity()
        assert isinstance(integrity, dict)
        
        return True
    except Exception as e:
        print(f"Agent integration test failed: {e}")
        return False

def test_simulation_functionality():
    """Test basic simulation functionality"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        config = pynlm.create_minimal_config()
        brain = pynlm.createBrain(config)
        
        # Test simple simulation step
        brain.step(1)
        
        # Test simulation with time
        brain.step(1, 0.0)
        
        # Test neuron injection
        from pynlm import NeuronId
        neuron_id = NeuronId()
        neuron_id.value = 123
        brain.injectCurrent(neuron_id, 1.0)
        
        # Test injection to type
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 1.0)
        
        # Test spike system (if available)
        spike_system = brain.getSpikeSystem()
        if spike_system:
            # Test spike system methods
            spike_count = spike_system.getSpikeCount()
            assert isinstance(spike_count, int)
            assert spike_count >= 0
        
        return True
    except Exception as e:
        print(f"Simulation functionality test failed: {e}")
        return False

def test_comprehensive_workflow():
    """Test comprehensive workflow combining multiple features"""
    if not PYNLM_AVAILABLE:
        return False
        
    try:
        # Create complete agent setup
        setup = pynlm.create_complete_agent_setup()
        
        brain = setup['brain']
        world = setup['world']
        agent_brain = setup['agent_brain']
        
        # Configure agent
        agent_brain.initialize(world)
        
        # Run a simple episode
        episode_config = {'max_steps': 10, 'max_time': 1.0}
        reward, steps = agent_brain.run_agent_episode(world, episode_config)
        
        assert isinstance(reward, float)
        assert isinstance(steps, int)
        assert steps > 0
        
        # Get agent state
        state = agent_brain.get_agent_state()
        assert isinstance(state, dict)
        
        # Reset for next episode
        agent_brain.reset()
        world.reset()
        world.setAgentStart(0.0, 0.0)
        
        # Test memory system
        memory_status = brain.get_memory_system_status()
        assert isinstance(memory_status, dict)
        
        # Test neuromodulation
        neuromodulation = brain.get_neuromodulation_system()
        assert isinstance(neuromodulation, dict)
        
        # Test development
        dev_system = brain.getDevelopmentSystem()
        if dev_system:
            stage = dev_system.getStage()
            # Stage should be valid
        
        return True
    except Exception as e:
        print(f"Comprehensive workflow test failed: {e}")
        return False

def run_all_tests():
    """Run all tests and return summary"""
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Config Dict Interface", test_config_dict_interface),
        ("Config Serialization", test_config_serialization),
        ("Convenience Methods", test_convenience_methods),
        ("Error Handling", test_error_handling),
        ("World and Sensory Wrappers", test_world_and_sensory_wrappers),
        ("Agent Integration", test_agent_integration),
        ("Simulation Functionality", test_simulation_functionality),
        ("Comprehensive Workflow", test_comprehensive_workflow),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print(f"{'='*50}")
        
        try:
            success = test_func()
            results.append((test_name, success))
            print(f"Result: {'PASS' if success else 'FAIL'}")
        except Exception as e:
            print(f"Result: FAIL - {e}")
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print(f"{'='*50}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\nAll tests passed! ✓")
        return True
    else:
        print(f"\n{total - passed} tests failed ✗")
        return False

if __name__ == "__main__":
    if PYNLM_AVAILABLE:
        success = run_all_tests()
        sys.exit(0 if success else 1)
    else:
        print("pynlm module not available. Skipping tests.")
        sys.exit(0)