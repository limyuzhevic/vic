#!/usr/bin/env python3
"""
NLM Build and Test Automation Script

This script provides advanced build, test, and development utilities for the NLM project.
"""

import os
import sys
import subprocess
import argparse
import json
import time
from pathlib import Path

# Configuration
CONFIG_FILE = "build_config.json"
BUILD_DIR = "build"

class NLMBuilder:
    """Main builder class for NLM project."""
    
    def __init__(self):
        self.config = self.load_config()
        self.project_root = Path(__file__).parent
        
    def load_config(self):
        """Load or create build configuration."""
        config_path = self.project_root / CONFIG_FILE
        if config_path.exists():
            with open(config_path, 'r') as f:
                return json.load(f)
        else:
            # Create default config
            config = {
                "build_type": "Release",
                "parallel_jobs": 4,
                "enable_tests": True,
                "enable_benchmarks": True,
                "python_bindings": True,
                "optimization_level": "O3",
                "verbose": False,
                "install_prefix": "/usr/local"
            }
            with open(config_path, 'w') as f:
                json.dump(config, f, indent=2)
            return config
    
    def update_config(self, **kwargs):
        """Update configuration values."""
        for key, value in kwargs.items():
            if key in self.config:
                self.config[key] = value
        # Save updated config
        with open(self.project_root / CONFIG_FILE, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    def run_cmd(self, cmd, cwd=None, check=True):
        """Run shell command with proper error handling."""
        cwd = cwd or self.project_root
        if self.config.get("verbose", False):
            print(f"Running: {cmd}")
        
        try:
            result = subprocess.run(cmd, shell=True, cwd=cwd, 
                                  capture_output=True, text=True, check=check)
            if result.stdout:
                print(result.stdout)
            return result
        except subprocess.CalledProcessError as e:
            print(f"Command failed: {cmd}")
            print(f"Error: {e.stderr}")
            raise
    
    def build_cmake(self, clean=False):
        """Build the C++ project using CMake."""
        print("=== Building NLM C++ Project ===")
        
        build_path = self.project_root / BUILD_DIR
        if clean and build_path.exists():
            print("Cleaning build directory...")
            import shutil
            shutil.rmtree(build_path)
        
        build_path.mkdir(exist_ok=True)
        
        cmake_cmd = f"cmake .. -DCMAKE_BUILD_TYPE={self.config['build_type']} \
                    -DCMAKE_CXX_FLAGS='-{self.config['optimization_level']}'"
        
        self.run_cmd(cmake_cmd, cwd=build_path)
        
        make_cmd = f"make -j{self.config['parallel_jobs']}"
        self.run_cmd(make_cmd, cwd=build_path)
        
        print("✓ Build completed successfully")
    
    def build_python_bindings(self):
        """Build Python bindings using scikit-build."""
        print("=== Building Python Bindings ===")
        
        # First, ensure C++ core is built
        if not (self.project_root / BUILD_DIR / "libnlm_core.a").exists():
            print("Building C++ core first...")
            self.build_cmake()
        
        # Build Python package
        pyproject_path = self.project_root / "pyproject.toml"
        if not pyproject_path.exists():
            print("ERROR: pyproject.toml not found")
            return False
        
        # Install the package in development mode
        cmd = f"pip install -e . --no-build-isolation"
        self.run_cmd(cmd, cwd=self.project_root)
        
        print("✓ Python bindings built successfully")
        return True
    
    def run_tests(self, test_filter=""):
        """Run the test suite."""
        print("=== Running NLM Tests ===")
        
        build_path = self.project_root / BUILD_DIR
        if not build_path.exists():
            print("ERROR: Build directory not found. Run build first.")
            return False
        
        # Run C++ tests
        test_executable = build_path / "nlm"
        if test_executable.exists():
            print("Running C++ tests...")
            cmd = f"{test_executable} 2>&1"
            result = self.run_cmd(cmd, cwd=build_path, check=False)
            
            if "All Tests PASSED" in result.stdout:
                print("✓ C++ tests passed")
                return True
            else:
                print("✗ C++ tests failed")
                return False
        
        # Try running Python tests
        print("Checking for Python tests...")
        test_files = list(self.project_root.rglob("test_*.py"))
        if test_files:
            for test_file in test_files:
                print(f"Running Python test: {test_file}")
                cmd = f"python {test_file}"
                self.run_cmd(cmd, cwd=self.project_root, check=False)
        
        return True
    
    def run_phase6_demo(self):
        """Run the Phase 6 integration demo."""
        print("=== Running Phase 6 Integration Demo ===")
        
        build_path = self.project_root / BUILD_DIR
        demo_executable = build_path / "nlm_phase6_demo"
        
        if demo_executable.exists():
            print("Running Phase 6 demo...")
            cmd = f"{demo_executable} 2>&1"
            result = self.run_cmd(cmd, cwd=build_path, check=False)
            print(result.stdout)
            return True
        else:
            print("ERROR: Phase 6 demo executable not found")
            print("Trying to build demo first...")
            self.build_cmake()
            return self.run_phase6_demo()
    
    def create_advanced_config(self):
        """Create an advanced configuration file for power users."""
        print("=== Creating Advanced Configuration ===")
        
        advanced_config = {
            "build_type": "RelWithDebInfo",
            "parallel_jobs": os.cpu_count() or 4,
            "enable_tests": True,
            "enable_benchmarks": True,
            "python_bindings": True,
            "optimization_level": "O3",
            "verbose": False,
            "install_prefix": "/usr/local",
            "advanced_features": {
                "enable_debug_logging": True,
                "enable_performance_profiling": False,
                "enable_checkpointing": True,
                "enable_replay": True,
                "enable_structural_plasticity": True,
                "enable_curiosity": True,
                "enable_prediction_error": True
            },
            "neural_network_config": {
                "neuron_count": 1000,
                "region_count": 5,
                "connection_probability": 0.15,
                "initial_plasticity": 1.0,
                "plasticity_decay_rate": 0.001
            },
            "sensory_config": {
                "vision_width": 32,
                "vision_height": 32,
                "audio_sample_rate": 16000,
                "internal_signals_count": 8
            },
            "motor_config": {
                "action_space_size": 15,
                "exploration_rate": 0.1,
                "novelty_threshold": 0.5
            }
        }
        
        config_path = self.project_root / "advanced_config.json"
        with open(config_path, 'w') as f:
            json.dump(advanced_config, f, indent=2)
        
        print(f"✓ Advanced configuration created at {config_path}")
        return advanced_config
    
    def build_with_advanced_config(self, config_file="advanced_config.json"):
        """Build with advanced configuration."""
        print("=== Building with Advanced Configuration ===")
        
        config_path = self.project_root / config_file
        if not config_path.exists():
            print(f"ERROR: Config file {config_file} not found")
            return False
        
        with open(config_path, 'r') as f:
            advanced_config = json.load(f)
        
        # Update build configuration
        self.update_config(
            build_type=advanced_config.get("build_type", "Release"),
            parallel_jobs=advanced_config.get("parallel_jobs", 4),
            optimization_level=advanced_config.get("optimization_level", "O3")
        )
        
        # Build
        self.build_cmake()
        
        # Build Python bindings
        self.build_python_bindings()
        
        print("✓ Advanced build completed successfully")
        return True

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="NLM Build and Test Automation")
    parser.add_argument("command", choices=["build", "test", "demo", "config", "advanced-build"],
                       help="Command to execute")
    parser.add_argument("--clean", action="store_true", help="Clean build directory")
    parser.add_argument("--filter", help="Filter for specific tests")
    
    args = parser.parse_args()
    
    builder = NLMBuilder()
    
    try:
        if args.command == "build":
            builder.build_cmake(clean=args.clean)
        elif args.command == "test":
            builder.run_tests(args.filter)
        elif args.command == "demo":
            builder.run_phase6_demo()
        elif args.command == "config":
            builder.create_advanced_config()
        elif args.command == "advanced-build":
            builder.build_with_advanced_config()
            
        print("\n✓ Command completed successfully")
        return 0
        
    except Exception as e:
        print(f"\n✗ Command failed: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())