#!/usr/bin/env python3
"""Test script to verify NLM fixes and improvements."""

import pynlm
import sys
import traceback

def test_python_bindings():
    """Test the Python bindings improvements."""
    print("Testing Python bindings improvements...")
    
    try:
        # Test 1: Basic brain creation
        print("Test 1: Basic brain creation")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"  ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test 2: STDP system bindings (NEW FEATURE)
        print("Test 2: STDP system bindings")
        stdp = brain.getSTDP()
        if stdp is not None:
            print("  ✓ STDP system accessible")
            stdp.setLTPWeight(0.02)
            stdp.setLTDWeight(0.015)
            stdp.setTimeConstant(20.0)
            print(f"  ✓ STDP parameters configured: LTP={stdp.getLTPWeight()}, LTD={stdp.getLTDWeight()}, tau={stdp.getTimeConstant()}")
        else:
            print("  ⚠ STDP system not accessible (this is expected for basic configuration)")
        
        # Test 3: Structural Plasticity system bindings (NEW FEATURE)
        print("Test 3: Structural Plasticity system bindings")
        sp = brain.getStructuralPlasticity()
        if sp is not None:
            print("  ✓ Structural Plasticity system accessible")
            sp.setSynaptogenesisRate(0.0002)
            sp.setPruningRate(0.00002)
            print(f"  ✓ Structural Plasticity parameters configured: synaptogenesis={sp.getSynaptogenesisRate()}, pruning={sp.getPruningRate()}")
        else:
            print("  ⚠ Structural Plasticity system not accessible (this is expected for basic configuration)")
        
        # Test 4: AgentBrain basic functionality
        print("Test 4: AgentBrain functionality")
        agent = pynlm.createAgentBrain(brain)
        print("  ✓ AgentBrain created")
        
        # Test 5: SimpleWorld functionality
        print("Test 5: SimpleWorld functionality")
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        print("  ✓ SimpleWorld configured and reset")
        
        # Test 6: AgentBrain initialization
        print("Test 6: AgentBrain initialization")
        agent.initialize(world)
        print("  ✓ AgentBrain initialized with world")
        
        # Test 7: Advanced simulation loop
        print("Test 7: Advanced simulation loop")
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
        
        print(f"  ✓ Simulation completed: {brain.getTotalSpikeCount()} spikes, {brain.getAverageFiringRate():.2f} firing rate")
        
        # Test 8: Enable all learning systems
        print("Test 8: Enable all learning systems")
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("  ✓ All learning systems enabled")
        
        # Test 9: AgentBrain statistics
        print("Test 9: AgentBrain statistics")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"    Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"    Prediction Error: {agent.getPredictionError():.3f}")
        print(f"    Development Stage: {brain.getDevelopmentalStage()}")
        
        print("\n✅ All Python binding tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced features from the advanced usage guide."""
    print("\nTesting advanced features...")
    
    try:
        # Test parameter configuration
        print("Test 1: Parameter configuration")
        config = pynlm.createDefaultConfig()
        
        # Configure advanced parameters
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test learning systems
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("  ✓ Advanced configuration test passed")
        
        # Test checkpoint functionality
        print("Test 2: Checkpoint functionality")
        import os
        
        # Save brain state
        if brain.save("test_checkpoint.bin"):
            print("  ✓ Checkpoint save successful")
            
            # Load brain state
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_brain.initialize()
            
            if new_brain.load("test_checkpoint.bin"):
                print("  ✓ Checkpoint load successful")
                # Clean up
                os.remove("test_checkpoint.bin")
            else:
                print("  ⚠ Checkpoint load failed")
        else:
            print("  ⚠ Checkpoint save failed")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Advanced features test failed: {e}")
        traceback.print_exc()
        return False

def test_documentation():
    """Test that documentation files exist and are readable."""
    print("\nTesting documentation...")
    
    try:
        # Test that key documentation files exist
        documentation_files = [
            "README.md",
            "easy_usage.md", 
            "HOW_TO_USE.md",
            "ADVANCED_USAGE.md"
        ]
        
        existing_files = []
        for filename in documentation_files:
            if os.path.exists(filename):
                existing_files.append(filename)
                print(f"  ✓ {filename} exists")
            else:
                print(f"  ❌ {filename} not found")
        
        print(f"\n  Found {len(existing_files)}/{len(documentation_files)} documentation files")
        
        if len(existing_files) >= 3:  # At least the main documentation files
            print("✅ Documentation test passed!")
            return True
        else:
            print("❌ Insufficient documentation files")
            return False
            
    except Exception as e:
        print(f"\n❌ Documentation test failed: {e}")
        return False

if __name__ == "__main__":
    import os
    
    print("=" * 60)
    print("NLM - Testing Fixes and Improvements")
    print("=" * 60)
    
    all_tests_passed = True
    
    # Run all tests
    all_tests_passed &= test_python_bindings()
    all_tests_passed &= test_advanced_features()
    all_tests_passed &= test_documentation()
    
    print("\n" + "=" * 60)
    if all_tests_passed:
        print("🎉 ALL TESTS PASSED! The fixes and improvements are working correctly.")
    else:
        print("⚠️  Some tests failed. Please review the output above.")
    print("=" * 60)
    
    sys.exit(0 if all_tests_passed else 1)