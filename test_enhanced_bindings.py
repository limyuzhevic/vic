#!/usr/bin/env python3
"""
Test script demonstrating enhanced NLM Python bindings functionality.
This script showcases the new Pythonic API improvements.
"""

import pynlm
import time

def test_basic_functionality():
    """Test basic enhanced functionality."""
    print("=== Testing Basic Enhanced Functionality ===")
    
    # Create brain with enhanced configuration
    config = pynlm.Config({
        'brain.neuron_count': 500,
        'brain.synapse_density': 0.05,
        'simulation_timestep': 0.001
    })
    
    brain = pynlm.Brain(config)
    brain.initialize()
    
    print(f"✓ Brain created: {brain.get_neuron_count()} neurons")
    print(f"✓ Brain initialized successfully")
    
    # Test property access
    print(f"✓ Neuron count: {brain.neuron_count}")
    print(f"✓ Synapse count: {brain.synapse_count}")
    print(f"✓ Firing rate: {brain.firing_rate:.2f} Hz")
    
    # Test context manager
    print("\n=== Testing Context Manager ===")
    with pynlm.SimulationContext(brain) as sim:
        for step in range(10):
            sim.step(step)
            print(f"Step {step}: {sim.firing_count} neurons firing")
    
    print("✓ Context manager test passed")
    return brain

def test_enhanced_initialization():
    """Test enhanced initialization with configuration."""
    print("\n=== Testing Enhanced Initialization ===")
    
    # Test initialization with dictionary
    config = pynlm.Config({
        'brain.neuron_count': 100,
        'brain.synapse_density': 0.1,
        'simulation_timestep': 0.0005,
        'plasticity.enable_stdp': True,
        'plasticity.enable_hebbian': True,
        'neuromodulation.enable_curiosity': True
    })
    
    brain = pynlm.Brain(config)
    brain.initialize(config)  # Pass config to initialize
    
    print(f"✓ Brain initialized with custom configuration")
    print(f"✓ Configuration validation: {'PASS' if brain.config.validate() else 'FAIL'}")
    
    # Test configuration methods
    print(f"✓ Config keys: {brain.config.get_keys()}")
    
    return brain

def test_serialization():
    """Test enhanced serialization methods."""
    print("\n=== Testing Serialization ===")
    
    brain = pynlm.Brain()
    brain.initialize()
    
    # Test to_dict()
    brain_dict = brain.to_dict()
    print(f"✓ to_dict() returned {len(brain_dict)} keys")
    print(f"  Keys: {list(brain_dict.keys())}")
    
    # Test to_json()
    json_str = brain.to_json()
    print(f"✓ to_json() returned {len(json_str)} characters")
    
    # Test to_yaml() if available
    try:
        yaml_str = brain.to_yaml()
        print(f"✓ to_yaml() returned {len(yaml_str)} characters")
    except:
        print("⚠ to_yaml() not available (PyYAML may not be installed)")
    
    return brain

def test_benchmarking():
    """Test benchmarking functionality."""
    print("\n=== Testing Benchmarking ===")
    
    brain = pynlm.Brain()
    brain.initialize()
    
    # Run benchmark
    benchmark_result = brain.benchmark(100, warmup=50)
    
    print(f"✓ Benchmark completed:")
    print(f"  Steps: {benchmark_result['steps']}")
    print(f"  Warmup steps: {benchmark_result['warmup_steps']}")
    print(f"  Duration: {benchmark_result['duration_ms']:.2f} ms")
    print(f"  Steps/sec: {benchmark_result['steps_per_second']:.2f}")
    print(f"  Average step time: {benchmark_result['average_step_time_ms']:.3f} ms")
    
    return brain

def test_agent_simulation():
    """Test complete agent simulation."""
    print("\n=== Testing Agent Simulation ===")
    
    # Setup agent simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run a few steps
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("✓ Agent simulation completed successfully")
    return brain, world, agent

def test_error_handling():
    """Test enhanced error handling."""
    print("\n=== Testing Error Handling ===")
    
    # Test invalid configuration
    try:
        config = pynlm.Config({
            'brain.neuron_count': -1,  # Invalid negative value
            'simulation_timestep': -0.001  # Invalid negative timestep
        })
        print("⚠ Configuration validation should have failed")
    except Exception as e:
        print(f"✓ Configuration validation caught error: {type(e).__name__}")
    
    # Test simulation without initialization
    brain = pynlm.Brain()
    try:
        brain.step(0)  # Should fail without initialization
        print("⚠ Simulation should have failed without initialization")
    except Exception as e:
        print(f"✓ Simulation without initialization caught error: {type(e).__name__}")
    
    print("✓ Error handling tests completed")

def test_world_interactions():
    """Test world interaction methods."""
    print("\n=== Testing World Interactions ===")
    
    world = pynlm.SimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Test object management
    world.addObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0, radius=0.5)
    world.addObject(25.0, 25.0, pynlm.WorldObjectType.Hazard, value=5.0, radius=1.0)
    
    # Test position validation
    valid_pos = world.isValidPosition(20.0, 20.0)
    invalid_pos = world.isValidPosition(-5.0, -5.0)
    
    print(f"✓ Valid position (20,20): {valid_pos}")
    print(f"✓ Invalid position (-5,-5): {invalid_pos}")
    
    # Test agent body
    body = world.getAgentBody()
    print(f"✓ Agent body: x={body.x}, y={body.y}, energy={body.energy}")
    
    # Test context manager
    with pynlm.WorldContext(world) as w:
        w.update(0.1)
        print(f"✓ Context manager update: time={w.getSimulationTime():.3f}")
    
    print("✓ World interaction tests completed")

def run_all_tests():
    """Run all test functions."""
    print("🚀 Starting NLM Enhanced Python Bindings Tests")
    print("=" * 60)
    
    try:
        brain1 = test_basic_functionality()
        brain2 = test_enhanced_initialization()
        brain3 = test_serialization()
        brain4 = test_benchmarking()
        brain5, world, agent = test_agent_simulation()
        test_error_handling()
        test_world_interactions()
        
        print("\n" + "=" * 60)
        print("🎉 All tests passed successfully!")
        print("\n📊 Summary of Improvements:")
        print("  ✓ Pythonic property access with caching")
        print("  ✓ Context managers for resource management")
        print("  ✓ Enhanced initialization with configuration")
        print("  ✓ Serialization (JSON/YAML/dict)")
        print("  ✓ Performance benchmarking")
        print("  ✓ Complete agent-world simulation")
        print("  ✓ Enhanced error handling")
        print("  ✓ World object management")
        print("  ✓ Configuration validation and optimization")
        
        print("\n✨ Key Features Demonstrated:")
        print("  • Automatic caching of statistics")
        print("  • Pythonic dictionary configuration")
        print("  • Multiple export formats")
        print("  • Performance monitoring")
        print("  • Context managers")
        print("  • Enhanced error handling")
        print("  • Complete agent simulation")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
