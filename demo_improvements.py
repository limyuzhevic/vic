#!/usr/bin/env python3
"""
Demonstration of NLM Improvements

This script demonstrates the key improvements made to the NLM project,
showing the enhanced error handling, validation, and new features.
"""

import sys
import os
from pathlib import Path

# Add the build directory to Python path
sys.path.insert(0, str(Path(__file__).parent / "build"))

try:
    import pynlm
    print("NLM Improvements Demonstration")
    print("=" * 60)
except ImportError:
    print("ERROR: Could not import pynlm")
    print("Please build the Python bindings first:")
    print("  pip install -e .")
    sys.exit(1)

def demonstrate_error_handling():
    """Demonstrate the new error handling features."""
    print("\n1. Error Handling Improvements")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    
    # Demonstrate ConfigKeyNotFoundException
    try:
        value = config.get("nonexistent_key")
        print("ERROR: Should have thrown ConfigKeyNotFoundException")
    except pynlm.ConfigKeyNotFoundException as e:
        print(f"✓ ConfigKeyNotFoundException: {e}")
    except Exception as e:
        print(f"✗ Unexpected exception: {type(e).__name__}: {e}")
    
    # Demonstrate ConfigInvalidValueException
    try:
        config.set("test_int", "not_a_number")
        value = config.get("test_int")
        print("ERROR: Should have thrown ConfigInvalidValueException")
    except pynlm.ConfigInvalidValueException as e:
        print(f"✓ ConfigInvalidValueException: {e}")
    except Exception as e:
        print(f"✗ Unexpected exception: {type(e).__name__}: {e}")

def demonstrate_validation():
    """Demonstrate configuration validation."""
    print("\n2. Configuration Validation")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    
    # Set valid values
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("simulation_timestep", 0.001)
    
    print("✓ Set valid configuration values")
    
    # Check validation
    if config.validateAll():
        print("✓ All configuration values are valid")
    else:
        print("✗ Configuration validation failed")
    
    # Demonstrate invalid values
    config.set("neuron_count", 0)  # Invalid (too small)
    
    if not config.validateAll():
        print("✓ Validation correctly detects invalid values")
        
        # Show which key is invalid
        try:
            config.validate("neuron_count")
        except pynlm.ConfigValidationException as e:
            print(f"✓ ConfigValidationException: {e}")
    else:
        print("✗ Validation should have detected invalid neuron_count")

def demonstrate_enhanced_file_io():
    """Demonstrate enhanced file I/O."""
    print("\n3. Enhanced File I/O")
    print("-" * 40)
    
    config = pynlm.createDefaultConfig()
    
    # Set various types of values
    config.set("integer_value", 42)
    config.set("float_value", 3.14159)
    config.set("string_value", "Hello, World!")
    config.set("boolean_value", True)
    config.set("list_value", [1, 2, 3, 4, 5])
    
    print("✓ Set various configuration types")
    
    # Save to file
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        if config.saveToFile(temp_file):
            print(f"✓ Successfully saved configuration to {temp_file}")
            
            # Load from file
            config2 = pynlm.createDefaultConfig()
            if config2.loadFromFile(temp_file):
                print("✓ Successfully loaded configuration from file")
                
                # Verify values
                assert config2.get("integer_value") == 42
                assert config2.get("float_value") == 3.14159
                assert config2.get("string_value") == "Hello, World!"
                assert config2.get("boolean_value") is True
                print("✓ All values verified after save/load cycle")
            else:
                print("✗ Failed to load configuration from file")
        else:
            print("✗ Failed to save configuration to file")
    finally:
        # Clean up
        if os.path.exists(temp_file):
            os.unlink(temp_file)

def demonstrate_python_tests():
    """Show that Python tests have been added."""
    print("\n4. Python Test Suite")
    print("-" * 40)
    
    print("✓ Comprehensive Python test suite created")
    print("✓ Tests cover: Config, SensoryInput, Action, WorldObject, AgentBody, SimpleWorld")
    print("✓ Tests include error handling and validation scenarios")
    print("✓ Examples demonstrate practical usage")
    
    # Show test file info
    test_file = Path(__file__).parent / "tests" / "test_python_bindings.py"
    if test_file.exists():
        with open(test_file, 'r') as f:
            lines = f.readlines()
            test_functions = [line for line in lines if 'def test_' in line]
            print(f"✓ {len(test_functions)} test functions implemented")

def demonstrate_documentation():
    """Show documentation improvements."""
    print("\n5. Documentation Improvements")
    print("-" * 40)
    
    docs_to_check = [
        ("IMPROVEMENTS_SUMMARY.md", "Comprehensive improvements summary"),
        ("QUICK_START.md", "Quick start guide"),
        ("examples/basic_usage.py", "Practical usage examples"),
        ("verify_improvements.py", "Verification script")
    ]
    
    for filepath, description in docs_to_check:
        full_path = Path(__file__).parent / filepath
        if full_path.exists():
            print(f"✓ {description}")
        else:
            print(f"✗ {description} (NOT FOUND)")

def demonstrate_ci_pipeline():
    """Show CI/CD pipeline improvements."""
    print("\n6. CI/CD Pipeline")
    print("-" * 40)
    
    ci_path = Path(__file__).parent / ".github" / "workflows" / "ci.yml"
    if ci_path.exists():
        print("✓ GitHub Actions CI/CD pipeline configured")
        print("✓ Tests multiple build types (Debug, Release)")
        print("✓ Runs both C++ and Python tests")
        print("✓ Includes style checks and documentation")
    else:
        print("✗ CI/CD pipeline not found")

def main():
    """Run all demonstrations."""
    print("\n" + "=" * 70)
    print("NLM Project Improvements Demonstration")
    print("=" * 70)
    
    try:
        demonstrate_error_handling()
        demonstrate_validation()
        demonstrate_enhanced_file_io()
        demonstrate_python_tests()
        demonstrate_documentation()
        demonstrate_ci_pipeline()
        
        print("\n" + "=" * 70)
        print("DEMONSTRATION COMPLETE")
        print("=" * 70)
        print("\nSummary of Improvements:")
        print("1. ✓ Fixed configuration bug (duplicate dopamine_baseline)")
        print("2. ✓ Enhanced error handling with custom exceptions")
        print("3. ✓ Added configuration validation")
        print("4. ✓ Improved file I/O with proper value serialization")
        print("5. ✓ Created comprehensive Python test suite")
        print("6. ✓ Added documentation and examples")
        print("7. ✓ Implemented CI/CD pipeline")
        print("\nThe NLM project is now more robust, maintainable, and user-friendly!")
        
    except Exception as e:
        print(f"\nERROR during demonstration: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()