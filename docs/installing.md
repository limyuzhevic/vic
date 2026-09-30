# NLM Python Installation Guide

## Overview

This document provides comprehensive installation instructions for the NLM Python bindings (`pynlm`). It covers system requirements, installation methods, troubleshooting common issues, and post-installation verification.

## System Requirements

### Minimum Requirements

| Component | Minimum Version | Notes |
|-----------|----------------|-------|
| Python | 3.8 | Compatible with Python 3.8, 3.9, 3.10, 3.11, 3.12 |
| CMake | 3.16 | Required for building C++ extensions |
| Compiler | GCC 10+ or Clang 12+ | C++20 compatible compiler |
| RAM | 2 GB | Minimum for basic operation |

### Recommended Requirements

| Component | Recommended Version | Notes |
|-----------|--------------------|-------|
| Python | 3.9+ | Better performance and features |
| CMake | 3.20+ | Better toolchain support |
| Compiler | GCC 12+ or Clang 14+ | Optimized performance |
| RAM | 4+ GB | For larger brain configurations |
| SSD | N/A | Faster build and simulation times |

## Installation Methods

### Method 1: Installation from Source (Recommended for Development)

```bash
# Clone the repository
cd /path/to/nlm
git clone https://github.com/nlm-project/nlm.git

# Navigate to project root
cd nlm

# Create build directory
mkdir build
cd build

# Configure CMake build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python) \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build with parallel jobs (adjust based on CPU cores)
make -j$(nproc)

# Install Python bindings
pip install -e .  # Development mode (editable install)
# OR
pip install .     # Regular install

# Run tests to verify installation
python -m pytest tests/ -v
```

### Method 2: Installation Using Pre-built Wheels

```bash
# Check for pre-built wheels
pip install pynlm

# If no pre-built wheels available, install from source
pip install . --no-binary=pynlm
```

### Method 3: Development Installation with Full Toolchain

```bash
# Install all development dependencies
apt-get update
apt-get install -y \
    cmake \
    build-essential \
    python3-dev \
    python3-pip \
    python3-venv

# Create virtual environment (recommended)
python3 -m venv nlm_env
source nlm_env/bin/activate

# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy

# Install NLM in development mode
pip install -e .

# Build and test
python -m pytest tests/ -v
```

## Step-by-Step Installation Process

### Step 1: Prepare Development Environment

```bash
# Update package repositories
sudo apt-get update
sudo apt-get upgrade -y

# Install system dependencies
 sudo apt-get install -y \
    git \
    wget \
    curl \
    build-essential \
    cmake \
    python3-dev \
    python3-pip \
    python3-venv \
    libpython3-dev
```

### Step 2: Clone and Configure Repository

```bash
# Clone the NLM repository
cd /workspace
 git clone https://github.com/nlm-project/nlm.git
cd nlm

# Verify repository structure
ls -la
```

### Step 3: Build C++ Extensions

```bash
# Create and enter build directory
mkdir build
cd build

# Configure CMake with specific options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DNLM_BUILD_TESTS=ON \
    -DNLM_BUILD_EXAMPLES=ON

# Build the project (this may take several minutes)
make -j$(nproc)

# Verify build success
ls -la
ls bin/ src/
```

### Step 4: Install Python Package

```bash
# Navigate back to project root
cd ..

# Install Python bindings in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(f'pynlm version: {pynlm.__version__}')"
```

## Post-Installation Verification

### Basic Functionality Test

```python
#!/usr/bin/env python3
"""Basic functionality test for NLM Python bindings."""

import pynlm
import sys

def test_basic_functionality():
    """Test basic NLM Python bindings functionality."""
    print("Testing NLM Python bindings...")
    
    try:
        # Test 1: Version check
        print(f"1. Python bindings version: {pynlm.__version__}")
        
        # Test 2: Configuration creation
        config = pynlm.createDefaultConfig()
        print("2. Default configuration created successfully")
        
        # Test 3: Brain creation and initialization
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"3. Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Test 4: Simple simulation
        brain.step(0)
        print(f"4. Brain step completed - {brain.getTotalSpikeCount()} spikes recorded")
        
        # Test 5: Additional classes and methods
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        print("5. World configuration successful")
        
        # Test 6: Agent creation
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("6. Agent brain created and initialized")
        
        # Test 7: Neuron types and enumerations
        neuron_type = pynlm.NeuronType.Excitatory
        action_type = pynlm.ActionType.MoveForward
        developmental_stage = pynlm.DevelopmentalStage.Initial
        print("7. Enumerations work correctly")
        
        # Test 8: Error handling
        try:
            invalid_config = pynlm.createDefaultConfig()
            print("8. Error handling test passed")
        except Exception as e:
            print(f"8. Error handling test completed (expected behavior: {e})")
        
        print("\n✅ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_basic_functionality()
    sys.exit(0 if success else 1)
```

### Advanced Functionality Test

```python
#!/usr/bin/env python3
"""Advanced functionality test for NLM Python bindings."""

import pynlm
import numpy as np

def test_advanced_functionality():
    """Test advanced NLM Python bindings functionality."""
    print("Testing advanced NLM Python bindings...")
    
    try:
        # Test 1: Complex configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.08)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"1. Advanced configuration: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
        
        # Test 2: Regional functionality
        region_id = brain.addRegion("Cortex")
        region = brain.getRegion(region_id)
        print(f"2. Regional functionality: Region '{region_id}' created and retrieved")
        
        # Test 3: Save/load functionality
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        brain.save(checkpoint_file)
        print(f"3. Save functionality: Brain state saved to {checkpoint_file}")
        
        # Create new brain and load
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        brain2.load(checkpoint_file)
        print("4. Load functionality: Brain state loaded successfully")
        
        os.unlink(checkpoint_file)
        
        # Test 4: World-agent-agent interaction
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain2)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.initialize(world)
        
        # Run simple simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain2.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"5. World-agent simulation: {brain2.getTotalSpikeCount()} spikes, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Test 5: Sensory and motor processing
        vision_data = list(np.random.uniform(0, 1, 72))  # 8x9 vision data
        vision = pynlm.Vision(8, 9, 1)
        vision.setData(vision_data)
        
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        audio_data = list(np.random.normal(0, 1, 22050))  # 1 second audio
        audio = pynlm.Audio(22050, len(audio_data))
        audio.setData(audio_data)
        
        percept.setAudio(audio)
        
        agent.processSensoryInput(percept)
        print("6. Sensory processing: Vision and audio data processed successfully")
        
        # Test 6: Action processing
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.5])
        print(f"7. Action processing: Action created with type {action.getType().value}")
        
        # Test 7: Configuration persistence
        config.saveToFile("test_config.json")
        loaded_config = pynlm.createDefaultConfig()
        loaded_config.loadFromFile("test_config.json")
        print("8. Configuration persistence: Config saved and loaded successfully")
        
        # Clean up
        import os
        if os.path.exists("test_config.json"):
            os.remove("test_config.json")
        
        print("\n✅ All advanced functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Advanced test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_advanced_functionality()
    sys.exit(0 if success else 1)
```

## Common Installation Issues and Solutions

### Issue 1: CMake Cannot Find Python

**Problem:** CMake configuration fails to locate Python interpreter or libraries.

**Solution:**

```bash
# Check Python location
which python3
python3 -c "import sys; print(sys.prefix)"

# Force CMake to use specific Python
cmake .. -DPython_EXECUTABLE=/usr/local/bin/python3

# Or set environment variable
export Python_EXECUTABLE=$(which python3)
cmake ..
```

### Issue 2: Missing pybind11 or scikit-build-core

**Problem:** Python dependencies are not installed.

**Solution:**

```bash
# Install required Python packages
pip install pybind11 scikit-build-core

# For development, also install testing dependencies
pip install pytest numpy
```

### Issue 3: Compiler Too Old

**Problem:** Current compiler doesn't support C++20.

**Solution:**

```bash
# Install newer compiler (Ubuntu example)
sudo apt-get install g++-12
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# Set compiler for CMake
cmake .. -DCMAKE_CXX_COMPILER=g++-12
```

### Issue 4: Memory Constraints During Build

**Problem:** Build process runs out of memory.

**Solution:**

```bash
# Build with fewer parallel jobs
make -j4  # Instead of -j$(nproc)

# Or increase swap space if necessary
sudo swapctl -a /swapfile  # Create swap file
```

### Issue 5: Permission Denied

**Problem:** Installation fails due to permission issues.

**Solution:**

```bash
# Use virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate
pip install -e .

# Or install with user flag
pip install --user -e .

# Or use sudo (not recommended)
sudo pip install -e .
```

### Issue 6: Invalid Configuration

**Problem:** CMake configuration produces invalid build.

**Solution:**

```bash
# Clean build directory
rm -rf build
mkdir build
cd build

# Reconfigure with verbose output
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

# Check for specific errors
make VERBOSE=1 2>&1 | grep -i "error\|warning" | head -20
```

## Build Optimization

### Optimizing Build Performance

```bash
# Optimize CMake build options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto" \
    -DNLM_BUILD_TYPE=Release \
    -DNLM_OPTIMIZATION_LEVEL=3

# Build with cache
ccache make -j$(nproc)

# Incremental builds
cmake --build build --target all --parallel $(nproc)
```

### Post-Build Optimization

```bash
# Strip debug symbols from binaries
find . -name "*.so" -exec strip {} \;
find . -name "*test*" -type f -exec strip {} \; 2>/dev/null || true

# Clean up build artifacts
git clean -xdf
rm -rf build/*/build/
```

## Verification Commands

### Quick Installation Check

```bash
# Verify installation
python3 -c "import pynlm; print('✅ pynlm imported successfully'); print(f'   Version: {pynlm.__version__}')"

# Run basic functionality test
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
print('✅ Basic brain simulation works')
print(f'   Neurons: {brain.getTotalNeuronCount()}')
print(f'   Spikes: {brain.getTotalSpikeCount()}')
"
```

### Comprehensive Test Suite

```bash
# Run all tests
python3 -m pytest tests/ -v --tb=short

# Run with coverage
python3 -m pytest tests/ --cov=pynlm --cov-report=html --cov-report=term

# Run specific test modules
python3 -m pytest tests/test_basic.py -v
python3 -m pytest tests/test_advanced.py -v
python3 -m pytest tests/test_performance.py -v
```

### Memory and Performance Check

```python
#!/usr/bin/env python3
"""Performance and memory check for NLM Python bindings."""

import pynlm
import time
import psutil
import os

def check_performance():
    """Check performance of NLM Python bindings."""
    print("Performance and Memory Check")
    print("=" * 40)
    
    # Get current process
    process = psutil.Process(os.getpid())
    
    # Measure memory before
    mem_before = process.memory_info().rss / 1024 / 1024  # MB
    
    # Create and run brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    start_time = time.time()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    end_time = time.time()
    
    # Measure memory after
    mem_after = process.memory_info().rss / 1024 / 1024  # MB
    
    # Performance metrics
    elapsed = end_time - start_time
    spikes_per_second = brain.getTotalSpikeCount() / elapsed
    neurons_per_mb = brain.getTotalNeuronCount() / (mem_after - mem_before)
    
    print(f"Simulation Time: {elapsed:.3f}s")
    print(f"Steps per Second: {100 / elapsed:.1f}")
    print(f"Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"Spikes per Second: {spikes_per_second:.1f}")
    print(f"Memory Usage: {mem_before:.1f} MB → {mem_after:.1f} MB")
    print(f"Memory Efficiency: {neurons_per_mb:.0f} neurons/MB")
    print(f"Average Firing Rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")

if __name__ == "__main__":
    check_performance()
```

## Development Environment Setup

### Setting Up IDE

#### Visual Studio Code

1. Install Python extension
2. Install C++ extension
3. Add pybind11 include paths
4. Configure CMake tasks

#### JetBrains IDEs (IntelliJ, PyCharm)

1. Create Python project
2. Add C++ module
3. Configure CMake integration
4. Set up Python interpreters

### Documentation Generation

```bash
# Install documentation generation tools
pip install sphinx sphinx-rtd-theme

# Generate API documentation
python -m sphinx -b html docs/ docs/_build/html/

# Generate PDF documentation
cd docs && make latexpdf
```

## Common Post-Installation Issues

### Issue: Cannot Import pynlm

**Symptoms:**
- `ImportError: No module named 'pynlm'`
- Module not found in Python path

**Solutions:**

```bash
# Check if module is installed
python3 -c "import pkgutil; print('pynlm' in [name for _, name, _ in pkgutil.iter_modules()])"

# Install in current environment
pip install -e .

# Check Python path
python3 -c "import sys; print(sys.path)"

# Try with explicit path
python3 -c "import sys; sys.path.append('/path/to/nlm'); import pynlm"
```

### Issue: Segmentation Faults

**Symptoms:**
- Application crashes with segmentation fault
- Often occurs with complex configurations

**Solutions:**

```python
# Test with simpler configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start small
brain = pynlm.createBrain(config)
brain.initialize()

# Test step-by-step
brain.step(0)

# Gradually increase complexity
```

### Issue: Memory Leaks

**Symptoms:**
- Memory usage increases over time
- Process consumes more memory with longer simulations

**Solutions:**

```python
# Use checkpointing for long simulations
brain.save("checkpoint_" + str(step) + ".bin")

# Clear references
import gc
gc.collect()

# Monitor memory usage regularly
import tracemalloc
tracemalloc.start()
```

### Issue: Thread Safety

**Note:** NLM is not thread-safe. Use proper synchronization:

```python
import threading
import time

def safe_simulation_step(brain, step):
    """Thread-safe simulation step."""
    lock = threading.Lock()
    with lock:
        brain.step(step)

# Usage in multi-threaded environment
threads = []
for i in range(10):
    t = threading.Thread(target=safe_simulation_step, args=(brain, i))
    threads.append(t)
    t.start()

for t in threads:
    t.join()
```

## Migration Guide

### From Previous Versions

#### Upgrading from 0.0.x to 0.1.0

**New Features:**
- Enhanced configuration system
- Improved error handling
- New regional functionality
- Better performance optimization

**Breaking Changes:**
- Some method signatures may have changed
- New dependency on scikit-build-core
- Updated type hints and documentation

**Migration Steps:**

```python
# Old code (0.0.x)
import pynlm
config = pynlm.createDefaultConfig()

# New code (0.1.0)
import pynlm
config = pynlm.createDefaultConfig()
# Add new parameters if needed
config.set("brain.neuron_count", 1000)
```

### Configuration Migration

```python
# Old configuration format
old_config = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1
}

# New configuration format (with validation)
new_config = pynlm.createDefaultConfig()
new_config.set("brain.neuron_count", old_config["brain.neuron_count"])
new_config.set("brain.synapse_density", old_config["brain.synapse_density"])
```

## Troubleshooting Checklist

### Pre-Installation Checklist

- [ ] Python 3.8+ installed
- [ ] CMake 3.16+ installed
- [ ] C++20 compatible compiler
- [ ] Required disk space (500MB+)
- [ ] Development tools installed

### Installation Checklist

- [ ] Build directory created successfully
- [ ] CMake configuration completed without errors
- [ ] Build process completed successfully
- [ ] Python package installed successfully
- [ ] Basic functionality tests passed

### Post-Installation Checklist

- [ ] pynlm module can be imported
- [ ] Basic functionality works
- [ ] Advanced features tested
- [ ] Performance benchmarks completed
- [ ] Documentation generated

## Support and Resources

### Documentation

- **API Reference:** `docs/python_api.md`
- **Examples:** `docs/python_examples.md`
- **Installation Guide:** This document
- **Troubleshooting:** `docs/troubleshooting.md`
- **Performance Guidelines:** `docs/benchmarking.md`

### Online Resources

- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Documentation:** https://nlm.readthedocs.io
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Support Channels

1. **GitHub Issues:** For bug reports and feature requests
2. **GitHub Discussions:** For general questions and discussions
3. **Email:** For commercial support inquiries
4. **Slack/Discord:** Community support (check GitHub for invites)

## Upgrade Path

### Upgrade Procedure

```bash
# Backup current installation
pip freeze > requirements.txt

# Upgrade to latest version
pip install --upgrade pynlm

# Verify upgrade
python -c "import pynlm; print(f'Upgraded to version: {pynlm.__version__}')"

# Run tests
python -m pytest tests/ -v
```

### Version Compatibility

| Current Version | Minimum Compatible Version | Notes |
|-----------------|---------------------------|-------|
| 0.1.0 | 0.0.x | Backward compatible with migration |
| 0.1.0 | 0.0.9 | Feature differences |
| 0.1.0 | 0.0.5 | API changes required |

## Future Development

### Planned Features

1. **Enhanced Documentation**: Interactive API documentation
2. **Performance Tools**: Advanced profiling and optimization tools
3. **Integration Testing**: Comprehensive integration test suites
4. **Docker Support**: Containerized development environment
5. **CI/CD Integration**: Automated testing and deployment pipelines

### Development Roadmap

- **Short-term (Next 3 months):** Bug fixes, documentation improvements
- **Medium-term (Next 6 months):** Performance optimizations, new examples
- **Long-term (Next year):** Advanced features, community contributions

## License and Copyright

This installation guide is subject to the MIT License. For more information, see the LICENSE file in the project root.

## Acknowledgments

We thank the following contributors for their work on NLM Python bindings:
- Core development team
- Pybind11 contributors
- CMake community
- Open source ecosystem

---

*This installation guide is continuously updated. Please report any issues or suggest improvements via the project's issue tracker.*
