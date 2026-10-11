#!/usr/bin/env python3
"""
Quick Verification Test for Enhanced NLM Framework

This script performs a quick verification test to ensure that the enhanced
NLM framework is working correctly after all improvements.
"""

import pynlm as nlm
import numpy as np

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Test 1: Create default config
    config = nlm.createDefaultConfig()
    assert config is not None, "Failed to create default config"
    print("✓ Default config creation successful")
    
    # Test 2: Create brain with config
    brain = nlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    print("✓ Brain creation successful")
    
    # Test 3: Initialize brain
    success = brain.initialize()
    assert success, "Failed to initialize brain"
    print("✓ Brain initialization successful")
    
    # Test 4: Get statistics
    stats = brain.get_statistics()
    assert isinstance(stats, dict), "Statistics should be a dictionary"
    assert "total_neurons" in stats, "Statistics should contain total_neurons"
    assert "total_synapses" in stats, "Statistics should contain total_synapses"
    print("✓ Statistics retrieval successful")
    print(f"  Network: {stats['total_neurons']} neurons, {stats['total_synapses']} synapses")
    
    return brain

def test_advanced_api():
    """Test advanced Python API features."""
    print("\nTesting advanced API features...")
    
    # Create brain with enhanced configuration
    config = nlm.createPresetConfig("complex")
    brain = nlm.createBrain(config)
    success = brain.initialize()
    assert success, "Failed to initialize complex brain"
    
    # Test 1: Network topology analysis
    topology = brain.analyze_network_topology()
    assert isinstance(topology, dict), "Topology should be a dictionary"
    assert "region_count" in topology, "Topology should contain region_count"
    assert "overall_stats" in topology, "Topology should contain overall_stats"
    print("✓ Network topology analysis successful")
    
    # Test 2: Learning experiment
    experiment_results = brain.create_learning_experiment({
        "duration": 500,
        "pattern_size": 20,
        "learning_rate": 0.01,
    })
    assert isinstance(experiment_results, dict), "Experiment results should be a dictionary"
    assert "learning_effectiveness" in experiment_results, "Should contain learning effectiveness"
    print("✓ Learning experiment successful")
    print(f"  Learning effectiveness: {experiment_results['learning_effectiveness']:.2f}")
    
    # Test 3: Plasticity parameter control
    brain.set_plasticity_parameters({
        "stdp_ltp_weight": 0.03,
        "stdp_ltd_weight": 0.02,
        "enable_hebbian": True,
        "enable_stdp": True,
        "enable_structural": False,
    })
    print("✓ Plasticity parameter control successful")
    
    # Test 4: Enhanced statistics
    enhanced_stats = brain.get_statistics()
    enhanced_stats.update(brain.get_plasticity_status())
    enhanced_stats.update(brain.get_development_status())
    print("✓ Enhanced statistics collection successful")
    
    return brain

def test_agent_simulation():
    """Test agent-based simulation capabilities."""
    print("\nTesting agent simulation...")
    
    # Create world and agent
    world = nlm.createSimpleWorld()
    world.configure(400, 300, 160, 120)
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent_brain = nlm.createAgentBrain(brain)
    
    # Initialize
    success = brain.initialize()
    agent_brain.initialize(world)
    assert success, "Failed to initialize for agent simulation"
    
    # Run simple simulation
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Produce action
        action = brain.produceAction()
        if action:
            # Apply action
            result = world.applyMotorCommand(
                agent_brain.decodeMotorCommand(),
                step * 0.001
            )
        
        # Update world
        world.update(0.001)
    
    # Verify results
    body = world.getAgentBody()
    assert body is not None, "Agent body should not be None"
    assert body.energy >= 0.0, "Energy should be non-negative"
    assert body.health >= 0.0, "Health should be non-negative"
    
    print("✓ Agent simulation successful")
    print(f"  Final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  Final energy: {body.energy:.1f}")
    print(f"  Final health: {body.health:.1f}")
    
    return brain, world, agent_brain

def test_world_integration():
    """Test world integration features."""
    print("\nTesting world integration...")
    
    world = nlm.createSimpleWorld()
    world.configure(800, 600, 320, 240)
    
    # Test 1: Add objects to world
    # Add resources
    for i in range(3):
        obj = nlm.WorldObject(100 + i * 50, 100, nlm.WorldObjectType.Resource, 10.0)
        world.addObject(obj)
    
    # Add hazards
    for i in range(2):
        obj = nlm.WorldObject(200 + i * 50, 200, nlm.WorldObjectType.Hazard, 0.0, 0.5)
        world.addObject(obj)
    
    # Add walls
    for i in range(2):
        obj = nlm.WorldObject(300 + i * 100, 400, nlm.WorldObjectType.Wall, 0.0, 0.8)
        world.addObject(obj)
    
    print("✓ World object management successful")
    
    # Test 2: Check object validation
    assert world.isValidPosition(400, 300), "Should be valid position"
    assert not world.isValidPosition(-10, -10), "Should be invalid position"
    print("✓ Position validation successful")
    
    # Test 3: Get agent start
    world.setAgentStart(400, 300)
    print("✓ Agent start setting successful")
    
    # Test 4: Export/import world state
    state = world.export_world_state()
    assert isinstance(state, dict), "World state should be a dictionary"
    assert "width" in state, "State should contain width"
    assert "height" in state, "State should contain height"
    assert "agent" in state, "State should contain agent state"
    
    # Import the same state
    import_world_result = world.import_world_state(state)
    assert "successfully" in import_world_result, "Import should succeed"
    print("✓ World state export/import successful")
    
    # Test 5: Print world summary
    world.print_world_summary()
    print("✓ World summary display successful")
    
    return world

def run_performance_test():
    """Run a basic performance test."""
    print("\nRunning performance test...")
    
    import time
    
    config = nlm.createPresetConfig("simple")
    brain = nlm.createBrain(config)
    success = brain.initialize()
    assert success, "Failed to initialize for performance test"
    
    # Time the initialization
    start_time = time.time()
    for step in range(1000):
        brain.step(step, step * 0.001)
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_second = 1000 / duration
    
    print(f"✓ Performance test successful")
    print(f"  Processed 1000 steps in {duration:.2f} seconds")
    print(f"  Performance: {steps_per_second:.0f} steps/second")
    
    return steps_per_second

def test_error_handling():
    """Test error handling and edge cases."""
    print("\nTesting error handling...")
    
    # Test 1: Invalid configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Try to access non-existent key
    value = config.get("non_existent_key", "default")
    assert value == "default", "Should return default value for missing key"
    print("✓ Default value handling successful")
    
    # Test 2: Empty learning experiment
    empty_results = brain.create_learning_experiment({"duration": 0})
    assert "error" not in empty_results, "Empty experiment should not error"
    print("✓ Empty experiment handling successful")
    
    print("✓ All error handling tests passed")

def main():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Framework Verification Test")
    print("=" * 60)
    
    try:
        # Run all tests
        brain1 = test_basic_functionality()
        brain2 = test_advanced_api()
        brain3, world, agent_brain = test_agent_simulation()
        world2 = test_world_integration()
        performance = run_performance_test()
        test_error_handling()
        
        # Print summary
        print("\n" + "=" * 60)
        print("VERIFICATION SUMMARY")
        print("=" * 60)
        print("✓ All basic functionality tests passed")
        print("✓ All advanced API tests passed")
        print("✓ All agent simulation tests passed")
        print("✓ All world integration tests passed")
        print("✓ Performance test completed")
        print("✓ Error handling tests passed")
        print(f"\nOverall Performance: {performance:.0f} steps/second")
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED SUCCESSFULLY!")
        print("=" * 60)
        print("\nThe enhanced NLM framework is working correctly.")
        print("You can now use the framework for neural system research and development.")
        
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)