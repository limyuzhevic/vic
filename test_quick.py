#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("Testing NLM Python bindings...")

# Test 1: Check if we can import the main modules
try:
    from pynlm import Config, Brain, SimpleWorld, create_default_config
    print("✓ Successfully imported core NLM modules")
except ImportError as e:
    print(f"✗ Failed to import core modules: {e}")
    sys.exit(1)

# Test 2: Test type definitions
try:
    from pynlm import NeuronId, SynapseId, RegionId, ActionType
    
    # Create instances
    neuron_id = NeuronId(1)
    synapse_id = SynapseId(42)
    region_id = RegionId(100)
    
    action_type = ActionType.MoveForward
    
    print(f"✓ Created NeuronId: {neuron_id}")
    print(f"✓ Created SynapseId: {synapse_id}")
    print(f"✓ Created RegionId: {region_id}")
    print(f"✓ Created ActionType: {action_type}")
    
except Exception as e:
    print(f"✗ Failed type definitions: {e}")
    sys.exit(1)

# Test 3: Test sensory classes
try:
    from pynlm import Vision, Audio, InternalSignals
    
    # Create instances
    vision = Vision(100, 200, 3)
    audio = Audio(44100, 1000)
    signals = InternalSignals()
    
    print(f"✓ Created Vision: {vision}")
    print(f"✓ Created Audio: {audio}")
    print(f"✓ Created InternalSignals: {signals}")
    
except Exception as e:
    print(f"✗ Failed sensory classes: {e}")
    sys.exit(1)

# Test 4: Test motor classes
try:
    from pynlm import Action, ActionResult, ActionType
    
    # Create instances
    action = Action(ActionType.MoveForward, [1.0, 0.5])
    result = ActionResult(5.0, True, "Success!")
    
    print(f"✓ Created Action: {action}")
    print(f"✓ Created ActionResult: {result}")
    
except Exception as e:
    print(f"✗ Failed motor classes: {e}")
    sys.exit(1)

# Test 5: Test utility functions
try:
    from pynlm import create_default_config, parse_action_string, format_action
    
    # Test config
    config = create_default_config()
    print(f"✓ Created config: {config}")
    
    # Test action parsing
    parsed_action = parse_action_string("move forward")
    formatted_action = format_action(parsed_action)
    
    print(f"✓ Parsed action: {parsed_action}")
    print(f"✓ Formatted action: {formatted_action}")
    
except Exception as e:
    print(f"✗ Failed utility functions: {e}")
    sys.exit(1)

# Test 6: Test advanced features
try:
    from pynlm import SimulationLoop, ActionSequence
    
    # Test ActionSequence
    sequence = ActionSequence()
    action1 = Action(ActionType.MoveForward)
    action2 = Action(ActionType.TurnLeft, [0.5])
    
    sequence.add_action(action1)
    sequence.add_action(action2)
    
    analysis = sequence.analyze_patterns()
    print(f"✓ Created ActionSequence with {len(sequence.actions)} actions")
    print(f"✓ Action analysis: {analysis['total_actions']} actions, {analysis['unique_actions']} unique types")
    
except Exception as e:
    print(f"✗ Failed advanced features: {e}")
    sys.exit(1)

print("\n" + "="*70)
print("✓ ALL TESTS PASSED!")
print("NLM Python bindings are working correctly.")
print("="*70)
