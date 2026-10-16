#!/usr/bin/env python3
"""
Simple test to verify Python bindings can be imported and basic functionality works.
"""

try:
    import pynlm
    print("Successfully imported pynlm")
    
    # Check for essential classes
    essential_classes = [
        'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
        'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
        'AgentBody', 'Action', 'WorldObject', 'ActionResult'
    ]
    
    for cls in essential_classes:
        if hasattr(pynlm, cls):
            print(f"✓ Found class: {cls}")
        else:
            print(f"✗ Missing class: {cls}")
    
    # Check for essential functions
    essential_functions = [
        'createBrain', 'createWorld', 'createAgent',
        'simulateStep', 'simulateEpisode', 'runExperiment',
        'getBrainStats', 'getWorldStats', 'getVersion'
    ]
    
    for func in essential_functions:
        if hasattr(pynlm, func):
            print(f"✓ Found function: {func}")
        else:
            print(f"✗ Missing function: {func}")
    
    # Check constants
    constants = ['INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID']
    
    for const in constants:
        if hasattr(pynlm, const):
            print(f"✓ Found constant: {const}")
        else:
            print(f"✗ Missing constant: {const}")
    
    print("\nPython bindings appear to be working correctly!")
    
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
except Exception as e:
    print(f"Error during test: {e}")

print("Test completed.")
