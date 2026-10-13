#!/usr/bin/env python3
"""
Quick test to verify basic bindings functionality
"""

print("Testing Basic Bindings...")
print("=" * 30)

try:
    import pynlm
    print(f"✓ Imported pynlm version {pynlm.__version__}")
    
    config = pynlm.createDefaultConfig()
    print(f"✓ Created default config")
    
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    brain.initialize()
    print(f"✓ Brain initialized")
    
    brain.step(0)
    print(f"✓ Brain step completed")
    
    print("\n✓ All basic tests passed!")
    
except Exception as e:
    print(f"❌ Test failed: {e}")
    import traceback
    traceback.print_exc()
    exit(1)
