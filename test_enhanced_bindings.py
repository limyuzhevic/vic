#!/usr/bin/env python3
"""
Quick test of enhanced NLM Python bindings
"""

import sys
print("Testing enhanced NLM Python bindings...")

try:
    import pynlm
    print(f"✓ Successfully imported pynlm v{pynlm.__version__}")
except Exception as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

# Test basic functionality
try:
    # Test configuration creation
    config = pynlm.create_config()
    print("✓ Created default configuration")
    
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    brain.initialize()
    print("✓ Initialized brain")
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    print("✓ Ran simulation steps")
    
    # Test enhanced features
    stats = brain.get_statistics()
    print(f"✓ Got statistics: {stats['total_neurons']} neurons, {stats['firing_rate']:.2f} Hz")
    
    # Test action builders
    action = pynlm.createActionMoveToward(5.0, 5.0)
    print(f"✓ Created action: {action.getType()}")
    
    # Test world creation
    world = pynlm.create_default_world()
    print(f"✓ Created world: {world.getWidth()}x{world.getHeight()}")
    
    # Test world objects
    world.add_resource(2.0, 2.0, value=5.0)
    print("✓ Added resource to world")
    
    # Test agent
    agent = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain")
    
    print("\n" + "="*60)
    print("All tests passed! Enhanced NLM Python bindings working correctly.")
    print("="*60)
    
except Exception as e:
    print(f"✗ Test failed: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
