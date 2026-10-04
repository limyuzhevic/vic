#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Installation and Setup Guide

This script provides comprehensive installation and setup instructions for
installing NLM from source with Python bindings.
"""

import sys
import os
import platform
import subprocess
import shutil

def print_header(title):
    """Print a formatted header"""
    print("\n" + "=" * 70)
    print(f"NLM - {title}")
    print("=" * 70)

def print_step(step_num, title):
    """Print a formatted step header"""
    print(f"\nStep {step_num}: {title}")
    print("-" * 70)

def check_python_version():
    """Check if Python version is sufficient"""
    print_step(1, "Check Python Installation")
    
    current_version = sys.version_info
    print(f"Current Python version: {current_version.major}.{current_version.minor}.{current_version.micro}")
    
    if current_version.major == 3 and current_version.minor >= 8:
        print("✓ Python version requirement met (3.8+)")
        return True
    else:
        print("✗ Python version requirement not met (need 3.8+)")
        return False

def check_cmake():
    """Check if CMake is installed"""
    print_step(2, "Check CMake Installation")
    
    try:
        result = subprocess.run(['cmake', '--version'], 
                              capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            version_line = result.stdout.split('\n')[0]
            print(f"✓ CMake found: {version_line}")
            return True
        else:
            print("✗ CMake not found")
            return False
    except (subprocess.TimeoutExpired, FileNotFoundError):
        print("✗ CMake not found")
        return False

def check_compiler():
    """Check if C++ compiler is available"""
    print_step(3, "Check C++ Compiler")
    
    system = platform.system()
    
    if system == "Linux":
        compilers = [
            ('g++', '--version'),
            ('g++', '-dumpversion'),
            ('clang++', '--version')
        ]
    elif system == "Darwin":  # macOS
        compilers = [
            ('clang++', '--version'),
            ('g++', '--version')
        ]
    elif system == "Windows":
        compilers = [
            ('cl', '/?), None)  # MSVC
        ]
    else:
        compilers = []
    
    found = False
    for compiler, flag in compilers:
        try:
            if flag is None:
                # Special handling for MSVC
                result = subprocess.run([compiler], capture_output=True, text=True, timeout=30)
            else:
                result = subprocess.run([compiler, flag], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                print(f"✓ {compiler} found")
                found = True
                break
        except (subprocess.TimeoutExpired, FileNotFoundError):
            continue
    
    if not found:
        print("✗ No suitable C++ compiler found")
        if system == "Linux":
            print("   Try: sudo apt-get install g++ clang")
        elif system == "Darwin":
            print("   Try: xcode-select --install (for Xcode)")
        elif system == "Windows":
            print("   Try: Install Visual Studio with C++ tools")
    
    return found

def install_python_dependencies():
    """Install Python dependencies"""
    print_step(4, "Install Python Dependencies")
    
    # Check if pip is available
    try:
        result = subprocess.run([sys.executable, '-m', 'pip', '--version'],
                              capture_output=True, text=True, timeout=30)
        if result.returncode != 0:
            print("✗ pip not found")
            return False
    except (subprocess.TimeoutExpired, FileNotFoundError):
        print("✗ pip not found")
        return False
    
    print("✓ pip is available")
    
    # Install required dependencies
    dependencies = [
        "scikit-build-core>=0.5.0",
        "pybind11>=2.11.0",
        "pytest>=7.0",
        "numpy>=1.20",
        "scipy>=1.6.0"
    ]
    
    print("Installing dependencies...")
    for dep in dependencies:
        print(f"  Installing {dep}...")
        try:
            result = subprocess.run([sys.executable, '-m', 'pip', 'install', dep],
                                  capture_output=True, text=True, timeout=300)
            if result.returncode == 0:
                print(f"    ✓ {dep} installed successfully")
            else:
                print(f"    ✗ Failed to install {dep}")
                print(f"      Error: {result.stderr}")
                return False
        except subprocess.TimeoutExpired:
            print(f"    ✗ Timeout installing {dep}")
            return False
    
    return True

def build_nlm():
    """Build NLM from source"""
    print_step(5, "Build NLM from Source")
    
    # Check if we're in the NLM directory
    current_dir = os.getcwd()
    if not os.path.exists('CMakeLists.txt'):
        print("✗ CMakeLists.txt not found. Please run this script from the NLM source directory.")
        return False
    
    # Create build directory
    build_dir = 'build'
    if os.path.exists(build_dir):
        print(f"Build directory '{build_dir}' already exists.")
        shutil.rmtree(build_dir)
    
    os.makedirs(build_dir)
    os.chdir(build_dir)
    
    print("Configuring CMake...")
    try:
        # Configure with CMake
        cmake_cmd = [
            'cmake', '..',
            '-DCMAKE_BUILD_TYPE=Release',
            '-DPYTHON_EXECUTABLE=' + sys.executable
        ]
        
        result = subprocess.run(cmake_cmd, capture_output=True, text=True, timeout=300)
        if result.returncode != 0:
            print("✗ CMake configuration failed")
            print(f"Error: {result.stderr}")
            os.chdir('..')
            return False
        
        print("✓ CMake configuration completed")
        
    except subprocess.TimeoutExpired:
        print("✗ CMake configuration timed out")
        os.chdir('..')
        return False
    
    print("Building NLM...")
    try:
        # Build with make
        build_cmd = ['make', '-j4']
        
        result = subprocess.run(build_cmd, capture_output=True, text=True, timeout=600)
        if result.returncode != 0:
            print("✗ Build failed")
            print(f"Error: {result.stderr}")
            os.chdir('..')
            return False
        
        print("✓ NLM built successfully")
        
    except subprocess.TimeoutExpired:
        print("✗ Build timed out")
        os.chdir('..')
        return False
    
    # Go back to source directory
    os.chdir('..')
    
    # List build outputs
    print("\nBuild outputs:")
    if os.path.exists('build/nlm'):
        print(f"  - C++ executable: build/nlm")
    if os.path.exists('build/nlm_phase3_demo'):
        print(f"  - Phase 3 demo: build/nlm_phase3_demo")
    if os.path.exists('build/nlm_phase4_demo'):
        print(f"  - Phase 4 demo: build/nlm_phase4_demo")
    if os.path.exists('build/nlm_phase6_demo'):
        print(f"  - Phase 6 demo: build/nlm_phase6_demo")
    
    return True

def install_python_module():
    """Install Python module"""
    print_step(6, "Install Python Module")
    
    print("Installing pynlm Python module...")
    try:
        # Use pip to install in development mode
        result = subprocess.run([sys.executable, '-m', 'pip', 'install', '-e', '.'],
                              capture_output=True, text=True, timeout=300)
        
        if result.returncode == 0:
            print("✓ pynlm installed successfully")
            return True
        else:
            print("✗ Failed to install pynlm")
            print(f"Error: {result.stderr}")
            return False
            
    except subprocess.TimeoutExpired:
        print("✗ Installation timed out")
        return False

def verify_installation():
    """Verify that installation was successful"""
    print_step(7, "Verify Installation")
    
    try:
        # Try to import pynlm
        import pynlm
        
        print("✓ Successfully imported pynlm")
        
        # Check if key classes are available
        required_classes = [
            'createDefaultConfig',
            'createBrain',
            'createSimpleWorld',
            'createAgentBrain',
            'NeuronType',
            'ActionType',
            'DevelopmentalStage'
        ]
        
        missing = []
        for cls in required_classes:
            if not hasattr(pynlm, cls):
                missing.append(cls)
        
        if missing:
            print(f"✗ Missing expected classes: {missing}")
            return False
        
        print("✓ All required classes are available")
        
        # Test basic functionality
        print("\nTesting basic functionality...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✓ Successfully created and initialized brain")
        
        # Run a simple simulation
        for i in range(5):
            brain.step(i)
        
        print("✓ Successfully ran simulation")
        
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Verification failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main installation function"""
    print_header("Installation and Setup Guide")
    
    print("\nThis guide will help you install NLM (Neural Learning Machine)")
    print("from source, including Python bindings and all dependencies.")
    
    # Run all installation steps
    steps = [
        check_python_version,
        check_cmake,
        check_compiler,
        install_python_dependencies,
        build_nlm,
        install_python_module,
        verify_installation
    ]
    
    results = []
    for i, step in enumerate(steps, 1):
        try:
            result = step()
            results.append((i, result))
        except Exception as e:
            print(f"✗ Step {i} failed with exception: {e}")
            results.append((i, False))
    
    # Summary
    print_header("Installation Summary")
    
    all_passed = all(result for _, result in results)
    
    if all_passed:
        print("✓ All installation steps completed successfully!")
        print("\nYou can now:")
        print("  1. Run example scripts from the examples/ directory")
        print("  2. Build and run C++ executables from the build/ directory")
        print("  3. Use pynlm in your own Python projects")
        print("\nQuick start:")
        print("  - Import: import pynlm")
        print("  - Create brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("  - Run simulation: brain.step(step_number)")
        return 0
    else:
        print("✗ Some installation steps failed:")
        for step_num, result in results:
            if not result:
                print(f"  - Step {step_num} failed")
        
        print("\nPlease check the error messages above and fix any issues.")
        print("You may need to install system dependencies manually.")
        return 1

if __name__ == "__main__":
    sys.exit(main())