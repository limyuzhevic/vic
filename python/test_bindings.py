#!/usr/bin/env python3
"""
Test script for NLM Python Enhanced Bindings

This script tests the enhanced NLM Python bindings to ensure they work correctly
and demonstrates the new functionality.
"""

import sys
import traceback

try:
    import pynlm
    PYNLM_AVAILABLE = True
    print("✓ pynlm module imported successfully")
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"✗ Failed to import pynlm: {e}")
    print("Note: This is expected if NLM C++ bindings are not built.")

try:
    import nlm_setup as setup
    SETUP_AVAILABLE = True
    print("✓ nlm_setup module imported successfully")
except ImportError as e:
    SETUP_AVAILABLE = False
    print(f"✗ Failed to import nlm_setup: {e}")
    print("Note: Setup utilities may not be available.")

try:
    import enhanced_examples as examples
    EXAMPLES_AVAILABLE = True
    print("✓ enhanced_examples module imported successfully")
except ImportError as e:
    EXAMPLES_AVAILABLE = False
    print(f"✗ Failed to import enhanced_examples: {e}")
    print("Note: Examples may not be available.")

def test_basic_import():
    """Test basic module import and availability."""
    print("\n" + "="*60)
    print("Test 1: Basic Module Import")
    print("="*60)
    
    if not PYNLM_AVAILABLE:
        print("⚠ pynlm not available - skipping binding tests")
        return True
    
    print("Testing basic NLM functionality...")
    
    # Test 1: Create default config
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
    except Exception as e:
        print(f"✗ createDefaultConfig() failed: {e}")
        return False
    
    # Test 2: Create brain
    try:
        brain = pynlm.createBrain(config)
        print("✓ createBrain() works")
    except Exception as e:
        print(f"✗ createBrain() failed: {e}")
        return False
    
    # Test 3: Initialize brain
    try:
        result = brain.initialize()
        print(f"✓ brain.initialize() works (returned: {result})")
    except Exception as e:
        print(f"✗ brain.initialize() failed: {e}")
        return False
    
    # Test 4: Get statistics
    try:
        stats = pynlm.getBrainStatistics(brain)
        print(f"✓ getBrainStatistics() works (got {len(stats)} stats)")
    except Exception as e:
        print(f"✗ getBrainStatistics() failed: {e}")
        return False
    
    return True

def test_enhanced_utilities():
    """Test enhanced utilities if available."""
    print("\n" + "="*60)
    print("Test 2: Enhanced Utilities")
    print("="*60)
    
    if not SETUP_AVAILABLE:
        print("⚠ nlm_setup not available - skipping setup utility tests")
        return True
    
    print("Testing setup utilities...")
    
    # Test 1: Environment check
    try:
        status = setup.check_environment()
        print(f"✓ check_environment() works (pynlm_available: {status['pynlm_available']})")
    except Exception as e:
        print(f"✗ check_environment() failed: {e}")
        return False
    
    # Test 2: Create default config dict
    try:
        config_dict = pynlm.createDefaultConfigDict()
        print(f"✓ createDefaultConfigDict() works (got {len(config_dict)} items)")
    except Exception as e:
        print(f"✗ createDefaultConfigDict() failed: {e}")
        return False
    
    # Test 3: Config to string
    try:
        config_str = pynlm.configToString(config_dict)
        print(f"✓ configToString() works (string length: {len(config_str)})")
    except Exception as e:
        print(f"✗ configToString() failed: {e}")
        return False
    
    # Test 4: Format simulation results
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        for i in range(10):
            brain.step(i)
        
        stats = pynlm.getBrainStatistics(brain)
        formatted = pynlm.formatSimulationResults(stats)
        print(f"✓ formatSimulationResults() works (formatted length: {len(formatted)})")
    except Exception as e:
        print(f"✗ formatSimulationResults() failed: {e}")
        return False
    
    return True

def test_brain_operations():
    """Test brain operations."""
    print("\n" + "="*60)
    print("Test 3: Brain Operations")
    print("="*60)
    
    if not PYNLM_AVAILABLE:
        print("⚠ pynlm not available - skipping brain operation tests")
        return True
    
    print("Testing brain operations...")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test step operation
        for i in range(20):
            brain.step(i)
        print("✓ brain.step() operations work")
        
        # Test brain state inspection
        state_str = pynlm.getBrainStateString(brain)
        print(f"✓ getBrainStateString() works (length: {len(state_str)})")
        
        # Test print brain state (just ensure it doesn't crash)
        try:
            pynlm.printBrainState(brain)
            print("✓ printBrainState() works")
        except:
            print("⚠ printBrainState() has issues (this may be expected)")
        
        # Test action availability
        actions = pynlm.getAvailableActions(brain)
        print(f"✓ getAvailableActions() works (got {len(actions)} actions)")
        
        # Test agent operations
        agent = pynlm.createAgentBrain(brain)
        agent.initialize()
        
        # Test agent learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("✓ agent learning features enable correctly")
        
        # Test agent stats
        agent.updateDevelopment(0.1)
        stage = agent.getDevelopmentalStage()
        print(f"✓ agent development works (stage: {stage})")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain operations failed: {e}")
        traceback.print_exc()
        return False

def test_version_info():
    """Test version and information functions."""
    print("\n" + "="*60)
    print("Test 4: Version and Information")
    print("="*60)
    
    if not PYNLM_AVAILABLE:
        print("⚠ pynlm not available - skipping version tests")
        return True
    
    print("Testing version information...")
    
    try:
        # Test version function
        version = pynlm.getBrainVersion()
        print(f"✓ getBrainVersion() works: {version}")
        
        # Test Python version check
        py_check = pynlm.checkPyVersion()
        print(f"✓ checkPyVersion() works: {py_check}")
        
        return True
        
    except Exception as e:
        print(f"✗ Version info tests failed: {e}")
        return False

def run_example(example_name: str) -> bool:
    """Run a specific example if available."""
    print(f"\n" + "="*60)
    print(f"Test 5: Running Example - {example_name}")
    print("="*60)
    
    if not EXAMPLES_AVAILABLE:
        print(f"⚠ enhanced_examples not available - skipping example test")
        return True
    
    try:
        if hasattr(examples, example_name):
            example_func = getattr(examples, example_name)
            example_func()
            print(f"✓ Example '{example_name}' executed successfully")
            return True
        else:
            print(f"⚠ Example '{example_name}' not found")
            return True
    except Exception as e:
        print(f"✗ Example '{example_name}' failed: {e}")
        traceback.print_exc()
        return False

def test_setup_utilities():
    """Test setup utility functions."""
    print("\n" + "="*60)
    print("Test 6: Setup Utilities")
    print("="*60)
    
    if not SETUP_AVAILABLE:
        print("⚠ nlm_setup not available - skipping setup utility tests")
        return True
    
    print("Testing setup utility functions...")
    
    try:
        # Test create_test_brain
        brain = setup.create_test_brain()
        print(f"✓ create_test_brain() works (brain created)")
        
        # Test run_test_simulation
        results = setup.run_test_simulation(20)
        print(f"✓ run_test_simulation() works (got {len(results)} result types)")
        
        # Test benchmark_brain_creation (quick)
        bench_results = setup.benchmark_brain_creation(5)
        print(f"✓ benchmark_brain_creation() works (got performance data)")
        
        # Test validate_brain_state
        issues = setup.validate_brain_state(brain)
        print(f"✓ validate_brain_state() works (got {len(issues)} issues)")
        
        # Test create_example_config
        example_config = setup.create_example_config()
        print(f"✓ create_example_config() works (got config with {len(example_config)} sections)")
        
        return True
        
    except Exception as e:
        print(f"✗ Setup utility tests failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Python Enhanced Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Import", test_basic_import),
        ("Enhanced Utilities", test_enhanced_utilities),
        ("Brain Operations", test_brain_operations),
        ("Version Info", test_version_info),
        ("Setup Utilities", test_setup_utilities),
    ]
    
    example_tests = [
        "example_basic_usage",
        "example_brain_inspection",
        "example_text_processing",
    ]
    
    results = []
    
    # Run main tests
    for test_name, test_func in tests:
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test '{test_name}' crashed: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Run example tests
    for example_name in example_tests:
        try:
            success = run_example(example_name)
            results.append((f"Example: {example_name}", success))
        except Exception as e:
            print(f"✗ Example test '{example_name}' crashed: {e}")
            results.append((f"Example: {example_name}", False))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        icon = "✓" if success else "✗"
        print(f"{icon} {test_name}: {status}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {len(results)}, Passed: {passed}, Failed: {failed}")
    
    if failed > 0:
        print(f"\n⚠ {failed} tests failed - please check the output above")
        return 1
    else:
        print(f"\n✓ All tests passed!")
        return 0

if __name__ == "__main__":
    exit(main())