#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Setup Script

This script automates the setup and validation of the NLM environment.
"""

import os
import sys
import subprocess
import platform

def print_header():
    print("=" * 60)
    print("NLM (Neural Learning Machine) - Setup Script")
    print("=" * 60)
    print()

def run_command(cmd, check=True, description=None):
    """Run a command and return success status."""
    if description:
        print(f"✓ {description}")
    
    try:
        result = subprocess.run(
            cmd, shell=True, check=check,
            capture_output=True, text=True
        )
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Command failed: {' '.join(cmd)}")
        if e.stderr:
            print(f"  Error: {e.stderr.strip()}")
        return False

def check_python_version():
    """Check Python version."""
    version = sys.version_info
    if version.major == 3 and version.minor >= 8:
        print(f"✓ Python {version.major}.{version.minor} is supported")
        return True
    else:
        print(f"✗ Python {version.major}.{version.minor} is not supported")
        print("  Requires Python 3.8 or higher")
        return False

def check_cmake():
    """Check CMake availability."""
    return run_command(
        "cmake --version",
        description="Check CMake"
    )

def check_compiler():
    """Check C++ compiler."""
    if platform.system() == "Windows":
        return run_command(
            "cl /?",
            description="Check MSVC compiler"
        )
    else:
        return run_command(
            "g++ --version",
            description="Check g++ compiler"
        )

def check_build_tools():
    """Check build tools."""
    tools_ok = True
    
    if not run_command("make --version", description="Check make"):
        tools_ok = False
    
    if not run_command(f"{sys.executable} -m pip --version", description="Check pip"):
        tools_ok = False
    
    return tools_ok

def install_python_package():
    """Install Python package."""
    print("Installing NLM Python package...")
    
    # Check if setup.py exists
    if not os.path.exists("setup.py"):
        print("✗ setup.py not found")
        return False
    
    # Install package
    success = run_command(
        f"{sys.executable} setup.py install",
        description="Install Python package"
    )
    
    if success:
        print("✓ Python package installed successfully")
        print("  You can now import NLM: python -c 'import pynlm; print(\"NLM imported successfully!\")'")
    
    return success

def create_example_config():
    """Create example configuration files."""
    print("Creating example configuration files...")
    
    # Create configs directory
    os.makedirs("configs", exist_ok=True)
    
    # Create example config
    config_content = """# Example NLM Configuration
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1
curiosity_enabled = true
novelty_enabled = true

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
"""
    
    with open("configs/default.cfg", "w") as f:
        f.write(config_content)
    
    print("✓ Created example configuration at: configs/default.cfg")
    return True

def create_readme():
    """Create README with setup instructions."""
    print("Creating README with setup instructions...")
    
    readme_content = """# NLM (Neural Learning Machine)

NLM is a brain-inspired spiking neural network simulator written in C++20.

## Quick Start

### Installation

1. Clone this repository
2. Build the C++ library:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. Install Python package:
   ```bash
   pip install -e .
   ```

### Usage Example

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Documentation

- [README.md](README.md) - This file
- [easy_usage.md](easy_usage.md) - Beginner guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive Python API reference
- [docs/](docs/) - Technical documentation

## License

MIT License.
"""
    
    with open("README.md", "w") as f:
        f.write(readme_content)
    
    print("✓ Created README.md")
    return True

def main():
    print_header()
    
    print("NLM Environment Setup")
    print()
    print("This script will check your environment and set up NLM.")
    print()
    
    steps = [
        ("Check Python version", check_python_version),
        ("Check CMake", check_cmake),
        ("Check C++ compiler", check_compiler),
        ("Check build tools", check_build_tools),
        ("Create example configuration", create_example_config),
        ("Create README", create_readme),
        ("Install Python package", install_python_package),
    ]
    
    completed = 0
    failed = 0
    
    for step_name, step_func in steps:
        print(f"[{completed + 1}] {step_name}")
        print("-" * 60)
        
        try:
            if step_func():
                completed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Error: {e}")
            failed += 1
        
        print()
    
    print("=" * 60)
    print("Setup Complete!")
    print(f"✓ {completed} steps completed successfully")
    print(f"✗ {failed} steps failed")
    print("=" * 60)
    
    if failed > 0:
        print("\nSome steps failed. Please check the output above.")
        print("You may need to manually complete the failed steps.")
    
    print("\nNext steps:")
    print("1. Build the C++ library:")
    print("   mkdir build && cd build && cmake .. && make -j4")
    print("2. Test the setup:")
    print("   python -c 'import pynlm; print(\"NLM imported successfully!\")'")
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
