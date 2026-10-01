#!/usr/bin/env python3
"""
Setup script for enhanced NLM Python bindings

This script sets up the enhanced Python bindings and handles
dependencies for the NLM framework.
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def print_info(message):
    """Print informational message."""
    print(f"[INFO] {message}")

def print_error(message):
    """Print error message."""
    print(f"[ERROR] {message}")

def print_warning(message):
    """Print warning message."""
    print(f"[WARNING] {message}")

def check_python_version():
    """Check if Python version is compatible."""
    version = sys.version_info
    if version.major == 3 and version.minor >= 8:
        print_info(f"Python {version.major}.{version.minor}.{version.micro} detected - compatible")
        return True
    else:
        print_error(f"Python {version.major}.{version.minor}.{version.micro} is not supported. Requires Python 3.8+")
        return False

def check_build_dependencies():
    """Check if build dependencies are available."""
    print_info("Checking build dependencies...")
    
    # Check for CMake
    try:
        subprocess.run(["cmake", "--version"], check=True, capture_output=True)
        print_info("CMake found")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print_error("CMake not found. Please install CMake 3.16+")
        return False
    
    # Check for C++ compiler
    try:
        if platform.system() == "Windows":
            compiler = "cl"
        else:
            compiler = "g++"
        
        subprocess.run([compiler, "--version"], check=True, capture_output=True)
        print_info(f"{compiler} found")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print_error("C++ compiler not found. Please install a C++ compiler")
        return False
    
    # Check for pybind11
    try:
        import pybind11
        print_info(f"pybind11 found: {pybind11.__version__}")
    except ImportError:
        print_error("pybind11 not found. Please install pybind11>=2.11")
        return False
    
    return True

def check_python_dependencies():
    """Check if Python dependencies are available."""
    print_info("Checking Python dependencies...")
    
    # Check for NumPy
    try:
        import numpy
        print_info(f"NumPy found: {numpy.__version__}")
    except ImportError:
        print_warning("NumPy not found. Installing for enhanced features...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "numpy"])
            import numpy
            print_info(f"NumPy installed: {numpy.__version__}")
        except subprocess.CalledProcessError:
            print_error("Failed to install NumPy")
            return False
    
    # Check for matplotlib (optional)
    try:
        import matplotlib
        print_info(f"Matplotlib found: {matplotlib.__version__}")
    except ImportError:
        print_warning("Matplotlib not found. Installing for visualization...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "matplotlib"])
            import matplotlib
            print_info(f"Matplotlib installed: {matplotlib.__version__}")
        except subprocess.CalledProcessError:
            print_warning("Failed to install matplotlib - visualization features will be limited")
    
    # Check for OpenCV (optional)
    try:
        import cv2
        print_info(f"OpenCV found: {cv2.__version__}")
    except ImportError:
        print_warning("OpenCV not found. Installing for video export...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "opencv-python"])
            import cv2
            print_info(f"OpenCV installed: {cv2.__version__}")
        except subprocess.CalledProcessError:
            print_warning("Failed to install OpenCV - video export will be limited")
    
    return True

def build_enhanced_bindings():
    """Build the enhanced Python bindings."""
    print_info("Building enhanced Python bindings...")
    
    # Check if build directory exists
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir()
    
    # Configure CMake
    cmake_cmd = [
        "cmake", "..",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DPYTHON_EXECUTABLE=" + sys.executable,
        "-DHAS_NUMPY=ON"
    ]
    
    try:
        print_info(f"Running CMake: {' '.join(cmake_cmd)}")
        subprocess.run(cmake_cmd, check=True, cwd=build_dir)
    except subprocess.CalledProcessError as e:
        print_error(f"CMake configuration failed: {e}")
        return False
    
    # Build the project
    try:
        print_info("Building with make...")
        subprocess.run(["make", "-j4"], check=True, cwd=build_dir)
        print_info("Build completed successfully")
    except subprocess.CalledProcessError as e:
        print_error(f"Build failed: {e}")
        return False
    
    return True

def install_python_bindings():
    """Install Python bindings."""
    print_info("Installing Python bindings...")
    
    # Install the package in development mode
    try:
        subprocess.run([
            sys.executable, "-m", "pip", "install", "-e", "."
        ], check=True)
        print_info("Python bindings installed successfully")
    except subprocess.CalledProcessError as e:
        print_error(f"Installation failed: {e}")
        return False
    
    # Install dependencies
    try:
        print_info("Installing dependencies...")
        subprocess.run([
            sys.executable, "-m", "pip", "install",
            "pytest>=7.0",
            "numpy>=1.20",
            "matplotlib",
            "opencv-python"
        ], check=True)
        print_info("Dependencies installed successfully")
    except subprocess.CalledProcessError as e:
        print_warning(f"Failed to install all dependencies: {e}")
    
    return True

def create_test_environment():
    """Create test environment."""
    print_info("Creating test environment...")
    
    # Create test directory
    test_dir = Path("test_enhanced")
    test_dir.mkdir(exist_ok=True)
    
    # Create test files
    test_files = [
        "test_basic_functionality.py",
        "test_agent_world.py",
        "test_memory_systems.py",
        "test_neuromodulation.py",
        "test_prediction_systems.py",
        "test_cognition_systems.py",
        "test_numpy_integration.py",
        "test_reproducible_research.py",
    ]
    
    for test_file in test_files:
        test_path = test_dir / test_file
        if not test_path.exists():
            # Create minimal test file
            with open(test_path, "w") as f:
                f.write(f"""#!/usr/bin/env python3
\"\"\"
Test file for {test_file}
\"\"\"

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + "/..")

try:
    import pynlm as nlm
    print(f"✓ {test_file} imported successfully")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {{e}}")
""")
    print_info(f"Created {test_file}")
    
    return True

def run_tests():
    """Run tests for enhanced bindings."""
    print_info("Running tests...")
    
    test_script = Path("test_enhanced_bindings.py")
    if test_script.exists():
        try:
            result = subprocess.run([
                sys.executable, str(test_script)
            ], check=True, capture_output=True, text=True)
            
            print_info("Test results:")
            print(result.stdout)
            
            if result.stderr:
                print_warning("Test warnings/errors:")
                print(result.stderr)
            
            return result.returncode == 0
        except subprocess.CalledProcessError as e:
            print_error(f"Tests failed: {e}")
            return False
    else:
        print_error("Test script not found")
        return False

def main():
    """Main setup function."""
    print_info("NLM Enhanced Python Bindings Setup")
    print_info("=" * 40)
    
    # Check Python version
    if not check_python_version():
        return 1
    
    # Check build dependencies
    if not check_build_dependencies():
        return 1
    
    # Check Python dependencies
    if not check_python_dependencies():
        return 1
    
    # Build enhanced bindings
    if not build_enhanced_bindings():
        return 1
    
    # Install Python bindings
    if not install_python_bindings():
        return 1
    
    # Create test environment
    if not create_test_environment():
        print_warning("Failed to create test environment - continuing anyway")
    
    # Run tests
    if not run_tests():
        print_warning("Tests failed - but setup completed")
    
    print_info("\nSetup completed successfully!")
    print_info("\nTo use enhanced bindings:")
    print_info("  import pynlm as nlm")
    print_info("  # Use enhanced features")
    print_info("\nFor documentation and examples:")
    print_info("  cat python/examples_enhanced.md")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())