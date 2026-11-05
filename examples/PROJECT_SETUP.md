# NLM Project Setup Guide

## Overview

This guide provides step-by-step instructions for setting up and running the NLM (熙然) neural learning machine project. It covers both development and usage scenarios.

## Quick Start (5 Minutes)

### Prerequisites
```bash
# Check for required tools
which cmake g++ python3 pip

# Install dependencies (if needed)
pip install pybind11 numpy
```

### Step 1: Clone and Build
```bash
# Clone the repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Step 2: Install Python Bindings
```bash
cd /path/to/nlm
pip install -e .
```

### Step 3: Run Basic Demo
```bash
# Run Phase 2 basic neural computation demo
cd /path/to/nlm
./nlm

# Or run Python example
python -c "import pynlm; print('NLM loaded successfully')"
```

## Detailed Setup Instructions

### 1. Source Code Setup

#### Clone the Repository
```bash
ggit clone https://github.com/your-org/nlm.git
 cd nlm
```

#### Create Build Directory
```bash
mkdir build
cd build
```

#### Configure Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### Build the Code
```bash
make -j4
```

### 2. Python Environment Setup

#### Install Python Dependencies
```bash
# Install system dependencies for pybind11
apt-get update
apt-get install -y python3-dev python3-pip

# Install NLM Python package
cd /path/to/nlm
pip install -e .

# Verify installation
python3 -c "import pynlm; print('NLM Python bindings loaded successfully')"
```

#### Create Virtual Environment (Recommended)
```bash
# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate

# Install NLM in virtual environment
pip install -e /path/to/nlm
```

### 3. Configuration Setup

#### Copy Default Configuration
```bash
# Copy default configuration
cp nlm/configs/default.cfg my_config.cfg

# Edit configuration as needed
# Open my_config.cfg in your favorite text editor
```

#### Create Project-Specific Configuration
```bash
# Create project configuration
cat > nlm/configs/project_config.cfg << 'EOF'
# Project-specific NLM configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Add project-specific parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
environment_name = GridWorld
log_level = INFO
EOF
```

### 4. Running Demos

#### Build Demos
```bash
cd /path/to/nlm/build
```

#### Available Demos
```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Cognitive development
./nlm_phase4_demo

# Phase 6: Integration test
./nlm_phase6_demo

# Benchmark
s ./nlm_benchmark
```

#### Running with Custom Configuration
```bash
# Run with custom configuration
./nlm --config configs/my_config.cfg

# Run specific demo with configuration
./nlm_phase6_demo --config configs/experimental.cfg
```

#### Running Python Examples
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

### 5. Development Setup

#### Edit Configuration Files
```bash
# Edit configuration using nano
nano nlm/configs/default.cfg

# Or using vim
vim nlm/configs/default.cfg
```

#### Run Tests
```bash
# Run unit tests
cd nlm/build
ctest --output-on-failure

# Run integration tests
./nlm_phase6_demo --test

# Run performance benchmarks
./nlm_benchmark
```

#### IDE Configuration
```bash
# Create .vscode/settings.json for VS Code
cat > .vscode/settings.json << 'EOF'
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureArgs": ["-DCMAKE_BUILD_TYPE=Release"],
    "editor.formatOnSave": true,
    "python.pythonPath": "/usr/bin/python3"
}
EOF
```

### 6. Advanced Setup

#### Set Up Git Hooks
```bash
# Install git hooks
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
echo "Running pre-commit checks..."
make lint || exit 1
ctest --output-on-failure || exit 1
echo "Pre-commit checks passed!"
EOF

chmod +x .git/hooks/pre-commit
```

#### Environment Variables
```bash
# Add to ~/.bashrc or ~/.bash_profile
cat >> ~/.bashrc << 'EOF'
# NLM environment variables
export NLM_HOME=/path/to/nlm
export NLM_BUILD_DIR=/path/to/nlm/build
export PYTHONPATH=$PYTHONPATH:$NLM_HOME/src:$NLM_HOME/python

# Configuration paths
export NLM_CONFIG_DIR=$NLM_HOME/configs
export NLM_LOG_DIR=$NLM_HOME/logs
EOF

# Source the changes
source ~/.bashrc
```

#### Project Structure
```
/workspace/app
├── nlm/                          # Source code
│   ├── src/                     # C++ source files
│   │   ├── core/               # Core utilities
│   │   ├── agent/              # Agent-brain interface
│   │   ├── world/              # Simple world
│   │   ├── performance/        # Checkpoints, pools, queues
│   │   ├── brain/              # Neural components
│   │   ├── dynamics/           # Neural dynamics
│   │   ├── plasticity/         # Plasticity rules
│   │   └── ...                 # Other components
│   ├── python/                 # Python bindings
│   └── include/nlm/            # C++ headers
├── build/                       # Build directory
├── configs/                     # Configuration files
├── docs/                        # Documentation
├── examples/                    # Example configurations
├── tests/                       # Tests
├── nlm/                         # Python package
│   ├── __init__.py
│   └── bindings.cpp
├── README.md
├── LICENSE
└── CMakeLists.txt
```

## Troubleshooting

### Common Issues

#### "Build failed"
**Solution**: Check if all required dependencies are installed:
```bash
# Check for required compilers and tools
which cmake g++ python3

# Install missing dependencies (Ubuntu)
sudo apt-get install build-essential cmake python3-dev python3-pip
```

#### "Python bindings not found"
**Solution**: Install Python package properly:
```bash
# Install in development mode
cd /path/to/nlm
pip install -e .

# Or install from build directory
cd /path/to/nlm/build
pip install -e ../nlm
```

#### "Configuration file not found"
**Solution**: Check configuration file location:
```bash
# List configuration files
ls -la nlm/configs/

# Create missing configuration file
touch nlm/configs/my_config.cfg
```

#### "Demo not found"
**Solution**: Rebuild demos:
```bash
cd /path/to/nlm/build
make -j4
```

### Debug Information

#### Check Build Configuration
```bash
cd /path/to/nlm/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make help
```

#### Check Python Package
```python
import pynlm
print(pynlm.__file__)

# Check available modules
print(dir(pynlm))
```

## Project Maintenance

### Regular Tasks

#### Daily
```bash
# Run quick validation
./nlm_phase6_demo --test

# Check for new commits
git fetch origin
```

#### Weekly
```bash
# Run full test suite
cd nlm/build
ctest --output-on-failure

# Clean build artifacts
make clean
```

#### Monthly
```bash
# Update documentation
./update_docs.py

# Check for license compliance
make license-check
```

### Backup and Version Control

#### Backup Configuration
```bash
# Backup important configurations
cp nlm/configs/*.cfg ~/nlm_configs_backup/

# Backup build artifacts
cd nlm/build
cp -r bin lib *.so ../backups/
```

#### Version Control
```bash
# Commit changes with appropriate commit message
ggit add .
ggit commit -m "Feature: Add new configuration parameter"

# Push to remote (if authorized)
ggit push origin feature-branch
```

## Performance Optimization

### Build Optimization
```bash
# Optimize for speed
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPTIMIZE_FOR_PERFORMANCE=ON
make -j$(nproc)
```

### Runtime Optimization
```python
import pynlm

# Create optimized configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Appropriate size
config.set("simulation_timestep", 0.001)  # Appropriate timestep

# Initialize brain with optimization
brain = pynlm.createBrain(config)
brain.initialize()

# Run optimized simulation
for step in range(1000):
    brain.step(step)
```

## Conclusion

This setup guide provides everything needed to get the NLM project running. Whether you're a developer, researcher, or user, you now have all the tools and documentation to effectively work with the NLM neural learning machine.

For more advanced usage and customization, refer to the other documentation files in this directory.

---
*Project setup guide v1.0.0*
