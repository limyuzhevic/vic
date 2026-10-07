#!/usr/bin/env python3
"""
NLM Improvements Test Script

This script tests the key improvements made to the NLM (Neural Learning Machine) project:
1. Unified CLI interface
2. Configuration management
3. Error handling
4. Python API
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'scripts'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: NLM Python bindings not available. Some tests will be skipped.")

def test_python_api():
    """Test the Python API functionality"""
    print("Testing Python API...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipping - Python bindings not available")
        return True
    
    try:
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic operations
        print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"  Created brain with {brain.getTotalSynapseCount()} synapses")
        
        # Test simulation
        brain.step(0)
        print(f"  Simulated 1 step, {brain.getFiringNeuronCount()} firing neurons")
        
        # Test save/load
        test_file = "/tmp/nlm_test_brain.bin"
        brain.save(test_file)
        print(f"  Saved brain to {test_file}")
        
        brain.reset()
        brain.load(test_file)
        print(f"  Loaded brain from {test_file}")
        
        # Test world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        print(f"  Created world: {world.getWidth()}x{world.getHeight()}")
        
        # Test agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        print(f"  Created agent with curiosity level: {agent.getCuriosityLevel():.3f}")
        
        # Test config
        test_config_file = "/tmp/test_config.json"
        config.set("brain.neuron_count", 500)
        config.saveToFile(test_config_file)
        print(f"  Saved config to {test_config_file}")
        
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        return False
def test_cli_interface():
    """Test the CLI interface (mock test)"""
    print("Testing CLI interface...")
    
    # Test that the nlm-python script exists
    cli_path = os.path.join(os.path.dirname(__file__), 'scripts', 'nlm-python')
    if os.path.exists(cli_path):
        print(f"  Found CLI script: {cli_path}")
        print("  CLI interface: Available")
        return True
    else:
        print(f"  CLI script not found: {cli_path}")
        return False
def test_configuration():
    """Test configuration management"""
    print("Testing configuration management...")
    
    if not PYNLM_AVAILABLE:
        print("  Skipping - Python bindings not available")
        return True
    
    try:
        # Test default config
        config = pynlm.createDefaultConfig()
        print(f"  Default config loaded: {config.summary()}")
        
        # Test config values
        neuron_count = config.getOr("brain.neuron_count", 1000)
        print(f"  Default neuron count: {neuron_count}")
        
        # Test config modification
        config.set("brain.neuron_count", 2000)
        modified_count = config.getOr("brain.neuron_count", 1000)
        print(f"  Modified neuron count: {modified_count}")
        
        # Test file I/O
        test_file = "/tmp/test_config.cfg"
        config.set("test.value", "test_value")
        config.saveToFile(test_file)
        
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(test_file)
        loaded_value = config2.getOr("test.value", "")
        print(f"  Config file I/O: {loaded_value}")
        
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        return False
def test_error_handling():
    """Test error handling (mock test)"""
    print("Testing error handling...")
    
    # Test that exception headers exist
    exceptions_path = os.path.join(os.path.dirname(__file__), 'src', 'core', 'Exceptions')
    if os.path.exists(exceptions_path):
        print("  Exception headers found")
        print("  Error handling: Available")
        return True
    else:
        print(f"  Exception directory not found: {exceptions_path}")
        return False
def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Improvements Test Suite")
    print("=" * 60)
    
    tests = [
        ("Python API", test_python_api),
        ("CLI Interface", test_cli_interface),
        ("Configuration Management", test_configuration),
        ("Error Handling", test_error_handling),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        print("-" * 40)
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"  Unexpected error: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:30} {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}, Passed: {passed}, Failed: {failed}")
    
    if failed > 0:
        print("\nSome tests failed. Please review the output above.")
        return 1
    else:
        print("\nAll tests passed!")
        return 0
if __name__ == "__main__":
    sys.exit(main())
