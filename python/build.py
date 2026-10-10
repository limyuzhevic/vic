#!/usr/bin/env python3
"""
NLM Python Bindings - Build Script

This script automates the build process for the enhanced NLM Python bindings.
It handles dependency management, compilation, and installation.

Features:
- Clean project structure
- Efficient compilation
- Dependency management
- Installation management
- Cross-platform compatibility
"""

import os
import sys
import subprocess
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional

class NLMPythonBindingsBuilder:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.python_dir = self.project_root / "python"
        self.src_dir = self.project_root / "src"
        self.build_dir = self.project_root / "build"
        self.install_dir = self.project_root / "install"
        
        # Configuration
        self.config = self.load_config()
        
    def load_config(self) -> Dict:
        """Load build configuration"""
        config_path = self.project_root / "build_config.json"
        if config_path.exists():
            with open(config_path, 'r') as f:
                return json.load(f)
        
        # Default configuration
        return {
            "generator": "Unix Makefiles",
            "build_type": "Release",
            "python_version": "3.8",
            "parallel_jobs": 4,
            "install_python_package": True,
            "create_virtual_env": False,
            "test_after_build": True
        }
    
    def log(self, message: str, level: str = "INFO"):
        """Log message with timestamp"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] {level}: {message}")
    
    def run_command(self, command: str, cwd: Optional[Path] = None, check: bool = True) -> subprocess.CompletedProcess:
        """Run command with error handling"""
        self.log(f"Running: {command}")
        try:
            result = subprocess.run(
                command, shell=True, cwd=cwd, 
                capture_output=True, text=True, check=check
            )
            if result.returncode != 0:
                self.log(f"Command failed: {result.stderr}", "ERROR")
            return result
        except subprocess.CalledProcessError as e:
            self.log(f"Command failed: {e}", "ERROR")
            raise
    
    def check_dependencies(self) -> bool:
        """Check if all required dependencies are installed"""
        self.log("Checking dependencies...")
        
        required_deps = [
            ("cmake", "CMake 3.16+"),
            ("g++", "C++ compiler"),
            ("pybind11", "Pybind11 headers"),
            ("nlohmann_json", "nlohmann JSON library"),
        ]
        
        for cmd, desc in required_deps:
            result = subprocess.run(f"which {cmd}", shell=True, capture_output=True, text=True)
            if result.returncode != 0:
                self.log(f"Missing dependency: {desc}", "ERROR")
                return False
            self.log(f"Found: {desc}")
        
        return True
    
    def prepare_build_directory(self) -> None:
        """Prepare build directory"""
        self.log("Preparing build directory...")
        
        # Remove existing build directory
        if self.build_dir.exists():
            import shutil
            shutil.rmtree(self.build_dir)
        
        self.build_dir.mkdir(parents=True, exist_ok=True)
        
        # Create symbolic links for headers
        include_dir = self.build_dir / "include"
        include_dir.mkdir(exist_ok=True)
        
        # Link core headers
        for src_dir in [self.src_dir / "core", self.src_dir / "brain", self.src_dir / "agent", 
                       self.src_dir / "world", self.src_dir / "sensory", self.src_dir / "motor",
                       self.src_dir / "plasticity", self.src_dir / "development",
                       self.src_dir / "neuromodulation", self.src_dir / "memory",
                       self.src_dir / "cognition", self.src_dir / "prediction",
                       self.src_dir / "environment", self.src_dir / "experiments"]:
            if src_dir.exists():
                link_name = include_dir / src_dir.relative_to(self.src_dir)
                if not link_name.parent.exists():
                    link_name.parent.mkdir(parents=True, exist_ok=True)
                os.symlink(src_dir, link_name)
    
    def configure_cmake(self) -> None:
        """Configure CMake build system"""
        self.log("Configuring CMake build system...")
        
        # CMake options
        cmake_options = [
            f"-DCMAKE_BUILD_TYPE={self.config['build_type']}",
            f"-DCMAKE_CXX_STANDARD=20",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DNLM_SOURCE_DIR={self.src_dir}",
        ]
        
        # Additional options
        if "cmake_args" in self.config:
            cmake_options.extend(self.config["cmake_args"])
        
        cmake_command = f"cmake {' '.join(cmake_options)} .."
        self.run_command(cmake_command, cwd=self.build_dir)
    
    def build_project(self) -> None:
        """Build the project"""
        self.log("Building project...")
        
        # Build command
        build_command = f"make -j{self.config['parallel_jobs']}"
        self.run_command(build_command, cwd=self.build_dir)
    
    def build_python_bindings(self) -> None:
        """Build Python bindings"""
        self.log("Building Python bindings...")
        
        # Find the bindings target
        bindings_target = "pynlm"
        
        # Build bindings
        build_command = f"make {bindings_target}"
        self.run_command(build_command, cwd=self.build_dir)
        
        # Find the generated module
        bindings_output = self.build_dir / f"lib{bindings_target}.so"
        if not bindings_output.exists():
            bindings_output = self.build_dir / f"{bindings_target}.so"
        
        if not bindings_output.exists():
            # Look for .pyd extension on Windows
            bindings_output = self.build_dir / f"{bindings_target}.pyd"
        
        if not bindings_output.exists():
            self.log(f"Bindings output not found: {bindings_output}", "ERROR")
            return
        
        self.log(f"Bindings built: {bindings_output}")
    
    def install_python_package(self) -> None:
        """Install Python package"""
        if not self.config.get("install_python_package", True):
            return
        
        self.log("Installing Python package...")
        
        # Determine installation location
        if sys.platform.startswith("win"):
            install_path = self.project_root / "python_package"
            os.environ["PYTHONPATH"] = str(install_path)
        else:
            # Try to find site-packages location
            result = subprocess.run(
                [sys.executable, "-c", "import site; print(site.getsitepackages()[0])"],
                capture_output=True, text=True
            )
            if result.returncode == 0:
                install_path = Path(result.stdout.strip())
            else:
                install_path = Path(os.environ.get("PYTHONPATH", ""))
                if not install_path.exists():
                    install_path = self.project_root / "python_package"
        
        # Copy bindings to Python package
        python_package_dir = self.project_root / "python_package"
        python_package_dir.mkdir(exist_ok=True)
        
        # Copy bindings module
        import shutil
        shutil.copy2(self.build_dir / "libpynlm.so", python_package_dir / "pynlm.so")
        
        # Copy Python files
        for py_file in self.python_dir.glob("*.py"):
            if py_file.name != "__pycache__":
                shutil.copy2(py_file, python_package_dir / py_file.name)
        
        self.log(f"Python package installed to: {python_package_dir}")
    
    def run_tests(self) -> None:
        """Run test suite"""
        if not self.config.get("test_after_build", True):
            return
        
        self.log("Running tests...")
        
        # Run comprehensive test suite
        test_file = self.python_dir / "test_pynlm_comprehensive.py"
        if test_file.exists():
            try:
                # Run tests with the built bindings
                env = os.environ.copy()
                env["PYTHONPATH"] = str(self.build_dir) + ":" + env.get("PYTHONPATH", "")
                
                result = subprocess.run(
                    [sys.executable, str(test_file)],
                    cwd=self.project_root,
                    env=env,
                    capture_output=True,
                    text=True
                )
                
                if result.returncode == 0:
                    self.log("All tests passed!")
                else:
                    self.log(f"Tests failed:\n{result.stdout}\n{result.stderr}", "ERROR")
                    
            except Exception as e:
                self.log(f"Error running tests: {e}", "ERROR")
        else:
            self.log("Test file not found: test_pynlm_comprehensive.py", "WARNING")
    
    def create_readme(self) -> None:
        """Create README for the built package"""
        readme_path = self.project_root / "README_ENHANCED.md"
        
        content = f"""# Enhanced NLM Python Bindings

## Overview

This is the enhanced NLM (Neural Learning Machine) Python bindings package.
It provides a comprehensive interface to the NLM C++ neural simulation framework.

## Features

- Complete Brain simulation API
- Advanced Configuration system with validation
- World environment with 2D physics and objects
- Agent brain interface with neuromodulation
- Type-safe operations with automatic conversion
- Comprehensive error handling
- Convenience factory functions
- Performance optimization utilities
- Memory management tools
- Batch operations capabilities

## Quick Start

### Installation

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The Python bindings will be installed to the Python site-packages
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
stats = brain.getStatistics()
print(f"Spike count: {{stats['total_spikes']}}")
```

### Advanced Usage

See `examples.py` for practical examples of:
- Configuration management
- World simulation
- Learning episodes
- Performance monitoring
- Error handling

### Documentation

- **Complete API Reference**: `ENHANCED_PYTHON_BINDINGS.md`
- **Usage Examples**: `examples.py`
- **Test Coverage**: `test_pynlm_comprehensive.py`
- **Enhancement Summary**: `python/SUMMARY.md`

## File Structure

python/
├── __init__.py                    # Python module entry point
├── bindings_final_consolidated.cpp # Core C++ bindings implementation
├── test_pynlm_comprehensive.py    # Complete test suite
├── examples.py                   # Usage examples
└── ENHANCED_PYTHON_BINDINGS.md   # Complete documentation

## Build Configuration

The build is controlled by `build_config.json` in the project root.
Key configuration options:

- `generator`: CMake generator (Unix Makefiles, Ninja, etc.)
- `build_type`: Build type (Debug, Release, RelWithDebInfo)
- `python_version`: Required Python version
- `parallel_jobs`: Number of parallel build jobs
- `install_python_package`: Whether to install Python package
- `create_virtual_env`: Whether to create virtual environment
- `test_after_build`: Whether to run tests after build

## Advanced Features

### Configuration Management

```python
# Advanced configuration with validation
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 4)
config.set("connection_probability", 0.08)

# Save and reload configuration
config.saveToFile("config.json")
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("config.json")
```

### World Simulation

```python
# Create world with objects
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Add walls and resources
for x in range(0, 100, 20):
    wall = pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall)
    world.addObject(wall)
```

### Agent with Neuromodulation

```python
# Create agent brain with neuromodulation
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Enable advanced features
agent_brain.enableRewardModulation(True)
agent_brain.enableCuriosity(True)
agent_brain.enableDevelopment(True)
```

### Batch Operations

```python
# Create multiple brains
brains = pynlm.BatchOperations.createBrainBatch(10, config)

# Run batch simulation
results = pynlm.BatchOperations.batchSimulate(brains, [100, 200, 150])
```

### Performance Monitoring

```python
# Benchmark operations
avg_time, ops_per_sec = pynlm.benchmark_operation(
    "simulation", 
    lambda: brain.step(0), 
    iterations=100
)
```

## Error Handling

The enhanced bindings provide specific exception classes:

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("valid_key", "valid_value")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {{e}}")
except pynlm.BrainError as e:
    print(f"Brain error: {{e}}")
except pynlm.WorldError as e:
    print(f"World error: {{e}}")
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure the Python bindings are built and installed
   - Check if PYTHONPATH includes the build directory

2. **RuntimeError: Could not load Python bindings**
   - Ensure the pynlm module is built correctly
   - Check for missing dependencies

3. **TypeError: invalid argument types**
   - Check function signatures and argument types
   - Ensure correct parameter order

### Getting Help

- **Documentation**: See `ENHANCED_PYTHON_BINDINGS.md` for detailed information
- **Examples**: Look in `examples.py` for complete usage examples
- **Issues**: Report issues on the NLM GitHub repository
- **Community**: Join the NLM discussion forum

## License

This package is provided under the MIT License.

## Authors

Enhanced NLM Python Bindings Project Team

## Version

Final consolidated version with complete API coverage and advanced features.
"""
        
        with open(readme_path, 'w') as f:
            f.write(content)
        
        self.log(f"README created: {readme_path}")
    
    def build(self) -> None:
        """Main build method"""
        self.log("Starting NLM Python Bindings build...")
        
        try:
            # Check dependencies
            if not self.check_dependencies():
                sys.exit(1)
            
            # Prepare build directory
            self.prepare_build_directory()
            
            # Configure CMake
            self.configure_cmake()
            
            # Build project
            self.build_project()
            
            # Build Python bindings
            self.build_python_bindings()
            
            # Install Python package
            self.install_python_package()
            
            # Run tests
            self.run_tests()
            
            # Create documentation
            self.create_readme()
            
            self.log("Build completed successfully!")
            
        except Exception as e:
            self.log(f"Build failed: {e}", "ERROR")
            sys.exit(1)

if __name__ == "__main__":
    import datetime
    
    parser = argparse.ArgumentParser(description="Build NLM Python Bindings")
    parser.add_argument("--project-root", default=".", help="Project root directory")
    parser.add_argument("--config", help="Build configuration file")
    
    args = parser.parse_args()
    
    builder = NLMPythonBindingsBuilder(args.project_root)
    if args.config:
        builder.config.update(json.load(open(args.config)))
    
    builder.build()
