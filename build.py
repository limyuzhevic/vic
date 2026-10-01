#!/usr/bin/env python3
"""
Build script for NLM Python bindings.

This script builds the NLM Python package using scikit-build-core,
which provides native Python integration for C++ extensions.
"""

import sys
import os
import subprocess
import argparse
from pathlib import Path


def check_prerequisites():
    """Check if all prerequisites are available."""
    print("Checking prerequisites...")
    
    # Check Python version
    if sys.version_info < (3, 8):
        print("❌ Python 3.8 or higher is required")
        return False
    
    print("✓ Python version OK")
    
    # Check if pybind11 is available
    try:
        import pybind11
        print(f"✓ pybind11 version {pybind11.__version__} found")
    except ImportError:
        print("❌ pybind11 is not installed. Please install it with:")
        print("   pip install pybind11")
        return False
    
    # Check if scikit-build-core is available
    try:
        import scikit_build_core
        print(f"✓ scikit-build-core version {scikit_build_core.__version__} found")
    except ImportError:
        print("❌ scikit-build-core is not installed. Please install it with:")
        print("   pip install scikit-build-core")
        return False
    
    # Check if CMake is available
    try:
        result = subprocess.run(
            ["cmake", "--version"],
            capture_output=True,
            text=True,
            check=True
        )
        cmake_version = result.stdout.split('\n')[0]
        print(f"✓ CMake found: {cmake_version.strip()}")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ CMake is not installed or not in PATH")
        print("   Install CMake from: https://cmake.org/download/")
        return False
    
    return True


def build_with_sklearn_build():
    """Build using scikit-build-core."""
    print("\nBuilding with scikit-build-core...")
    
    # Build the Python package
    try:
        print("Running: pip install -e .")
        result = subprocess.run(
            [sys.executable, "-m", "pip", "install", "-e", "."],
            check=True,
            capture_output=True,
            text=True
        )
        print("✓ Build successful")
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print("❌ Build failed")
        print("Error output:")
        print(e.stderr)
        return False


def build_with_pip_build():
    """Build using pip build."""
    print("\nBuilding with pip build...")
    
    try:
        print("Running: python -m build")
        result = subprocess.run(
            [sys.executable, "-m", "build"],
            check=True,
            capture_output=True,
            text=True
        )
        print("✓ Build successful")
        
        # Install the built package
        dist_dir = Path("dist")
        if dist_dir.exists():
            wheels = list(dist_dir.glob("*.whl"))
            if wheels:
                print(f"\nFound wheel: {wheels[0]}")
                print("You can install it with:")
                print(f"  pip install {wheels[0]}")
        
        return True
    except subprocess.CalledProcessError as e:
        print("❌ Build failed")
        print("Error output:")
        print(e.stderr)
        return False


def build_with_custom_cmake():
    """Build using custom CMake configuration."""
    print("\nBuilding with custom CMake...")
    
    # Create build directory
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # Configure CMake
    cmake_cmd = [
        "cmake",
        "..",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DPYTHON_EXECUTABLE=" + sys.executable,
        "-DPYTHON_MODULE_EXTENSION=.pyd" if sys.platform == "win32" else "-DPYTHON_MODULE_EXTENSION=.so",
    ]
    
    try:
        print(f"Configuring CMake: {' '.join(cmake_cmd)}")
        result = subprocess.run(
            cmake_cmd,
            cwd=build_dir,
            check=True,
            capture_output=True,
            text=True
        )
        print("✓ CMake configuration successful")
        
        # Build
        print("Running: make -j4")
        result = subprocess.run(
            ["make", "-j4"],
            cwd=build_dir,
            check=True,
            capture_output=True,
            text=True
        )
        print("✓ Build successful")
        
        # Install Python module
        print("Installing Python module...")
        
        # Find the built module
        module_paths = list(build_dir.rglob("pynlm*.so")) + list(build_dir.rglob("pynlm*.pyd"))
        if module_paths:
            module_path = module_paths[0]
            
            # Install to site-packages
            site_packages = subprocess.run(
                [sys.executable, "-c", "import site; print(site.getsitepackages()[0])"],
                capture_output=True,
                text=True,
                check=True
            ).stdout.strip()
            
            target_dir = Path(site_packages) / "nlm"
            target_dir.mkdir(exist_ok=True)
            
            # Copy module files
            for ext in ['*.so', '*.pyd', '*.py']:
                for src in build_dir.rglob(ext):
                    if src.is_file():
                        dst = target_dir / src.name
                        dst.write_bytes(src.read_bytes())
            
            print(f"✓ Python module installed to {target_dir}")
        
        return True
        
    except subprocess.CalledProcessError as e:
        print("❌ Build failed")
        print("Error output:")
        print(e.stderr)
        return False


def run_tests():
    """Run tests after building."""
    print("\nRunning tests...")
    
    try:
        # Try to import the module
        print("Testing module import...", end=" ")
        
        # Add current directory to path
        sys.path.insert(0, str(Path(__file__).parent))
        
        import nlm
        print("✓ PASSED")
        
        # Test basic functionality
        print("Testing basic functionality...", end=" ")
        brain = nlm.Brain()
        assert brain is not None
        print("✓ PASSED")
        
        # Test Python wrapper
        print("Testing Python wrapper functionality...", end=" ")
        config = nlm.ConfigManager()
        config.set("brain.neuron_count", 10)
        assert config.get_int("brain.neuron_count") == 10
        print("✓ PASSED")
        
        print("✓ All basic tests passed!")
        return True
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Build script for NLM Python bindings",
        prog="build.py"
    )
    
    parser.add_argument(
        "--build-system", choices=["scikit-build-core", "pip-build", "cmake"],
        default="scikit-build-core",
        help="Build system to use (default: scikit-build-core)"
    )
    
    parser.add_argument(
        "--skip-tests", action="store_true",
        help="Skip running tests after build"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    print("NLM Python Build Script")
    print("=" * 40)
    
    # Check prerequisites
    if not check_prerequisites():
        sys.exit(1)
    
    # Build
    success = False
    
    if args.build_system == "scikit-build-core":
        success = build_with_sklearn_build()
    
    elif args.build_system == "pip-build":
        success = build_with_pip_build()
    
    elif args.build_system == "cmake":
        success = build_with_custom_cmake()
    
    # Run tests
    if success and not args.skip_tests:
        if not run_tests():
            success = False
    
    if success:
        print("\n✓ Build completed successfully!")
        print("\nYou can now use the NLM Python package:")
        print("  import nlm")
        print("  brain = nlm.Brain()")
        print("  world = nlm.SimpleWorld()")
        print("  agent = nlm.Agent(brain)")
        
        return 0
    else:
        print("\n❌ Build failed!")
        return 1


if __name__ == "__main__":
    exit(main())
