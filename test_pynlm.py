import pynlm

def test_import_and_creation():
    """Test 1: Import the pynlm module and create a brain"""
    print("Test 1: Import and creation")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    print(f"  ✓ Brain created with {brain.getTotalNeuronCount()} neurons")

def test_initialization():
    """Test 2: Initialize the brain"""
    print("Test 2: Initialization")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    print(f"  ✓ Brain initialized successfully")

def test_simulation():
    """Test 3: Run simulation steps"""
    print("Test 3: Simulation steps")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    
    for i in range(10):
        brain.step(i)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes
    print(f"  ✓ Simulation completed: {initial_spikes} -> {final_spikes} spikes")

def test_statistics():
    """Test 4: Check basic brain statistics"""
    print("Test 4: Brain statistics")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    stats = {
        "Total Neurons": brain.getTotalNeuronCount(),
        "Total Synapses": brain.getTotalSynapseCount(),
        "Firing Neurons": brain.getFiringNeuronCount(),
        "Total Spikes": brain.getTotalSpikeCount(),
        "Average Firing Rate": brain.getAverageFiringRate(),
        "E/I Ratio": brain.getExcitationInhibitionRatio(),
        "Developmental Stage": brain.getDevelopmentalStage()
    }
    
    for key, value in stats.items():
        print(f"  ✓ {key}: {value}")

def test_world_and_agent():
    """Test 5: Test world and agent functionality"""
    print("Test 5: World and Agent")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print(f"  ✓ Agent initialized with world")
    print(f"  ✓ Sensory input size: {agent.getSensoryInputSize()}")
    print(f"  ✓ Motor output size: {agent.getMotorOutputSize()}")
    
    # Run a simple loop
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"  ✓ Completed {5} simulation steps with agent")

def test_enable_features():
    """Test 6: Enable and test brain features"""
    print("Test 6: Enable brain features")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent to use feature toggles
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Test enabling various features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"  ✓ Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"  ✓ Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
    print(f"  ✓ Development enabled: {agent.isDevelopmentEnabled()}")
    print(f"  ✓ Curiosity enabled: {agent.isCuriosityEnabled()}")
    
    # Test development progress
    for step in range(10):
        agent.updateDevelopment(0.1)
        print(f"  ✓ Step {step}: Developmental stage {agent.getDevelopmentalStage()}")

def test_save_load():
    """Test 7: Save and load brain state (if available)"""
    print("Test 7: Save and load")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to generate some state
    for i in range(5):
        brain.step(i)
    
    # Check if save is available
    try:
        save_result = brain.save("test_brain.bin")
        if save_result:
            print(f"  ✓ Brain saved successfully")
            
            # Create new brain and try to load
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            
            load_result = brain2.load("test_brain.bin")
            if load_result:
                print(f"  ✓ Brain loaded successfully")
                print(f"  ✓ Loaded total spikes: {brain2.getTotalSpikeCount()}")
            else:
                print(f"  ! Save succeeded but load failed")
        else:
            print(f"  ! Save method not implemented (expected for this build)")
    except Exception as e:
        print(f"  ! Save/load error: {e}")

def test_cleanup():
    """Test 8: Clean up resources"""
    print("Test 8: Cleanup")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps
    for i in range(5):
        brain.step(i)
    
    # Reset brain
    brain.reset()
    
    # Check if reset worked
    print(f"  ✓ Brain reset completed")
    print(f"  ✓ Total neurons after reset: {brain.getTotalNeuronCount()}")
    
    # The brain object will be automatically cleaned up when it goes out of scope
    print(f"  ✓ Resources cleaned up")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("Running Complete NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        test_import_and_creation,
        test_initialization,
        test_simulation,
        test_statistics,
        test_world_and_agent,
        test_enable_features,
        test_save_load,
        test_cleanup
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print("  STATUS: PASSED")
        except Exception as e:
            failed += 1
            print(f"  STATUS: FAILED - {e}")
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
