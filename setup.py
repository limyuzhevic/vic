#!/usr/bin/env python3
"""
Setup script for building and distributing NLM Python bindings
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return the result"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            shell=True,
            check=True,
            capture_output=True,
            text=True
        )
        print(f"Success: {result.stdout}")
        return True, result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False, e.stderr

def check_python_bindings():
    """Check if Python bindings exist"""
    bindings_path = Path("python/bindings.cpp")
    if bindings_path.exists():
        print(f"✓ Python bindings found at {bindings_path}")
        return True
    else:
        print(f"✗ Python bindings not found at {bindings_path}")
        return False

def check_cpp_source():
    """Check if C++ source exists"""
    src_path = Path("src")
    if src_path.exists() and any(src_path.iterdir()):
        print(f"✓ C++ source found at {src_path}")
        return True
    else:
        print(f"✗ C++ source not found at {src_path}")
        return False

def build_python_bindings():
    """Build Python bindings using scikit-build"""
    print("\nBuilding Python bindings...")
    
    # Create build directory
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # Run CMake
    cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    success, output = run_command(cmake_cmd, cwd=build_dir)
    if not success:
        print("CMake configuration failed")
        return False
    
    # Run make
    make_cmd = "make -j4"
    success, output = run_command(make_cmd, cwd=build_dir)
    if not success:
        print("Make build failed")
        return False
    
    print("✓ Python bindings built successfully")
    return True

def check_package_structure():
    """Check if package structure is correct"""
    print("\nChecking package structure...")
    
    required_dirs = ["python", "pynlm", "examples"]
    all_good = True
    
    for dir_name in required_dirs:
        dir_path = Path(dir_name)
        if dir_path.exists():
            print(f"✓ Directory {dir_name} exists")
        else:
            print(f"✗ Directory {dir_name} missing")
            all_good = False
    
    required_files = [
        "python/bindings.cpp",
        "pynlm/__init__.py",
        "examples/basic_brain.py",
        "examples/agent_world.py",
        "examples/checkpointing.py",
        "examples/analysis.py",
        "examples/cli.py",
        "examples/demo.py"
    ]
    
    for file_path in required_files:
        if Path(file_path).exists():
            print(f"✓ File {file_path} exists")
        else:
            print(f"✗ File {file_path} missing")
            all_good = False
    
    return all_good

def create_setup_py():
    """Create setup.py file if it doesn't exist"""
    setup_path = Path("setup.py")
    
    if setup_path.exists():
        print(f"✓ setup.py already exists at {setup_path}")
        return True
    
    print(f"Creating setup.py at {setup_path}")
    
    setup_content = '''"""Setup script for NLM Python bindings"""

import os
import sys
from setuptools import setup, find_packages
from pathlib import Path

def read_requirements():
    """Read requirements from file if it exists"""
    requirements_file = Path("requirements.txt")
    if requirements_file.exists():
        with open(requirements_file, "r") as f:
            return [line.strip() for line in f if line.strip() and not line.startswith("#")]
    return []

def read_readme():
    """Read README for long description"""
    readme_file = Path("pynlm/README.md")
    if readme_file.exists():
        with open(readme_file, "r", encoding="utf-8") as f:
            return f.read()
    return ""

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="nlm@example.com",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    package_dir={"": "python"},
    package_data={
        "pynlm": ["*.py", "*.md", "*.txt"],
    },
    include_package_data=True,
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
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=">=3.8",
    install_requires=read_requirements(),
    extras_require={
        "dev": [
            "pytest>=7.0",
            "pytest-cov>=4.0",
            "matplotlib>=3.5",
            "jupyter>=1.0",
            "black>=23.0",
            "ruff>=0.1.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm=examples.cli:main",
            "nlm-demo=examples.demo:main",
        ],
    },
    zip_safe=False,
)
'''
    
    try:
        with open(setup_path, "w") as f:
            f.write(setup_content)
        print("✓ setup.py created successfully")
        return True
    except Exception as e:
        print(f"✗ Failed to create setup.py: {e}")
        return False

def main():
    """Main setup function"""
    print("=" * 70)
    print("NLM Python Bindings - Setup Script")
    print("=" * 70)
    
    # Check system
    print(f"\nPlatform: {platform.system()} {platform.release()}")
    print(f"Python: {sys.version}")
    
    # Check requirements
    all_checks_passed = True
    
    if not check_cpp_source():
        all_checks_passed = False
    
    if not check_python_bindings():
        all_checks_passed = False
    
    if not check_package_structure():
        all_checks_passed = False
    
    # Create setup.py if needed
    if not create_setup_py():
        all_checks_passed = False
    
    if all_checks_passed:
        print("\n" + "=" * 70)
        print("✓ All checks passed!")
        print("\nNext steps:")
        print("  1. Install Python dependencies:")
        print("     pip install -r requirements.txt")
        print("  2. Build Python bindings:")
        print("     python setup.py build_ext --inplace")
        print("  3. Install the package:")
        print("     pip install -e .")
        print("=" * 70)
        return True
    else:
        print("\n" + "=" * 70)
        print("✗ Some checks failed. Please fix the issues above.")
        print("=" * 70)
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
