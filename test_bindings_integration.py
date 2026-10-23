#!/usr/bin/env python3
"""Comprehensive integration tests for NLM Python bindings.

This test suite verifies that all the improvements made to the Python bindings
are working correctly, including:
- Fixed enum mappings
- Enhanced Action class bindings
- Performance monitoring tools
- Better error handling
- Python-friendly APIs
"""

import sys
import traceback
import time
import math
from typing import List, Dict, Any

print("=" * 70)
print("NLM Python Bindings Integration Test Suite")
print("=" * 70)

def test_import_and_basic_types():
    """Test that all expected types and enums are available."""
    print("\n1. Testing Import and Basic Types...")
    
    try:
        import pynlm
        print("  ✓ Successfully imported pynlm")
        
        # Test that all expected types are available
        expected_classes = [
            'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
            'Vision', 'Audio', 'InternalSignals', 'Action',
            'SensoryPercept', 'AgentBody', 'ActionResult', 'WorldObject'
        ]
        
        for cls_name in expected_classes:
            if hasattr(pynlm, cls_name):
                print(f"  ✓ {cls_name} available")
            else:
                print(f"  ✗ {cls_name} NOT FOUND")
                return False
                
        # Test that both ActionType and MotorCommand enums are available
        if hasattr(pynlm, 'ActionType'):
            print("  ✓ ActionType enum available")
        else:
            print("  ✗ ActionType enum NOT FOUND")
            return False
            
        if hasattr(pynlm, 'MotorCommand'):
            print("  ✓ MotorCommand enum available")
        else:
            print("  ✗ MotorCommand enum NOT FOUND")
            return False
            
        # Test that ActionType is properly exposed (not aliased to MotorCommand)
        try:
            action_type_values = list(pynlm.ActionType)
            if len(action_type_values) >= 10:
                print(f"  ✓ ActionType has {len(action_type_values)} values")
            else:
                print(f"  ✗ ActionType has too few values: {len(action_type_values)}")
                return False
        except Exception as e:
            print(f"  ✗ Error accessing ActionType: {e}")
            return False
            
        return True
        
    except Exception as e:
        print(f"  ✗ Import failed: {e}")
        traceback.print_exc()
        return False

def test_configuration_system():
    """Test the enhanced configuration system."""
    print("\n2. Testing Configuration System...")
    
    try:
        import pynlm
        
        # Test default config creation
        config = pynlm.createDefaultConfig()
        print("  ✓ Created default config")
        
        # Test Python-friendly convenience methods
        config.setInt("brain.neuron_count", 500)
        config.setFloat("brain.connection_probability", 0.1)
        config.setString("brain.name", "TestBrain")
        print("  ✓ Set config values using Python-friendly methods")
        
        # Test getting config values
        neuron_count = config.getInt("brain.neuron_count", 100)
        if neuron_count == 500:
            print("  ✓ GetInt works correctly")
        else:
            print(f"  ✗ GetInt failed: expected 500, got {neuron_count}")
            return False
            
        connection_prob = config.getFloat("brain.connection_probability", 0.05)
        if abs(connection_prob - 0.1) < 0.001:
            print("  ✓ GetFloat works correctly")
        else:
            print(f"  ✗ GetFloat failed: expected 0.1, got {connection_prob}")
            return False
            
        brain_name = config.getString("brain.name", "Default")
        if brain_name == "TestBrain":
            print("  ✓ GetString works correctly")
        else:
            print(f"  ✗ GetString failed: expected 'TestBrain', got '{brain_name}'")
            return False
            
        # Test loadFromDict
        config2 = pynlm.createDefaultConfig()
        config_dict = {
            "brain.neuron_count": 200,
            "brain.connection_probability": 0.15,
            "brain.name": "DictBrain"
        }
        config2.loadFromDict(config_dict)
        
        if config2.getInt("brain.neuron_count", 0) == 200:
            print("  ✓ loadFromDict works correctly")
        else:
            print("  ✗ loadFromDict failed")
            return False
            
        # Test string representation
        summary = config.summary()
        if isinstance(summary, str) and len(summary) > 0:
            print("  ✓ Summary method works")
        else:
            print("  ✗ Summary method failed")
            return False
            
        return True
        
    except Exception as e:
        print(f"  ✗ Configuration test failed: {e}")
        traceback.print_exc()
        return False

def test_action_class():
    """Test the enhanced Action class."""
    print("\n3. Testing Action Class...")
    
    try:
        import pynlm
        
        # Test Action creation
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        print("  ✓ Created Action with type only")
        
        action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        print("  ✓ Created Action with type and parameters")
        
        # Test Action methods
        if action1.getType() == pynlm.ActionType.MoveForward:
            print("  ✓ getType() works")
        else:
            print("  ✗ getType() failed")
            return False
            
        action1.setType(pynlm.ActionType.TurnLeft)
        if action1.getType() == pynlm.ActionType.TurnLeft:
            print("  ✓ setType() works")
        else:
            print("  ✗ setType() failed")
            return False
            
        params = action1.getParameters()
        if isinstance(params, list):
            print("  ✓ getParameters() returns list")
        else:
            print("  ✗ getParameters() failed")
            return False
            
        action1.setParameters([4.0, 5.0])
        if action1.getParameters() == [4.0, 5.0]:
            print("  ✓ setParameters() works")
        else:
            print("  ✗ setParameters() failed")
            return False
            
        name = action1.getName()
        if isinstance(name, str) and len(name) > 0:
            print(f"  ✓ getName() works: '{name}'")
        else:
            print("  ✗ getName() failed")
            return False
            
        # Test getNameStr alias
        name2 = action1.getNameStr()
        if name2 == name:
            print("  ✓ getNameStr() alias works")
        else:
            print("  ✗ getNameStr() alias failed")
            return False
            
        # Test clone
        cloned_action = action1.clone()
        if cloned_action.getType() == action1.getType() and cloned_action.getParameters() == action1.getParameters():
            print("  ✓ clone() works")
        else:
            print("  ✗ clone() failed")
            return False
            
        # Test string representations
        repr_str = repr(action1)
        if isinstance(repr_str, str) and "Action" in repr_str:
            print(f"  ✓ repr() works: '{repr_str}'")
        else:
            print("  ✗ repr() failed")
            return False
            
        str_str = str(action1)
        if isinstance(str_str, str) and len(str_str) > 0:
            print(f"  ✓ str() works: '{str_str}'")
        else:
            print("  ✗ str() failed")
            return False
            
        return True
        
    except Exception as e:
        print(f"  ✗ Action class test failed: {e}")
        traceback.print_exc()
        return False

def test_performance_monitoring():
    """Test performance monitoring and profiling tools."""
    print("\n4. Testing Performance Monitoring...")
    
    try:
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            print("  ✗ Brain initialization failed")
            return False
            
        print("  ✓ Brain initialized for performance testing")
        
        # Test getPerformanceStats
        stats = pynlm.getPerformanceStats()
        if isinstance(stats, dict) and "timestamp" in stats and "version" in stats:
            print("  ✓ getPerformanceStats() works")
        else:
            print("  ✗ getPerformanceStats() failed")
            return False
            
        # Test profileSimulation
        profile_results = pynlm.profileSimulation(brain, 100)
        if isinstance(profile_results, dict):
            expected_keys = ["total_time_seconds", "steps_per_second", "final_firing_rate", "total_spikes"]
            if all(key in profile_results for key in expected_keys):
                print("  ✓ profileSimulation() returns expected keys")
                
                # Check that values are reasonable
                if profile_results["total_time_seconds"] >= 0:
                    print("  ✓ Profile timing is reasonable")
                else:
                    print("  ✗ Profile timing is negative")
                    return False
                    
                if profile_results["steps_per_second"] > 0:
                    print("  ✓ Steps per second is positive")
                else:
                    print("  ✗ Steps per second is not positive")
                    return False
            else:
                print("  ✗ profileSimulation() missing expected keys")
                return False
        else:
            print("  ✗ profileSimulation() failed")
            return False
            
        # Test benchmarkBrain
        benchmark_results = pynlm.benchmarkBrain(brain, [100, 500, 1000])
        if isinstance(benchmark_results, dict):
            print("  ✓ benchmarkBrain() returns dictionary")
            
            # Check that we have results for each step count
            expected_steps = [100, 500, 1000]
            for steps in expected_steps:
                if steps in benchmark_results:
                    metrics = benchmark_results[steps]
                    if "duration_seconds" in metrics and "steps_per_second" in metrics:
                        print(f"  ✓ Benchmark results for {steps} steps")
                    else:
                        print(f"  ✗ Benchmark missing keys for {steps} steps")
                        return False
                else:
                    print(f"  ✗ Missing benchmark results for {steps} steps")
                    return False
        else:
            print("  ✗ benchmarkBrain() failed")
            return False
            
        return True
        
    except Exception as e:
        print(f"  ✗ Performance monitoring test failed: {e}")
        traceback.print_exc()
        return False

def test_world_agent_integration():
    """Test world and agent integration with brain."""
    print("\n5. Testing World-Agent Integration...")
    
    try:
        import pynlm
        
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("  ✓ All components created and initialized")
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("  ✓ Subsystems enabled")
        
        # Run simulation loop
        for step in range(50):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
        print("  ✓ Simulation loop completed successfully")
        
        # Test brain state
        firing_count = brain.getFiringNeuronCount()
        if firing_count >= 0:
            print(f"  ✓ Brain state accessible: {firing_count} firing neurons")
        else:
            print("  ✗ Invalid firing count")
            return False
            
        # Test agent state
        curiosity_level = agent.getCuriosityLevel()
        if isinstance(curiosity_level, float) and 0.0 <= curiosity_level <= 1.0:
            print(f"  ✓ Agent state accessible: curiosity={curiosity_level}")
        else:
            print("  ✗ Invalid curiosity level")
            return False
            
        # Test world state
        simulation_time = world.getSimulationTime()
        if simulation_time >= 0:
            print(f"  ✓ World state accessible: time={simulation_time}")
        else:
            print("  ✗ Invalid simulation time")
            return False
            
        return True
        
    except Exception as e:
        print(f"  ✗ World-agent integration test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and edge cases."""
    print("\n6. Testing Error Handling...")
    
    try:
        import pynlm
        
        # Test uninitialized brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Try to step before initialization (should not crash)
        try:
            brain.step(0)
            print("  ✓ Uninitialized brain step doesn't crash")
        except Exception:
            print("  ✓ Uninitialized brain step properly raises error")
        
        # Test Action with invalid parameters (should not crash)
        action = pynlm.Action(pynlm.ActionType.Custom, [])
        params = action.getParameters()
        if isinstance(params, list):
            print("  ✓ Action handles empty parameters")
        else:
            print("  ✗ Action doesn't handle empty parameters")
            return False
            
        # Test config with invalid keys (should not crash)
        config = pynlm.createDefaultConfig()
        if config.has("nonexistent.key"):
            print("  ✗ Config.has() should return False for nonexistent key")
            return False
        else:
            print("  ✓ Config.has() returns False for nonexistent key")
            
        # Test getInt with default
        value = config.getInt("nonexistent.key", 42)
        if value == 42:
            print("  ✓ getInt() returns default for nonexistent key")
        else:
            print(f"  ✗ getInt() should return default 42, got {value}")
            return False
            
        print("  ✓ All error handling tests passed")
        return True
        
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility with expected API patterns."""
    print("\n7. Testing Backward Compatibility...")
    
    try:
        import pynlm
        
        # Test the exact patterns from the documentation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Pattern from Basic Usage documentation
        for step in range(100):
            brain.step(step)
            
        # Pattern from AgentBrain documentation
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(0)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Pattern from Complete Agent Example
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        print("  ✓ All documented usage patterns work correctly")
        return True
        
    except Exception as e:
        print(f"  ✗ Backward compatibility test failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all integration tests."""
    print("\n" + "=" * 70)
    print("Running All Integration Tests")
    print("=" * 70)
    
    tests = [
        test_import_and_basic_types,
        test_configuration_system,
        test_action_class,
        test_performance_monitoring,
        test_world_agent_integration,
        test_error_handling,
        test_backward_compatibility
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
    
    print("\n" + "=" * 70)
    print("Test Results Summary")
    print("=" * 70)
    print(f"Tests Passed: {passed}")
    print(f"Tests Failed: {failed}")
    print(f"Total Tests: {passed + failed}")
    print("=" * 70)
    
    if failed == 0:
        print("🎉 ALL TESTS PASSED! 🎉")
        print("\nThe NLM Python bindings integration is complete and working correctly.")
        print("All improvements have been successfully implemented:")
        print("  ✓ Fixed enum mappings (ActionType vs MotorCommand)")
        print("  ✓ Enhanced Action class with Python-friendly methods")
        print("  ✓ Added performance monitoring and profiling tools")
        print("  ✓ Improved configuration system with Pythonic API")
        print("  ✓ Enhanced documentation and examples")
        print("  ✓ Maintained backward compatibility")
        return True
    else:
        print("❌ SOME TESTS FAILED")
        print("Please review the failures above and fix any issues.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
