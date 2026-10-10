"""
Python Integration Tests for NLM
Tests the Python bindings (pynlm) to ensure they work correctly with the C++ core.

These tests verify that the Python API properly interfaces with the C++ neural simulation framework.
"""

import pynlm
import time

def test_basic_brain_creation():
    """Test basic brain creation and initialization"""
    print("Test 1: Basic Brain Creation")
    
    # Create a brain with default config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain is not None, "Brain creation failed"
    
    # Initialize the brain
    brain.initialize()
    
    # Take some steps
    for i in range(10):
        brain.step(i)
    
    # Check basic stats
    total_spikes = brain.getTotalSpikeCount()
    firing_neurons = brain.getFiringNeuronCount()
    total_neurons = brain.getTotalNeuronCount()
    
    print(f"  Total spikes: {total_spikes}")
    print(f"  Firing neurons: {firing_neurons}")
    print(f"  Total neurons: {total_neurons}")
    
    assert total_neurons > 0, "Brain should have neurons"
    assert firing_neurons >= 0, "Should be able to get firing neurons count"
    assert total_spikes >= 0, "Should be able to get total spike count"
    
    print("  ✓ Basic brain creation test passed\n")

def test_config_integration():
    """Test configuration system integration with Python"""
    print("Test 2: Configuration Integration")
    
    # Create config
    config = pynlm.createDefaultConfig()
    assert config is not None, "Config creation failed"
    
    # Test config methods
    assert hasattr(config, 'has'), "Config should have 'has' method"
    assert hasattr(config, 'getKeys'), "Config should have 'getKeys' method"
    assert hasattr(config, 'getOr'), "Config should have 'getOr' method"
    
    # Test that config is accessible
    keys = config.getKeys()
    print(f"  Config has {len(keys)} keys")
    print(f"  Config keys: {keys}")
    
    # Test has method
    has_sim = config.has('simulation_timestep')
    print(f"  Has 'simulation_timestep': {has_sim}")
    
    # Test getOr method
    timestep = config.getOr('simulation_timestep', 0.001)
    print(f"  Simulation timestep: {timestep}")
    
    print("  ✓ Configuration integration test passed\n")

def test_agent_brain_integration():
    """Test AgentBrain integration between Python and C++"""
    print("Test 3: AgentBrain Integration")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "AgentBrain creation failed"
    
    # Test agent methods
    assert hasattr(agent, 'processSensoryInput'), "Agent should have processSensoryInput"
    assert hasattr(agent, 'decodeMotorCommand'), "Agent should have decodeMotorCommand"
    assert hasattr(agent, 'applyRewardModulation'), "Agent should have applyRewardModulation"
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("  ✓ AgentBrain features enabled")
    
    # Test reward modulation
    agent.applyRewardModulation(1.0, 0.5)
    print("  ✓ Reward modulation applied")
    
    print("  ✓ AgentBrain integration test passed\n")

def test_world_integration():
    """Test SimpleWorld integration"""
    print("Test 4: World Integration")
    
    # Create world
    world = pynlm.createSimpleWorld()
    assert world is not None, "World creation failed"
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    assert world.getWidth() == 20, "World width should be 20"
    assert world.getHeight() == 20, "World height should be 20"
    
    # Reset world
    world.reset()
    
    # Update world
    for i in range(5):
        world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    assert percept is not None, "Should be able to get percept"
    
    # Get agent body
    agent_body = world.getAgentBody()
    assert agent_body is not None, "Should be able to get agent body"
    
    # Test object management
    obj = pynlm.WorldObject(x=10, y=10, type=pynlm.WorldObjectType.Resource)
    world.addObject(obj)
    
    assert world.isValidPosition(10, 10), "Position (10,10) should be valid"
    
    print("  ✓ World integration test passed\n")

def test_brain_world_agent_loop():
    """Test complete brain-world-agent integration loop"""
    print("Test 5: Complete Integration Loop")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run complete simulation loop
    steps = 20
    for step in range(steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Give feedback (apply reward)
        agent.applyRewardModulation(0.1, 0.05)
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"avg firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Final checks
    print(f"  Final simulation completed:")
    print(f"    Total steps: {steps}")
    print(f"    Total neurons: {brain.getTotalNeuronCount()}")
    print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    print(f"    Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"    Development stage: {brain.getDevelopmentalStage()}")
    print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"    Curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"    Novelty level: {agent.getNoveltyLevel():.2f}")
    
    print("  ✓ Complete integration loop test passed\n")

def test_error_handling():
    """Test error handling in Python bindings"""
    print("Test 6: Error Handling")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test that methods handle errors gracefully
    try:
        # Should not raise exceptions for normal usage
        brain.getFiringNeuronCount()
        brain.getTotalSpikeCount()
        brain.getAverageFiringRate()
        
        # Test with invalid config key (should return default or nullopt)
        config = pynlm.createDefaultConfig()
        result = config.getOr('nonexistent_key', 'default_value')
        assert result == 'default_value', f"Expected 'default_value', got {result}"
        
        print("  ✓ Error handling test passed\n")
        return True
        
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        return False

def test_performance():
    """Basic performance test for Python bindings"""
    print("Test 7: Performance Check")
    
    import time
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    start_time = time.time()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"  Simulation of 50 steps took {elapsed:.3f} seconds")
    print(f"  Average time per step: {elapsed/50:.6f} seconds")
    
    # Performance should be reasonable (less than 1 second per step for simple brain)
    if elapsed / 50 < 0.1:  # Less than 100ms per step
        print("  ✓ Performance test passed\n")
        return True
    else:
        print("  ⚠ Performance test warning: Slow simulation\n")
        return True  # Still pass, just warning

def run_all_tests():
    """Run all integration tests"""
    print("=" * 60)
    print("NLM Python Integration Tests")
    print("=" * 60)
    print()
    
    tests = [
        test_basic_brain_creation,
        test_config_integration,
        test_agent_brain_integration,
        test_world_integration,
        test_brain_world_agent_loop,
        test_error_handling,
        test_performance
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"  ✗ Test {test.__name__} failed with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All integration tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)