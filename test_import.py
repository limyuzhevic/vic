import sys
import os

# Try to import pynlm
try:
    import pynlm
    print("SUCCESS: pynlm module imported successfully")
    print(f"Module location: {pynlm.__file__}")
    
    # Try to create a simple object
    config = pynlm.createDefaultConfig()
    print(f"Config created: {config}")
    
    # Check available classes
    print("\nAvailable classes:")
    print(f"  Config: {pynlm.Config}")
    print(f"  Brain: {pynlm.Brain}")
    print(f"  SimpleWorld: {pynlm.SimpleWorld}")
    print(f"  AgentBrain: {pynlm.AgentBrain}")
    print(f"  SensoryPercept: {pynlm.SensoryPercept}")
    print(f"  Action: {pynlm.Action}")
    print(f"  Vision: {pynlm.Vision}")
    print(f"  Audio: {pynlm.Audio}")
    print(f"  NeuronId: {pynlm.NeuronId}")
    print(f"  SynapseId: {pynlm.SynapseId}")
    
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    print("The pynlm module needs to be built first.")
    
    # Check what files exist
    print("\nLooking for pynlm module files...")
    
    # Check if there's a build directory
    if os.path.exists("./build"):
        print("Found build directory")
        build_files = os.listdir("./build")
        print(f"Build directory contents: {build_files}")
    
    # Check if there's a python directory with compiled files
    if os.path.exists("./python"):
        python_files = os.listdir("./python")
        print(f"Python directory contents: {python_files}")
        
        # Look for any compiled modules
        for f in python_files:
            if f.endswith('.so') or f.endswith('.pyd'):
                print(f"Found compiled module: {f}")
    
    # Try to build using scikit-build if available
    try:
        import scikit_build_core
        print("\nscikit-build-core is available")
        
        # Try to build
        import subprocess
        result = subprocess.run([sys.executable, "-m", "scikit_build_core.build"], 
                               capture_output=True, text=True)
        print(f"Build output: {result.stdout}")
        if result.stderr:
            print(f"Build errors: {result.stderr}")
            
    except ImportError:
        print("\nscikit-build-core is not available")
    
    except Exception as e:
        print(f"\nBuild failed: {e}")

except Exception as e:
    print(f"ERROR: {e}")
    import traceback
    traceback.print_exc()