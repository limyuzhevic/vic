#!/usr/bin/env python3
"""
NLM Python Bindings Setup and Installation Script

This script provides setup and configuration commands for the NLM Python bindings,
including installation, testing, and development workflows.
"""

import argparse
import subprocess
import sys
import os
from pathlib import Path

def print_banner():
    """Print the NLM Python bindings banner"""
    print("=" * 70)
    print("NLM Python Bindings Setup and Configuration")
    print("=" * 70)
    print("Phase 6 Enhanced Neural Network Python Interface")
    print("=" * 70)

def install_dependencies():
    """Install NLM Python dependencies"""
    print("\n[INSTALLING DEPENDENCIES]")
    print("-" * 40)
    
    try:
        # Install core dependencies
        print("Installing core Python packages...")
        subprocess.run([sys.executable, "-m", "pip", "install", ">=1.20", "numpy"], check=True)
        subprocess.run([sys.executable, "-m", "pip", "install", ">=1.8.0", "scipy"], check=True)
        
        print("✅ Core dependencies installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to install dependencies: {e}")
        return False

def build_native():
    """Build the native NLM library"""
    print("\n[BUILDING NATIVE LIBRARY]")
    print("-" * 40)
    
    build_dir = Path("build")
    
    try:
        print("Creating build directory...")
        build_dir.mkdir(exist_ok=True)
        
        print("Running CMake configuration...")
        subprocess.run([
            "cmake", "..", 
            "-DCMAKE_BUILD_TYPE=Release",
            "-DNLM_BUILD_PYTHON_BINDINGS=ON",
            "-DPYTHON_EXECUTABLE=" + sys.executable
        ], cwd=build_dir, check=True)
        
        print("Building with parallel compilation...")
        subprocess.run(["make", "-j4"], cwd=build_dir, check=True)
        
        print("✅ Native library built successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to build native library: {e}")
        return False

def install_python_package():
    """Install the Python package"""
    print("\n[INSTALLING PYTHON PACKAGE]")
    print("-" * 40)
    
    try:
        print("Installing pynlm package...")
        subprocess.run([sys.executable, "-m", "pip", "install", "-e", "."], check=True)
        
        print("✅ Python package installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to install Python package: {e}")
        return False

def run_tests():
    """Run NLM tests"""
    print("\n[RUNNING TESTS]")
    print("-" * 40)
    
    try:
        print("Running pytest...")
        subprocess.run([sys.executable, "-m", "pytest", "-v", "--tb=short"], check=True)
        
        print("✅ Tests completed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Tests failed: {e}")
        return False

def configure_environment():
    """Configure development environment"""
    print("\n[CONFIGURING ENVIRONMENT]")
    print("-" * 40)
    
    try:
        # Create virtual environment if not exists
        venv_path = Path("venv")
        if not venv_path.exists():
            print("Creating virtual environment...")
            subprocess.run([sys.executable, "-m", "venv", "venv"], check=True)
            
            # Install dependencies in virtual environment
            pip_path = venv_path / "bin" / "pip"
            if sys.platform == "win32":
                pip_path = venv_path / "Scripts" / "pip.exe"
            
            subprocess.run([str(pip_path), "install", ">=1.20", "numpy", ">=1.8.0", "scipy"], check=True)
            
            print("✅ Virtual environment configured")
        
        # Create .env file for configuration
        env_content = """# NLM Environment Configuration
# Set these variables to customize NLM behavior

# Neural network parameters
NLM_NEURON_COUNT=1000
NLM_SIMULATION_STEPS=10000

# Reward and neuromodulation
NLM_REWARD_ENABLED=true
NLM_DOPAMINE_ENABLED=true
NLM_CURIOUSITY_ENABLED=true

# Activity control
NLM_ACTIVITY_MONITORING=true
NLM_MAX_FIRING_RATE=100.0
NLM_TARGET_FIRING_RATE=10.0

# Memory systems
NLM_WORKING_MEMORY_ENABLED=true
NLM_EPISODIC_MEMORY_ENABLED=true

# Development
NLM_DEVELOPMENT_ENABLED=true
NLM_DEVELOPMENT_STAGES=6

# Performance
NLM_CHECKPOINT_INTERVAL=5000
NLM_LOG_LEVEL=INFO
"""
        
        with open(".env", "w") as f:
            f.write(env_content)
        
        print("✅ Environment configured")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to configure environment: {e}")
        return False

def demonstrate_features():
    """Demonstrate available features"""
    print("\n[FEATURE DEMONSTRATION]")
    print("-" * 40)
    
    print("\nAvailable NLM Python Bindings Features:")
    print("=" * 50)
    
    features = [
        ("Neural Network Access", "Create and manipulate neural networks"),
        ("Simulation Control", "Start/stop neural simulations"),
        ("Memory Inspection", "Examine working and episodic memory"),
        ("Neuromodulation", "Access dopamine, curiosity, and reward systems"),
        ("Activity Monitoring", "Monitor neural activity levels"),
        ("Configuration Management", "Runtime configuration updates"),
        ("Performance Metrics", "Access performance statistics"),
        ("Checkpoint Management", "Save and load brain states"),
    ]
    
    for feature, description in features:
        print(f"  {feature:25} - {description}")
    
    print("\nUsage Examples:")
    print("-" * 20)
    print("  from pynlm import NLM")
    print("  nlm = NLM(neuron_count=1000)")
    print("  nlm.simulate(steps=1000)")
    print("  print(nlm.get_activity_summary())")
    
    print("\n✅ Features demonstrated")
    return True

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="NLM Python Bindings Setup")
    parser.add_argument(
        "--action", 
        choices=["install", "build", "test", "demo", "all"],
        default="all",
        help="Action to perform"
    )
    parser.add_argument(
        "--skip-deps",
        action="store_true",
        help="Skip dependency installation"
    )
    parser.add_argument(
        "--skip-native",
        action="store_true",
        help="Skip native library building"
    )
    parser.add_argument(
        "--skip-tests",
        action="store_true",
        help="Skip testing"
    )
    
    args = parser.parse_args()
    
    print_banner()
    
    success = True
    
    if args.action in ["install", "all"] and not args.skip_deps:
        success &= install_dependencies()
    
    if args.action in ["build", "all"] and not args.skip_native:
        success &= build_native()
    
    if args.action in ["test", "all"] and not args.skip_tests:
        success &= run_tests()
    
    if args.action in ["demo", "all"]:
        success &= demonstrate_features()
    
    if success:
        print("\n" + "=" * 70)
        print("✅ SETUP COMPLETED SUCCESSFULLY")
        print("=" * 70)
        print("\nNext Steps:")
        print("1. Run: python demo_enhanced.py")
        print("2. Run: python setup.py build")
        print("3. Run: python -m pytest")
        print("4. Start developing your NLM applications!")
    else:
        print("\n" + "=" * 70)
        print("❌ SETUP FAILED")
        print("=" * 70)
        print("Please check the error messages above and try again.")
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())