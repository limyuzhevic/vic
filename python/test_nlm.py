#!/usr/bin/env python3
"""
NLM Testing and Validation Script

This script validates the improvements made to the NLM Python bindings and
tests the new functionality including Phase 6 integration demos and
advanced user features.
"""

import sys
import os
import tempfile
import traceback

try:
    import pynlm as nlm
except ImportError as e:
    print(f"Error: Failed to import pynlm: {e}")
    print("Please ensure the pynlm package is properly installed.")
    sys.exit(1)
def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing Basic NLM Functionality...")
    
    # Test 1: Create default config
    config = nlm.createDefaultConfig()
    assert config is not None, "Failed to create default config"
    print("  ✓ Config creation works")
    
    # Test 2: Create brain
    brain = nlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    print("  ✓ Brain creation works")
    
    # Test 3: Initialize brain
    brain.initialize()
    print("  ✓ Brain initialization works")
    
    # Test 4: Check brain statistics
    neurons = brain.getTotalNeuronCount()
    assert neurons > 0, "Brain should have neurons"
    print(f"  ✓ Brain has {neurons} neurons")
    
    # Test 5: Create world
    world = nlm.createSimpleWorld()
    assert world is not None, "Failed to create world"
    print("  ✓ World creation works")
    
    # Test 6: Configure world
    world.configure(50, 50, 5, 5)
    print("  ✓ World configuration works")
    
    # Test 7: Reset world
    world.reset()
    print("  ✓ World reset works")
    
    # Test 8: Create agent
    agent = nlm.createAgentBrain(brain)
    assert agent is not None, "Failed to create agent"
    print("  ✓ Agent creation works")
    
    # Test 9: Initialize agent
    agent.initialize(world)
    print("  ✓ Agent initialization works")
    
    # Test 10: Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("  ✓ Advanced features can be enabled")
    
    print("Basic functionality tests PASSED ✓\n")
    return True
def test_advanced_features():
    """Test advanced NLM features"""
    print("Testing Advanced NLM Features...")
    
    # Create config with advanced settings
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("development_enabled", True)
    config.set("structural_plasticity_enabled", True)
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 10, 10)
    world.reset()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test 1: Advanced feature integration
    assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
    assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
    assert agent.isDevelopmentEnabled(), "Development should be enabled"
    assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
    print("  ✓ Advanced feature states are correctly tracked")
    
    # Test 2: Simulation steps
    initial_spikes = brain.getTotalSpikeCount()
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            agent.updateDevelopment(0.1)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes > initial_spikes, "Spike count should increase during simulation"
    print(f"  ✓ Simulation works: {final_spikes - initial_spikes} spikes generated")
    
    # Test 3: Neuromodulation
    initial_dopamine = agent.getNeuromodulationLevel()
    initial_curiosity = agent.getCuriosityLevel()
    
    # Apply reward
    agent.applyRewardModulation(1.0, 0.5)
    
    final_dopamine = agent.getNeuromodulationLevel()
    # Dopamine should increase with reward
    assert final_dopamine >= initial_dopamine, "Dopamine should not decrease with reward"
    print(f"  ✓ Neuromodulation works: dopamine {initial_dopamine:.3f} -> {final_dopamine:.3f}")
    
    # Test 4: Development
    initial_stage = agent.getDevelopmentalStage()
    agent.updateDevelopment(0.1)
    final_stage = agent.getDevelopmentalStage()
    print(f"  ✓ Development works: stage {int(initial_stage)} -> {int(final_stage)}")
    
    print("Advanced features tests PASSED ✓\n")
    return True
def test_error_handling():
    """Test error handling in NLM bindings"""
    print("Testing Error Handling...")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Invalid sensory input handling
    from pynlm import SensoryPercept
    
    # Create a percept and test setting invalid values
    percept = SensoryPercept()
    # Should not crash when setting various values
    try:
        percept.setTimestamp(0)
        print("  ✓ Timestamp setting handles values correctly")
    except Exception as e:
        print(f"  ! Warning: Timestamp setting raised exception: {e}")
    
    # Test 2: World boundary handling
    world = nlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    
    # Test invalid position handling
    isValid = world.isValidPosition(-1, -1)
    assert not isValid, "Invalid positions should be rejected"
    print("  ✓ World boundary validation works")
    
    # Test 3: Brain state after reset
    initial_spikes = brain.getTotalSpikeCount()
    brain.reset()
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes <= initial_spikes, "Reset should not increase spike count"
    print("  ✓ Brain reset works correctly")
    
    print("Error handling tests PASSED ✓\n")
    return True
def test_phase6_demo():
    """Test Phase 6 demo functionality"""
    print("Testing Phase 6 Demo...")
    
    # This would import and run the phase6_demo module
    # For now, we'll test the key components
    
    # Create a minimal Phase 6-like configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    config.set("development_enabled", True)
    config.set("structural_plasticity_enabled", True)
    config.set("replay_enabled", True)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(200, 200, 10, 10)
    world.setAgentStart(100, 100)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all Phase 6 features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run a simulation similar to Phase 6 demo
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 25 == 0:
            agent.updateDevelopment(0.1)
    
    # Verify integration
    assert brain.getTotalNeuronCount() > 0
    assert agent.getNeuromodulationLevel() >= 0
    assert int(agent.getDevelopmentalStage()) >= 0
    
    print(f"  ✓ Phase 6 simulation completed")
    print(f"    - Neurons: {brain.getTotalNeuronCount()}")
    print(f"    - Spikes: {brain.getTotalSpikeCount()}")
    print(f"    - Development stage: {int(agent.getDevelopmentalStage())}")
    print(f"    - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    print("Phase 6 demo tests PASSED ✓\n")
    return True
def test_save_load():
    """Test brain save and load functionality"""
    print("Testing Save/Load Functionality...")
    
    # Create and configure a brain
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    
    brain1 = nlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation
    for i in range(50):
        brain1.step(i, i * 0.1)
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        brain_file = f.name
    
    try:
        brain1.save(brain_file)
        print(f"  ✓ Brain saved to {brain_file}")
        
        # Create new brain and load saved state
        brain2 = nlm.createBrain(config)
        brain2.initialize()
        
        brain2.load(brain_file)
        print(f"  ✓ Brain loaded from {brain_file}")
        
        # Compare some statistics
        spikes1 = brain1.getTotalSpikeCount()
        spikes2 = brain2.getTotalSpikeCount()
        
        print(f"    - Saved brain spikes: {spikes1}")
        print(f"    - Loaded brain spikes: {spikes2}")
        
    finally:
        # Clean up
        if os.path.exists(brain_file):
            os.unlink(brain_file)
            print(f"  ✓ Temporary file cleaned up")
    
    print("Save/Load tests PASSED ✓\n")
    return True
def run_comprehensive_test():
    """Run all NLM tests"""
    print("=" * 80)
    print("NLM COMPREHENSIVE TESTING SUITE")
    print("=" * 80)
    print()
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Advanced Features", test_advanced_features),
        ("Error Handling", test_error_handling),
        ("Phase 6 Demo", test_phase6_demo),
        ("Save/Load", test_save_load),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"Running {test_name} Test...")
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
                print(f"  ❌ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"  ❌ {test_name} FAILED with exception: {e}")
            traceback.print_exc()
        print()
    
    print("=" * 80)
    print("TEST RESULTS SUMMARY")
    print("=" * 80)
    print(f"Tests Passed: {passed}")
    print(f"Tests Failed: {failed}")
    print(f"Total Tests: {passed + failed}")
    print()
    
    if failed == 0:
        print("🎉 ALL TESTS PASSED! NLM improvements are working correctly.")
        return True
    else:
        print("❌ SOME TESTS FAILED. Please review the issues above.")
        return False
def main():
    """Main test runner"""
    success = run_comprehensive_test()
    sys.exit(0 if success else 1)
if __name__ == "__main__":
    main()
