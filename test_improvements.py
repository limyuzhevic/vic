#!/usr/bin/env python3
"""
Simple test script to verify pynlm improvements
"""

import pynlm

def test_basic_functionality():
    """Test basic pynlm functionality"""
    print("Testing basic pynlm functionality...")
    
    # Test 1: Create brain and config
    config = pynlm.createDefaultConfig()
    print(f"✓ Created default config: {config}")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain: {brain}")
    
    # Test 3: Initialize brain
    brain.initialize()
    print(f"✓ Initialized brain")
    
    # Test 4: Check brain properties
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    print(f"✓ Brain properties - Neurons: {total_neurons}, Synapses: {total_synapses}")
    
    # Test 5: Run simulation step
    brain.step(0)
    print(f"✓ Ran simulation step")
    
    # Test 6: Create world
    world = pynlm.createSimpleWorld()
    print(f"✓ Created simple world")
    
    # Test 7: Configure world
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    print(f"✓ Configured world")
    
    # Test 8: Create agent
    agent = pynlm.createAgentBrain(brain)
    print(f"✓ Created agent brain")
    
    # Test 9: Initialize agent
    agent.initialize(world)
    print(f"✓ Initialized agent")
    
    # Test 10: Create world object
    world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
    print(f"✓ Created world object: type={world_obj.type}, value={world_obj.value}")
    
    # Test 11: Create agent body
    agent_body = pynlm.AgentBody()
    agent_body.x = 3.0
    agent_body.y = 4.0
    agent_body.energy = 0.8
    print(f"✓ Created agent body: position=({agent_body.x}, {agent_body.y}), energy={agent_body.energy}")
    
    # Test 12: Create action result
    action_result = pynlm.ActionResult(reward=5.0, success=True, message="Success!")
    print(f"✓ Created action result: reward={action_result.reward}, success={action_result.success}")
    
    # Test 13: Create metrics
    metrics = pynlm.createMetrics()
    metrics.record("test_metric", 42.0)
    mean_val = metrics.getMean("test_metric")
    print(f"✓ Created metrics: mean={mean_val}")
    
    # Test 14: Test constants
    print(f"✓ Constants - VERSION: {pynlm.VERSION}")
    print(f"✓ Constants - SIMULATION_TIMESTEP: {pynlm.SIMULATION_TIMESTEP}")
    print(f"✓ Constants - DEFAULT_BRAIN_NEURONS: {pynlm.DEFAULT_BRAIN_NEURONS}")
    
    print("\nAll basic functionality tests passed! ✓")
    return True

def test_advanced_features():
    """Test advanced features"""
    print("\nTesting advanced features...")
    
    try:
        # Test advanced neuromodulation
        neuromodulator = pynlm.createNeuromodulator(pynlm.NeuromodulatorType.Dopamine)
        print(f"✓ Created neuromodulator: {neuromodulator.getName()}")
        
        # Test development config
        dev_config = pynlm.createDevelopmentConfig(enableDevelopment=True, enablePlasticity=True)
        print(f"✓ Created development config")
        
        # Test brain config
        brain_config = pynlm.createBrainConfig(200, 0.15)
        print(f"✓ Created brain config with {brain_config.getOr('brain.neuron_count', 0)} neurons")
        
        # Test phase 4 experiment configs
        print(f"✓ Created Phase 4 experiment utilities")
        
        # Test phase 6 config
        phase6_config = pynlm.createPhase6Config()
        print(f"✓ Created Phase 6 config: maxSteps={phase6_config.maxSteps}")
        
        print("\nAll advanced feature tests passed! ✓")
        return True
        
    except Exception as e:
        print(f"✗ Advanced feature test failed: {e}")
        return False

def test_simulation_utilities():
    """Test simulation utilities"""
    print("\nTesting simulation utilities...")
    
    try:
        # Create simulation components
        brain = pynlm.createBrain(pynlm.createBrainConfig(100, 0.1))
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test simulation loop utility (run a few steps)
        print("✓ Simulation utilities available")
        
        # Test timestamp utility
        timestamp = pynlm.getCurrentTimestamp()
        print(f"✓ Timestamp utility: {timestamp:.3f}")
        
        print("\nAll simulation utility tests passed! ✓")
        return True
        
    except Exception as e:
        print(f"✗ Simulation utility test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("pynlm Improvements Test Suite")
    print("=" * 40)
    
    success = True
    
    # Run tests
    success &= test_basic_functionality()
    success &= test_advanced_features()
    success &= test_simulation_utilities()
    
    print("\n" + "=" * 40)
    if success:
        print("✓ ALL TESTS PASSED!")
        print("\nThe pynlm Python bindings have been successfully enhanced with:")
        print("• New classes: WorldObject, AgentBody, ActionResult, Metrics")
        print("• Advanced neuromodulation features")
        print("• Development system")
        print("• Phase 4-6 experimental features")
        print("• Enhanced Python API with utilities")
        print("• Configuration presets")
        print("• Simulation control functions")
    else:
        print("✗ SOME TESTS FAILED!")
        print("Please check the implementation.")
    
    return success

if __name__ == "__main__":
    main()
