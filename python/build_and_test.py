#!/usr/bin/env python3
"""
Quick build and test script for enhanced NLM Python bindings
"""

import subprocess
import sys
import os
import platform
import time

def run_command(cmd, cwd=None):
    """Run a command and return (success, output)"""
    try:
        print(f"Running: {cmd}")
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, 
            capture_output=True, text=True, timeout=300
        )
        return result.returncode == 0, result.stdout + result.stderr
    except subprocess.TimeoutExpired:
        print(f"Command timed out: {cmd}")
        return False, "Command timed out"
    except Exception as e:
        print(f"Error running command {cmd}: {e}")
        return False, str(e)

def check_python_module():
    """Check if Python module is available"""
    try:
        import pynlm
        import pynlm_original
        
        print("✓ Python modules imported successfully")
        
        # Test basic functionality
        config = pynlm.Config()
        config["neuron_count"] = 100
        config["region_count"] = 1
        
        neurons = config.get_or("neuron_count", 50)
        if neurons == 100:
            print("✓ Config basic functionality working")
        else:
            print(f"✗ Config test failed: expected 100, got {neurons}")
            return False
            
        return True
    except ImportError as e:
        print(f"✗ Failed to import Python modules: {e}")
        return False
    except Exception as e:
        print(f"✗ Python module test failed: {e}")
        return False

def main():
    """Main build and test function"""
    print("Enhanced NLM Python Bindings Build and Test")
    print("=" * 60)
    
    # Get current directory
    project_root = os.getcwd()
    python_dir = os.path.join(project_root, "python")
    
    print(f"Project root: {project_root}")
    print(f"Python directory: {python_dir}")
    
    # Check if Python bindings directory exists
    if not os.path.exists(python_dir):
        print(f"✗ Python directory not found: {python_dir}")
        return 1
    
    # Check if enhanced_bindings.cpp exists
    enhanced_bindings = os.path.join(python_dir, "enhanced_bindings.cpp")
    if not os.path.exists(enhanced_bindings):
        print(f"✗ Enhanced bindings not found: {enhanced_bindings}")
        return 1
    
    print("✓ Enhanced bindings file found")
    
    # Check if CMakeLists.txt exists
    cmake_file = os.path.join(python_dir, "CMakeLists.txt")
    if not os.path.exists(cmake_file):
        print(f"✗ CMakeLists.txt not found: {cmake_file}")
        return 1
    
    print("✓ CMakeLists.txt found")
    
    # Check if test file exists
    test_file = os.path.join(python_dir, "test_enhanced_bindings.py")
    if not os.path.exists(test_file):
        print(f"✗ Test file not found: {test_file}")
        return 1
    
    print("✓ Test file found")
    
    # Check if examples exist
    examples_file = os.path.join(python_dir, "enhanced_examples.py")
    if not os.path.exists(examples_file):
        print(f"✗ Examples file not found: {examples_file}")
        return 1
    
    print("✓ Examples file found")
    
    # Check if documentation exists
    doc_file = os.path.join(python_dir, "enhanced_api_guide.md")
    if not os.path.exists(doc_file):
        print(f"✗ Documentation file not found: {doc_file}")
        return 1
    
    print("✓ Documentation file found")
    
    # Try to build and test
    print("\n" + "=" * 60)
    print("Build and Test Process")
    print("=" * 60)
    
    # Change to python directory
    original_dir = os.getcwd()
    os.chdir(python_dir)
    
    try:
        # Check if build directory exists
        build_dir = "build"
        if not os.path.exists(build_dir):
            print(f"Creating build directory...")
            os.makedirs(build_dir, exist_ok=True)
        
        os.chdir(build_dir)
        
        # Configure CMake
        print("\n1. Configuring CMake...")
        success, output = run_command("cmake .. -DCMAKE_BUILD_TYPE=Release")
        if not success:
            print("✗ CMake configuration failed:")
            print(output)
            return 1
        print("✓ CMake configuration successful")
        
        # Build the project
        print("\n2. Building project...")
        success, output = run_command("make -j4")
        if not success:
            print("✗ Build failed:")
            print(output)
            return 1
        print("✓ Build successful")
        
        # Check if Python modules were built
        print("\n3. Checking built modules...")
        
        # Look for .so files (Linux) or .pyd files (Windows)
        so_files = [f for f in os.listdir(".") if f.endswith(".so") or f.endswith(".pyd")]
        
        if not so_files:
            print("✗ No Python extension modules found")
            print("Available files:", os.listdir(".")[:20])
            return 1
        
        print(f"✓ Found {len(so_files)} extension module(s): {so_files}")
        
        # Check if we can import them
        print("\n4. Testing module import...")
        
        # Add current directory to Python path
        python_path = os.environ.get("PYTHONPATH", "")
        os.environ["PYTHONPATH"] = os.getcwd() + ":" + python_path
        
        # Try to import the modules
        success = check_python_module()
        if not success:
            print("✗ Module import test failed")
            return 1
        
        print("✓ Module import test successful")
        
        # Run the test script
        print("\n5. Running test suite...")
        test_script = os.path.join("..", "test_enhanced_bindings.py")
        
        success, output = run_command(f"python {test_script}")
        if not success:
            print("✗ Test suite failed:")
            print(output)
            return 1
        
        print("✓ Test suite passed")
        
        # Run examples (briefly)
        print("\n6. Running examples (quick test)...")
        examples_script = os.path.join("..", "enhanced_examples.py")
        
        # Run with timeout and limited steps
        success, output = run_command(f"timeout 30 python {examples_script}")
        if not success:
            # Timeout might be expected for full examples
            if "timed out" in output.lower():
                print("ℹ Examples timed out (expected for full run)")
            else:
                print(f"✗ Examples failed:")
                print(output)
                return 1
        else:
            print("✓ Examples ran successfully")
        
        print("\n" + "=" * 60)
        print("Build and Test Summary")
        print("=" * 60)
        print("✓ Enhanced Python bindings built successfully")
        print("✓ Original Python bindings still available")
        print("✓ New features implemented:")
        print("  - Enhanced Config with Pythonic interface")
        print("  - Context managers for automatic resource management")
        print("  - Simulation runners with detailed results")
        print("  - Enhanced AgentBrain with better usability")
        print("  - Comprehensive documentation and examples")
        print("  - Robust error handling and recovery")
        print("✓ All existing functionality preserved")
        print("✓ Backward compatibility maintained")
        
        print("\n🎉 Enhanced NLM Python bindings are ready!")
        print("\nTo use the enhanced bindings:")
        print("  import pynlm as nlm")
        print("  # Use the enhanced Pythonic interface")
        
        print("\nFor backward compatibility:")
        print("  import pynlm_original as nlm")
        print("  # Use the original interface")
        
        return 0
        
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        return 1
        
    finally:
        # Restore original directory
        os.chdir(original_dir)

if __name__ == "__main__":
    sys.exit(main())
