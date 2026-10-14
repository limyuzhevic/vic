#!/usr/bin/env python3

"""
Installation and build instructions for enhanced NLM Python bindings.
This script provides comprehensive instructions for building and installing
the enhanced Python bindings.
"""

import os
import sys
import subprocess
import platform

def print_header(text):
    """Print a formatted header."""
    print("\n" + "="*70)
    print(text.center(70))
    print("="*70)

def print_section(text):
    """Print a formatted section header."""
    print(f"\n{'-'*70}")
    print(text)
    print(f"{'-'*70}")

def check_python_version():
    """Check if Python version is supported."""
    print_header("Python Version Check")
    
    version = sys.version_info
    print(f"Python version: {version.major}.{version.minor}.{version.micro}")
    print(f"Python executable: {sys.executable}")
    
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        print("❌ ERROR: Python 3.8 or higher is required")
        return False
    
    print("✅ Python version is supported")
    return True

def check_dependencies():
    """Check for required dependencies."""
    print_header("Dependency Check")
    
    dependencies = [
        ("pybind11", "pybind11 module not found"),
        ("NumPy", "NumPy not found"),
        ("CMake", "CMake not found"),
        ("Compiler", "C++ compiler not found"),
    ]
    
    all_ok = True
    
    for name, error_msg in dependencies:
        try:
            if name == "pybind11":
                import pybind11
                print(f"✅ {name}: Found (version {pybind11.__version__})")
            elif name == "NumPy":
                import numpy
                print(f"✅ {name}: Found (version {numpy.__version__})")
            elif name == "CMake":
                # Check if CMake is available
                result = subprocess.run(['cmake', '--version'], 
                                      capture_output=True, text=True)
                if result.returncode == 0:
                    print(f"✅ {name}: Found ({result.stdout.split()[2]})")
                else:
                    print(f"❌ {name}: {error_msg}")
                    all_ok = False
            elif name == "Compiler":
                # Check for compiler
                if platform.system() == "Windows":
                    # Try to find MSVC
                    print(f"✅ {name}: Windows (assumed available)")
                else:
                    # Try g++ or clang++
                    for compiler in ['g++', 'clang++']:
                        result = subprocess.run(['which', compiler], 
                                              capture_output=True, text=True)
                        if result.returncode == 0:
                            print(f"✅ {name}: Found ({compiler})")
                            break
                    else:
                        print(f"❌ {name}: {error_msg}")
                        all_ok = False
        except ImportError:
            print(f"❌ {name}: {error_msg}")
            all_ok = False
    
    return all_ok

def setup_build_environment():
    """Set up the build environment."""
    print_header("Build Environment Setup")
    
    # Get current directory
    current_dir = os.path.dirname(os.path.abspath(__file__))
    python_dir = os.path.join(current_dir, "python")
    
    print(f"Project root: {current_dir}")
    print(f"Python bindings directory: {python_dir}")
    
    # Check for required files
    required_files = [
        ("python/bindings.cpp", "Enhanced Python bindings"),
        ("python/CMakeLists.txt", "CMake build configuration"),
        ("examples/basic_usage.py", "Basic usage examples"),
        ("examples/advanced_usage_examples.py", "Advanced usage examples"),
        ("test_enhanced_bindings.py", "Test suite"),
    ]
    
    all_ok = True
    for file_path, description in required_files:
        full_path = os.path.join(current_dir, file_path)
        if os.path.exists(full_path):
            print(f"✅ {description}: Found ({file_path})")
        else:
            print(f"❌ {description}: Missing ({file_path})")
            all_ok = False
    
    return all_ok

def build_bindings():
    """Build the Python bindings."""
    print_header("Building Python Bindings")
    
    current_dir = os.path.dirname(os.path.abspath(__file__))
    python_dir = os.path.join(current_dir, "python")
    build_dir = os.path.join(python_dir, "build")
    
    print(f"Build directory: {build_dir}")
    
    # Create build directory
    os.makedirs(build_dir, exist_ok=True)
    
    # Configure CMake
    print("\n🔧 Configuring CMake...")
    cmake_cmd = [
        "cmake", "..",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DPYTHON_EXECUTABLE=" + sys.executable
    ]
    
    try:
        result = subprocess.run(cmake_cmd, cwd=build_dir, 
                              capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✅ CMake configuration successful")
        else:
            print("❌ CMake configuration failed:")
            print(result.stderr)
            return False
    except Exception as e:
        print(f"❌ CMake configuration failed: {e}")
        return False
    
    # Build the project
    print("\n🔨 Building project...")
    make_cmd = ["make", "-j4"]
    
    try:
        result = subprocess.run(make_cmd, cwd=build_dir,
                              capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✅ Build successful")
            
            # Find the built module
            dist_dir = os.path.join(build_dir, "dist")
            if os.path.exists(dist_dir):
                print(f"📦 Built modules in {dist_dir}")
            
            return True
        else:
            print("❌ Build failed:")
            print(result.stderr)
            return False
    except Exception as e:
        print(f"❌ Build failed: {e}")
        return False

def install_bindings():
    """Install the Python bindings."""
    print_header("Installing Python Bindings")
    
    python_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "python")
    build_dir = os.path.join(python_dir, "build")
    
    # Check if build directory exists
    if not os.path.exists(build_dir):
        print("❌ Build directory not found. Please build first.")
        return False
    
    # Install using pip
    print("\n📦 Installing via pip...")
    
    # Get the directory containing the built module
    module_files = []
    for root, dirs, files in os.walk(build_dir):
        for file in files:
            if file.endswith((".so", ".pyd", ".so.dSYM")):
                module_files.append(os.path.join(root, file))
    
    if not module_files:
        print("❌ No Python module files found in build directory")
        return False
    
    # Install the module
    try:
        # Create a simple setup.py for the module
        setup_py = os.path.join(build_dir, "setup.py")
        with open(setup_py, "w") as f:
            f.write(f'''from setuptools import setup, Extension
import os

# Find the module files
module_dir = r"{os.path.dirname(module_files[0])}"
extension = Extension(
    'pynlm',
    sources=[],
    include_dirs=[],
    library_dirs=[module_dir],
    libraries=[],
    extra_compile_args=[],
)

setup(
    name='pynlm',
    version='1.0.0',
    description='Enhanced NLM Python bindings for neural simulation',
    ext_modules=[extension],
    packages=[],
    package_dir={{'': r"{os.path.dirname(module_files[0])}"}},
    install_requires=[],
)
''')
        
        # Install the module
        result = subprocess.run([
            sys.executable, "-m", "pip", "install", 
            "--no-deps", build_dir
        ], capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✅ Installation successful")
            print(f"📦 Module installed to: {sys.executable} -m site")
            return True
        else:
            print("❌ Installation failed:")
            print(result.stderr)
            return False
            
    except Exception as e:
        print(f"❌ Installation failed: {e}")
        return False

def run_tests():
    """Run the test suite."""
    print_header("Running Test Suite")
    
    # Check if test file exists
    test_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "test_enhanced_bindings.py")
    
    if not os.path.exists(test_file):
        print("❌ Test file not found")
        return False
    
    print(f"\n🧪 Running tests from: {test_file}")
    
    try:
        result = subprocess.run([
            sys.executable, test_file
        ], capture_output=True, text=True)
        
        print(result.stdout)
        
        if result.returncode == 0:
            print("✅ All tests passed")
            return True
        else:
            print("❌ Some tests failed:")
            print(result.stderr)
            return False
    except Exception as e:
        print(f"❌ Test execution failed: {e}")
        return False

def show_usage_examples():
    """Show usage examples."""
    print_header("Usage Examples")
    
    examples_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "examples")
    
    if os.path.exists(examples_dir):
        print("\n📁 Available example files:")
        for file in os.listdir(examples_dir):
            if file.endswith(".py"):
                file_path = os.path.join(examples_dir, file)
                with open(file_path, "r") as f:
                    content = f.read()
                    # Find the first docstring or first few lines
                    lines = content.split('\n')
                    first_line = lines[0].strip() if lines else ""
                    print(f"  • {file}: {first_line}")
    
    print("\n📖 Complete documentation available in:")
    print("  • examples/ENHANCED_USAGE_GUIDE.md")

def main():
    """Main function."""
    print("NLM Enhanced Python Bindings - Installation and Build Script")
    print("="*70)
    
    # Step 1: Check Python version
    if not check_python_version():
        return 1
    
    # Step 2: Check dependencies
    if not check_dependencies():
        print("\n⚠️  Some dependencies are missing. Please install them:")
        print("  • pybind11 (should be available with pybind11-dev)")
        print("  • NumPy (pip install numpy)")
        print("  • CMake (https://cmake.org/download/)")
        print("  • C++ compiler (g++, clang++, or MSVC)")
    
    # Step 3: Setup build environment
    if not setup_build_environment():
        return 1
    
    # Step 4: Build bindings
    if not build_bindings():
        return 1
    
    # Step 5: Install bindings
    if not install_bindings():
        return 1
    
    # Step 6: Run tests
    if not run_tests():
        return 1
    
    # Step 7: Show usage examples
    show_usage_examples()
    
    print_header("Installation Complete")
    print("\n🎉 Enhanced NLM Python bindings have been successfully installed!")
    print("\n📋 Next Steps:")
    print("  1. Import the module: import pynlm")
    print("  2. Run examples: python examples/basic_usage.py")
    print("  3. Run tests: python test_enhanced_bindings.py")
    print("  4. Review documentation: examples/ENHANCED_USAGE_GUIDE.md")
    
    return 0

if __name__ == "__main__":
    exit(main())