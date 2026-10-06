#!/usr/bin/env python3
"""
Comprehensive test runner for NLM (Neural Learning Machine)
This script runs both C++ unit tests and Python bindings tests
"""

import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd, description):
    """Run a command and print output"""
    print(f"\n=== {description} ===")
    print(f"Command: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(f"STDERR:\n{result.stderr}")
        return result.returncode == 0
    except Exception as e:
        print(f"Error running command: {e}")
        return False

def build_cpp_tests():
    """Build C++ tests"""
    print("Building C++ tests...")
    
    project_dir = Path(__file__).parent
    build_dir = project_dir / "build_cpp_tests"
    build_dir.mkdir(exist_ok=True)
    
    # Configure CMake
    cmake_cmd = f"cmake {project_dir} -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON"
    if not run_command(cmake_cmd, "Configure CMake"):
        return False
    
    # Build tests
    if not run_command("make -j$(nproc)", "Build C++ tests"):
        return False
    
    # Run C++ tests
    if os.path.exists(build_dir / "test_error_handling"):
        if not run_command("./test_error_handling", "Run error handling test"):
            return False
    
    test_executable = build_dir / "test"
    if test_executable.exists():
        if not run_command(str(test_executable), "Run NLM main test"):
            return False
    
    # Clean up
    import shutil
    shutil.rmtree(build_dir, ignore_errors=True)
    return True

def run_python_tests():
    """Run Python tests"""
    print("Running Python tests...")
    
    project_dir = Path(__file__).parent
    
    # Change to python directory
    python_dir = project_dir / "python"
    if not python_dir.exists():
        print(f"Python directory not found: {python_dir}")
        return False
    
    # Install package in development mode
    if not run_command("pip install -e .", "Install Python package in development mode"):
        return False
    
    # Run pytest
    if not run_command("pytest tests/ -v --tb=short", "Run pytest on Python tests"):
        return False
    
    return True

def run_integration_tests():
    """Run integration tests between C++ and Python"""
    print("Running integration tests...")
    
    project_dir = Path(__file__).parent
    
    # Create a simple integration test script
    integration_test = project_dir / "integration_test.py"
    test_content = '''#!/usr/bin/env python3
"""
Integration test for NLM C++ core and Python bindings
"""
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "python"))

import pynlm as nlm

def test_basic_config():
    """Test basic config functionality"""
    print("Testing Config...")
    config = nlm.Config()
    
    # Test dict-like access
    config["neuron_count"] = 100
    config["connection_probability"] = 0.1
    
    assert config["neuron_count"] == "100"
    assert float(config["connection_probability"]) == 0.1
    
    # Test type-safe access
    neuron_count = config.get_int("neuron_count")
    assert neuron_count == 100
    
    print("✓ Config test passed")

def test_brain_creation():
    """Test brain creation and basic functionality"""
    print("Testing Brain...")
    
    config_dict = nlm.get_default_config()
    config_dict["neuron_count"] = 10
    config_dict["region_count"] = 1
    
    world, brain, agent_brain = nlm.create_agent_simulation(config_dict)
    
    # Test basic brain operations
    assert brain.neuron_count() == 10
    assert brain.get_region_count() == 1
    
    # Test step operation
    brain.step(0)
    brain.step(1)
    
    print("✓ Brain test passed")

def test_error_handling():
    """Test error handling"""
    print("Testing Error Handling...")
    
    # Import error management if available
    try:
        import pynlm.error
        error_manager = pynlm.get_error_manager()
        error_manager.enable(True)
        
        # Create a config and trigger an error
        config = nlm.Config()
        result = config.load_from_file("/nonexistent/file")
        
        # Error should have been recorded
        error_count = error_manager.get_error_count()
        print(f"   Errors encountered: {error_count}")
        
    except ImportError:
        print("   Error management not available, skipping detailed error tests")
    
    print("✓ Error handling test passed")

def test_workflow():
    """Test complete workflow"""
    print("Testing Workflow...")
    
    # Create simulation
    config = nlm.get_default_config()
    config["neuron_count"] = 50
    config["region_count"] = 2
    config["enable_development"] = False
    
    world, brain, agent_brain = nlm.create_agent_simulation(config)
    
    # Run several steps
    actions = []
    for step in range(5):
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process with agent brain
        agent_brain.process_sensory_input(percept)
        
        # Get action
        action = agent_brain.decode_motor_command()
        actions.append(action)
        
        # Update world
        world.step(0.1)
    
    # Verify results
    assert len(actions) == 5
    assert brain.neuron_count() == 50
    assert brain.get_region_count() == 2
    
    print(f"✓ Workflow test passed ({len(actions)} actions taken)")

def main():
    """Run all integration tests"""
    print("Running NLM Integration Tests")
    print("=" * 50)
    
    tests = [
        test_basic_config,
        test_brain_creation,
        test_error_handling,
        test_workflow,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        sys.exit(1)
    
    print("All integration tests passed!")

if __name__ == "__main__":
    main()
'''
    
    with open(integration_test, 'w') as f:
        f.write(test_content)
    
    # Make it executable
    os.chmod(integration_test, 0o755)
    
    # Run the integration test
    if not run_command(f"python {integration_test}", "Run integration tests"):
        return False
    
    # Clean up
    os.remove(integration_test)
    return True

def main():
    """Main test runner function"""
    print("NLM Comprehensive Test Suite")
    print("=" * 50)
    
    tests = [
        ("C++ Tests", build_cpp_tests),
        ("Python Tests", run_python_tests),
        ("Integration Tests", run_integration_tests),
    ]
    
    results = {}
    
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running {test_name}")
        print(f"{'='*50}")
        
        try:
            success = test_func()
            results[test_name] = success
            status = "PASSED" if success else "FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            results[test_name] = False
            print(f"\n{test_name}: FAILED with exception: {e}")
    
    # Summary
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print(f"{'='*50}")
    
    all_passed = True
    for test_name, success in results.items():
        status = "PASSED" if success else "FAILED"
        print(f"{test_name}: {status}")
        if not success:
            all_passed = False
    
    if all_passed:
        print("\n✓ All tests passed!")
        return 0
    else:
        print("\n✗ Some tests failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
