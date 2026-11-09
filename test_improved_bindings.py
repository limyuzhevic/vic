#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.
This script tests the new Pythonic features and improvements.
"""

import pynlm
import time
import sys

def test_basic_functionality():
    """Test basic functionality of improved bindings."""
    print("Testing basic functionality...")
    
    # Test 1: Basic brain creation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Pythonic config interface
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count") == 1000
    print("✓ Pythonic config interface works")
    
    # Test 3: Enhanced action creation
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    action.set_parameters([0.5, 1.0])
    print(f"✓ Created action: {action.get_name()}, params: {action.get_parameters()}")
    
    # Test 4: Enhanced world object
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
    assert obj.is_valid()
    print(f"✓ Created world object: {obj.type}, valid: {obj.is_valid()}")
    
    # Test 5: Agent body enhanced methods
    body = pynlm.AgentBody()
    body.x = 10.0
    body.y = 20.0
    body.set_position(15.0, 25.0)
    pos = body.position()
    assert abs(pos[0] - 15.0) < 0.01
    assert abs(pos[1] - 25.0) < 0.01
    print(f"✓ Enhanced agent body methods work")
    
    # Test 6: Sensory percept dictionary conversion
    percept = pynlm.SensoryPercept()
    vision = pynlm.Vision()
    vision.set_data([1.0, 2.0, 3.0])
    percept.setVision(vision)
    dict_repr = percept.to_dict()
    assert "vision" in dict_repr
    print(f"✓ Sensory percept dictionary conversion works")
    
    # Test 7: Action dictionary conversion
    action_dict = action.as_dict()
    assert "type" in action_dict
    assert "parameters" in action_dict
    print(f"✓ Action dictionary conversion works")
    
    # Test 8: Action with_parameters convenience method
    action2 = action.with_parameters([0.1, 0.2, 0.3])
    assert action2.get_parameters() == [0.1, 0.2, 0.3]
    print(f"✓ Action.with_parameters convenience method works")
    
    # Test 9: Enhanced ActionResult
    result = pynlm.ActionResult(reward=1.5, success=True, message="Success!")
    assert result.is_success()
    result_dict = result.to_dict()
    assert result_dict["reward"] == 1.5
    print(f"✓ Enhanced ActionResult works")
    
    # Test 10: World object distance calculation
    obj1 = pynlm.WorldObject(0.0, 0.0)
    distance = obj1.distance_to(3.0, 4.0)
    assert abs(distance - 5.0) < 0.01
    print(f"✓ World object distance calculation works")
    
    return True

def test_advanced_features():
    """Test advanced Python features."""
    print("\nTesting advanced features...")
    
    # Test 1: Context manager pattern
    with pynlm.NLMSimulation() as sim:
        assert sim.brain is not None
        sim.brain.initialize()
        print("✓ Context manager pattern works")
    
    # Test 2: Enhanced statistics
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps to generate data
    for i in range(50):
        brain.step(i)
    
    stats = brain.get_statistics()
    required_keys = ["total_neurons", "total_synapses", "active_neurons", 
                    "firing_neurons", "avg_firing_rate", "e_i_ratio", 
                    "total_spikes", "developmental_stage"]
    
    for key in required_keys:
        assert key in stats, f"Missing key {key} in statistics"
    
    print(f"✓ Enhanced brain statistics works")
    print(f"  - {stats['total_neurons']} neurons, {stats['firing_neurons']} firing")
    print(f"  - {stats['total_spikes']} total spikes")
    
    # Test 3: Health check
    health_issues = brain.health_check()
    print(f"✓ Health check completed: {len(health_issues)} issues")
    
    # Test 4: Action convenience methods
    action = pynlm.Action(pynlm.ActionType.Eat)
    action_dict = action.as_dict()
    print(f"✓ Action convenience methods work: {action_dict}")
    
    return True

def test_agent_simulation():
    """Test complete agent simulation."""
    print("\nTesting agent simulation...")
    
    # Create complete agent simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"✓ Complete agent simulation works")
    print(f"  - Steps completed: 100")
    print(f"  - Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return True

def test_utility_functions():
    """Test utility functions."""
    print("\nTesting utility functions...")
    
    # Test run_agent_simulation utility
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    agent = pynlm.createAgentBrain(brain)
    
    result = pynlm.run_agent_simulation(brain, world, agent, 50)
    assert "steps_completed" in result
    assert "final_neurons" in result
    assert "total_spikes" in result
    print(f"✓ run_agent_simulation utility works")
    
    # Test create_agent_from_config utility
    config2 = pynlm.createDefaultConfig()
    brain2, agent2 = pynlm.create_agent_from_config(config2, world)
    
    assert brain2 is not None
    assert agent2 is not None
    print(f"✓ create_agent_from_config utility works")
    
    # Test get_brain_version utility
    version_info = pynlm.get_brain_version()
    assert "project" in version_info
    assert "pybind11" in version_info
    print(f"✓ get_brain_version utility works")
    
    # Test simulate_step utility
    brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain3.initialize()
    pynlm.simulate_step(brain3, step=10, time=0.1)
    print(f"✓ simulate_step utility works")
    
    # Test batch_process utility
    brain4 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain4.initialize()
    
    firing_counts = pynlm.batch_process(brain4, start_step=0, end_step=20)
    assert len(firing_counts) == 21
    print(f"✓ batch_process utility works")
    
    return True

def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("\nTesting performance monitoring...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test PerformanceMonitor
    from pynlm import PerformanceMonitor
    
    monitor = PerformanceMonitor(brain)
    monitor.start_monitoring(0.05)  # 50ms intervals
    
    # Run simulation while monitoring
    for step in range(20):
        brain.step(step)
    
    monitor.stop_monitoring()
    
    stats = monitor.get_performance_stats()
    if stats:
        assert "avg_firing_rate" in stats
        assert "max_firing_rate" in stats
        assert "duration" in stats
        print(f"✓ Performance monitoring works")
        print(f"  - Average firing rate: {stats['avg_firing_rate']:.2f}")
        print(f"  - Duration: {stats['duration']:.2f}s")
    else:
        print(f"⚠ Performance monitoring: No stats collected")
    
    return True

def test_error_handling():
    """Test improved error handling."""
    print("\nTesting error handling...")
    
    # Test Config error handling
    config = pynlm.createDefaultConfig()
    
    # Set with invalid type should work (Python handles conversion)
    config.set("brain.neuron_count", "1000")  # String should work
    print(f"✓ Config accepts flexible types")
    
    # Test Action with invalid parameters
    action = pynlm.Action(pynlm.ActionType.Custom)
    action.set_parameters([])  # Empty parameters
    print(f"✓ Action handles empty parameters")
    
    # Test World object validation
    invalid_obj = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Empty)
    invalid_obj.active = False
    assert not invalid_obj.is_valid()
    print(f"✓ World object validation works")
    
    return True

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("Testing Improved NLM Python Bindings")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_advanced_features,
        test_agent_simulation,
        test_utility_functions,
        test_performance_monitoring,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Results")
    print("=" * 60)
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved Python bindings work correctly.")
        return True
    else:
        print(f"\n❌ {failed} tests failed.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)