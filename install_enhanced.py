#!/usr/bin/env python3
"""
Setup script for NLM Python bindings (both basic and enhanced modules)

This script provides commands to install, test, and manage the NLM Python bindings.
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

# Colors for output
class Colors:
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    END = "\033[0m"

def print_info(message):
    print(f"{Colors.GREEN}[INFO]{Colors.END} {message}")

def print_warning(message):
    print(f"{Colors.YELLOW}[WARNING]{Colors.END} {message}")

def print_error(message):
    print(f"{Colors.RED}[ERROR]{Colors.END} {message}")

def check_python_version():
    """Check Python version compatibility"""
    version = sys.version_info
    if version.major < 3 or version.minor < 8:
        print_error("Python 3.8 or higher is required")
        return False
    print_info(f"Python {version.major}.{version.minor}.{version.micro} detected")
    return True

def check_pybind11():
    """Check if pybind11 is available"""
    try:
        import pybind11
        print_info(f"pybind11 version: {pybind11.__version__}")
        return True
    except ImportError:
        print_error("pybind11 not found. Please install pybind11: pip install pybind11")
        return False

def check_dependencies():
    """Check if required Python packages are installed"""
    required_packages = [
        ("numpy", "numpy"),
        ("matplotlib", "matplotlib"),
        ("seaborn", "seaborn"),
        ("pandas", "pandas"),
    ]
    
    missing_packages = []
    for import_name, package_name in required_packages:
        try:
            __import__(import_name)
            print_info(f"✓ {package_name} found")
        except ImportError:
            print_warning(f"⚠ {package_name} not found - some features may not work")
            missing_packages.append(package_name)
    
    return missing_packages

def install_enhanced_features():
    """Install enhanced features"""
    print_info("Installing enhanced NLM Python bindings...")
    
    # Check if we're in the right directory
    current_dir = Path.cwd()
    if not (current_dir / "src").exists():
        print_error("This script must be run from the NLM project root directory")
        return False
    
    # Build and install enhanced module
    print_info("Building enhanced bindings module...")
    result = subprocess.run([
        "cmake", "-B", "build/enhanced",
        "-DENABLE_ENHANCED_FEATURES=ON",
        "-DPYTHON_EXECUTABLE=" + sys.executable,
        "-GNinja"
    ], cwd=current_dir, capture_output=True, text=True)
    
    print_info("Building enhanced bindings module...")
    result = subprocess.run([
        "cmake", "--build", "build/enhanced",
        "--config", "Release"
    ], cwd=current_dir, capture_output=True, text=True)
    
    if result.returncode != 0:
        print_error(f"Failed to build enhanced bindings: {result.stderr}")
        return False
    
    print_info("Installing enhanced module...")
    result = subprocess.run([
        "pip", "install", "-e", "./enhanced"
    ], cwd=current_dir, capture_output=True, text=True)
    
    if result.returncode != 0:
        print_error(f"Failed to install enhanced module: {result.stderr}")
        return False
    
    return True

def test_enhanced_features():
    """Test enhanced features"""
    print_info("Testing enhanced NLM Python bindings...")
    
    try:
        import pynlm_enhanced as nlm
        
        # Test configuration creation
        config = nlm.createBrainConfig()
        print_info("✓ Brain configuration created")
        
        # Test enhanced configuration
        config = nlm.createAdvancedConfig()
        print_info("✓ Advanced configuration created")
        
        # Test configuration validation
        errors = nlm.validateConfiguration(config)
        if errors:
            print_warning(f"⚠ Configuration validation found {len(errors)} errors")
        else:
            print_info("✓ Configuration validation passed")
        
        # Test enhanced analysis tools
        metrics = nlm.analyzeNetworkTopology(config)
        print_info(f"✓ Network topology analysis completed: {len(metrics)} metrics")
        
        # Test learning curve analysis
        import numpy as np
        rewards = np.random.randn(100).cumsum() + 100
        learning_analysis = nlm.analyzeLearningCurve(rewards, list(range(1, 101)))
        print_info(f"✓ Learning curve analysis completed: {len(learning_analysis)} metrics")
        
        print_info("All enhanced features tests passed!")
        return True
        
    except ImportError as e:
        print_error(f"Enhanced module not found: {e}")
        return False
    except Exception as e:
        print_error(f"Error testing enhanced features: {e}")
        return False

def test_basic_features():
    """Test basic features"""
    print_info("Testing basic NLM Python bindings...")
    
    try:
        import pynlm as nlm
        
        # Test basic configuration
        config = nlm.createDefaultConfig()
        print_info("✓ Basic configuration created")
        
        # Test basic brain creation
        brain = nlm.createBrain(config)
        print_info("✓ Basic brain created")
        
        print_info("All basic features tests passed!")
        return True
        
    except ImportError as e:
        print_error(f"Basic module not found: {e}")
        return False
    except Exception as e:
        print_error(f"Error testing basic features: {e}")
        return False

def print_usage():
    """Print usage information"""
    print("\n" + "="*60)
    print("NLM Python Bindings Installation and Usage")
    print("="*60)
    
    print("\nAvailable commands:")
    print("  install-all      Install both basic and enhanced modules")
    print("  install-basic    Install only basic module")
    print("  install-enhanced Install only enhanced module")
    print("  test-all         Test all modules")
    print("  test-basic       Test basic module")
    print("  test-enhanced    Test enhanced module")
    print("  help             Show this help message")
    
    print("\nInstallation options:")
    print("  pip install .                    Install from current directory")
    print("  pip install pynlm                 Install basic module only")
    print("  pip install pynlm-enhanced         Install enhanced module only")
    
    print("\nQuick start:")
    print("  1. Install the enhanced module:")
    print("     pip install pynlm-enhanced")
    print("  2. Create a configuration:")
    print("     import pynlm_advanced as nlm")
    print("     config = nlm.createBrainConfig()")
    print("  3. Create a brain:")
    print("     brain = nlm.createBrain(config)")
    print("  4. Run simulation and analyze results")
    
    print("\nFor more information, visit:")
    print("  https://github.com/nlm-project/nlm")
    print("  https://github.com/nlm-project/nlm-enhanced")

def main():
    if len(sys.argv) < 2:
        print_usage()
        return 1
    
    command = sys.argv[1].lower()
    
    if command == "install-all":
        print_info("Installing NLM Python bindings (all modules)...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        missing_packages = check_dependencies()
        
        if install_enhanced_features():
            print_info("✓ Enhanced features installed successfully")
        else:
            print_error("Failed to install enhanced features")
            return 1
        
        if test_enhanced_features():
            print_info("✓ Enhanced features testing passed")
        else:
            print_warning("⚠ Enhanced features testing failed")
        
    elif command == "install-basic":
        print_info("Installing basic NLM Python bindings...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        # Install basic module (current directory)
        try:
            subprocess.run([sys.executable, "-m", "pip", "install", "-e", "."],
                         check=True, capture_output=True, text=True)
            print_info("✓ Basic module installed successfully")
            
            if test_basic_features():
                print_info("✓ Basic features testing passed")
            else:
                print_warning("⚠ Basic features testing failed")
                
        except subprocess.CalledProcessError as e:
            print_error(f"Failed to install basic module: {e}")
            return 1
            
    elif command == "install-enhanced":
        print_info("Installing enhanced NLM Python bindings...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        missing_packages = check_dependencies()
        
        if install_enhanced_features():
            print_info("✓ Enhanced features installed successfully")
        else:
            print_error("Failed to install enhanced features")
            return 1
        
        if test_enhanced_features():
            print_info("✓ Enhanced features testing passed")
        else:
            print_warning("⚠ Enhanced features testing failed")
            
    elif command == "test-all":
        print_info("Testing all NLM Python bindings...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        missing_packages = check_dependencies()
        
        success = True
        
        if test_basic_features():
            print_info("✓ Basic features testing passed")
        else:
            success = False
            print_warning("⚠ Basic features testing failed")
        
        if test_enhanced_features():
            print_info("✓ Enhanced features testing passed")
        else:
            success = False
            print_warning("⚠ Enhanced features testing failed")
        
        if success:
            print_info("All tests passed!")
        else:
            print_error("Some tests failed")
            return 1
            
    elif command == "test-basic":
        print_info("Testing basic NLM Python bindings...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        if test_basic_features():
            print_info("✓ Basic features testing passed")
            return 0
        else:
            print_error("Basic features testing failed")
            return 1
            
    elif command == "test-enhanced":
        print_info("Testing enhanced NLM Python bindings...")
        if not check_python_version():
            return 1
        
        if not check_pybind11():
            return 1
        
        missing_packages = check_dependencies()
        
        if test_enhanced_features():
            print_info("✓ Enhanced features testing passed")
            return 0
        else:
            print_error("Enhanced features testing failed")
            return 1
            
    elif command == "help":
        print_usage()
    
    else:
        print_error(f"Unknown command: {command}")
        print_usage()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())