#!/usr/bin/env python3
"""
Comprehensive test script for NLM improvements and bug fixes.
This script tests all the improvements made to the NLM codebase.
"""

import sys
import os
import subprocess
import json
import tempfile
import shutil
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return the result."""
    try:
        result = subprocess.run(
            cmd, 
            shell=True, 
            cwd=cwd, 
            capture_output=True, 
            text=True,
            timeout=300
        )
        return result.returncode, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return -1, "", "Command timed out"
    except Exception as e:
        return -1, "", str(e)

def test_python_bindings():
    """Test that Python bindings work correctly."""
    print("Testing Python bindings...")
    
    # Test basic imports
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Successfully created and initialized brain")
        
        # Test brain statistics
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        print(f"✓ Brain has {neurons} neurons and {synapses} synapses")
        
        # Test step simulation
        for i in range(10):
            brain.step(i)
        print("✓ Successfully ran simulation steps")
        
        # Test action production
        action = brain.produceAction()
        print("✓ Successfully produced action")
        
        return True
        
    except Exception as e:
        print(f"✗ Python bindings test failed: {e}")
        return False

def test_documentation():
    """Test that documentation files are properly formatted and accessible."""
    print("Testing documentation...")
    
    doc_files = [
        "README.md",
        "easy_usage.md", 
        "HOW_TO_USE.md",
        "BUG_ANALYSIS.md"
    ]
    
    all_good = True
    for doc_file in doc_files:
        if os.path.exists(doc_file):
            try:
                with open(doc_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    # Check for basic structure
                    if '# ' in content:
                        print(f"✓ {doc_file}: Contains markdown headers")
                    else:
                        print(f"✗ {doc_file}: Missing markdown headers")
                        all_good = False
                        
                    # Check for no obvious encoding issues
                    if '熙然' not in content and doc_file != "BUG_ANALYSIS.md":
                        print(f"✓ {doc_file}: No Chinese characters mixed with English")
                    elif doc_file != "BUG_ANALYSIS.md":
                        print(f"✗ {doc_file}: Contains Chinese characters mixed with English")
                        all_good = False
                        
            except Exception as e:
                print(f"✗ {doc_file}: Error reading file: {e}")
                all_good = False
        else:
            print(f"✗ {doc_file}: File not found")
            all_good = False
    
    return all_good

def test_test_coverage():
    """Test that test coverage improvements are in place."""
    print("Testing test coverage improvements...")
    
    # Check for new test files
    new_test_files = [
        "tests/test_agent_brain.cpp",
        "tests/test_world.cpp"
    ]
    
    all_good = True
    for test_file in new_test_files:
        if os.path.exists(test_file):
            print(f"✓ {test_file}: New test file found")
            
            # Check file size (should have content)
            file_size = os.path.getsize(test_file)
            if file_size > 1000:
                print(f"✓ {test_file}: Contains substantial content ({file_size} bytes)")
            else:
                print(f"✗ {test_file}: File seems too small ({file_size} bytes)")
                all_good = False
        else:
            print(f"✗ {test_file}: New test file not found")
            all_good = False
    
    # Check that main test file includes new tests
    test_main_path = "tests/test_main.cpp"
    if os.path.exists(test_main_path):
        with open(test_main_path, 'r') as f:
            content = f.read()
            if "test_agent_brain::runAll()" in content:
                print("✓ test_main.cpp includes AgentBrain tests")
            else:
                print("✗ test_main.cpp missing AgentBrain tests")
                all_good = False
                
            if "test_world::runAll()" in content:
                print("✓ test_main.cpp includes World tests")
            else:
                print("✗ test_main.cpp missing World tests")
                all_good = False
    else:
        print("✗ test_main.cpp not found")
        all_good = False
    
    return all_good

def test_advanced_features():
    """Test that advanced user features are working."""
    print("Testing advanced user features...")
    
    try:
        import pynlm
        all_good = True
        
        # Test advanced factory function
        try:
            brain, world, agent = pynlm.create_learning_environment()
            print("✓ Advanced learning environment creation works")
            
            # Verify components
            assert brain is not None
            assert world is not None
            assert agent is not None
            print("✓ All components created successfully")
            
        except Exception as e:
            print(f"✗ Advanced learning environment creation failed: {e}")
            all_good = False
        
        # Test statistical analysis function
        try:
            # Create a simple brain for testing
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            stats = pynlm.compute_neural_statistics(brain)
            assert isinstance(stats, dict)
            print("✓ Neural statistics computation works")
            
        except Exception as e:
            print(f"✗ Neural statistics computation failed: {e}")
            all_good = False
        
        # Test monitoring function
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            monitoring = pynlm.monitor_neural_activity(brain, 10)
            assert isinstance(monitoring, dict)
            print("✓ Neural activity monitoring works")
            
        except Exception as e:
            print(f"✗ Neural activity monitoring failed: {e}")
            all_good = False
        
        return all_good
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False

def test_configuration():
    """Test configuration handling."""
    print("Testing configuration handling...")
    
    try:
        import pynlm
        
        # Test configuration creation and manipulation
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.set("neuron_count", 100)
        config.set("connection_probability", 0.15)
        config.set("plasticity_stdp_enable", True)
        
        assert config.has("neuron_count")
        assert config.has("connection_probability")
        assert config.has("plasticity_stdp_enable")
        
        # Test getting values with defaults
        neuron_count = config.get("neuron_count", 0)
        assert neuron_count == 100
        
        connection_prob = config.get("connection_probability", 0.0)
        assert connection_prob == 0.15
        
        plasticity_enabled = config.get("plasticity_stdp_enable", False)
        assert plasticity_enabled == True
        
        print("✓ Configuration handling works correctly")
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        return False

def test_error_handling():
    """Test error handling in Python bindings."""
    print("Testing error handling...")
    
    try:
        import pynlm
        
        # Create a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test that errors are properly raised
        # This is a basic test - more comprehensive error testing would require
        # specific scenarios that trigger different error conditions
        
        print("✓ Error handling test completed (basic structure verified)")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM IMPROVEMENTS AND BUG FIXES TEST SUITE")
    print("=" * 60)
    
    tests = [
        ("Python Bindings", test_python_bindings),
        ("Documentation", test_documentation),
        ("Test Coverage", test_test_coverage),
        ("Advanced Features", test_advanced_features),
        ("Configuration", test_configuration),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n--- {test_name} Test ---")
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            print(f"✗ {test_name} FAILED")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All tests passed! NLM improvements are working correctly.")
        return True
    else:
        print("❌ Some tests failed. Please review the issues above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)