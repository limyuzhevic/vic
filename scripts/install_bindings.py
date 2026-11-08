#!/usr/bin/env python3
"""
NLM Bindings Installer - Installation script for enhanced Python bindings

This script handles the installation of enhanced NLM Python bindings,
including dependency checking, build configuration, and installation verification.
"""

import sys
import os
import subprocess
import argparse
from pathlib import Path

def check_python_version():
    """Check if Python version meets requirements"""
    print("Checking Python version...")
    
    version = sys.version_info
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        raise RuntimeError(f"Python 3.8+ required, found {version.major}.{version.minor}")
    
    print(f"✓ Python {version.major}.{version.minor}.{version.micro} meets requirements")
    return True

def check_dependencies():
    """Check for required Python dependencies"""
    print("Checking Python dependencies...")
    
    # Check numpy
    try:
        import numpy
        print(f"✓ NumPy {numpy.__version__} found")
    except ImportError:
        print("✗ NumPy not found. Installing...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "numpy>=1.20"])
        print("✓ NumPy installed successfully")
    
    # Check pybind11
    try:
        import pybind11
        print(f"✓ pybind11 {pybind11.__version__} found")
    except ImportError:
        print("✗ pybind11 not found. Installing...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "pybind11"])
        print("✓ pybind11 installed successfully")
    
    # Check scikit-build-core
    try:
        import scikit_build_core
        print(f"✓ scikit-build-core {scikit_build_core.__version__} found")
    except ImportError:
        print("✗ scikit-build-core not found. Installing...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "scikit-build-core"])
        print("✓ scikit-build-core installed successfully")
    
    return True

def check_cmake():
    """Check if CMake is available"""
    print("Checking CMake availability...")
    
    try:
        result = subprocess.run(["cmake", "--version"], 
                               capture_output=True, text=True, check=True)
        print(f"✓ CMake found: {result.stdout.split()[0]}")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        raise RuntimeError("CMake is required but not found. Please install CMake from https://cmake.org/download/")

def check_compilation_tools():
    """Check for C++ compiler tools"""
    print("Checking C++ compiler availability...")
    
    # Try to find g++ or clang++
    compilers = ["g++", "clang++"]
    
    for compiler in compilers:
        try:
            result = subprocess.run([compiler, "--version"], 
                                   capture_output=True, text=True, check=True)
            print(f"✓ {compiler} found")
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            continue
    
    raise RuntimeError("No C++ compiler found. Please install g++ or clang++")

def build_bindings(build_dir=None, generator=None):
    """Build the NLM Python bindings"""
    print("Building NLM Python bindings...")
    
    if build_dir is None:
        build_dir = Path(__file__).parent / "build"
    
    build_dir.mkdir(exist_ok=True)
    
    cmake_args = [
        "cmake",
        str(Path(__file__).parent),
        "-DNLM_BUILD_PYTHON_BINDINGS=ON",
        "-DCMAKE_BUILD_TYPE=Release"
    ]
    
    if generator:
        cmake_args.extend(["-G", generator])
    
    try:
        # Configure
        print("Configuring CMake build...")
        subprocess.run(cmake_args, check=True, cwd=build_dir)
        
        # Build
        print("Building bindings...")
        subprocess.run(["cmake", "--build", ".", "--config", "Release"], 
                      check=True, cwd=build_dir)
        
        print("✓ Bindings built successfully")
        return build_dir / "pynlm.so"
        
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Build failed: {e}")

def install_bindings(bindings_path, install_dir=None):
    """Install bindings to Python site-packages"""
    if install_dir is None:
        import site
        install_dir = Path(site.getsitepackages()[0])
    
    install_dir = Path(install_dir)
    install_dir.mkdir(parents=True, exist_ok=True)
    
    # Copy bindings file
    target_path = install_dir / "pynlm.so"
    
    if bindings_path.exists():
        print(f"Copying bindings to {target_path}...")
        import shutil
        shutil.copy2(bindings_path, target_path)
        print("✓ Bindings installed successfully")
    else:
        raise RuntimeError(f"Bindings file not found: {bindings_path}")
    
    # Try to install through pip for better integration
    print("Installing via pip for better integration...")
    try:
        subprocess.run([
            sys.executable, "-m", "pip", "install", 
            "--force-reinstall", 
            str(Path(__file__).parent)
        ], check=True, cwd=Path(__file__).parent.parent)
        print("✓ Installation completed via pip")
    except subprocess.CalledProcessError:
        print("⚠ Pip installation failed, but bindings were copied manually")
    
    return target_path

def verify_installation(bindings_path):
    """Verify that bindings were installed correctly"""
    print("Verifying installation...")
    
    try:
        # Try to import the module
        sys.path.insert(0, str(bindings_path.parent))
        import pynlm
        
        # Test basic functionality
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test convenience methods
        potentials = brain.get_neuron_potentials()
        weights = brain.get_synapse_weights()
        stats = brain.get_statistics()
        
        print(f"✓ pynlm imported successfully")
        print(f"✓ Created brain with {brain.get_total_neuron_count()} neurons")
        print(f"✓ Convenience methods working (potentials: {len(potentials)}, weights: {len(weights)})")
        
        return True
        
    except Exception as e:
        raise RuntimeError(f"Installation verification failed: {e}")

def main():
    """Main installation function"""
    parser = argparse.ArgumentParser(
        description="Install NLM Python bindings",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "--build-dir", "-b",
        help="Build directory path (default: ./build)",
        default=None
    )
    
    parser.add_argument(
        "--generator", "-G",
        help="CMake generator (e.g., 'Ninja', 'Unix Makefiles')",
        default=None
    )
    
    parser.add_argument(
        "--install-dir", "-i",
        help="Installation directory (default: system site-packages)",
        default=None
    )
    
    parser.add_argument(
        "--skip-build", "-s",
        help="Skip building, assume bindings already exist",
        action="store_true"
    )
    
    parser.add_argument(
        "--verbose", "-v",
        help="Verbose output",
        action="store_true"
    )
    
    args = parser.parse_args()
    
    try:
        print("NLM Python Bindings Installer")
        print("=" * 50)
        
        # Step 1: Check Python version
        check_python_version()
        
        # Step 2: Check dependencies
        check_dependencies()
        
        # Step 3: Check build tools
        check_cmake()
        check_compilation_tools()
        
        # Step 4: Build bindings
        if not args.skip_build:
            build_dir = Path(args.build_dir) if args.build_dir else Path(__file__).parent / "build"
            bindings_path = build_bindings(
                build_dir=Path(args.build_dir),
                generator=args.generator
            )
        else:
            # Look for existing bindings
            build_dir = Path(args.build_dir) if args.build_dir else Path(__file__).parent / "build"
            bindings_path = build_dir / "pynlm.so"
            
            if not bindings_path.exists():
                raise RuntimeError(f"Bindings not found at {bindings_path}")
            
            print(f"✓ Found existing bindings at {bindings_path}")
        
        # Step 5: Install bindings
        install_dir = Path(args.install_dir) if args.install_dir else None
        install_path = install_bindings(bindings_path, install_dir)
        
        # Step 6: Verify installation
        verify_installation(install_path)
        
        print("\n" + "=" * 50)
        print("Installation completed successfully!")
        print("\nYou can now:")
        print("  import pynlm")
        print("  nlm = pynlm")
        print("  brain = nlm.create_brain(nlm.create_default_config())")
        print("  brain.initialize()")
        
        return 0
        
    except Exception as e:
        print(f"\nInstallation failed: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())