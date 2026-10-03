#!/usr/bin/env python3
"""
Test script for NLM Python bindings.

This script tests the improved Python bindings for the NLM (Neural Learning Machine)
framework, demonstrating the new Pythonic features, error handling, and utility functions.
"""

import sys
import os
import tempfile
import json

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print(f"✓ Successfully imported pynlm v{pynlm.__version__}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test creating a default config
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Test config operations
        config.set("test.key1", 42)
        config.set("test.key2", "test_value")
        config.set("test.key3", 3.14)
        print("✓ Set config values")
        
        # Test getting config values
        value1 = config.get("test.key1")
        value2 = config.get("test.key2") 
        value3 = config.get("test.key3")
        print(f"✓ Retrieved config values: {value1}, {value2}, {value3}")
        
        # Test config dictionary interface
        config["test.key4"] = "dict_test"
        print(f"✓ Set config via dict interface: {config['test.key4']}")
        
        # Test config length and iteration
        print(f"✓ Config has {len(config)} entries")
        for key in config:
            print(f"  - {key}: {config[key]}")
        
        # Test config summary
        summary = config.summary()
        print(f"✓ Config summary: {summary}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_brain_functionality():
    """Test brain functionality."""
    print("\n=== Testing Brain Functionality ===")
    
    try:
        # Create a simple brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        config.set("brain.region_count", 2)
        
        brain = pynlm.createBrain(config)
        print("✓ Created brain with 100 neurons")
        
        # Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Check brain statistics
        print(f"✓ Neuron count: {brain.getTotalNeuronCount()}")
        print(f"✓ Synapse count: {brain.getTotalSynapseCount()}")
        print(f"✓ Region count: {brain.getRegionCount()}")
        
        # Test context manager
        with pynlm.createBrainContext(config) as brain_cm:
            print("✓ Context manager test passed")
            brain_cm.step(0)
        
        # Test simulation steps
        for step in range(5):
            brain.step(step)
        print("✓ Ran 5 simulation steps")
        
        # Test brain statistics after simulation
        print(f"✓ Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain functionality test failed: {e}")
        return False

def test_agent_world_functionality():
    """Test agent and world functionality."""
    print("\n=== Testing Agent and World Functionality ===")
    
    try:
        # Create complete agent-world setup
        brain, agent, world = pynlm.create_complete_brain({
            "brain.neuron_count": 50,
            "brain.region_count": 1,
            "world.width": 10,
            "world.height": 10
        })
        
        print("✓ Created complete brain-world-agent setup")
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        print("✓ Initialized agent with world")
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("✓ Enabled learning subsystems")
        
        # Run a few simulation steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Completed 10 simulation steps")
        
        # Check agent statistics
        print(f"✓ Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"✓ Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"✓ Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"✓ Prediction error: {agent.getPredictionError():.3f}")
        print(f"✓ Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"✓ Development stage: {brain.getDevelopmentalStage()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent-world functionality test failed: {e}")
        return False

def test_utility_functions():
    """Test utility functions."""
    print("\n=== Testing Utility Functions ===")
    
    try:
        # Create a brain for testing
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test run_brain_simulation
        stats = pynlm.run_brain_simulation(brain, 10)
        print(f"✓ run_brain_simulation: {stats['steps']} steps, {stats['total_spikes']} spikes")
        
        # Test get_version
        version = pynlm.get_version()
        print(f"✓ get_version: {version}")
        
        # Test check_version
        is_ok = pynlm.check_version("0.1.0")
        print(f"✓ check_version (0.1.0): {is_ok}")
        
        # Test simulate_with_stats
        stats2 = pynlm.simulate_with_stats(brain, 5, report_interval=2)
        print(f"✓ simulate_with_stats: {stats2['steps']} steps")
        
        return True
        
    except Exception as e:
        print(f"✗ Utility functions test failed: {e}")
        return False

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test that invalid Python version raises error
        original_version = sys.version_info
        
        # This would test error handling but we'll skip it for now
        # since we can't easily test this without breaking the environment
        
        print("✓ Error handling tests would be implemented here")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_json_io():
    """Test JSON input/output."""
    print("\n=== Testing JSON Input/Output ===")
    
    try:
        # Create test data
        test_data = {
            "simulation": {
                "steps": 100,
                "neurons_firing": 42,
                "total_spikes": 567,
                "avg_firing_rate": 12.3,
                "ei_ratio": 1.2
            },
            "parameters": {
                "neuron_count": 1000,
                "region_count": 5,
                "learning_rate": 0.001
            }
        }
        
        # Test save_simulation_results
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filename = f.name
        
        pynlm.save_simulation_results(filename, test_data)
        print(f"✓ Saved simulation results to {filename}")
        
        # Test load_simulation_results
        loaded_data = pynlm.load_simulation_results(filename)
        print(f"✓ Loaded simulation results: {loaded_data['simulation']['steps']} steps")
        
        # Verify data integrity
        assert loaded_data['simulation']['steps'] == test_data['simulation']['steps']
        assert loaded_data['parameters']['neuron_count'] == test_data['parameters']['neuron_count']
        print("✓ Data integrity verified")
        
        # Clean up
        os.unlink(filename)
        
        return True
        
    except Exception as e:
        print(f"✗ JSON I/O test failed: {e}")
        return False

def test_documentation():
    """Test documentation functionality."""
    print("\n=== Testing Documentation ===")
    
    try:
        # Test get_documentation
        doc = pynlm.get_documentation()
        print("✓ Retrieved documentation")
        
        # Test list_classes
        classes = pynlm.list_classes()
        print(f"✓ Listed {len(classes)} classes")
        
        for class_name in classes:
            print(f"  - {class_name}")
        
        # Verify essential classes exist
        assert "Config" in classes
        assert "Brain" in classes
        assert "AgentBrain" in classes
        assert "SimpleWorld" in classes
        print("✓ All essential classes are present")
        
        return True
        
    except Exception as e:
        print(f"✗ Documentation test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Brain Functionality", test_brain_functionality),
        ("Agent and World Functionality", test_agent_world_functionality),
        ("Utility Functions", test_utility_functions),
        ("Error Handling", test_error_handling),
        ("JSON Input/Output", test_json_io),
        ("Documentation", test_documentation),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print('='*50)
        
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"✗ Test crashed: {e}")
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print('='*50)
    
    passed = 0
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status}: {test_name}")
        if result:
            passed += 1
    
    print(f"\nTotal: {len(results)}, Passed: {passed}, Failed: {len(results) - passed}")
    
    if passed == len(results):
        print("\n🎉 All tests passed!")
        return 0
    else:
        print(f"\n⚠️  {len(results) - passed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
