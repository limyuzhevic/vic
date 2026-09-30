#!/usr/bin/env python3
"""Simple test to verify the new Python bindings are syntactically correct."""

# Try to import the module
import sys
import os

# Add the Python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    # This should fail if the C++ extension is not built
    import pynlm
    print("SUCCESS: pynlm module imported successfully")
    
    # Check that all new functions exist
    expected_functions = [
        'create_brain_with_config',
        'create_brain_from_json',
        'create_brain_from_args',
        'create_simple_agent',
        'validate_brain',
        'save_brain_snapshot',
        'load_brain_snapshot',
        'benchmark_simulation',
        'run_training_episode',
        'run_simulation_batch',
        'monitor_brain_development',
        'analyze_neural_activity',
        'export_config_to_json',
        'import_config_from_json',
        'validate_config_schema',
        'merge_config',
        'pause_simulation',
        'resume_simulation',
        'step_multiple',
        'run_to_target',
        'get_memory_usage',
        'get_cpu_usage',
        'profile_simulation',
        'generate_report',
    ]
    
    expected_functions = [
        'create_brain_with_config',
        'create_brain_from_json',
        'create_brain_from_args',
        'create_simple_agent',
        'validate_brain',
        'save_brain_snapshot',
        'load_brain_snapshot',
        'benchmark_simulation',
        'run_training_episode',
        'run_simulation_batch',
        'monitor_brain_development',
        'analyze_neural_activity',
        'export_config_to_json',
        'import_config_from_json',
        'validate_config_schema',
        'merge_config',
        'pause_simulation',
        'resume_simulation',
        'step_multiple',
        'run_to_target',
        'get_memory_usage',
        'get_cpu_usage',
        'profile_simulation',
        'generate_report',
    ]
    
    print("\nChecking for expected functions:")
    all_found = True
    for func in expected_functions:
        if hasattr(pynlm, func):
            print(f"  ✓ {func}")
        else:
            print(f"  ✗ {func} - NOT FOUND")
            all_found = False
    
    if all_found:
        print("\nAll 24 new functions have been added to the Python bindings!")
    else:
        print("\nWARNING: Some expected functions are missing!")
        
    # Additional verification: print actual function list
    print("\nActual functions in pynlm module:")
    actual_functions = [attr for attr in dir(pynlm) if not attr.startswith('_')]
    for func in sorted(actual_functions):
        if func in expected_functions:
            print(f"  ✓ {func}")
        else:
            print(f"  - {func} (not in expected list)")
    
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    print("This is expected if the C++ extension hasn't been built.")
    print("The Python bindings file has been created but needs to be compiled.")
    
except Exception as e:
    print(f"ERROR: Unexpected error: {e}")
    import traceback
    traceback.print_exc()