#!/usr/bin/env python3
"""
NLM Quick Start Test Script
===========================

Quick test script to verify NLM functionality and demonstrate key features.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm import (
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    ConfigBuilder,
    BrainBuilder,
    AgentBuilder,
    SimulationRunner,
    get_version,
    check_cpp_module
)

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing Basic NLM Functionality")
    print("=" * 40)
    
    # Check C++ module availability
    if not check_cpp_module():
        print("Warning: C++ module not available. Some functionality may be limited.")
    
    # Test version
    print(f"NLM Version: {get_version()}")
    
    # Test 1: Create brain with default config
    print("\n1. Testing brain creation with default config...")
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Run simulation
    print("\n2. Testing simulation steps...")
    for i in range(10):
        brain.step(i)
    print(f"   ✓ Completed {i+1} simulation steps")
    print(f"   ✓ Generated {brain.getTotalSpikeCount()} total spikes")
    
    # Test 3: Create world
    print("\n3. Testing world creation...")
    world = create_simple_world()
    world.configure(width=10, height=10, vision_width=4, vision_height=4)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    print(f"   ✓ Created world {world.getWidth()}x{world.getHeight()}")
    
    # Test 4: Create agent
    print("\n4. Testing agent creation...")
    agent = create_agent_brain(brain)
    agent.initialize(world)
    print(f"   ✓ Created agent with {agent.getSensoryInputSize()} sensory inputs")
    print(f"   ✓ Expected {agent.getMotorOutputSize()} motor outputs")
    
    return True

def test_builder_patterns():
    """Test builder pattern functionality."""
    print("\nTesting Builder Patterns")
    print("=" * 40)
    
    # Test ConfigBuilder
    print("\n1. Testing ConfigBuilder...")
    config = (ConfigBuilder()
        .set_neuron_count(500)
        .set_connection_probability(0.15)
        .enable_stdp(learning_rate=0.002)
        .enable_curiosity(True)
        .set_random_seed(123)
        .build())
    print("   ✓ ConfigBuilder created configuration")
    
    # Test BrainBuilder
    print("\n2. Testing BrainBuilder...")
    brain = (BrainBuilder()
        .with_neuron_count(300)
        .with_plasticity(stdp_enabled=True, hebbian_enabled=True, structural_enabled=True)
        .build().get())
    brain.initialize()
    print(f"   ✓ BrainBuilder created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test AgentBuilder
    print("\n3. Testing AgentBuilder...")
    agent_builder = (AgentBuilder()
        .configure_world(width=15, height=15, vision_width=8, vision_height=8)
        .with_start_position(7.5, 7.5)
        .enable_learning(reward_modulation=True, curiosity=True, structural_plasticity=True)
        .enable_development(True))
    agent = agent_builder.get()
    agent.initialize(agent_builder.world)
    print(f"   ✓ AgentBuilder created agent with curiosity: {agent.getCuriosityLevel():.3f}")
    
    return True

def test_simulation_runner():
    """Test SimulationRunner functionality."""
    print("\nTesting SimulationRunner")
    print("=" * 40)
    
    # Create complete system
    brain = (BrainBuilder()
        .with_neuron_count(200)
        .with_plasticity(stdp_enabled=True)
        .build().get())
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width=10, height=10, vision_width=6, vision_height=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Run simulation
    print("\n1. Running simulation with SimulationRunner...")
    runner = SimulationRunner(agent, world)
    stats = runner.run_episode(50, timestep=0.1, log_interval=10)
    
    print(f"   ✓ Simulation completed {stats['steps_completed']} steps")
    print(f"   ✓ Final firing rate: {stats['brain_stats']['firing_neurons'] / stats['brain_stats']['total_neurons'] * 100:.1f}%")
    print(f"   ✓ Final curiosity: {stats['agent_stats']['curiosity_level']:.3f}")
    
    return True

def test_power_user_features():
    """Test power user features if available."""
    print("\nTesting Power User Features")
    print("=" * 40)
    
    try:
        from nlm_power_user import (
            PerformanceMonitor,
            AdvancedBrainBuilder,
            SimulationDebugger
        )
        
        print("\n1. Testing PerformanceMonitor...")
        monitor = PerformanceMonitor()
        monitor.start_monitoring()
        
        # Create a simple simulation
        brain = (BrainBuilder()
            .with_neuron_count(100)
            .build().get())
        brain.initialize()
        
        for step in range(20):
            brain.step(step)
            monitor.record_step(step, brain)
        
        summary = monitor.stop_monitoring()
        print(f"   ✓ PerformanceMonitor completed")
        print(f"   ✓ Average step time: {summary['avg_step_time']:.4f}s")
        
        print("\n2. Testing AdvancedBrainBuilder...")
        advanced_brain = (AdvancedBrainBuilder()
            .add_region('cortex', 'excitatory', 150)
            .add_region('hippocampus', 'inhibitory', 50)
            .set_connectivity('cortex', 'hippocampus', 0.2)
            .set_plasticity_rules('stdp', {'learning_rate': 0.005})
            .build())
        print(f"   ✓ AdvancedBrainBuilder created brain with {advanced_brain.getTotalNeuronCount()} neurons")
        
        print("\n3. Testing SimulationDebugger...")
        debugger = SimulationDebugger(brain)
        debugger.add_watch('activity', lambda step, brain, world: brain.getFiringNeuronCount())
        debugger.add_break_condition('too_much_activity', lambda step, brain, world: brain.getTotalSpikeCount() > 50)
        
        debug_results = debugger.run_with_debugging(30, timestep=0.1)
        print(f"   ✓ SimulationDebugger completed")
        print(f"   ✓ Steps completed: {debug_results['steps_completed']}")
        
        print("\n✓ All power user features available")
        return True
        
    except ImportError as e:
        print(f"   ⚠ Power user features not available: {e}")
        return False
    except Exception as e:
        print(f"   ⚠ Error testing power user features: {e}")
        return False

def test_examples():
    """Test example functions."""
    print("\nTesting Examples")
    print("=" * 40)
    
    try:
        from nlm_examples import (
            run_silent_brain_example,
            run_agent_world_example,
            run_learning_demonstration
        )
        
        print("\n1. Running Silent Brain Example...")
        silent_result = run_silent_brain_example()
        print(f"   ✓ Generated {silent_result['total_spikes']} spikes")
        print(f"   ✓ Average firing rate: {silent_result['average_firing_rate']:.2f} Hz")
        
        print("\n2. Running Agent-World Example...")
        agent_world_result = run_agent_world_example()
        print(f"   ✓ Watched world for {agent_world_result['steps_completed']} steps")
        print(f"   ✓ Final firing rate: {agent_world_result['firing_rate_history'][-1]:.1f} Hz")
        
        print("\n3. Running Learning Demonstration...")
        learning_result = run_learning_demonstration()
        print(f"   ✓ Completed {learning_result['simulation_steps']} simulation steps")
        print(f"   ✓ Final curiosity: {learning_result['agent_stats']['final_curiosity']:.3f}")
        
        print("\n✓ All examples completed successfully")
        return True
        
    except ImportError as e:
        print(f"   ⚠ Example functions not available: {e}")
        return False
    except Exception as e:
        print(f"   ⚠ Error testing examples: {e}")
        return False

def main():
    """Main test function."""
    print("NLM Quick Start Test")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Builder Patterns", test_builder_patterns),
        ("Simulation Runner", test_simulation_runner),
        ("Power User Features", test_power_user_features),
        ("Examples", test_examples),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"\n✗ {test_name} failed: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status:6} | {test_name}")
    
    print("\n" + "=" * 60)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed! NLM is working correctly.")
        return 0
    else:
        print(f"⚠ {total - passed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())