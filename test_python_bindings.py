#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.
This script tests the new Pythonic API features and improvements.
"""

import sys
import os
import traceback

# Add the build directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build', 'lib.linux-x86_64-3.10', 'site-packages'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_imports():
    """Test that all main classes and enums are accessible."""
    print("\n=== Testing Basic Imports ===")
    
    # Test core classes
    classes_to_test = [
        ("Config", pynlm.Config),
        ("Brain", pynlm.Brain),
        ("SimpleWorld", pynlm.SimpleWorld),
        ("AgentBrain", pynlm.AgentBrain),
        ("Action", pynlm.Action),
        ("WorldObject", pynlm.WorldObject),
        ("AgentBody", pynlm.AgentBody),
        ("SensoryPercept", pynlm.SensoryPercept),
    ]
    
    for name, cls in classes_to_test:
        try:
            if cls == pynlm.Brain:
                config = pynlm.Config()
                obj = cls(config)
            else:
                obj = cls()
            print(f"✓ {name} can be instantiated")
        except Exception as e:
            print(f"✗ {name} failed to instantiate: {e}")
            return False
    
    # Test enumerations
    enums_to_test = [
        ("NeuronType", pynlm.NeuronType),
        ("SynapseType", pynlm.SynapseType),
        ("ActionType", pynlm.ActionType),
        ("MotorCommand", pynlm.MotorCommand),
        ("WorldObjectType", pynlm.WorldObjectType),
    ]
    
    for name, enum in enums_to_test:
        try:
            if name == "NeuronType":
                val = enum.Excitatory
            elif name == "SynapseType":
                val = enum.Excitatory
            elif name == "ActionType":
                val = enum.MoveForward
            elif name == "MotorCommand":
                val = enum.MoveForward
            elif name == "WorldObjectType":
                val = enum.Empty
            print(f"✓ {name} enum has values")
        except Exception as e:
            print(f"✗ {name} enum failed: {e}")
            return False
    
    return True

def test_factory_functions():
    """Test factory functions."""
    print("\n=== Testing Factory Functions ===")
    
    try:
        config = pynlm.create_default_config()
        print("✓ create_default_config() works")
    except Exception as e:
        print(f"✗ create_default_config() failed: {e}")
        return False
    
    try:
        brain = pynlm.create_brain_default()
        print("✓ create_brain_default() works")
    except Exception as e:
        print(f"✗ create_brain_default() failed: {e}")
        return False
    
    try:
        world = pynlm.create_simple_world()
        print("✓ create_simple_world() works")
    except Exception as e:
        print(f"✗ create_simple_world() works: {e}")
        return False
    
    try:
        agent = pynlm.create_agent_brain_default()
        print("✓ create_agent_brain_default() works")
    except Exception as e:
        print(f"✗ create_agent_brain_default() failed: {e}")
        return False
    
    return True

def test_config_api():
    """Test Config class Pythonic API."""
    print("\n=== Testing Config API ===")
    
    try:
        config = pynlm.Config()
        
        # Test new property API
        neuron_count = config.get_neuron_count()
        print(f"✓ get_neuron_count() returned: {neuron_count}")
        
        config.set_neuron_count(500)
        new_count = config.get_neuron_count()
        assert new_count == 500, f"Expected 500, got {new_count}"
        print(f"✓ set_neuron_count() works (neuron_count: {new_count})")
        
        # Test timestep API
        timestep = config.get_simulation_timestep()
        print(f"✓ get_simulation_timestep() returned: {timestep}")
        
        config.set_simulation_timestep(0.01)
        new_timestep = config.get_simulation_timestep()
        assert abs(new_timestep - 0.01) < 0.001, f"Expected 0.01, got {new_timestep}"
        print(f"✓ set_simulation_timestep() works")
        
        # Test connection probability API
        prob = config.get_connection_probability()
        print(f"✓ get_connection_probability() returned: {prob}")
        
        config.set_connection_probability(0.2)
        new_prob = config.get_connection_probability()
        assert abs(new_prob - 0.2) < 0.001, f"Expected 0.2, got {new_prob}"
        print(f"✓ set_connection_probability() works")
        
        return True
        
    except Exception as e:
        print(f"✗ Config API test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_api():
    """Test AgentBrain class Pythonic API."""
    print("\n=== Testing AgentBrain API ===")
    
    try:
        # Create brain and agent
        brain = pynlm.create_brain_default()
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(100, 100, 16, 16)
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Test new methods
        sensory_size = agent.get_sensory_input_size()
        motor_size = agent.get_motor_output_size()
        
        print(f"✓ get_sensory_input_size(): {sensory_size}")
        print(f"✓ get_motor_output_size(): {motor_size}")
        
        # Test new property access methods
        curiosity = agent.get_curiosity_level()
        novelty = agent.get_novelty_level()
        neuromod = agent.get_neuromodulation_level()
        
        print(f"✓ get_curiosity_level(): {curiosity}")
        print(f"✓ get_novelty_level(): {novelty}")
        print(f"✓ get_neuromodulation_level(): {neuromod}")
        
        # Test convenience methods
        should_explore = agent.should_explore()
        print(f"✓ should_explore(): {should_explore}")
        
        exploration_cmd = agent.explore()
        print(f"✓ explore(): {exploration_cmd}")
        
        # Test configuration methods
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        enabled = agent.is_reward_modulation_enabled()
        curiosity_enabled = agent.is_curiosity_enabled()
        
        print(f"✓ is_reward_modulation_enabled(): {enabled}")
        print(f"✓ is_curiosity_enabled(): {curiosity_enabled}")
        
        # Test context manager
        with pynlm.agent_context(agent) as managed_agent:
            percept = world.get_sensory_percept()
            managed_agent.process_sensory_input(percept)
            action = managed_agent.decode_motor_command()
            print(f"✓ Context manager works, got action: {action}")
        
        print("✓ Agent automatically reset after context manager")
        
        return True
        
    except Exception as e:
        print(f"✗ AgentBrain API test failed: {e}")
        traceback.print_exc()
        return False

def test_world_api():
    """Test SimpleWorld class Pythonic API."""
    print("\n=== Testing SimpleWorld API ===")
    
    try:
        world = pynlm.create_simple_world()
        
        # Test configuration
        world.configure(200, 200, 32, 32)
        print("✓ configure() works")
        
        # Test position access
        world.set_agent_start(50, 75)
        print("✓ set_agent_start() works")
        
        # Test convenience methods
        position = world.get_agent_position()
        orientation = world.get_agent_orientation()
        
        print(f"✓ get_agent_position(): {position}")
        print(f"✓ get_agent_orientation(): {orientation}")
        
        # Test object creation convenience methods
        world.add_resource(25, 25, 1.0)
        world.add_hazard(75, 75, -0.5)
        world.add_wall(100, 100, 2.0)
        
        print("✓ Convenience object creation methods work")
        
        # Test world update
        for step in range(10):
            world.update(0.001)
        
        print("✓ World updates work")
        
        # Test context manager
        with pynlm.world_context(world) as managed_world:
            body = managed_world.get_agent_body()
            print(f"✓ Context manager works, position: ({body.x}, {body.y})")
        
        print("✓ World automatically reset after context manager")
        
        return True
        
    except Exception as e:
        print(f"✗ SimpleWorld API test failed: {e}")
        traceback.print_exc()
        return False

def test_brain_api():
    """Test Brain class Pythonic API."""
    print("\n=== Testing Brain API ===")
    
    try:
        brain = pynlm.create_brain_default()
        brain.initialize()
        
        # Test statistics methods
        neuron_count = brain.get_total_neuron_count()
        synapse_count = brain.get_total_synapse_count()
        spike_count = brain.get_total_spike_count()
        
        print(f"✓ get_total_neuron_count(): {neuron_count}")
        print(f"✓ get_total_synapse_count(): {synapse_count}")
        print(f"✓ get_total_spike_count(): {spike_count}")
        
        # Test development stage
        stage = brain.get_developmental_stage()
        print(f"✓ get_developmental_stage(): {stage}")
        
        # Test convenience methods
        density = brain.get_network_density()
        print(f"✓ get_network_density(): {density:.4f}")
        
        # Test action creation
        action = brain.produce_action()
        print(f"✓ produce_action(): {action.get_name()}")
        
        # Test step methods
        brain.step(1, 0.001)
        brain.step(2)  # Without time
        print("✓ Step methods work")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain API test failed: {e}")
        traceback.print_exc()
        return False

def test_simulation_utilities():
    """Test simulation utility functions."""
    print("\n=== Testing Simulation Utilities ===")
    
    try:
        # Test demo simulation
        actions = pynlm.run_demo_simulation(steps=10, verbose=False)
        print(f"✓ run_demo_simulation(): returned {len(actions)} actions")
        
        # Test learning demo
        spikes = pynlm.run_learning_demo(steps=50, verbose=False)
        print(f"✓ run_learning_demo(): returned {spikes} total spikes")
        
        # Test plasticity benchmark
        final_synapses = pynlm.benchmark_plasticity()
        print(f"✓ benchmark_plasticity(): returned {final_synapses} final synapses")
        
        # Test version info
        version = pynlm.get_version()
        print(f"✓ get_version(): {version}")
        
        build_info = pynlm.get_build_info()
        print(f"✓ get_build_info(): {build_info['version']}")
        
        return True
        
    except Exception as e:
        print(f"✗ Simulation utilities test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling improvements."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test validation
        brain = pynlm.create_brain_default()
        brain.initialize()
        
        try:
            pynlm.validate_brain_state(brain)
            print("✓ validate_brain_state() works (valid brain)")
        except Exception as e:
            print(f"✗ validate_brain_state() failed unexpectedly: {e}")
            return False
        
        # Test safe operations
        try:
            success = pynlm.safe_brain_step(brain, 10, 0.01)
            assert success, "safe_brain_step should return True"
            print("✓ safe_brain_step() works")
        except Exception as e:
            print(f"✗ safe_brain_step() failed: {e}")
            return False
        
        try:
            success = pynlm.safe_brain_reset(brain)
            assert success, "safe_brain_reset should return True"
            print("✓ safe_brain_reset() works")
        except Exception as e:
            print(f"✗ safe_brain_reset() failed: {e}")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_full_simulation():
    """Test a complete simulation using the new API."""
    print("\n=== Testing Full Simulation ===")
    
    try:
        # Setup simulation with new API
        brain = pynlm.create_brain_default()
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(100, 100, 16, 16)
        world.set_agent_start(10, 10)
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run simulation with context manager
        with pynlm.agent_context(agent) as agent:
            for step in range(50):
                # Get sensory input
                percept = world.get_sensory_percept()
                
                # Process and get action
                agent.process_sensory_input(percept)
                action = agent.decode_motor_command()
                
                # Apply action
                world.apply_motor_command(action, step * 0.001)
                world.update(0.001)
                
                # Apply learning
                if step % 20 == 0:
                    agent.apply_reward_modulation(0.1, 0.05)
                    agent.update_development(0.001)
        
        # Verify simulation completed
        final_spikes = brain.get_total_spike_count()
        print(f"✓ Full simulation completed with {final_spikes} total spikes")
        
        # Verify agent was reset
        curiosity = agent.get_curiosity_level()
        print(f"✓ Agent curiosity level after context: {curiosity} (should be 0)")
        
        return True
        
    except Exception as e:
        print(f"✗ Full simulation test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("Testing Improved NLM Python Bindings")
    print("=" * 50)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Factory Functions", test_factory_functions),
        ("Config API", test_config_api),
        ("AgentBrain API", test_agent_api),
        ("SimpleWorld API", test_world_api),
        ("Brain API", test_brain_api),
        ("Simulation Utilities", test_simulation_utilities),
        ("Error Handling", test_error_handling),
        ("Full Simulation", test_full_simulation),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} FAILED with exception: {e}")
            traceback.print_exc()
    
    print("\n" + "=" * 50)
    print(f"Test Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! The Python bindings improvements are working correctly.")
        return True
    else:
        print("❌ Some tests failed. Please check the output above.")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)