#!/usr/bin/env python3
"""
NLM Python Package Setup

This script provides utilities for installing and managing the NLM Python bindings.
It includes helper functions for installation, configuration, and package management.
"""

import os
import sys
import json
import platform
import subprocess
from pathlib import Path
from typing import Dict, List, Optional

def get_platform():
    """Get the current platform information."""
    system = platform.system()
    machine = platform.machine()
    
    if system == "Linux":
        return "linux"
    elif system == "Darwin":
        return "macos"
    elif system == "Windows":
        return "windows"
    else:
        return "unknown"

def get_python_version():
    """Get Python version information."""
    return f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}"

def check_dependencies() -> Dict[str, bool]:
    """Check if required dependencies are installed."""
    dependencies = {
        "numpy": False,
        "pybind11": False,
        "pytest": False,
        "cmake": False,
        "make": False
    }
    
    # Check Python packages
    for dep in ["numpy", "pybind11", "pytest"]:
        try:
            __import__(dep)
            dependencies[dep] = True
            print(f"✓ {dep} is installed")
        except ImportError:
            print(f"✗ {dep} is not installed")
    
    # Check system tools
    for tool in ["cmake", "make"]:
        try:
            subprocess.run([tool, "--version"], 
                          capture_output=True, 
                          check=True)
            dependencies[tool] = True
            print(f"✓ {tool} is installed")
        except (subprocess.CalledProcessError, FileNotFoundError):
            print(f"✗ {tool} is not installed")
    
    return dependencies

def create_build_directory() -> Path:
    """Create build directory for NLM compilation."""
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    return build_dir

def configure_cmake(build_dir: Path, config_file: str = "configs/default.cfg") -> bool:
    """Configure CMake build."""
    try:
        cmd = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE=Release",
            f"-DNLM_SOURCE_DIR=src",
            f"-DCMAKE_CXX_STANDARD=20"
        ]
        
        if Path(config_file).exists():
            cmd.append(f"-DCONFIG_FILE={config_file}")
        
        result = subprocess.run(
            cmd,
            cwd=build_dir,
            capture_output=True,
            text=True
        )
        
        if result.returncode == 0:
            print("✓ CMake configuration successful")
            return True
        else:
            print(f"✗ CMake configuration failed: {result.stderr}")
            return False
            
    except Exception as e:
        print(f"✗ CMake configuration error: {e}")
        return False

def build_nlm(build_dir: Path) -> bool:
    """Build NLM with CMake."""
    try:
        # Determine number of CPU cores for parallel build
        import multiprocessing
        cpu_count = multiprocessing.cpu_count()
        
        cmd = ["cmake", "--build", ".", "--config", "Release", "-j", str(cpu_count)]
        
        result = subprocess.run(
            cmd,
            cwd=build_dir,
            capture_output=True,
            text=True
        )
        
        if result.returncode == 0:
            print("✓ NLM build successful")
            return True
        else:
            print(f"✗ NLM build failed: {result.stderr}")
            return False
            
    except Exception as e:
        print(f"✗ NLM build error: {e}")
        return False

def install_python_bindings(build_dir: Path) -> bool:
    """Install Python bindings."""
    try:
        # Check if the module is already built
        module_path = build_dir / "python" / "pynlm.so"
        if not module_path.exists():
            # Try to find the module in common locations
            for ext in ["*.so", "*.pyd"]:
                modules = list(build_dir.glob(f"**/{ext}"))
                if modules:
                    module_path = modules[0]
                    break
        
        if module_path.exists():
            # Copy module to appropriate location
            target_dir = Path("lib") / "python" / f"nlm-{get_python_version()}"
            target_dir.mkdir(parents=True, exist_ok=True)
            
            # Copy module
            import shutil
            shutil.copy2(module_path, target_dir / module_path.name)
            
            # Create __init__.py
            init_file = target_dir / "__init__.py"
            init_file.write_text('"""NLM Python bindings module."""\nfrom .pynlm import *\n')
            
            print(f"✓ Python bindings installed to {target_dir}")
            return True
        else:
            print(f"✗ Could not find Python module at {module_path}")
            return False
            
    except Exception as e:
        print(f"✗ Python bindings installation error: {e}")
        return False

def create_symlink_for_import() -> bool:
    """Create symlink for easier Python import."""
    try:
        if get_platform() in ["linux", "macos"]:
            # Create symlink in site-packages
            python_path = subprocess.run(
                [sys.executable, "-c", "import site; print(site.getsitepackages()[0])"],
                capture_output=True,
                text=True
            ).stdout.strip()
            
            if python_path:
                symlink_path = Path(python_path) / "nlm"
                target_path = Path.cwd() / "lib" / "python" / f"nlm-{get_python_version()}"
                
                if symlink_path.exists():
                    symlink_path.unlink()
                
                symlink_path.symlink_to(target_path)
                print(f"✓ Created symlink for import at {symlink_path}")
                return True
        
        print("⚠ Symlink creation not supported on this platform")
        return False
        
    except Exception as e:
        print(f"⚠ Symlink creation warning: {e}")
        return False

def setup_development_environment() -> bool:
    """Set up development environment for NLM."""
    print("Setting up development environment...")
    
    # Create necessary directories
    directories = [
        "build",
        "lib/python",
        "examples",
        "docs",
        "configs",
        "tests"
    ]
    
    for dir_name in directories:
        Path(dir_name).mkdir(exist_ok=True)
        print(f"✓ Created directory: {dir_name}")
    
    # Create requirements.txt if it doesn't exist
    requirements_path = Path("requirements.txt")
    if not requirements_path.exists():
        requirements = """# NLM Python dependencies
numpy>=1.20
pybind11>=2.11.0
pytest>=7.0
"""
        requirements_path.write_text(requirements)
        print("✓ Created requirements.txt")
    
    # Create setup.py if it doesn't exist
    setup_path = Path("setup.py")
    if not setup_path.exists():
        setup_content = '''from setuptools import setup, find_packages

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine)",
    author="NLM Authors",
    packages=find_packages(),
    install_requires=[
        "numpy>=1.20",
    ],
    python_requires=">=3.8",
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
    ],
)
'''
        setup_path.write_text(setup_content)
        print("✓ Created setup.py")
    
    return True

def run_tests() -> bool:
    """Run NLM tests."""
    try:
        # Check if tests exist
        test_dir = Path("tests")
        if not test_dir.exists():
            print("✗ Tests directory not found")
            return False
        
        # Find test files
        test_files = list(test_dir.glob("test_*.py"))
        if not test_files:
            print("✗ No test files found")
            return False
        
        print(f"✓ Found {len(test_files)} test files")
        
        # Run tests
        for test_file in test_files:
            print(f"\nRunning {test_file.name}...")
            result = subprocess.run(
                [sys.executable, "-m", "pytest", str(test_file), "-v"],
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                print(f"✓ {test_file.name} passed")
            else:
                print(f"✗ {test_file.name} failed:")
                print(result.stdout)
                print(result.stderr)
                return False
        
        print("✓ All tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Test execution error: {e}")
        return False

def main():
    """Main function to set up NLM Python bindings."""
    print("NLM Python Bindings Setup")
    print("=" * 50)
    
    # Check platform and Python version
    platform_name = get_platform()
    python_version = get_python_version()
    
    print(f"Platform: {platform_name}")
    print(f"Python version: {python_version}")
    print()
    
    # Check dependencies
    print("Checking dependencies...")
    dependencies = check_dependencies()
    
    # Check if all critical dependencies are installed
    critical_deps = ["pybind11", "cmake", "make"]
    missing_critical = [dep for dep in critical_deps if not dependencies.get(dep, False)]
    
    if missing_critical:
        print(f"✗ Missing critical dependencies: {', '.join(missing_critical)}")
        print("Please install missing dependencies and try again.")
        return 1
    
    # Setup development environment
    print("\nSetting up development environment...")
    if not setup_development_environment():
        print("✗ Development environment setup failed")
        return 1
    
    # Try to build NLM
    print("\nBuilding NLM...")
    build_dir = create_build_directory()
    
    if configure_cmake(build_dir):
        if build_nlm(build_dir):
            if install_python_bindings(build_dir):
                create_symlink_for_import()
                print("\n✓ NLM Python bindings setup completed successfully!")
                print("\nNext steps:")
                print("1. Test the installation with: python -c \"import nlm; print(nlm.__version__)\"")
                print("2. Run examples with: python examples/nlm_examples.py")
                print("3. Install in development mode with: pip install -e .")
                return 0
            else:
                print("\n✗ Python bindings installation failed")
                return 1
        else:
            print("\n✗ NLM build failed")
            return 1
    else:
        print("\n✗ CMake configuration failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
