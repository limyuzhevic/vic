#!/usr/bin/env python3
"""
Basic validation test for enhanced NLM Python bindings.
This script tests the core functionality of the enhanced Python API.
"""

import sys
import os
import tempfile
import pynlm

def test_basic_imports():
    """Test basic imports and version checking."""
    print("=== Testing Basic Imports ===")
    
    try:
        print(f"NLM Python version: {pynlm.__version__}")
        print(f"Python bindings loaded successfully")
        return True
    except Exception as e:
        print(f"Failed to load Python bindings: {e}")
        return False

def test_factory_functions():
    """Test new factory functions."""
    print("\n=== Testing Factory Functions ===")
    
    try:
        # Test createBrainWithNeurons
        print("Testing createBrainWithNeurons...")
        brain = pynlm.createBrainWithNeurons(100, 0.15)
        assert brain is not None, "createBrainWithNeurons failed"
        print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
        
        brain.initialize()
        brain.step(0)
        print(f"  Brain initialized successfully with {brain.getTotalSpikeCount()} spikes")
        
        # Test createLearningEnvironment
        print("Testing createLearningEnvironment...")
        brain2, world2, agent2 = pynlm.createLearningEnvironment(worldSize=10)
        assert brain2 is not None, "createLearningEnvironment failed"
        print(f"  Created environment with {brain2.getTotalNeuronCount()} neurons")
        
        assert agent2.isCuriosityEnabled(), "Curiosity should be enabled"
        assert agent2.isRewardModulationEnabled(), "Reward modulation should be enabled"
        print("  All agent subsystems enabled correctly")
        
        # Test configuration presets
        print("Testing configuration presets...")
        configs = [
            ("Standard", pynlm.createStandardConfig()),
            ("Small", pynlm.createSmallConfig()),
            ("Large", pynlm.createLargeConfig()),
            ("Research", pynlm.createResearchConfig())
        ]
        
        for name, config in configs:
            assert config is not None, f"{name} config creation failed"
            print(f"  {name} config: {config.getOr('brain.neuron_count', 0)} neurons")
        
        return True
        
    except Exception as e:
        print(f"Factory functions test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_configuration_utilities():
    """Test configuration utilities and helpers."""
    print("\n=== Testing Configuration Utilities ===")
    
    try:
        # Test configuration helpers
        print("Testing configuration helpers...")
        config = pynlm.createDefaultConfig()
        
        # Test development configuration
        dev_config = pynlm.configureForDevelopment(config)
        assert dev_config.getOr("development.enable", False), "Development should be enabled"
        print("  configureForDevelopment works")
        
        # Test stability configuration
        stability_config = pynlm.configureForStability(config)
        assert stability_config.getOr("brain.connection_probability", 1.0) < 0.15, "Connection probability should be low"
        print("  configureForStability works")
        
        # Test exploration configuration
        exploration_config = pynlm.configureForExploration(config)
        assert exploration_config.getOr("brain.connection_probability", 0.0) > 0.15, "Connection probability should be high"
        print("  configureForExploration works")
        
        # Test validation
        is_valid, errors = pynlm.validateConfig(config)
        assert is_valid, f"Valid config should pass validation: {errors}"
        print("  Configuration validation works")
        
        # Test quick presets
        quick_test_config = pynlm.createQuickTestConfig()
        assert quick_test_config.getOr("brain.neuron_count", 0) == 100, "Quick test config wrong"
        print("  Quick test configuration works")
        
        quick_demo_config = pynlm.createQuickDemoConfig()
        assert quick_demo_config.getOr("brain.neuron_count", 0) == 500, "Quick demo config wrong"
        print("  Quick demo configuration works")
        
        return True
        
    except Exception as e:
        print(f"Configuration utilities test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_memory_persistence():
    """Test memory persistence utilities."""
    print("\n=== Testing Memory Persistence ===")
    
    try:
        # Create and train a brain
        print("Creating and training brain...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run some training
        for step in range(50):
            brain.step(step)
        
        print(f"  Brain trained with {brain.getTotalSpikeCount()} spikes")
        
        # Test save functionality
        print("Testing save functionality...")
        with tempfile.NamedTemporaryFile(suffix='.pkl', delete=False) as f:
            save_path = f.name
        
        try:
            # Test enhanced save function
            pynlm.saveBrainState(brain, save_path)
            print(f"  Brain saved to {save_path}")
            
            # Verify file was created
            assert os.path.exists(save_path), "Save file should exist"
            assert os.path.getsize(save_path) > 0, "Save file should not be empty"
            print("  Save verification passed")
            
            # Test load functionality
            print("Testing load functionality...")
            loaded_brain = pynlm.loadBrainState(save_path)
            assert loaded_brain is not None, "Load should return brain"
            assert loaded_brain.getTotalNeuronCount() == brain.getTotalNeuronCount(), "Neuron count should match"
            print(f"  Brain loaded successfully with {loaded_brain.getTotalNeuronCount()} neurons")
            
            # Test that loaded brain can continue training
            print("Testing continued training after load...")
            for step in range(25):
                loaded_brain.step(step + 50)
            
            print(f"  Loaded brain has {loaded_brain.getTotalSpikeCount()} total spikes")
            
        finally:
            # Cleanup
            if os.path.exists(save_path):
                os.unlink(save_path)
                print(f"  Cleaned up temporary file: {save_path}")
        
        return True
        
    except Exception as e:
        print(f"Memory persistence test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_complete_simulation():
    """Test a complete simulation with all features."""
    print("\n=== Testing Complete Simulation ===")
    
    try:
        # Create learning environment
        print("Creating learning environment...")
        brain, world, agent = pynlm.createLearningEnvironment(
            worldSize=15,
            visionRange=8,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        
        print(f"  Environment created: {brain.getTotalNeuronCount()} neurons")
        
        # Run complete simulation
        total_reward = 0.0
        steps_completed = 0
        
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            steps_completed += 1
            
            # Print progress
            if step % 25 == 0:
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                firing_rate = brain.getAverageFiringRate()
                print(f"    Step {step}: Reward={total_reward:.2f}, "
                      f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                      f"FiringRate={firing_rate:.2f}")
        
        # Verify simulation results
        print(f"\nSimulation Results:")
        print(f"  Steps completed: {steps_completed}")
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
        # Verify agent features
        print(f"\nAgent Features:")
        print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
        print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
        print(f"  Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
        print(f"  Development enabled: {agent.isDevelopmentEnabled()}")
        
        # Verify development stage
        final_stage = brain.getDevelopmentalStage()
        learned_preference = brain.getLearnedPreference()
        
        print(f"\nDevelopment:")
        print(f"  Final stage: {final_stage}")
        print(f"  Learned preference: {learned_preference:.3f}")
        
        # Validate results
        assert steps_completed == 100, f"Expected 100 steps, got {steps_completed}"
        assert total_reward >= 0, f"Total reward should be non-negative, got {total_reward}"
        assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
        assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
        
        print("\n✓ Complete simulation test passed")
        return True
        
    except Exception as e:
        print(f"Complete simulation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all validation tests."""
    print("=" * 60)
    print("NLM Python Bindings Validation Tests")
    print("=" * 60)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Factory Functions", test_factory_functions),
        ("Configuration Utilities", test_configuration_utilities),
        ("Memory Persistence", test_memory_persistence),
        ("Complete Simulation", test_complete_simulation)
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print("VALIDATION TEST SUMMARY")
    print("=" * 60)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {len(tests)}")
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        print("The enhanced NLM Python bindings are working correctly!")
        return 0
    else:
        print(f"\n❌ {failed} tests failed")
        print("Please check the output above for details on failed tests.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
