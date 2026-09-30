"""
Test script for advanced NLM Python examples.

This script tests basic functionality of the examples and provides
a quick way to verify that all examples can be imported and run.
"""

import sys
import os
import importlib

def test_imports():
    """Test that all example modules can be imported."""
    print("=== Testing Example Imports ===")
    
    example_modules = [
        "advanced_neural_networks",
        "memory_system_integration", 
        "neuromodulation_learning",
        "custom_world_simulations",
        "long_running_experiments",
        "visualization_debugging",
        "performance_optimization",
        "production_deployment",
        "research_experimentation"
    ]
    
    imported_modules = []
    import_errors = []
    
    for module_name in example_modules:
        try:
            module = importlib.import_module(f"python.examples.{module_name}")
            imported_modules.append(module_name)
            print(f"✓ {module_name:30} - Imported successfully")
        except ImportError as e:
            import_errors.append((module_name, str(e)))
            print(f"✗ {module_name:30} - Import failed: {e}")
        except Exception as e:
            import_errors.append((module_name, str(e)))
            print(f"⚠ {module_name:30} - Warning: {e}")
    
    print(f"\nImport Results:")
    print(f"  Successfully imported: {len(imported_modules)}/{len(example_modules)}")
    print(f"  Import errors: {len(import_errors)}")
    
    if import_errors:
        print(f"\nImport Errors:")
        for module_name, error in import_errors:
            print(f"  {module_name}: {error}")
    
    return len(import_errors) == 0

def test_example_classes():
    """Test that example classes can be accessed."""
    print("\n=== Testing Example Classes ===")
    
    class_tests = [
        ("python.examples.advanced_neural_networks", "AdvancedNeuralNetworkExamples", "ultra_large_scale_network"),
        ("python.examples.memory_system_integration", "MemorySystemExamples", "create_working_memory_example"),
        ("python.examples.neuromodulation_learning", "NeuromodulationExamples", "create_reward_modulation_example"),
        ("python.examples.custom_world_simulations", "CustomWorldExamples", "create_2d_resource_world"),
        ("python.examples.long_running_experiments", "LongRunningExperimentExamples", "create_continuous_learning_experiment"),
        ("python.examples.visualization_debugging", "VisualizationExamples", "create_brain_state_visualizer"),
        ("python.examples.performance_optimization", "PerformanceExamples", "create_memory_optimization_example"),
        ("python.examples.production_deployment", "ProductionExamples", "create_production_ready_deployment"),
        ("python.examples.research_experimentation", "ResearchExamples", "create_scientific_experiment")
    ]
    
    class_tests_passed = 0
    class_tests_failed = 0
    
    for module_path, class_name, method_name in class_tests:
        try:
            module = importlib.import_module(module_path)
            cls = getattr(module, class_name)
            
            # Test that class exists
            if hasattr(cls, method_name):
                print(f"✓ {class_name:40} - {method_name}() method available")
                class_tests_passed += 1
            else:
                print(f"✗ {class_name:40} - {method_name}() method missing")
                class_tests_failed += 1
                
        except ImportError as e:
            print(f"✗ {class_name:40} - Import failed: {e}")
            class_tests_failed += 1
        except AttributeError as e:
            print(f"✗ {class_name:40} - Attribute error: {e}")
            class_tests_failed += 1
        except Exception as e:
            print(f"⚠ {class_name:40} - Warning: {e}")
            class_tests_failed += 1
    
    print(f"\nClass Test Results:")
    print(f"  Passed: {class_tests_passed}/{len(class_tests)}")
    print(f"  Failed: {class_tests_failed}/{len(class_tests)}")
    
    return class_tests_failed == 0

def test_example_syntax():
    """Test that example files have valid Python syntax."""
    print("\n=== Testing Example Syntax ===")
    
    example_files = [
        "python/examples/advanced_neural_networks.py",
        "python/examples/memory_system_integration.py",
        "python/examples/neuromodulation_learning.py",
        "python/examples/custom_world_simulations.py",
        "python/examples/long_running_experiments.py",
        "python/examples/visualization_debugging.py",
        "python/examples/performance_optimization.py",
        "python/examples/production_deployment.py",
        "python/examples/research_experimentation.py"
    ]
    
    syntax_errors = []
    
    for file_path in example_files:
        try:
            with open(file_path, 'r') as f:
                # Read and compile the file
                source = f.read()
                compile(source, file_path, 'exec')
            print(f"✓ {os.path.basename(file_path):40} - Valid Python syntax")
        except SyntaxError as e:
            syntax_errors.append((file_path, str(e)))
            print(f"✗ {os.path.basename(file_path):40} - Syntax error: {e}")
        except Exception as e:
            syntax_errors.append((file_path, str(e)))
            print(f"⚠ {os.path.basename(file_path):40} - Warning: {e}")
    
    print(f"\nSyntax Test Results:")
    print(f"  Valid syntax: {len(example_files) - len(syntax_errors)}/{len(example_files)}")
    print(f"  Syntax errors: {len(syntax_errors)}")
    
    if syntax_errors:
        print(f"\nSyntax Errors:")
        for file_path, error in syntax_errors:
            print(f"  {os.path.basename(file_path)}: {error}")
    
    return len(syntax_errors) == 0

def main():
    """Run all tests."""
    print("Advanced NLM Python Examples - Test Suite")
    print("=" * 50)
    
    # Change to examples directory
    os.chdir("python/examples")
    
    # Run tests
    import_test = test_imports()
    class_test = test_example_classes()
    syntax_test = test_example_syntax()
    
    # Summary
    print("\n" + "=" * 50)
    print("Test Summary:")
    print(f"  Import Tests:    {'PASS' if import_test else 'FAIL'}")
    print(f"  Class Tests:     {'PASS' if class_test else 'FAIL'}")
    print(f"  Syntax Tests:    {'PASS' if syntax_test else 'FAIL'}")
    
    all_passed = import_test and class_test and syntax_test
    
    if all_passed:
        print(f"\n🎉 All tests passed! The advanced NLM examples are ready to use.")
    else:
        print(f"\n❌ Some tests failed. Please check the output above.")
    
    return all_passed

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
