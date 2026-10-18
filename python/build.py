import os
import platform
import subprocess
import sys

def print_usage():
    print("Usage: build.py [command]")
    print("Commands:")
    print("  build     - Build Python bindings")
    print("  install   - Build and install Python package")
    print("  test      - Run Python tests")
    print("  clean     - Clean build artifacts")
    print("  help      - Show this help message")

def run_command(cmd, cwd=None):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    
    if result.stdout:
        print("STDOUT:")
        print(result.stdout)
    
    if result.stderr:
        print("STDERR:")
        print(result.stderr)
    
    return result.returncode == 0

def build_bindings():
    print("Building NLM Python bindings...")
    
    # Check for required tools
    if not run_command("cmake --version"):
        print("CMake not found. Please install CMake.")
        return False
    
    if not run_command("python3 -m pybind11"):
        print("pybind11 not found. Please install pybind11.")
        return False
    
    # Create build directory
    if not os.path.exists("build"):
        os.makedirs("build")
    
    # Configure CMake
    cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    if not run_command(cmake_cmd, cwd="build"):
        return False
    
    # Build
    if not run_command("make -j$(nproc)", cwd="build"):
        return False
    
    # Copy bindings to python directory
    if os.path.exists("python/pynlm*.so"):
        os.remove("python/pynlm*.so")
    
    # Find the built library
    for root, dirs, files in os.walk("."):
        for file in files:
            if "pynlm" in file and (file.endswith(".so") or file.endswith(".pyd")):
                source = os.path.join(root, file)
                dest = os.path.join("python", file)
                os.rename(source, dest)
                print(f"✓ Copied {source} to {dest}")
    
    print("✓ Python bindings built successfully")
    return True

def install_package():
    print("Building and installing NLM Python package...")
    
    # Build bindings first
    if not build_bindings():
        return False
    
    # Build Python package
    setup_py_path = "python/setup.py"
    
    if not run_command(f"{sys.executable} {setup_py_path} build_ext --inplace"):
        return False
    
    # Install package
    if not run_command(f"{sys.executable} {setup_py_path} install --prefix /usr/local"):
        return False
    
    print("✓ NLM Python package installed successfully")
    return True

def run_tests():
    print("Running NLM Python tests...")
    
    # Change to python directory
    original_cwd = os.getcwd()
    os.chdir("python")
    
    try:
        # Run test script
        result = subprocess.run([
            sys.executable, "test_enhanced_bindings.py"
        ], capture_output=True, text=True)
        
        print("Test Output:")
        print(result.stdout)
        
        if result.stderr:
            print("Test Errors:")
            print(result.stderr)
        
        return result.returncode == 0
    finally:
        os.chdir(original_cwd)

def clean_build():
    print("Cleaning build artifacts...")
    
    # Remove build directory
    if os.path.exists("build"):
        import shutil
        shutil.rmtree("build")
        print("✓ Removed build directory")
    
    # Remove python module files
    for root, dirs, files in os.walk("python"):
        for file in files:
            if file.endswith(".so") or file.endswith(".pyd"):
                os.remove(os.path.join(root, file))
                print(f"✓ Removed {os.path.join(root, file)}")
    
    print("✓ Cleanup completed")
    return True

def main():
    if len(sys.argv) < 2:
        print_usage()
        return 1
    
    command = sys.argv[1]
    
    if command == "build":
        return 0 if build_bindings() else 1
    elif command == "install":
        return 0 if install_package() else 1
    elif command == "test":
        return 0 if run_tests() else 1
    elif command == "clean":
        return 0 if clean_build() else 1
    elif command == "help":
        print_usage()
        return 0
    else:
        print(f"Unknown command: {command}")
        print_usage()
        return 1

if __name__ == "__main__":
    sys.exit(main())