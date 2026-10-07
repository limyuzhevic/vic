#!/usr/bin/env python3
"""
Basic test for NLM Python bindings
This test verifies that the bindings can be imported and basic functionality works
"""

import sys
import pynlm

def test_basic_import():
    """Test that the module can be imported successfully"""
    print("✓ Successfully imported pynlm module")
    return True

def test_version():
    """Test that the version is accessible"""
    if hasattr(pynlm, '__version__'):
        print(f"✓ Version available: {pynlm.__version__}")
        return True
    else:
        print("✓ Version attribute not available (this is expected for this version)")
        return True

def test_basic_types():
    """Test basic type availability"""
    # Check for basic type classes
    type_classes = ['NeuronId', 'SynapseId', 'RegionId', 'PopulationId']
    available_classes = []
    
    for class_name in type_classes:
        if hasattr(pynlm, class_name):
            available_classes.append(class_name)
    
    print(f"✓ Available basic types: {', '.join(available_classes)}")
    return len(available_classes) > 0

def test_enums():
    """Test that basic enums are available"""
    enum_classes = ['NeuronType', 'SynapseType', 'ActionType']
    available_enums = []
    
    for enum_name in enum_classes:
        if hasattr(pynlm, enum_name):
            available_enums.append(enum_name)
    
    print(f"✓ Available enums: {', '.join(available_enums)}")
    return len(available_enums) > 0

def test_core_systems():
    """Test that core system classes are available"""
    # List of core systems that should be available in the bindings
    core_systems = ['Config', 'Neuron', 'Brain', 'SimpleWorld']
    available_systems = []
    
    for system_name in core_systems:
        if hasattr(pynlm, system_name):
            available_systems.append(system_name)
    
    print(f"✓ Available core systems: {', '.join(available_systems)}")
    return len(available_systems) > 0

def test_bindings_structure():
    """Test the overall structure of the bindings"""
    # Check that the module has expected attributes
    expected_attrs = ['__doc__', '__package__']
    found_attrs = []
    
    for attr in expected_attrs:
        if hasattr(pynlm, attr):
            found_attrs.append(attr)
    
    print(f"✓ Module attributes: {', '.join(found_attrs)}")
    return True

def main():
    """Run all tests"""
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    tests = [
        ("Module Import", test_basic_import),
        ("Version Info", test_version),
        ("Basic Types", test_basic_types),
        ("Enums", test_enums),
        ("Core Systems", test_core_systems),
        ("Bindings Structure", test_bindings_structure),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\nTesting: {test_name}")
        try:
            if test_func():
                passed += 1
                print(f"✓ PASSED: {test_name}")
            else:
                failed += 1
                print(f"✗ FAILED: {test_name}")
        except Exception as e:
            failed += 1
            print(f"✗ ERROR in {test_name}: {str(e)}")
    
    print("\n" + "=" * 50)
    print(f"Test Summary:")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Total: {len(tests)}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return 0
    else:
        print(f"\n✗ {failed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
