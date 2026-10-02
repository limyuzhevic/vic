import sys
print(f"Python path: {sys.path}")
print(f"Python version: {sys.version}")

# Check if we can import pynlm
try:
    import pynlm
    print("Successfully imported pynlm")
    print(f"pynlm version: {pynlm.__version__}")
    print(f"pynlm description: {pynlm.__description__}")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    brain.initialize()
    print(f"✓ Initialized brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run a quick simulation
    brain.step(0)
    print(f"✓ Ran brain step with {brain.getFiringNeuronCount()} firing neurons")
    
    print("\nAll basic tests passed!")
    
except ImportError as e:
    print(f"ImportError: {e}")
    print("This suggests the C++ bindings are not properly built")
    print("Please build the C++ library with: pip install -e .")
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
