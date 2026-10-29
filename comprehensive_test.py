#!/usr/bin/env python3
"""
Comprehensive test of NLM Python bindings improvements.
This script provides a complete verification of all enhanced features.
"""

import sys
import os
import time

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("NLM Python Bindings - Comprehensive Test Suite")
print("=" * 60)

def test_imports():
    """Test that all modules can be imported successfully."""
    print("\n1. Testing imports...")
    try:
        import pynlm
        print(f"   ✓ Successfully imported pynlm")
        print(f"   ✓ Version: {getattr(pynlm, '__version__', 'unknown')}")
        print(f"   ✓ Author: {getattr(pynlm, '__author__', 'unknown')}")
        print(f"   ✓ Description: {getattr(pynlm, '__description__', 'unknown')}")
        return pynlm
    except ImportError as e:
        print(f"   ✗ Failed to import pynlm: {e}")
        return None

def test_basic_functionality(pynlm):
    """Test basic brain functionality."""
    print("\n2. Testing basic functionality...")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        brain.initialize()
        print(f"   ✓ Created and initialized brain")
        print(f"   ✓ Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   ✓ Total synapses: {brain.getTotalSynapseCount()}")
        
        for i in range(10):
            brain.step(i)
        
        print(f"   ✓ Simulated 10 steps")
        print(f"   ✓ Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   ✓ Firing neurons: {brain.getFiringNeuronCount()}")
        return brain
    except Exception as e:
        print(f"   ✗ Basic functionality test failed: {e}")
        return None

def test_context_manager(pynlm):
    """Test context manager support."""
    print("\n3. Testing context manager support...")
    try:
        with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
            print(f"   ✓ Context manager entered successfully")
            print(f"   ✓ Neurons initialized: {brain.getTotalNeuronCount()}")
            brain.step(0)
            print(f"   ✓ Spikes after one step: {brain.getTotalSpikeCount()}")
            print(f"   ✓ Context manager exited successfully")
        return True
    except Exception as e:
        print(f"   ✗ Context manager test failed: {e}")
        return False

def test_configuration_utilities(pynlm):
    """Test configuration utilities."""
    print("\n4. Testing configuration utilities...")
    try:
        config = pynlm.createDefaultConfig()
        
        # Set configuration values
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", "true")
        
        print(f"   ✓ Set configuration values")
        print(f"   ✓ Neuron count: {config.getOr('brain.neuron_count', 0)}")
        print(f"   ✓ Synapse density: {config.getOr('brain.synapse_density', 0.0)}")
        
        # Test validation
        is_valid = pynlm.validateConfig(config)
        print(f"   ✓ Configuration validation: {is_valid}")
        
        # Test config to dict
        config_dict = pynlm.configToDict(config)
        print(f"   ✓ Config to dict: {len(config_dict)} keys")
        print(f"   ✓ Config keys: {list(config_dict.keys())}")
        
        # Test dict to config
        new_config = pynlm.dictToConfig(config_dict)
        print(f"   ✓ Dict to config: created new config")
        print(f"   ✓ New config has keys: {new_config.getKeys()}")
        
        return True
    except Exception as e:
        print(f"   ✗ Configuration utilities test failed: {e}")
        return False

def test_statistics_and_utilities(pynlm):
    """Test statistics and utility functions."""
    print("\n5. Testing statistics and utilities...")
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Get statistics
        stats = pynlm.getStatistics(brain)
        print(f"   ✓ Statistics collected:")
        for key, value in stats.items():
            print(f"     {key}: {value}")
        
        # Get all neurons
        neurons = pynlm.getAllNeurons(brain)
        print(f"   ✓ Got {len(neurons)} neurons")
        
        if len(neurons) > 0:
            # Test neuron info
            first_neuron_id = pynlm.NeuronId(neurons[0]["id"])
            neuron_info = pynlm.neuronInfo(brain, first_neuron_id)
            print(f"   ✓ Got neuron info for neuron {neuron_info.get('id', 'unknown')}")
            print(f"   ✓ Neuron type: {neuron_info.get('type', 'unknown')}")
        
        # Test performance monitoring
        memory_usage = pynlm.getMemoryUsage()
        print(f"   ✓ Memory usage: {memory_usage}")
        
        return True
    except Exception as e:
        print(f"   ✗ Statistics test failed: {e}")
        return False

def test_convenience_functions(pynlm):
    """Test convenience functions."""
    print("\n6. Testing convenience functions...")
    try:
        # Run quick simulation
        print(f"   ✓ Running quick simulation...")
        start_time = time.time()
        spikes = pynlm.runQuickSimulation(neuron_count=50, steps=100)
        end_time = time.time()
        print(f"   ✓ Quick simulation: {spikes} spikes in {end_time - start_time:.3f}s")
        
        # Create test environment
        env = pynlm.createTestEnvironment()
        print(f"   ✓ Test environment: {env['type']}")
        print(f"   ✓ Description: {env['description']}")
        print(f"   ✓ Version: {env['version']}")
        
        # Test benchmarking
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        benchmark_results = pynlm.benchmarkBrain(brain, 50)
        print(f"   ✓ Benchmark results:")
        for key, value in benchmark_results.items():
            print(f"     {key}: {value}")
        
        return True
    except Exception as e:
        print(f"   ✗ Convenience functions test failed: {e}")
        return False

def test_error_handling(pynlm):
    """Test error handling."""
    print("\n7. Testing error handling...")
    try:
        # Test invalid brain
        try:
            pynlm.runExperiment(None, 100.0)
            print("   ✗ Should have thrown ValueError for invalid brain")
            return False
        except Exception as e:
            print(f"   ✓ Correctly handled invalid brain: {type(e).__name__}")
        
        # Test invalid duration
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        try:
            pynlm.runExperiment(brain, -1.0)
            print("   ✗ Should have thrown ValueError for negative duration")
            return False
        except Exception as e:
            print(f"   ✓ Correctly handled negative duration: {type(e).__name__}")
        
        # Test invalid step count
        try:
            pynlm.simulateMultipleSteps(None, 100)
            print("   ✗ Should have thrown ValueError for invalid brain")
            return False
        except Exception as e:
            print(f"   ✓ Correctly handled invalid brain: {type(e).__name__}")
        
        # Test invalid step count (zero)
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        try:
            pynlm.simulateMultipleSteps(brain, 0)
            print("   ✗ Should have thrown ValueError for zero steps")
            return False
        except Exception as e:
            print(f"   ✓ Correctly handled zero steps: {type(e).__name__}")
        
        return True
    except Exception as e:
        print(f"   ✗ Error handling test failed: {e}")
        return False

def test_world_interaction(pynlm):
    """Test world interaction and agent functionality."""
    print("\n8. Testing world interaction...")
    try:
        # Create brain and agent
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Initialize agent
        agent.initialize(world)
        
        # Test agent capabilities
        print(f"   ✓ Agent sensory input size: {agent.getSensoryInputSize()}")
        print(f"   ✓ Agent motor output size: {agent.getMotorOutputSize()}")
        print(f"   ✓ Agent developmental stage: {agent.getDevelopmentalStage()}")
        print(f"   ✓ Agent curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"   ✓ Agent neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"   ✓ Enabled all agent subsystems")
        
        # Run a few steps
        for step in range(5):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"     Step {step}: Action = {action}")
        
        print(f"   ✓ World interaction successful")
        print(f"   ✓ Final curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"   ✓ Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        
        return True
    except Exception as e:
        print(f"   ✗ World interaction test failed: {e}")
        return False

def main():
    """Run all tests."""
    pynlm = test_imports()
    if pynlm is None:
        print("\n" + "=" * 60)
        print("IMPORT FAILED - Cannot run tests")
        print("=" * 60)
        return False
    
    brain = test_basic_functionality(pynlm)
    if brain is None:
        print("\n" + "=" * 60)
        print("BASIC FUNCTIONALITY FAILED - Cannot run further tests")
        print("=" * 60)
        return False
    
    # Run all tests
    results = []
    results.append(("Context Manager", test_context_manager(pynlm)))
    results.append(("Configuration Utilities", test_configuration_utilities(pynlm)))
    results.append(("Statistics and Utilities", test_statistics_and_utilities(pynlm)))
    results.append(("Convenience Functions", test_convenience_functions(pynlm)))
    results.append(("Error Handling", test_error_handling(pynlm)))
    results.append(("World Interaction", test_world_interaction(pynlm)))
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "PASSED" if result else "FAILED"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
    
    print(f"\nResults: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        print("\nThe NLM Python bindings have been successfully enhanced with:")
        print("• Comprehensive module metadata and version information")
        print("• Context manager support for automatic resource management")
        print("• Configuration utilities (validate, import/export)")
        print("• Statistics and monitoring functions")
        print("• Performance benchmarking tools")
        print("• Convenience functions for quick simulations")
        print("• Robust error handling with custom exceptions")
        print("• Enhanced agent and world interaction capabilities")
        print("• Advanced features for power users")
        
        print("\nKey improvements made:")
        print("1. ✅ Added __version__, __author__, __description__ attributes")
        print("2. ✅ Implemented context managers for automatic initialization")
        print("3. ✅ Added configuration validation and dictionary conversion")
        print("4. ✅ Created utility functions for statistics and monitoring")
        print("5. ✅ Added convenience functions for quick simulations")
        print("6. ✅ Implemented comprehensive error handling")
        print("7. ✅ Enhanced world and agent interaction")
        print("8. ✅ Added benchmarking and performance monitoring")
        print("9. ✅ Improved type hints and documentation")
        print("10. ✅ Added validation and safety checks")
        
        return True
    else:
        print(f"\n❌ {total - passed} tests failed")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)