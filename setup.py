#!/usr/bin/env python3
"""
NLM Python Bindings Setup Script

This script sets up the Python bindings for NLM and provides utilities for development.
"""

import os
import sys
import json
import argparse
import subprocess
from pathlib import Path
from typing import Dict, List, Optional

class NLMBindingsSetup:
    """Setup script for NLM Python bindings."""
    
    def __init__(self, repo_root: str = "."):
        """Initialize NLM bindings setup.
        
        Args:
            repo_root: Root directory of the NLM repository
        """
        self.repo_root = Path(repo_root).resolve()
        self.pyproject_path = self.repo_root / "pyproject.toml"
        self.pynlm_path = self.repo_root / "pynlm"
        self.src_path = self.repo_root / "src"
        
        # Configuration
        self.config = {
            'enable_python_bindings': True,
            'enable_advanced_api': True,
            'enable_performance_monitoring': True,
            'enable_debugging_tools': True,
            'optimization_level': 3,
            'logging_level': 'INFO',
        }
    
    def check_environment(self) -> bool:
        """Check if the environment is properly set up."""
        print("=== Checking Environment ===")
        
        checks = []
        
        # Check Python version
        python_version = sys.version_info
        checks.append(("Python version", f"{python_version.major}.{python_version.minor}.{python_version.micro}"))
        
        # Check if we have the required Python files
        pyproject_exists = self.pyproject_path.exists()
        checks.append(("pyproject.toml", "✓" if pyproject_exists else "✗"))
        
        # Check if NLM source exists
        nlm_source_exists = self.src_path.exists()
        checks.append(("NLM source directory", "✓" if nlm_source_exists else "✗"))
        
        # Check if Python bindings exist
        pybindings_exist = self.pynlm_path.exists()
        checks.append(("Python bindings", "✓" if pybindings_exist else "✗"))
        
        # Check CMake
        try:
            result = subprocess.run(['cmake', '--version'], capture_output=True, text=True)
            cmake_version = result.stdout.strip().split('\n')[0]
            checks.append(("CMake version", cmake_version))
        except:
            checks.append(("CMake", "✗"))
        
        # Check build system
        build_exists = (self.repo_root / "build").exists()
        checks.append(("Build directory", "✓" if build_exists else "⚠"))
        
        # Check Python dependencies
        try:
            import numpy
            checks.append(("NumPy", f"{numpy.__version__}"))
        except ImportError:
            checks.append(("NumPy", "✗"))
        
        try:
            import pybind11
            checks.append(("pybind11", "✓"))
        except ImportError:
            checks.append(("pybind11", "✗"))
        
        # Print results
        for check_name, result in checks:
            print(f"  {check_name}: {result}")
        
        # Determine if environment is ready
        ready = (pyproject_exists and nlm_source_exists and 
                 pybindings_exist and cmake_version.startswith("cmake version"))
        
        print(f"\nEnvironment {'✓' if ready else '✗'} ready for NLM Python bindings")
        return ready
    
    def create_configuration_files(self):
        """Create additional configuration files for development."""
        print("\n=== Creating Configuration Files ===")
        
        # Create .gitignore
        gitignore_content = """# NLM Python Bindings
__pycache__/
*.pyc
*.pyo
*.pyd
.Python
env/
venv/
ENV/
env.bak/
venv.bak/

# Build artifacts
build/
dist/
*.egg-info/
*.so
*.pyd

# Configuration files
config.json
*.config

# Log files
*.log
logs/

# Temporary files
*.tmp
*.temp

# IDE files
.vscode/
.idea/
*.swp
*.swo

# Operating System
.DS_Store
Thumbs.db
"""
        
        with open(self.repo_root / ".gitignore", "w") as f:
            f.write(gitignore_content)
        print("  ✓ Created .gitignore")
        
        # Create development config
        dev_config = {
            "project": "nlm",
            "version": "0.1.0",
            "description": "Python bindings for NLM",
            "dependencies": [
                "numpy>=1.20",
                "pybind11>=2.11.0",
                "scikit-build-core>=0.5.0",
                "pytest>=7.0",
                "matplotlib>=3.0",
                "jupyter>=1.0",
            ],
            "optional_dependencies": {
                "dev": [
                    "pytest>=7.0",
                    "numpy>=1.20",
                    "matplotlib>=3.0",
                    "jupyter>=1.0",
                    "sphinx>=4.0",
                    "sphinx-rtd-theme>=1.0",
                ],
                "examples": [
                    "jupyter>=1.0",
                    "matplotlib>=3.0",
                    "plotly>=4.0",
                ],
            },
        }
        
        dev_config_path = self.repo_root / "dev_config.json"
        with open(dev_config_path, "w") as f:
            json.dump(dev_config, f, indent=2)
        print(f"  ✓ Created {dev_config_path.name}")
        
        # Create examples directory structure
        examples_dir = self.repo_root / "examples"
        examples_dir.mkdir(exist_ok=True)
        
        # Basic example
        basic_example = examples_dir / "basic_example.py"
        basic_example.write_text("""
#!/usr/bin/env python3
""""
import pynlm

def main():
    print("=== NLM Basic Example ===")
    
    # Create configuration
    config = pynlm.create_default_config()
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            print(f"Step {i}: {brain.get_firing_neuron_count()} neurons firing")
    
    print("Basic example completed!")

if __name__ == "__main__":
    main()
""")
        print(f"  ✓ Created {basic_example.name}")
        
        # Advanced example
        advanced_example = examples_dir / "advanced_example.py"
        advanced_example.write_text("""
#!/usr/bin/env python3
""""
import pynlm

def advanced_example():
    print("=== NLM Advanced Example ===")
    
    # Create advanced configuration
    config = pynlm.create_default_config()
    
    # Enable advanced features
    config.set('advanced.enable_prediction_system', True)
    config.set('advanced.enable_memory_integration', True)
    config.set('advanced.enable_developmental_model', True)
    
    # Configure performance
    config.set('performance.use_vectorization', True)
    config.set('performance.optimization_level', 3)
    
    # Create brain
    brain = pynlm.create_brain(config, "advanced_brain")
    
    # Create world
    world = pynlm.create_world(width=30, height=30, vision_width=12, vision_height=12)
    
    # Create agent
    agent = pynlm.create_agent(brain, world)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run advanced simulation
    print("Running advanced simulation...")
    results = pynlm.create_advanced_api(config).run_advanced_simulation(
        brain, world, agent, 1000
    )
    
    print(f"Simulation completed with {results['performance']['total_steps']} steps")
    print(f"Final firing neurons: {results['brain_stats'][-1]['firing_neurons']}")
    print(f"Final curiosity level: {results['agent_stats'][-1]['curiosity_level']}")
    
    return results

if __name__ == "__main__":
    advanced_example()
""")
        print(f"  ✓ Created {advanced_example.name}")
        
        # Create example utilities
        utils_dir = examples_dir / "utils"
        utils_dir.mkdir(exist_ok=True)
        
        # Create utils/__init__.py
        (utils_dir / "__init__.py").write_text("")
        
        # Create visualization utilities
        viz_utils = utils_dir / "visualization.py"
        viz_utils.write_text("""
#!/usr/bin/env python3
""""
import matplotlib.pyplot as plt
import numpy as np

def plot_brain_activity(brain_stats):
    """Plot brain activity statistics."""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('NLM Brain Activity Analysis', fontsize=16)
    
    # Plot 1: Firing neurons over time
    ax = axes[0, 0]
    steps = [stat['step'] for stat in brain_stats]
    firing = [stat['firing_neurons'] for stat in brain_stats]
    ax.plot(steps, firing, 'b-', alpha=0.7, linewidth=2)
    ax.set_xlabel('Simulation Step')
    ax.set_ylabel('Firing Neurons')
    ax.set_title('Firing Neurons Over Time')
    ax.grid(True, alpha=0.3)
    
    # Plot 2: Average firing rate
    ax = axes[0, 1]
    firing_rates = [stat['average_firing_rate'] for stat in brain_stats]
    ax.plot(steps, firing_rates, 'r-', alpha=0.7, linewidth=2)
    ax.set_xlabel('Simulation Step')
    ax.set_ylabel('Average Firing Rate (Hz)')
    ax.set_title('Average Firing Rate')
    ax.grid(True, alpha=0.3)
    
    # Plot 3: Curiosity vs Novelty
    ax = axes[1, 0]
    curiosities = [stat.get('curiosity_level', 0) for stat in brain_stats]
    novelties = [stat.get('novelty_level', 0) for stat in brain_stats]
    ax.plot(curiosities, novelties, 'g.', alpha=0.5, markersize=8)
    ax.set_xlabel('Curiosity Level')
    ax.set_ylabel('Novelty Level')
    ax.set_title('Curiosity vs Novelty')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    
    # Plot 4: Action distribution
    ax = axes[1, 1]
    from collections import Counter
    action_counts = Counter([stat.get('action', 'Unknown') for stat in brain_stats])
    if action_counts:
        actions = list(action_counts.keys())
        counts = list(action_counts.values())
        bars = ax.bar(range(len(actions)), counts, alpha=0.7)
        ax.set_xlabel('Action')
        ax.set_ylabel('Count')
        ax.set_title('Action Distribution')
        ax.set_xticks(range(len(actions)))
        ax.set_xticklabels(actions, rotation=45)
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig

""")
        print(f"  ✓ Created visualization utilities")
        
        print("\nConfiguration files created successfully!")
    
    def build_python_bindings(self, build_dir: str = "build") -> bool:
        """Build Python bindings using scikit-build."""
        print(f"\n=== Building NLM Python Bindings ===")
        
        # Check if pyproject.toml exists
        if not self.pyproject_path.exists():
            print("✗ pyproject.toml not found!")
            return False
        
        # Create build directory
        build_path = self.repo_root / build_dir
        build_path.mkdir(exist_ok=True)
        
        # Change to build directory
        original_cwd = os.getcwd()
        os.chdir(build_path)
        
        try:
            # Run CMake configuration
            print("Running CMake configuration...")
            cmake_cmd = ['cmake', '..', '-DCMAKE_BUILD_TYPE=Release']
            if self.check_environment():
                cmake_cmd.extend(['-DPYTHON_EXECUTABLE=' + sys.executable])
            
            result = subprocess.run(cmake_cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"✗ CMake configuration failed:")
                print(result.stdout)
                print(result.stderr)
                return False
            print("✓ CMake configuration successful")
            
            # Build the project
            print("Building Python bindings...")
            build_cmd = ['make', '-j', str(os.cpu_count())]
            result = subprocess.run(build_cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"✗ Build failed:")
                print(result.stdout)
                print(result.stderr)
                return False
            print("✓ Build successful")
            
            # Check if bindings were created
            lib_pattern = "**/*.so" if sys.platform != "win32" else "**/*.pyd"
            bindings_found = any(build_path.rglob(lib_pattern))
            
            if bindings_found:
                print("✓ Python bindings created successfully")
                
                # Install the package
                print("Installing Python package...")
                install_cmd = [sys.executable, "-m", "pip", "install", "."]
                result = subprocess.run(install_cmd, capture_output=True, text=True)
                if result.returncode != 0:
                    print(f"⚠ Installation warning:")
                    print(result.stdout)
                    print(result.stderr)
                else:
                    print("✓ Python package installed successfully")
            else:
                print("⚠ No Python bindings found after build")
            
            return True
            
        except Exception as e:
            print(f"✗ Build error: {e}")
            return False
        finally:
            os.chdir(original_cwd)
    
    def create_development_environment(self):
        """Create a development environment setup."""
        print("\n=== Creating Development Environment ===")
        
        # Create requirements for development
        dev_requirements = [
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
            "pytest>=7.0",
            "numpy>=1.20",
            "matplotlib>=3.0",
            "jupyter>=1.0",
            "sphinx>=4.0",
            "sphinx-rtd-theme>=1.0",
            "pyyaml>=6.0",
            "requests>=2.25",
        ]
        
        requirements_path = self.repo_root / "requirements-dev.txt"
        with open(requirements_path, "w") as f:
            f.write("\n".join(dev_requirements))
        print(f"✓ Created {requirements_path.name}")
        
        # Create setup.py (for older Python packaging)
        setup_py_content = """
#!/usr/bin/env python3

from setuptools import setup, find_packages
import os

def read_requirements(filename):
    with open(filename) as f:
        return [line.strip() for line in f if line.strip() and not line.startswith('#')]

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM (Neural Learning Machine)',
    author='NLM Authors',
    packages=find_packages(),
    install_requires=read_requirements('requirements-dev.txt'),
    extras_require={
        'dev': read_requirements('requirements-dev.txt'),
        'examples': [
            'matplotlib>=3.0',
            'jupyter>=1.0',
            'plotly>=4.0',
        ],
    },
    python_requires='>=3.8',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
    ],
    url='https://github.com/nlm-project/nlm',
)
"""
        
        setup_py_path = self.repo_root / "setup.py"
        with open(setup_py_path, "w") as f:
            f.write(setup_py_content)
        print(f"✓ Created {setup_py_path.name}")
        
        # Create development README
        dev_readme_content = """# NLM Python Development Guide

This document provides instructions for developing the NLM Python bindings.

## Quick Start

### 1. Clone the repository
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
"""

### 2. Create virtual environment
```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\\Scripts\\activate
"""

### 3. Install dependencies
```bash
pip install -r requirements-dev.txt
pip install -e .  # Install in development mode
"""

### 4. Build Python bindings
```bash
# Using scikit-build (recommended)
cmake build
cd build
make

# Or using setup.py (older method)
pip install -e .
"""

### 5. Test the installation
```bash
python -c "import pynlm; print(pynlm.__version__)"
python examples/basic_example.py
"""

## Development Workflow

### Running tests
```bash
pytest tests/
pytest examples/
"""

### Building documentation
```bash
sphinx-build docs/ docs/_build/
"""

### Running examples
```bash
python examples/advanced_example.py
"""

## Advanced Configuration

### Environment variables
- `NLM_PYTHON_BINDINGS=1` - Enable Python bindings
- `NLM_LOG_LEVEL=DEBUG` - Set logging level
- `NLM_PERFORMANCE_PROFILE=1` - Enable performance profiling

### Configuration options
See `pynlm/` for detailed API documentation.

## Troubleshooting

### Common issues

**ImportError: No module named 'pynlm'**:
```bash
pip install -e .
```

**Build errors**:
```bash
# Ensure you have all required dependencies
pip install scikit-build-core pybind11
# Try building in a clean directory
rm -rf build
cmake build
make
```

**Performance issues**:
- Check `pynlm.NLMConfig.set('performance.optimization_level', 3)`
- Ensure `pynlm.NLMConfig.set('performance.use_vectorization', True)`
"""
        
        dev_readme_path = self.repo_root / "DEV_README.md"
        with open(dev_readme_path, "w") as f:
            f.write(dev_readme_content)
        print(f"✓ Created {dev_readme_path.name}")
        
        print("\nDevelopment environment setup completed!")
    
    def run_tests(self, test_dir: str = "tests") -> bool:
        """Run NLM tests."""
        print(f"\n=== Running NLM Tests ===")
        
        if not (self.repo_root / test_dir).exists():
            print(f"✗ Test directory '{test_dir}' not found!")
            return False
        
        try:
            import pytest
            result = pytest.main([test_dir, "-v"])
            return result == 0
        except ImportError:
            print("✗ pytest not installed!")
            return False
    
    def print_help(self):
        """Print help information."""
        help_text = """
=== NLM Python Bindings Setup ===

Usage: python setup.py <command> [options]

Commands:
  check          Check environment setup
  setup          Setup Python bindings
  build          Build Python bindings
  create-config  Create configuration files
  develop        Set up development environment
  test           Run tests
  help           Show this help

Examples:
  python setup.py check
  python setup.py setup
  python setup.py build
  python setup.py develop
  python setup.py test

For more information, visit: https://github.com/nlm-project/nlm
"""
        print(help_text)


# Main execution
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Python Bindings Setup Script")
    parser.add_argument('command', choices=['check', 'setup', 'build', 'create-config', 'develop', 'test', 'help'],
                       help='Command to execute')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--build-dir', default='build', help='Build directory')
    
    args = parser.parse_args()
    
    setup = NLMBindingsSetup(args.repo_root)
    
    if args.command == 'check':
        setup.check_environment()
    elif args.command == 'setup':
        if setup.check_environment():
            print("Environment is ready for setup")
        else:
            print("Environment setup incomplete")
    elif args.command == 'build':
        if setup.check_environment():
            success = setup.build_python_bindings(args.build_dir)
            if success:
                print("Build completed successfully!")
            else:
                print("Build failed!")
                sys.exit(1)
        else:
            print("Cannot build - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'create-config':
        setup.create_configuration_files()
    elif args.command == 'develop':
        if setup.check_environment():
            setup.create_development_environment()
        else:
            print("Cannot create development environment - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'test':
        setup.run_tests()
    elif args.command == 'help':
        setup.print_help()
"""
        setup_py_path = self.repo_root / "setup.py"
        with open(setup_py_path, "w") as f:
            f.write(setup_py_content)
        print(f"✓ Created {setup_py_path.name}")
        
        print("\nDevelopment environment setup completed!")
    
    def run_tests(self, test_dir: str = "tests") -> bool:
        """Run NLM tests."""
        print(f"\n=== Running NLM Tests ===")
        
        if not (self.repo_root / test_dir).exists():
            print(f"✗ Test directory '{test_dir}' not found!")
            return False
        
        try:
            import pytest
            result = pytest.main([test_dir, "-v"])
            return result == 0
        except ImportError:
            print("✗ pytest not installed!")
            return False
    
    def print_help(self):
        """Print help information."""
        help_text = """
=== NLM Python Bindings Setup ===

Usage: python setup.py <command> [options]

Commands:
  check          Check environment setup
  setup          Setup Python bindings
  build          Build Python bindings
  create-config  Create configuration files
  develop        Set up development environment
  test           Run tests
  help           Show this help

Examples:
  python setup.py check
  python setup.py setup
  python setup.py build
  python setup.py develop
  python setup.py test

For more information, visit: https://github.com/nlm-project/nlm
"""
        print(help_text)


# Main execution
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Python Bindings Setup Script")
    parser.add_argument('command', choices=['check', 'setup', 'build', 'create-config', 'develop', 'test', 'help'],
                       help='Command to execute')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--build-dir', default='build', help='Build directory')
    
    args = parser.parse_args()
    
    setup = NLMBindingsSetup(args.repo_root)
    
    if args.command == 'check':
        setup.check_environment()
    elif args.command == 'setup':
        if setup.check_environment():
            print("Environment is ready for setup")
        else:
            print("Environment setup incomplete")
    elif args.command == 'build':
        if setup.check_environment():
            success = setup.build_python_bindings(args.build_dir)
            if success:
                print("Build completed successfully!")
            else:
                print("Build failed!")
                sys.exit(1)
        else:
            print("Cannot build - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'create-config':
        setup.create_configuration_files()
    elif args.command == 'develop':
        if setup.check_environment():
            setup.create_development_environment()
        else:
            print("Cannot create development environment - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'test':
        setup.run_tests()
    elif args.command == 'help':
        setup.print_help()
"""
        
        setup_py_path = self.repo_root / "setup.py"
        with open(setup_py_path, "w") as f:
            f.write(setup_py_content)
        print(f"✓ Created {setup_py_path.name}")
        
        print("\nDevelopment environment setup completed!")
    
    def run_tests(self, test_dir: str = "tests") -> bool:
        """Run NLM tests."""
        print(f"\n=== Running NLM Tests ===")
        
        if not (self.repo_root / test_dir).exists():
            print(f"✗ Test directory '{test_dir}' not found!")
            return False
        
        try:
            import pytest
            result = pytest.main([test_dir, "-v"])
            return result == 0
        except ImportError:
            print("✗ pytest not installed!")
            return False
    
    def print_help(self):
        """Print help information."""
        help_text = """
=== NLM Python Bindings Setup ===

Usage: python setup.py <command> [options]

Commands:
  check          Check environment setup
  setup          Setup Python bindings
  build          Build Python bindings
  create-config  Create configuration files
  develop        Set up development environment
  test           Run tests
  help           Show this help

Examples:
  python setup.py check
  python setup.py setup
  python setup.py build
  python setup.py develop
  python setup.py test

For more information, visit: https://github.com/nlm-project/nlm
"""
        print(help_text)


# Main execution
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Python Bindings Setup Script")
    parser.add_argument('command', choices=['check', 'setup', 'build', 'create-config', 'develop', 'test', 'help'],
                       help='Command to execute')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--build-dir', default='build', help='Build directory')
    
    args = parser.parse_args()
    
    setup = NLMBindingsSetup(args.repo_root)
    
    if args.command == 'check':
        setup.check_environment()
    elif args.command == 'setup':
        if setup.check_environment():
            print("Environment is ready for setup")
        else:
            print("Environment setup incomplete")
    elif args.command == 'build':
        if setup.check_environment():
            success = setup.build_python_bindings(args.build_dir)
            if success:
                print("Build completed successfully!")
            else:
                print("Build failed!")
                sys.exit(1)
        else:
            print("Cannot build - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'create-config':
        setup.create_configuration_files()
    elif args.command == 'develop':
        if setup.check_environment():
            setup.create_development_environment()
        else:
            print("Cannot create development environment - environment setup incomplete")
            sys.exit(1)
    elif args.command == 'test':
        setup.run_tests()
    elif args.command == 'help':
        setup.print_help()
"""