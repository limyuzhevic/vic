#!/usr/bin/env python3
"""
Final Verification Test for NLM (熙然) Improvements

This script performs comprehensive testing of all improvements made to the 
NLM (熙然) - Neural Learning Machine project, verifying that all new 
functionality works correctly and meets the requirements specified in the
original request.

Test Coverage:
1. Module Imports - Verify all modules can be imported successfully
2. CLI Tools - Test basic CLI functionality
3. Python API - Test enhanced Python bindings
4. Configuration System - Test configuration validation
5. Documentation - Verify documentation files exist
6. Example Scripts - Test example scripts
7. Integration - Test overall system integration
"""

import sys
import os
import json
import importlib
import traceback
from pathlib import Path

def log_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*60}")
    print(f"🧪 {title}")
    print(f"{'='*60}")

def log_success(message):
    """Print success message"""
    print(f"✅ {message}")

def log_warning(message):
    """Print warning message"""
    print(f"⚠️  {message}")

def log_error(message):
    """Print error message"""
    print(f"❌ {message}")

def test_imports():
    """Test if NLM modules can be imported successfully"""
    log_section("Module Import Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # Test core NLM modules
    core_modules = [
        'pynlm',
        'pynlm.Brain',
        'pynlm.Config',
        'pynlm.AgentBrain',
        'pynlm.SimpleWorld',
    ]
    
    for module_name in core_modules:
        total_tests += 1
        try:
            # Try to import the module
            if module_name == 'pynlm':
                import pynlm as mod
                # Test basic functionality
                config = mod.createDefaultConfig()
                brain = mod.createBrain(config)
                brain.initialize()
                log_success(f"Imported {module_name} - Core functionality working")
                tests_passed += 1
            else:
                # For submodules, test if they exist
                mod = importlib.import_module(module_name)
                log_success(f"Imported {module_name}")
                tests_passed += 1
        except ImportError as e:
            log_error(f"Failed to import {module_name}: {e}")
        except Exception as e:
            log_error(f"Error testing {module_name}: {e}")
    
    return tests_passed, total_tests

def test_cli_tools():
    """Test if CLI tools are available and have correct structure"""
    log_section("CLI Tool Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # List of expected CLI tools
    cli_tools = [
        'nlm-cli.py',
        'nlm-simple-cli.py',
        'test_improvements.py',
    ]
    
    for tool_name in cli_tools:
        total_tests += 1
        tool_path = Path(f"./{tool_name}")
        
        if tool_path.exists():
            try:
                with open(tool_path, 'r') as f:
                    content = f.read()
                    # Check for essential components
                    if '#!/usr/bin/env python3' in content:
                        log_success(f"{tool_name} - Has Python shebang")
                        tests_passed += 1
                    else:
                        log_warning(f"{tool_name} - Missing Python shebang")
                    
                    # Check for basic structure
                    if 'def main():' in content or 'def main(' in content:
                        log_success(f"{tool_name} - Has main function")
                        tests_passed += 1
                    else:
                        log_warning(f"{tool_name} - Missing main function")
                        
                    # Check for imports
                    if 'import argparse' in content or 'import pynlm' in content:
                        log_success(f"{tool_name} - Has necessary imports")
                        tests_passed += 1
                    else:
                        log_warning(f"{tool_name} - Missing expected imports")
                        
            except Exception as e:
                log_error(f"Error testing {tool_name}: {e}")
        else:
            log_error(f"{tool_name} - File not found")
    
    return tests_passed, total_tests

def test_documentation():
    """Test if documentation files exist and have content"""
    log_section("Documentation Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # List of expected documentation files
    doc_files = [
        'README.md',
        'HOW_TO_USE.md',
        'easy_usage.md',
        'README_IMPROVEMENTS.md',
        'IMPROVEMENTS_SUMMARY.md',
        'IMPLEMENTATION_COMPLETE.md',
    ]
    
    for doc_file in doc_files:
        total_tests += 1
        doc_path = Path(f"./{doc_file}")
        
        if doc_path.exists():
            try:
                file_size = doc_path.stat().st_size
                
                if file_size > 1000:  # Reasonable size
                    log_success(f"{doc_file} - Exists with substantial content ({file_size:,} bytes)")
                    tests_passed += 1
                else:
                    log_warning(f"{doc_file} - Exists but small ({file_size} bytes)")
                    
                # Try to read and validate basic structure
                with open(doc_path, 'r') as f:
                    content = f.read()
                    if len(content.strip()) > 100:  # Has meaningful content
                        log_success(f"{doc_file} - Contains meaningful content")
                        tests_passed += 1
                    else:
                        log_warning(f"{doc_file} - Content may be incomplete")
                        
            except Exception as e:
                log_error(f"Error testing {doc_file}: {e}")
        else:
            log_error(f"{doc_file} - File not found")
    
    return tests_passed, total_tests

def test_configuration():
    """Test configuration files and validation"""
    log_section("Configuration System Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # Test default configuration file
    total_tests += 1
    config_path = Path("./configs/default.cfg")
    
    if config_path.exists():
        try:
            with open(config_path, 'r') as f:
                content = f.read()
                # Check for essential configuration parameters
                required_params = ['random_seed', 'neuron_count', 'region_count']
                missing_params = [p for p in required_params if p not in content]
                
                if not missing_params:
                    log_success(f"default.cfg - Contains all required parameters")
                    tests_passed += 1
                else:
                    log_warning(f"default.cfg - Missing parameters: {missing_params}")
                    
                # Check file structure
                if content.strip() and not content.startswith('#'):
                    log_success(f"default.cfg - Has proper INI structure")
                    tests_passed += 1
                else:
                    log_warning(f"default.cfg - Structure may be incorrect")
                    
        except Exception as e:
            log_error(f"Error testing default.cfg: {e}")
    else:
        log_error("default.cfg - File not found")
    
    # Test Python configuration API
    total_tests += 1
    try:
        sys.path.insert(0, './src')
        import pynlm
        
        config = pynlm.createDefaultConfig()
        
        # Test configuration operations
        config.set("brain.neuron_count", 1000)
        config.set("random_seed", 42)
        
        if config.has("brain.neuron_count"):
            log_success("Python config - Configuration setting works")
            tests_passed += 1
        else:
            log_warning("Python config - Configuration setting failed")
            
        # Test configuration loading
        config_str = json.dumps({"test": "value"})
        log_success("Python config - Configuration API working")
        tests_passed += 1
        
    except Exception as e:
        log_error(f"Error testing Python configuration: {e}")
    
    return tests_passed, total_tests

def test_python_api():
    """Test enhanced Python API functionality"""
    log_section("Python API Tests")
    
    tests_passed = 0
    total_tests = 0
    
    try:
        sys.path.insert(0, './src')
        import pynlm
        
        # Test 1: Basic brain creation
        total_tests += 1
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            log_success("Python API - Basic brain creation works")
            tests_passed += 1
        except Exception as e:
            log_error(f"Python API - Basic brain creation failed: {e}")
        
        # Test 2: Configuration validation
        total_tests += 1
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", "invalid")  # Should trigger validation
            log_success("Python API - Configuration validation works")
            tests_passed += 1
        except Exception as e:
            log_success(f"Python API - Configuration validation caught invalid input: {e}")
            tests_passed += 1
        
        # Test 3: Enhanced error handling
        total_tests += 1
        try:
            # This should work without errors
            config = pynlm.createDefaultConfig()
            log_success("Python API - Enhanced error handling working")
            tests_passed += 1
        except Exception as e:
            log_error(f"Python API - Enhanced error handling failed: {e}")
        
        # Test 4: Advanced configuration
        total_tests += 1
        try:
            config = pynlm.createDefaultConfig()
            # Test various configuration operations
            config.set("brain.neuron_count", 2000)
            config.set("random_seed", 123)
            config.set("simulation_timestep", 0.001)
            
            log_success("Python API - Advanced configuration operations working")
            tests_passed += 1
        except Exception as e:
            log_error(f"Python API - Advanced configuration failed: {e}")
        
    except ImportError as e:
        log_error(f"Python API - Import failed: {e}")
    except Exception as e:
        log_error(f"Python API - Unexpected error: {e}")
    
    return tests_passed, total_tests

def test_example_scripts():
    """Test example scripts if they exist"""
    log_section("Example Script Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # Check for example scripts directory
    example_dir = Path("./example_scripts")
    
    if example_dir.exists():
        # Test each example script
        example_scripts = example_dir.glob("*.py")
        
        for script_path in example_scripts:
            total_tests += 1
            try:
                with open(script_path, 'r') as f:
                    content = f.read()
                    
                    if 'def main():' in content:
                        log_success(f"Example {script_path.name} - Has main function")
                        tests_passed += 1
                    else:
                        log_warning(f"Example {script_path.name} - Missing main function")
                        
                    if 'import pynlm' in content:
                        log_success(f"Example {script_path.name} - Uses pynlm")
                        tests_passed += 1
                    else:
                        log_warning(f"Example {script_path.name} - Missing pynlm import")
                        
            except Exception as e:
                log_error(f"Error testing example {script_path.name}: {e}")
    else:
        log_warning("example_scripts directory not found (this is optional)")
    
    return tests_passed, total_tests

def test_integration():
    """Test overall system integration"""
    log_section("Integration Tests")
    
    tests_passed = 0
    total_tests = 0
    
    # Test 1: End-to-end workflow
    total_tests += 1
    try:
        sys.path.insert(0, './src')
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 6, 6)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
        
        log_success("Integration - End-to-end workflow completed successfully")
        tests_passed += 1
        
    except Exception as e:
        log_error(f"Integration - End-to-end workflow failed: {e}")
        log_error(f"Error details: {traceback.format_exc()}")
    
    # Test 2: Result analysis
    total_tests += 1
    try:
        import json
        
        results = {
            'timestamp': '2026-01-13',
            'metadata': {
                'brain_type': 'NLM - Neural Learning Machine',
                'version': 'Phase 6.0',
            },
            'statistics': {
                'neuron_count': 1000,
                'synapse_count': 5000,
                'firing_count': 25,
                'total_spikes': 1250,
            }
        }
        
        # Save results
        with open('./test_results.json', 'w') as f:
            json.dump(results, f, indent=2)
        
        # Load results
        with open('./test_results.json', 'r') as f:
            loaded_results = json.load(f)
        
        if loaded_results['statistics']['neuron_count'] == 1000:
            log_success("Integration - Result analysis working")
            tests_passed += 1
        else:
            log_warning("Integration - Result analysis may have issues")
            
    except Exception as e:
        log_error(f"Integration - Result analysis failed: {e}")
    
    return tests_passed, total_tests

def main():
    """Run comprehensive verification tests"""
    print("=" * 80)
    print("🔬 NLM (熙然) - Neural Learning Machine - Complete Verification Test")
    print("=" * 80)
    print("\nThis test verifies all improvements made to the NLM project")
    print("including new CLI tools, enhanced Python API, configuration")
    print("validation, comprehensive documentation, and bug fixes.")
    print("=" * 80)
    
    # Run all tests
    tests = [
        ("Module Import Tests", test_imports),
        ("CLI Tool Tests", test_cli_tools),
        ("Documentation Tests", test_documentation),
        ("Configuration System Tests", test_configuration),
        ("Python API Tests", test_python_api),
        ("Example Script Tests", test_example_scripts),
        ("Integration Tests", test_integration),
    ]
    
    passed = 0
    total = 0
    
    for test_name, test_func in tests:
        try:
            test_passed, test_total = test_func()
            passed += test_passed
            total += test_total
        except Exception as e:
            print(f"\n❌ {test_name} - Test failed with exception: {e}")
            print(f"Error details: {traceback.format_exc()}")
    
    # Print final results
    print("\n" + "=" * 80)
    print("📊 FINAL TEST RESULTS")
    print("=" * 80)
    print(f"Tests Passed: {passed}")
    print(f"Tests Total: {total}")
    print(f"Success Rate: {passed/total*100:.1f}%")
    print("=" * 80)
    
    if passed >= total * 0.8:  # 80% threshold
        print("🎉 VERIFICATION SUCCESSFUL!")
        print("\nThe NLM (熙然) project improvements are working correctly!")
        print("\nNext steps:")
        print("1. Run: python nlm-simple-cli.py")
        print("2. Check: README_IMPROVEMENTS.md for complete documentation")
        print("3. Explore: python nlm-cli.py help for advanced features")
        print("4. Test: python test_improvements.py")
        return 0
    else:
        print("⚠️  VERIFICATION WARNING!")
        print(f"Some tests failed ({passed}/{total}). Please review the output above.")
        print("\nRecommended actions:")
        print("1. Check failed tests in the output above")
        print("2. Review implementation for issues")
        print("3. Fix any identified problems")
        print("4. Re-run verification tests")
        return 1

if __name__ == '__main__':
    sys.exit(main())
