#!/usr/bin/env python3

"""
NLM Python Bindings Build and Installation Script

This script provides a unified interface for building and installing the NLM
Python bindings. It handles both CMake-based C++ compilation and Python package installation.

Usage:
    python build.py [--build-dir BUILD_DIR] [--install] [--help]
"""

import os
import sys
import subprocess
import argparse
import platform
import shutil
from pathlib import Path

class NLMBuilder:
    def __init__(self, args):
        self.args = args
        self.project_root = Path(__file__).parent.parent.parent
        self.build_dir = Path(args.build_dir) if args.build_dir else self.project_root / "build"
        self.python_dir = self.project_root / "python"
        
        # Platform detection
        self.is_windows = platform.system() == "Windows"
        self.is_linux = platform.system() == "Linux"
        self.is_macos = platform.system() == "Darwin"
        
        # Python information
        self.python_version = sys.version_info
        self.python_executable = sys.executable
        
    def log(self, message, level="INFO"):
        """Log a message with timestamp."""
        timestamp = "[{}]" .format(subprocess.check_output(['date'], text=True).strip()) if not self.is_windows else "[?]"
        print(f"{timestamp} {level}: {message}")
    
    def run_command(self, cmd, cwd=None, check=True):
        """Run a command and return the result."""
        self.log(f"Running: {' '.join(cmd)}")
        
        try:
            if self.is_windows:
                # On Windows, use subprocess with shell=True
                result = subprocess.run(cmd, cwd=cwd, shell=True, check=check, 
                                      capture_output=True, text=True)
            else:
                # On Unix-like systems, use subprocess without shell=True
                result = subprocess.run(cmd, cwd=cwd, check=check, 
                                      capture_output=True, text=True)
            
            if result.stdout:
                self.log(f"Output: {result.stdout.strip()}")
            
            return result
        except subprocess.CalledProcessError as e:
            self.log(f"Command failed: {e}", "ERROR")
            if e.stdout:
                self.log(f"stdout: {e.stdout}")
            if e.stderr:
                self.log(f"stderr: {e.stderr}")
            if check:
                raise
            return e
    
    def check_prerequisites(self):
        """Check if all prerequisites are installed."""
        self.log("Checking prerequisites...")
        
        # Check Python
        self.log(f"Python {self.python_version.major}.{self.python_version.minor}.{self.python_version.micro}")
        
        # Check CMake
        try:
            self.run_command(['cmake', '--version'], check=False)
            self.log("CMake is available")
        except:
            self.log("CMake is not available", "ERROR")
            return False
        
        # Check compiler
        if self.is_windows:
            # Check for MSVC
            try:
                self.run_command(['cl'], check=False)
                self.log("MSVC compiler is available")
            except:
                self.log("MSVC compiler is not available", "ERROR")
                return False
        else:
            # Check for gcc/clang
            try:
                self.run_command(['g++', '--version'], check=False)
                self.log("g++ compiler is available")
            except:
                try:
                    self.run_command(['clang++', '--version'], check=False)
                    self.log("clang++ compiler is available")
                except:
                    self.log("No C++ compiler found", "ERROR")
                    return False
        
        # Check pybind11
        pybind11_dir = self.python_dir / "pybind11"
        if pybind11_dir.exists():
            self.log("pybind11 found in python/ directory")
        else:
            try:
                import pybind11
                self.log(f"pybind11 found via pip: {pybind11.__version__}")
            except ImportError:
                self.log("pybind11 is not installed", "WARNING")
                self.log("Installing pybind11...")
                self.run_command([self.python_executable, '-m', 'pip', 'install', 'pybind11>=2.11.0'])
        
        # Check scikit-build-core
        try:
            import scikit_build_core
            self.log(f"scikit-build-core found: {scikit_build_core.__version__}")
        except ImportError:
            self.log("scikit-build-core is not installed", "WARNING")
            self.log("Installing scikit-build-core...")
            self.run_command([self.python_executable, '-m', 'pip', 'install', 'scikit-build-core>=0.5.0'])
        
        return True
    
    def build_cmake_project(self):
        """Build the C++ project using CMake."""
        self.log(f"Building CMake project in {self.build_dir}")
        
        # Create build directory
        self.build_dir.mkdir(parents=True, exist_ok=True)
        
        # Configure CMake
        cmake_args = [
            'cmake',
            '..',
            '-DCMAKE_BUILD_TYPE=Release',
            '-DPYTHON_EXECUTABLE=' + self.python_executable,
        ]
        
        # Add platform-specific options
        if self.is_windows:
            cmake_args.append('-G "Visual Studio 17 2022"')
            cmake_args.append('-A x64')
        elif self.is_macos:
            cmake_args.append('-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64')
        
        self.run_command(cmake_args, cwd=self.build_dir)
        
        # Build the project
        self.log("Building project...")
        
        if self.is_windows:
            # Use MSBuild on Windows
            self.run_command(['cmake', '--build', '.', '--config', 'Release', '--', '/m'])
        else:
            # Use make on Unix-like systems
            self.run_command(['make', '-j4'], cwd=self.build_dir)
        
        # Build Python bindings
        self.log("Building Python bindings...")
        
        python_build_dir = self.python_dir / "build"
        python_build_dir.mkdir(parents=True, exist_ok=True)
        
        pybind11_args = [
            'cmake',
            '..',
            '-DCMAKE_BUILD_TYPE=Release',
            '-DPYTHON_EXECUTABLE=' + self.python_executable,
            '-Dpybind11_DIR=' + str(self.python_dir / 'pybind11'),
        ]
        
        self.run_command(pybind11_args, cwd=python_build_dir)
        
        if self.is_windows:
            self.run_command(['cmake', '--build', '.', '--config', 'Release', '--', '/m'])
        else:
            self.run_command(['make', '-j4'], cwd=python_build_dir)
    
    def install_python_package(self):
        """Install the Python package."""
        self.log("Installing Python package...")
        
        # Install using pip in development mode
        install_cmd = [self.python_executable, '-m', 'pip', 'install', '-e', '.']
        
        try:
            self.run_command(install_cmd, cwd=self.python_dir)
        except Exception as e:
            self.log(f"pip install failed: {e}", "ERROR")
            return False
        
        # Verify installation
        return self.verify_installation()
    
    def verify_installation(self):
        """Verify that the installation was successful."""
        self.log("Verifying installation...")
        
        try:
            # Try to import pynlm
            import pynlm
            self.log(f"Successfully imported pynlm")
            
            # Check version
            self.log(f"NLM version: {pynlm.__version__}")
            
            # Check if key components are available
            config = pynlm.createDefaultConfig()
            self.log("Successfully created default configuration")
            
            brain = pynlm.createBrain(config)
            self.log("Successfully created brain")
            
            return True
            
        except ImportError as e:
            self.log(f"Failed to import pynlm: {e}", "ERROR")
            return False
        except Exception as e:
            self.log(f"Installation verification failed: {e}", "ERROR")
            return False
    
    def create_test_examples(self):
        """Create example scripts for testing."""
        self.log("Creating test examples...")
        
        examples_dir = self.project_root / "examples"
        examples_dir.mkdir(exist_ok=True)
        
        # Minimal example
        minimal_example = examples_dir / "minimal.py"
        minimal_example.write_text('''
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Minimal example complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
''')
        
        # Agent example
        agent_example = examples_dir / "agent.py"
        agent_example.write_text('''
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

print("Agent example complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
''')
        
        # Run NLM phase 6 demo
        demo_script = examples_dir / "run_nlm_demo.py"
        demo_script.write_text('''
import subprocess
import sys
import os

# Check if NLM executable exists
project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
build_dir = os.path.join(project_root, "build")
nlm_executable = os.path.join(build_dir, "nlm")

if not os.path.exists(nlm_executable):
    print("NLM executable not found. Building...")
    os.chdir(project_root)
    subprocess.run(["mkdir", "-p", "build"])
    subprocess.run(["cd", "build", "&&", "cmake", ".."], shell=True)
    subprocess.run(["cd", "build", "&&", "make", "-j4"], shell=True)
    
    nlm_executable = os.path.join(build_dir, "nlm")

print(f"Running NLM executable: {nlm_executable}")
subprocess.run([nlm_executable] + sys.argv[1:])
''')
        
        self.log(f"Created examples in {examples_dir}")
    
    def run_tests(self):
        """Run the test suite."""
        self.log("Running tests...")
        
        # Run C++ tests if available
        ctest_executable = self.build_dir / "ctest"
        if ctest_executable.exists():
            self.log("Running C++ tests...")
            self.run_command([str(ctest_executable), "--output-on-failure"])
        else:
            self.log("C++ tests not available")
        
        # Run Python tests if available
        python_tests_dir = self.python_dir / "tests"
        if python_tests_dir.exists():
            self.log("Running Python tests...")
            test_cmd = [self.python_executable, '-m', 'pytest', str(python_tests_dir)]
            self.run_command(test_cmd)
        else:
            self.log("Python tests not available")
    
    def create_documentation(self):
        """Create documentation for the build."""
        self.log("Creating documentation...")
        
        doc_dir = self.project_root / "docs"
        doc_dir.mkdir(exist_ok=True)
        
        # Build instructions
        build_instructions = doc_dir / "BUILD.md"
        build_instructions.write_text(f'''# NLM Build Instructions

## Quick Start

```bash
cd {self.project_root}
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# For Python bindings specifically
cd {self.python_dir}
mkdir build
cd build
cmake ..
make -j4
pip install -e .
```

## Project Structure

- `{self.project_root}/src/` - C++ source code
- `{self.project_root}/python/` - Python bindings
- `{self.project_root}/build/` - Build artifacts
- `{self.project_root}/examples/` - Example scripts

## Python API

After installation, you can use NLM in Python:

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {{brain.getTotalNeuronCount()}}")
print(f"Total spikes: {{brain.getTotalSpikeCount()}}")
```

## Testing

Run the test suite:

```bash
cd {self.build_dir}
ctest --output-on-failure
```

For Python tests:

```bash
pip install -e .
python -m pytest python/tests/
```

## Examples

Run the example scripts:

```bash
cd {self.project_root}
python examples/minimal.py
python examples/agent.py
python examples/run_nlm_demo.py
```
''')
        
        self.log(f"Created documentation in {doc_dir}")
    
    def build(self):
        """Main build method."""
        self.log("Starting NLM build process...")
        
        try:
            # Check prerequisites
            if not self.check_prerequisites():
                return False
            
            # Build CMake project
            self.build_cmake_project()
            
            # Install Python package
            if self.args.install:
                if not self.install_python_package():
                    return False
            
            # Create test examples
            self.create_test_examples()
            
            # Create documentation
            self.create_documentation()
            
            # Run tests if requested
            if self.args.tests:
                self.run_tests()
            
            self.log("Build completed successfully!")
            return True
            
        except Exception as e:
            self.log(f"Build failed: {e}", "ERROR")
            return False

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Build and Installation Script"
    )
    
    parser.add_argument(
        '--build-dir',
        default='build',
        help='Build directory path (default: build)'
    )
    
    parser.add_argument(
        '--install',
        action='store_true',
        help='Install the Python package after building'
    )
    
    parser.add_argument(
        '--tests',
        action='store_true',
        help='Run tests after building'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    
    builder = NLMBuilder(args)
    
    if not builder.build():
        sys.exit(1)

if __name__ == '__main__':
    main()