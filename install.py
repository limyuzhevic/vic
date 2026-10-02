#!/usr/bin/env python3
"""
NLM Python package installation script.

This script provides a comprehensive setup for installing the NLM Python package
with all its dependencies and optional features.
"""

import os
import sys
import subprocess
import argparse
import platform

def run_command(cmd, description):
    """Run a command and print status."""
    print(f"\n{description}")
    print(f"Running: {cmd}")
    
    try:
        subprocess.run(cmd, shell=True, check=True)
        print(f"✓ {description}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ {description} failed: {e}")
        return False

def install_package():
    """Install the NLM Python package."""
    print("=== NLM Python Package Installer ===")
    
    # Check Python version
    python_version = sys.version_info
    if python_version.major < 3 or python_version.minor < 8:
        print("✗ Python 3.8 or higher is required")
        return False
    
    print(f"Python {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    # Determine platform
    system = platform.system()
    print(f"Platform: {system}")
    
    # Check if we're in the right directory
    if not os.path.exists('setup.py'):
        print("✗ setup.py not found. Please run from the package root directory.")
        return False
    
    # Create build directory
    os.makedirs('build', exist_ok=True)
    print("Created build directory")
    
    # Parse arguments
    parser = argparse.ArgumentParser(description='Install NLM Python package')
    parser.add_argument('--develop', action='store_true', 
                       help='Install in development mode (editable)')
    parser.add_argument('--extras', action='store_true',
                       help='Install extra dependencies')
    parser.add_argument('--all', action='store_true',
                       help='Install all dependencies including extras')
    parser.add_argument('--check', action='store_true',
                       help='Check dependencies without installing')
    
    args = parser.parse_args()
    
    # Check dependencies
    if args.check:
        return check_dependencies()
    
    # Install package
    install_cmd = ['pip', 'install']
    if args.develop:
        install_cmd.append('-e')
    
    install_cmd.append('.')
    
    if not run_command(' '.join(install_cmd), 'Installing NLM Python package'):
        return False
    
    # Install extras if requested
    if args.extras or args.all:
        extras_cmd = ['pip', 'install', '.[visualization,analysis,performance,dev]']
        if not run_command(' '.join(extras_cmd), 'Installing extra dependencies'):
            print("Note: Some extra dependencies failed to install. Continuing...")
    
    # Create example symlinks
    create_example_symlinks()
    
    # Create documentation
    create_documentation()
    
    print("\n=== Installation Complete ===")
    print("The NLM Python package has been installed successfully!")
    print("\nGetting started:")
    print("1. Import the package: import pynlm")
    print("2. Create a brain: brain = pynlm.create_brain()")
    print("3. Run a simulation: for step in range(100): brain.step(step)")
    print("\nFor examples, see the examples/ directory.")
    
    return True

def check_dependencies():
    """Check if required dependencies are available."""
    print("\n=== Checking Dependencies ===")
    
    required_packages = [
        ('numpy', 'numpy'),
        ('pandas', 'pandas'),
        ('matplotlib', 'matplotlib'),
        ('seaborn', 'seaborn'),
        ('plotly', 'plotly'),
        ('pybind11', 'pybind11'),
        ('pyyaml', 'yaml'),
        ('scikit-learn', 'sklearn'),
        ('scipy', 'scipy'),
        ('psutil', 'psutil')
    ]
    
    missing = []
    
    for package_name, import_name in required_packages:
        try:
            __import__(import_name)
            print(f"✓ {package_name}")
        except ImportError:
            print(f"✗ {package_name} - NOT INSTALLED")
            missing.append(package_name)
    
    if missing:
        print(f"\nMissing dependencies: {', '.join(missing)}")
        print("Run 'pip install " + ' '.join(missing) + "' to install them.")
        return False
    else:
        print("\n✓ All required dependencies are installed.")
        return True

def create_example_symlinks():
    """Create symlinks to examples for easy access."""
    print("\n=== Creating Example Symlinks ===")
    
    try:
        # Create examples directory if it doesn't exist
        os.makedirs('examples', exist_ok=True)
        
        # Try to create symlinks (Unix-like systems)
        if os.name != 'nt':  # Not Windows
            example_files = [
                'basic_simulation.py',
                'agent_world.py',
                'phase3_experiment.py',
                'phase4_experiment.py',
                'phase6_experiment.py',
                'advanced_research.py',
                'visualization_examples.py'
            ]
            
            for example in example_files:
                example_path = f'examples/{example}'
                if os.path.exists(example_path):
                    # Create symlink if it doesn't exist
                    if not os.path.exists(example):
                        os.symlink(example_path, example)
                        print(f"✓ Created symlink: {example} -> {example_path}")
        
        print("✓ Example symlinks created")
        
    except Exception as e:
        print(f"Note: Could not create symlinks: {e}")

def create_documentation():
    """Create basic documentation."""
    print("\n=== Creating Documentation ===")
    
    try:
        # Create docs directory
        os.makedirs('docs', exist_ok=True)
        
        # Create basic README
        readme_content = """
NLM Python Package
=================

The NLM Python package provides a comprehensive interface to the NLM (Neural Learning Machine) neural simulation framework.

## Installation

```bash
pip install .[all]
```

## Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set('neuron_count', 1000)

# Create brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

## Examples

See the `examples/` directory for comprehensive examples.

## API Reference

See the `docs/` directory for detailed API documentation.
"""
        
        with open('docs/QUICKSTART.md', 'w') as f:
            f.write(readme_content)
        
        print("✓ Quickstart documentation created")
        
    except Exception as e:
        print(f"Note: Could not create documentation: {e}")

if __name__ == '__main__':
    success = install_package()
    sys.exit(0 if success else 1)