"""
NLM Python Bindings - Installation and Setup Guide

This file provides comprehensive setup instructions for using NLM Python bindings.
"""

import subprocess
import sys
import os

def check_python_version():
    """Check if Python version is compatible"""
    version = sys.version_info
    print(f"Python version: {version.major}.{version.minor}.{version.micro}")
    
    if version.major == 3 and version.minor >= 8:
        print("✓ Python version is compatible")
        return True
    else:
        print("✗ Python 3.8+ is required")
        return False

def check_pybind11():
    """Check if pybind11 is available"""
    try:
        import pybind11
        print(f"✓ pybind11 version: {pybind11.__version__}")
        return True
    except ImportError:
        print("✗ pybind11 is not installed")
        return False

def check_scikit_build():
    """Check if scikit-build-core is available"""
    try:
        import scikit_build_core
        print(f"✓ scikit-build-core version: {scikit_build_core.__version__}")
        return True
    except ImportError:
        print("✗ scikit-build-core is not installed")
        return False

def install_dependencies():
    """Install required Python dependencies"""
    print("\nInstalling Python dependencies...")
    
    packages = [
        "pybind11>=2.11.0",
        "scikit-build-core>=0.5.0",
        "pytest>=7.0",
        "numpy>=1.20"
    ]
    
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install"] + packages)
        print("✓ Dependencies installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Failed to install dependencies: {e}")
        return False

def build_from_source():
    """Build NLM from source"""
    print("\nBuilding NLM from source...")
    
    # Check if we're in the right directory
    current_dir = os.getcwd()
    if not os.path.exists("pyproject.toml"):
        print("✗ pyproject.toml not found. Please run this script from the NLM project root.")
        return False
    
    try:
        # Install in development mode
        subprocess.check_call([sys.executable, "-m", "pip", "install", "-e", "."])
        print("✓ NLM built and installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Build failed: {e}")
        return False

def test_installation():
    """Test the NLM installation"""
    print("\nTesting NLM installation...")
    
    try:
        import pynlm
        print(f"✓ pynlm module imported successfully")
        print(f"  Version: {pynlm.__version__ if hasattr(pynlm, '__version__') else 'Unknown'}")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created")
        
        brain = pynlm.createBrain(config)
        print(f"✓ Brain created with {brain.total_neurons} neurons")
        
        brain.initialize()
        print(f"✓ Brain initialized")
        
        brain.step(0)
        print(f"✓ Brain step executed")
        
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Test failed: {e}")
        return False

def main():
    """Main setup and installation function"""
    print("NLM Python Bindings - Installation and Setup")
    print("=" * 50)
    
    # Check system requirements
    print("\n1. Checking system requirements...")
    python_ok = check_python_version()
    pybind11_ok = check_pybind11()
    scikit_build_ok = check_scikit_build()
    
    if not (python_ok and pybind11_ok and scikit_build_ok):
        print("\n⚠ Some requirements are missing. Installing...")
        install_dependencies()
        
        # Re-check after installation
        python_ok = check_python_version()
        pybind11_ok = check_pybind11()
        scikit_build_ok = check_scikit_build()
    
    # Build from source
    print("\n2. Building from source...")
    build_success = build_from_source()
    
    # Test installation
    print("\n3. Testing installation...")
    test_success = test_installation()
    
    # Summary
    print("\n" + "=" * 50)
    print("Installation Summary:")
    print(f"  Python version: {'✓' if python_ok else '✗'}")
    print(f"  pybind11: {'✓' if pybind11_ok else '✗'}")
    print(f"  scikit-build-core: {'✓' if scikit_build_ok else '✗'}")
    print(f"  Build successful: {'✓' if build_success else '✗'}")
    print(f"  Installation test: {'✓' if test_success else '✗'}")
    
    if python_ok and pybind11_ok and scikit_build_ok and build_success and test_success:
        print("\n🎉 NLM Python bindings installed successfully!")
        print("\nNext steps:")
        print("  1. Run examples: python examples/advanced_examples.py")
        print("  2. Read documentation: HOW_TO_USE.md")
        print("  3. Start using NLM in your Python code!")
        return 0
    else:
        print("\n❌ Installation failed. Please check the output above for details.")
        return 1

if __name__ == "__main__":
    exit(main())