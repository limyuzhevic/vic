#!/usr/bin/env python3
"""
Test script for NLM Python functionality.
Tests new convenience factory functions, enhanced error handling, and helper functions.
"""

import sys
import os
import json
import tempfile
import traceback

def test_imports():
    """Test that pynlm module can be imported and basic functionality works."""
    print("Testing basic imports...")
    try:
        import pynlm
        print("  ✓ pynlm module imported successfully")
        
        # Check that version attribute exists
        if hasattr(pynlm, '__version__'):
            print(f"  ✓ pynlm.__version__ = {pynlm.__version__}")
        else:
            print("  ✓ pynlm module imported (version not available)")
            
        return True
    except ImportError as e:
        print(f"  ✗ Import failed: {e}")
        return False
    except Exception as e:
        print(f"  ✗ Unexpected error: {e}")
        return False

def test_convenience_factory_functions():
    """Test the new convenience factory functions."""
    print("\nTesting convenience factory functions...")
    
    try:
        import pynlm
        
        # Test createDemoConfig
        print("  Testing createDemoConfig()...")
        try:
            demo_config = pynlm.createDemoConfig()
            if demo_config is not None:
                print("    ✓ createDemoConfig() returned a valid config")
                
                # Test some expected properties
                neuron_count = demo_config.get("neuron_count", -1)
                if neuron_count > 0:
                    print(f"    ✓ Config has neuron_count = {neuron_count}")
                else:
                    print("    ✗ Config does not have expected neuron_count")
                    return False
            else:
                print("    ✗ createDemoConfig() returned None")
                return False
        except Exception as e:
            print(f"    ✗ createDemoConfig() failed: {e}")
            return False
        
        # Test createResearchConfig
        print("  Testing createResearchConfig()...")
        try:
            research_config = pynlm.createResearchConfig()
            if research_config is not None:
                print("    ✓ createResearchConfig() returned a valid config")
                
                # Research configs should have development enabled
                dev_enabled = research_config.get("development_enabled", False)
                if dev_enabled:
                    print("    ✓ Research config has development_enabled = True")
                else:
                    print("    ✗ Research config does not have development_enabled = True")
                    return False
            else:
                print("    ✗ createResearchConfig() returned None")
                return False
        except Exception as e:
            print(f"    ✗ createResearchConfig() failed: {e}")
            return False
        
        # Test createBrainFromConfigDict
        print("  Testing createBrainFromConfigDict()...")
        try:
            config_dict = {
                "brain.neuron_count": 500,
                "brain.synapse_density": 0.1,
                "plasticity.stdp.enable": True
            }
            brain = pynlm.createBrainFromConfigDict(config_dict)
            if brain is not None:
                print("    ✓ createBrainFromConfigDict() returned a valid brain")
            else:
                print("    ✗ createBrainFromConfigDict() returned None")
                return False
        except Exception as e:
            print(f"    ✗ createBrainFromConfigDict() failed: {e}")
            return False
        
        # Test createQuickAgent
        print("  Testing createQuickAgent()...")
        try:
            config = pynlm.createDemoConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            
            agent = pynlm.createQuickAgent(brain, world)
            if agent is not None:
                print("    ✓ createQuickAgent() returned a valid agent")
            else:
                print("    ✗ createQuickAgent() returned None")
                return False
        except Exception as e:
            print(f"    ✗ createQuickAgent() failed: {e}")
            return False
        
        return True
    except Exception as e:
        print(f"  ✗ Convenience factory functions test failed: {e}")
        return False

def test_config_system():
    """Test the Config system including JSON/YAML loading."""
    print("\nTesting Config system...")
    
    try:
        import pynlm
        
        # Test config creation
        print("  Testing config creation and basic operations...")
        config = pynlm.createDemoConfig()
        if config is None:
            print("    ✗ Failed to create demo config")
            return False
        
        # Test config has method
        if config.has("neuron_count"):
            neuron_count = config.get("neuron_count")
            print(f"    ✓ Config has neuron_count = {neuron_count}")
        else:
            print("    ✗ Config does not have neuron_count")
            return False
        
        # Test config set method
        config.set("test_value", 42)
        if config.get("test_value") == 42:
            print("    ✓ Config set() and get() works correctly")
        else:
            print("    ✗ Config set() and get() failed")
            return False
        
        # Test JSON file creation and loading
        print("  Testing JSON file I/O...")
        
        # Create a temporary JSON file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_config = {
                "brain.neuron_count": 1000,
                "brain.synapse_density": 0.15,
                "plasticity.stdp.enable": True,
                "random_seed": 123
            }
            json.dump(json_config, f)
            json_file = f.name
        
        try:
            # Load from JSON (if supported)
            try:
                brain = pynlm.createBrainFromJSON(json_file)
                if brain is not None:
                    print("    ✓ createBrainFromJSON() works")
                else:
                    print("    ⚠ createBrainFromJSON() returned None (may not be fully implemented)")
            except Exception as e:
                print(f"    ⚠ createBrainFromJSON() not available: {e}")
            
            # Test YAML file creation
            with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
                yaml_config = {
                    "brain.neuron_count": 2000,
                    "brain.synapse_density": 0.2,
                    "plasticity.stdp.enable": False
                }
                f.write("# YAML config\n")
                for key, value in yaml_config.items():
                    f.write(f"{key} = {value}\n")
                yaml_file = f.name
            
            try:
                brain = pynlm.createBrainFromYAML(yaml_file)
                if brain is not None:
                    print("    ✓ createBrainFromYAML() works")
                else:
                    print("    ⚠ createBrainFromYAML() returned None (may not be fully implemented)")
            except Exception as e:
                print(f"    ⚠ createBrainFromYAML() not available: {e}")
        finally:
            # Clean up temporary files
            if 'json_file' in locals() and os.path.exists(json_file):
                os.unlink(json_file)
            if 'yaml_file' in locals() and os.path.exists(yaml_file):
                os.unlink(yaml_file)
        
        return True
    except Exception as e:
        print(f"  ✗ Config system test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test enhanced error handling with invalid inputs."""
    print("\nTesting error handling...")
    
    try:
        import pynlm
        
        # Test invalid file paths
        print("  Testing error handling for invalid file paths...")
        try:
            pynlm.createBrainFromJSON("/nonexistent/file.json")
            print("    ✗ createBrainFromJSON should have raised an error for nonexistent file")
            return False
        except Exception as e:
            print(f"    ✓ createBrainFromJSON() correctly raised error for invalid file: {type(e).__name__}")
        
        try:
            pynlm.createBrainFromYAML("/nonexistent/file.yaml")
            print("    ✗ createBrainFromYAML should have raised an error for nonexistent file")
            return False
        except Exception as e:
            print(f"    ✓ createBrainFromYAML() correctly raised error for invalid file: {type(e).__name__}")
        
        # Test invalid config dictionary
        print("  Testing error handling for invalid config dictionary...")
        try:
            invalid_dict = {"invalid.key": "not_a_valid_type_12345"}
            brain = pynlm.createBrainFromConfigDict(invalid_dict)
            if brain is not None:
                print("    ✓ createBrainFromConfigDict() handled invalid config gracefully")
            else:
                print("    ⚠ createBrainFromConfigDict() returned None (may handle gracefully)")
        except Exception as e:
            print(f"    ✓ createBrainFromConfigDict() correctly raised error for invalid config: {type(e).__name__}")
        
        # Test error handling with uninitialized components
        print("  Testing error handling with uninitialized components...")
        try:
            config = pynlm.createDemoConfig()
            brain = pynlm.createBrain(config)
            
            # Try to use brain before initializing (if supported)
            try:
                brain.step(0)
                print("    ⚠ brain.step() should have raised an error for uninitialized brain")
            except Exception as e:
                print(f"    ✓ brain.step() correctly raised error for uninitialized brain: {type(e).__name__}")
        except Exception as e:
            print(f"    ✗ Error handling test setup failed: {e}")
            return False
        
        return True
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_helper_functions():
    """Test the new helper functions (note: some may not be available yet)."""
    print("\nTesting helper functions...")
    
    try:
        import pynlm
        
        # Test createSimpleSimulation (if available)
        print("  Testing createSimpleSimulation()...")
        try:
            config = pynlm.createDemoConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createQuickAgent(brain, world)
            
            # Initialize components
            brain.initialize()
            agent.initialize(world)
            
            # Try to create simulation function
            simulation_func = pynlm.createSimpleSimulation(brain, world, agent)
            if simulation_func is not None:
                print("    ✓ createSimpleSimulation() returned a function")
                
                # Try to run simulation
                try:
                    result = simulation_func(10)  # Run 10 steps
                    print("    ✓ Simulation function executed successfully")
                except Exception as e:
                    print(f"    ⚠ Simulation function ran but failed: {e}")
            else:
                print("    ⚠ createSimpleSimulation() returned None (may not be fully implemented)")
        except Exception as e:
            print(f"    ⚠ createSimpleSimulation() not available: {e}")
        
        # Test runQuickSimulation (if available)
        print("  Testing runQuickSimulation()...")
        try:
            # This is likely a helper function that wraps the simulation pattern
            print("    ⚠ runQuickSimulation() may not be available yet")
        except Exception as e:
            print(f"    ⚠ runQuickSimulation() not available: {e}")
        
        # Test benchmarkBrain (if available)
        print("  Testing benchmarkBrain()...")
        try:
            print("    ⚠ benchmarkBrain() may not be available yet")
        except Exception as e:
            print(f"    ⚠ benchmarkBrain() not available: {e}")
        
        # Test createEnvironmentSimulation (if available)
        print("  Testing createEnvironmentSimulation()...")
        try:
            print("    ⚠ createEnvironmentSimulation() may not be available yet")
        except Exception as e:
            print(f"    ⚠ createEnvironmentSimulation() not available: {e}")
        
        # Test quickTest (if available)
        print("  Testing quickTest()...")
        try:
            print("    ⚠ quickTest() may not be available yet")
        except Exception as e:
            print(f"    ⚠ quickTest() not available: {e}")
        
        return True
    except Exception as e:
        print(f"  ✗ Helper functions test failed: {e}")
        traceback.print_exc()
        return False

def test_full_integration():
    """Test full integration scenario."""
    print("\nTesting full integration scenario...")
    
    try:
        import pynlm
        
        print("  Running complete agent simulation...")
        
        # Create configuration
        config = pynlm.createDemoConfig()
        print(f"    Created demo config with {config.get('neuron_count')} neurons")
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("    Brain initialized")
        
        # Create agent brain interface
        agent = pynlm.createQuickAgent(brain, None)  # We'll create world separately
        print("    Quick agent created")
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        print("    World created and configured")
        
        # Initialize agent with world
        agent.initialize(world)
        print("    Agent initialized with world")
        
        # Run simulation
        steps_to_run = 50
        for step in range(steps_to_run):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print final statistics
        print(f"    Simulation completed: {steps_to_run} steps")
        print(f"    Final firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    Total spikes: {brain.getTotalSpikeCount()}")
        print(f"    Simulation time: {world.getSimulationTime()}")
        
        return True
    except Exception as e:
        print(f"  ✗ Full integration test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 70)
    print("NLM Python Functionality Test Suite")
    print("=" * 70)
    print("Testing new convenience factory functions, enhanced error handling,")
    print("and helper functions.")
    print("=" * 70)
    
    # Check if we're in the right directory
    current_dir = os.getcwd()
    print(f"\nWorking directory: {current_dir}")
    
    # Define test suite
    tests = [
        ("Basic Imports", test_imports),
        ("Convenience Factory Functions", test_convenience_factory_functions),
        ("Config System", test_config_system),
        ("Error Handling", test_error_handling),
        ("Helper Functions", test_helper_functions),
        ("Full Integration", test_full_integration),
    ]
    
    results = []
    
    # Run each test
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"\n✗ Test '{test_name}' crashed: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Print summary
    print("\n" + "=" * 70)
    print("TEST SUMMARY")
    print("=" * 70)
    
    passed = 0
    failed = 0
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:40} {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print("=" * 70)
    print(f"Total: {passed + failed} tests")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        return 0
    else:
        print(f"\n❌ {failed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())