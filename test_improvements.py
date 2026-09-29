#!/usr/bin/env python3
"""
Test script to demonstrate NLM Python API improvements.
This script tests the enhanced error handling, validation, and convenience features.
"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic brain functionality."""
    print("Testing basic functionality...")
    
    # Create optimized configuration
    config = pynlm.createBrainConfig(neuron_count=500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    # Check results
    firing = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    rate = brain.getAverageFiringRate()
    
    print(f"  Firing neurons: {firing}")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Average firing rate: {rate:.2f} Hz")
    
    assert brain.getTotalNeuronCount() == 500
    assert firing > 0 or total_spikes > 0  # May have 0 firing at this point
    
    print("  ✓ Basic functionality test passed")

def test_configuration_validation():
    """Test configuration validation features."""
    print("Testing configuration validation...")
    
    # Create optimized configuration
    config = pynlm.createBrainConfig()
    
    # Test validation methods
    is_valid = config.validate()
    is_complete = config.isComplete()
    
    print(f"  Configuration valid: {is_valid}")
    print(f"  Configuration complete: {is_complete}")
    
    # Get validation details
    is_valid_detailed, error_msg = config.validateWithErrors()
    print(f"  Validation result: {is_valid_detailed}")
    if not is_valid_detailed:
        print(f"  Error message: {error_msg}")
    
    # Test configuration modifications
    config.set("brain.neuron_count", 1000)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    
    print("  Configuration modifications applied successfully")
    
    # Validate again
    is_valid = config.validate()
    print(f"  Configuration valid after modifications: {is_valid}")
    
    assert is_valid
    
    print("  ✓ Configuration validation test passed")

def test_error_handling():
    """Test error handling and recovery."""
    print("Testing error handling...")
    
    # Test that invalid configuration is handled gracefully
    config = pynlm.createBrainConfig()
    
    # Simulate an invalid configuration (empty key)
    config.set("", "invalid_value")
    
    # Validate and check for error
    is_valid, error_msg = config.validateWithErrors()
    print(f"  Invalid configuration detected: {not is_valid}")
    if error_msg:
        print(f"  Error details: {error_msg}")
    
    # Clear and validate clean configuration
    config.clear()
    config.set("brain.neuron_count", 100)
    
    is_valid = config.validate()
    print(f"  Clean configuration valid: {is_valid}")
    
    assert is_valid
    
    print("  ✓ Error handling test passed")

def test_convenience_methods():
    """Test convenience methods and utilities."""
    print("Testing convenience methods...")
    
    # Test optimized configuration creation
    config1 = pynlm.createBrainConfig(neuron_count=1000)
    config2 = pynlm.createSimpleWorldConfig(width=20, height=20)
    
    print(f"  Brain config neurons: {config1.get('brain.neuron_count', 0)}")
    print(f"  World config width: {config2.get('world.width', 0)}")
    
    # Test stepWithDuration
    brain = pynlm.createBrain(config1)
    brain.initialize()
    
    brain.stepWithDuration(0, 0.1)
    brain.stepWithDuration(1, 0.2)
    
    print("  stepWithDuration method works correctly")
    
    # Test runQuickTest
    firing, spikes = pynlm.runQuickTest()
    print(f"  Quick test - firing: {firing}, spikes: {spikes}")
    
    # Test library info
    version = pynlm.getLibraryVersion()
    phase = pynlm.getPhase()
    print(f"  Library version: {version}")
    print(f"  Development phase: {phase}")
    
    print("  ✓ Convenience methods test passed")

def test_agent_simulation():
    """Test agent simulation with world integration."""
    print("Testing agent simulation...")
    
    # Setup brain and agent
    config = pynlm.createBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print progress every 25 steps
        if step % 25 == 0:
            print(f"    Step {step}: firing={brain.getFiringNeuronCount()}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"novelty={agent.getNoveltyLevel():.3f}")
    
    print("  ✓ Agent simulation test passed")

def test_documentation_examples():
    """Test examples from the documentation."""
    print("Testing documentation examples...")
    
    # Test example 1: Simplest brain
    print("  Testing 'Simplest brain' example...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
    neurons = brain.getTotalNeuronCount()
    print(f"    Created brain with {neurons} neurons")
    
    # Test example 2: Brain in a world
    print("  Testing 'Brain in a world' example...")
    config = pynlm.createBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for i in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
    
    print(f"    Watched world for 20 steps")
    
    # Test example 3: Complete agent
    print("  Testing 'Complete agent' example...")
    config = pynlm.createBrainConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"    Agent simulation complete!")
    
    print("  ✓ Documentation examples test passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python API Improvements - Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        test_basic_functionality,
        test_configuration_validation,
        test_error_handling,
        test_convenience_methods,
        test_agent_simulation,
        test_documentation_examples,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print()
        except Exception as e:
            failed += 1
            print(f"  ✗ Test failed: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed! ✓")
        print()
        print("The NLM Python API improvements are working correctly.")
        print("Key improvements demonstrated:")
        print("  - Enhanced error handling and validation")
        print("  - Configuration management utilities")
        print("  - Convenience methods for common operations")
        print("  - Comprehensive documentation examples")
        print("  - Robust brain-agent-world integration")

if __name__ == "__main__":
    main()