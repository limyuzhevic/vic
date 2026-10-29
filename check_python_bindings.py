#!/usr/bin/env python3
import os
import sys
import subprocess

def check_python_bindings():
    print("Checking Python bindings integration...")
    
    # Check if build directory exists
    build_dir = "/workspace/app/build"
    if not os.path.exists(build_dir):
        print(f"❌ Build directory not found: {build_dir}")
        print("   Trying to create build directory...")
        
        # Create build directory
        os.makedirs(build_dir, exist_ok=True)
        
        # Try to configure CMake
        print("Attempting CMake configuration...")
        try:
            subprocess.run(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"], 
                          cwd=build_dir, check=True)
            print("✅ CMake configuration successful")
        except subprocess.CalledProcessError as e:
            print(f"❌ CMake configuration failed: {e}")
            return False
    else:
        print(f"✅ Build directory exists: {build_dir}")
    
    # Try to build
    print("Attempting to build...")
    try:
        subprocess.run(["make", "-j4"], cwd=build_dir, check=True)
        print("✅ Build successful")
    except subprocess.CalledProcessError as e:
        print(f"❌ Build failed: {e}")
        return False
    
    # Check for Python module
    print("Checking for Python module...")
    lib_path = "/workspace/app/build/python/pynlm*.so"
    import glob
    python_modules = glob.glob("/workspace/app/build/python/*.so")
    
    if python_modules:
        print(f"✅ Found Python modules: {python_modules}")
        
        # Try to import one
        try:
            sys.path.insert(0, "/workspace/app/build/python")
            import pynlm
            print(f"✅ Successfully imported pynlm")
            print(f"   Version: {getattr(pynlm, '__version__', 'unknown')}")
            
            # Test basic functionality
            config = pynlm.createDefaultConfig()
            print(f"✅ Created default config")
            
            brain = pynlm.createBrain(config)
            print(f"✅ Created brain with {brain.getTotalNeuronCount()} neurons")
            
            return True
        except ImportError as e:
            print(f"❌ Failed to import pynlm: {e}")
            return False
    else:
        print("❌ No Python modules found in build directory")
        return False

if __name__ == "__main__":
    success = check_python_bindings()
    sys.exit(0 if success else 1)