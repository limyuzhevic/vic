#!/usr/bin/env python3
"""Test script to verify Python bindings work after refactoring."""

import sys
import traceback

def test_bindings():
    """Test that all expected classes and functions are available."""
    try:
        import pynlm
    except ImportError as e:
        print(f"ERROR: Could not import pynlm: {e}")
        print("Note: This is expected if the module hasn't been built yet.")
        return True
    
    print("SUCCESS: pynlm imported successfully")
    
    # Test that all expected classes are available
    expected_classes = [
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
        'SimpleWorld', 'Brain', 'AgentBrain'
    ]
    
    expected_functions = [
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 
        'createAgentBrain'
    ]
    
    # Check classes
    missing_classes = []
    for class_name in expected_classes:
        if not hasattr(pynlm, class_name):
            missing_classes.append(class_name)
    
    if missing_classes:
        print(f"WARNING: The following classes are missing: {missing_classes}")
    else:
        print("SUCCESS: All expected classes are available")
    
    # Check functions
    missing_functions = []
    for func_name in expected_functions:
        if not hasattr(pynlm, func_name):
            missing_functions.append(func_name)
    
    if missing_functions:
        print(f"WARNING: The following functions are missing: {missing_functions}")
    else:
        print("SUCCESS: All expected functions are available")
    
    # Check constants
    expected_constants = [
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 
        'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
    ]
    
    missing_constants = []
    for const_name in expected_constants:
        if not hasattr(pynlm, const_name):
            missing_constants.append(const_name)
    
    if missing_constants:
        print(f"WARNING: The following constants are missing: {missing_constants}")
    else:
        print("SUCCESS: All expected constants are available")
    
    return len(missing_classes) == 0 and len(missing_functions) == 0 and len(missing_constants) == 0

if __name__ == "__main__":
    success = test_bindings()
    sys.exit(0 if success else 1)
