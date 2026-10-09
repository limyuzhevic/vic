# Troubleshooting Guide

This document provides comprehensive troubleshooting guidance for NLM (Neural Learning Machine). It covers common issues, their solutions, and debugging techniques for resolving problems during installation, configuration, runtime, and performance.

## Overview

NLM is a complex system with multiple components and configurations. Even with comprehensive documentation, users may encounter issues during setup, configuration, or runtime. This troubleshooting guide provides systematic approaches to identify, diagnose, and resolve common problems.

### Troubleshooting Philosophy

1. **Start simple**: Check basic setup and configuration
2. **Isolate the problem**: Identify the specific component causing issues
3. **Use systematic debugging**: Apply structured approach to problem-solving
4. **Consult documentation**: Reference relevant sections for specific solutions
5. **Reproduce the issue**: Create minimal test case for consistent troubleshooting
6. **Apply fixes gradually**: Implement changes one at a time

## Common Issue Categories

### 1. Installation and Build Issues

#### Problem: Import Error - No module named 'pynlm'
**Symptoms**:
- Python script fails to import pynlm module
- Error: `ImportError: No module named 'pynlm'`
- Module installation appears successful but not importable

**Solutions**:

**Solution 1: Check Python Path**
```bash
# Check if pynlm is installed
pip list | grep pynlm

# Check Python path
python -c "import sys; print('\n'.join(sys.path))"

# Check if pynlm is importable
python -c "import pynlm; print('pynlm version:', pynlm.__version__)"
```

**Solution 2: Reinstall Python Bindings**
```bash
# Uninstall existing installation
pip uninstall pynlm

# Reinstall from source
pip install -e .

# Or install from PyPI
pip install pynlm
```

**Solution 3: Check Build Dependencies**
```bash
# Verify build dependencies
pip list | grep -E "(pybind11|scikit-build-core|cmake)"

# Install missing dependencies
pip install pybind11 scikit-build-core numpy

# Rebuild with dependencies
python -m pip install --upgrade pip
python -m pip install --no-build-isolation .
```

**Solution 4: Check Compiler Requirements**
```bash
# Verify compiler and build tools
gcc --version
make --version
cmake --version

# Install build tools if missing
apt-get update && apt-get install -y build-essential cmake
```

**Prevention**:
- Always use `pip install -e .` for development installation
- Maintain virtual environments for different projects
- Document exact installation commands for reproducibility

#### Problem: Build Failures
**Symptoms**:
- CMake configuration fails
- Compilation errors during build
- Missing header files or libraries

**Solutions**:

**Solution 1: CMake Configuration**
```bash
# Create build directory
mkdir build
cd build

# Configure with debugging
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

# Check configuration
make help
```

**Solution 2: Install Dependencies**
```bash
# Common missing dependencies
apt-get update
apt-get install -y \
    libpython3-dev \
    python3-dev \
    build-essential \
    libeigen3-dev \
    libboost-all-dev
```

**Solution 3: Check Source Code**
```bash
# Verify all source files are present
ls -la src/ core/ brain/ dynamics/ plasticity/ development/ neuromodulation/ memory/ prediction/ cognition/ sensory/ motor/ environment/ experiments/

# Check for missing files
find . -name "*.cpp" -o -name "*.hpp" | wc -l
```

**Solution 4: Platform-Specific Issues**
```bash
# macOS specific fixes
export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)

# Linux specific fixes
export CXXFLAGS="-std=c++20 -O2"
export LDFLAGS="-pthread"

# Windows specific (if using MinGW)
export CXXFLAGS="-std=c++20 -D_WIN32"
export LDFLAGS="-lws2_32"
```

### 2. Configuration Issues

#### Problem: Invalid Configuration Parameters
**Symptoms**:
- Configuration validation fails
- Invalid parameter values cause runtime errors
- Required parameters missing from configuration

**Solutions**:

**Solution 1: Configuration Validation**
```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()

# Check if configuration is valid
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors:")
    for error in config.getErrors():
        print(f"  - {error}")
```

**Solution 2: Required Parameters Check**
```python
# Check if required parameters are set
required_keys = [
    "brain.neuron_count",
    "brain.v_thresh",
    "brain.v_rest",
    "brain.v_reset",
    "plasticity.stdp.enable"
]

for key in required_keys:
    if config.has(key):
        print(f"✓ {key} is set")
    else:
        print(f"✗ {key} is required but not set")
```

**Solution 3: Parameter Range Validation**
```python
# Check parameter ranges
if config.has("brain.neuron_count"):
    neuron_count = config.getInt("brain.neuron_count")
    if neuron_count < 10:
        print("Warning: Very small neuron count may not be representative")
    elif neuron_count > 1000000:
        print("Warning: Very large neuron count may cause memory issues")

if config.has("plasticity.stdp.learning_rate"):
    stdp_rate = config.getFloat("plasticity.stdp.learning_rate")
    if stdp_rate < 0.0 or stdp_rate > 1.0:
        print("Error: STDP learning rate must be between 0.0 and 1.0")
```

**Solution 4: Configuration Migration**
```python
# Migration from older configuration versions
old_config = pynlm.createDefaultConfig()

# Apply migration rules
new_config = pynlm.createDefaultConfig()

# Migrate brain configuration
if old_config.has("neuron_count"):
    new_config.set("brain.neuron_count", old_config.getInt("neuron_count"))

# Migrate plasticity configuration  
if old_config.has("stdp_enabled"):
    new_config.set("plasticity.stdp.enable", old_config.getBool("stdp_enabled"))

# Apply other migrations...
```

#### Problem: Configuration File Issues
**Symptoms**:
- Cannot load configuration from file
- Invalid JSON/YAML configuration
- File permissions issues

**Solutions**:

**Solution 1: File Loading**
```python
# Try loading with error handling
try:
    config.loadFromFile("config.json")
    print("Configuration loaded successfully")
except FileNotFoundError:
    print("Configuration file not found")
    config = pynlm.createDefaultConfig()  # Use defaults
except json.JSONDecodeError as e:
    print(f"Invalid JSON in configuration file: {e}")
    # Fix the file or use a valid configuration
except Exception as e:
    print(f"Error loading configuration: {e}")
```

**Solution 2: File Format**
```python
# Create valid JSON configuration
import json

valid_config = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    }
}

# Write to file
with open("valid_config.json", "w") as f:
    json.dump(valid_config, f, indent=2)
```

### 3. Runtime Issues

#### Problem: Brain Initialization Errors
**Symptoms**:
- `brain.initialize()` fails with error messages
- Brain state is inconsistent
- Memory allocation issues

**Solutions**:

**Solution 1: Basic Troubleshooting**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Check configuration
print("Configuration check:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")

# Try creating brain with error handling
try:
    brain = pynlm.createBrain(config)
    print("Brain created successfully")
    
    # Try initializing
    brain.initialize()
    print("Brain initialized successfully")
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    print("Try reducing neuron count or increasing available memory")
    
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
    print("Try using memory pools or reducing memory requirements")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    print("Check system resources and configuration")
```

**Solution 2: Resource Management**
```python
import psutil
import os

# Check system resources
print("System resources:")
print(f"  Total memory: {psutil.virtual_memory().total / (1024**3):.2f} GB")
print(f"  Available memory: {psutil.virtual_memory().available / (1024**3):.2f} GB")
print(f"  CPU count: {psutil.cpu_count()}")
print(f"  CPU usage: {psutil.cpu_percent(interval=1):.1f}%")

# Monitor memory during simulation
process = psutil.Process(os.getpid())
print(f"\nProcess memory:")
print(f"  RSS: {process.memory_info().rss / (1024**2):.2f} MB")
print(f"  VMS: {process.memory_info().vms / (1024**2):.2f} MB")
```

**Solution 3: Cleanup and Retry**
```python
import pynlm

# Clean up before trying again
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start with small brain

brain = pynlm.createBrain(config)
brain.initialize()

print("Small brain created and initialized successfully")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Memory usage: {brain.getTotalNeuronCount() * 44 / (1024**2):.2f} MB")
```

#### Problem: Segmentation Faults
**Symptoms**:
- Program crashes with segmentation fault
- Often indicates memory corruption or buffer overflow
- Usually points to programming errors or invalid data

**Solutions**:

**Solution 1: Memory Debugging**
```bash
# Use valgrind for memory debugging
valgrind --tool=memcheck --leak-check=yes ./nlm --benchmark --neurons=1000 --steps=1000

# Use AddressSanitizer for runtime memory checks
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j$(nproc)
./nlm --benchmark --neurons=1000 --steps=1000
```

**Solution 2: Input Validation**
```python
# Validate input parameters before processing
def safe_brain_step(brain, step_number):
    try:
        # Validate step number
        if not isinstance(step_number, int) or step_number < 0:
            raise ValueError(f"Invalid step number: {step_number}")
        
        # Validate brain state
        if brain is None:
            raise ValueError("Brain is None")
        
        # Try the step
        brain.step(step_number)
        return True
        
    except ValueError as e:
        print(f"Input validation error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error during step {step_number}: {e}")
        return False
```

**Solution 3: System Stability**
```python
# Check system stability
import psutil
import time

print("System stability check:")
print(f"  Memory pressure: {psutil.virtual_memory().percent}%")
print(f"  CPU temperature: {psutil.sensors_temperatures().get('coretemp', [0])[0].current if psutil.sensors_temperatures() else 'N/A'}°C")
print(f"  Disk space: {psutil.disk_usage('/').percent}%")

# Check for memory leaks
process = psutil.Process()
print(f"  Memory growth: {process.memory_info().rss / (1024**2):.2f} MB RSS")
```

### 4. Python-Specific Issues

#### Problem: Python API Errors
**Symptoms**:
- Python API function calls fail
- Invalid parameter types
- Missing required parameters

**Solutions**:

**Solution 1: Parameter Validation**
```python
import pynlm

# Validate Python API parameters
def safe_create_brain(config):
    try:
        # Validate config type
        if not hasattr(config, 'has') or not hasattr(config, 'getInt'):
            raise ValueError("Invalid config object")
        
        # Validate required parameters
        if not config.has("brain.neuron_count"):
            config.set("brain.neuron_count", 1000)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
```

**Solution 2: Error Handling**
```python
# Comprehensive error handling for Python API
import pynlm

def robust_python_api_call():
    try:
        # Your Python API calls here
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            
        return brain
        
    except pynlm.NLMException as e:
        print(f"NLM Error: {e}")
        # Handle NLM-specific errors
        return None
        
    except pynlm.SimulationError as e:
        print(f"Simulation Error: {e}")
        # Handle simulation errors
        return None
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        # Handle all other errors
        return None
```

**Solution 3: Type Checking**
```python
# Type checking for Python API
def validate_api_parameters(**kwargs):
    """Validate Python API parameters."""
    
    # Expected types
    expected_types = {
        'config': pynlm.Config,
        'world': pynlm.SimpleWorld,
        'agent': pynlm.AgentBrain,
        'brain': pynlm.Brain,
        'steps': int,
        'enable': bool,
        'size': (int, float)
    }
    
    for param, value in kwargs.items():
        if param in expected_types:
            expected = expected_types[param]
            if not isinstance(value, expected):
                if isinstance(expected, tuple):
                    if not any(isinstance(value, t) for t in expected):
                        print(f"Warning: Parameter '{param}' should be {expected}, got {type(value)}")
                else:
                    print(f"Warning: Parameter '{param}' should be {expected.__name__}, got {type(value)}")
    
    return True
```

### 5. Performance Issues

#### Problem: Slow Performance
**Symptoms**:
- Low simulation throughput
- High memory usage
- Long simulation times
- System resource exhaustion

**Solutions**:

**Solution 1: Performance Monitoring**
```python
import pynlm
import time

# Performance monitoring
def monitor_performance(brain, num_steps, interval=10):
    print(f"Performance monitoring for {num_steps} steps...")
    print(f"{'Step':>6} {'Time':>8} {'Neurons/sec':>12} {'Spikes/sec':>12} {'Memory (MB)':>12}")
    print("-" * 60)
    
    total_time = 0
    start_time = time.time()
    
    for step in range(num_steps):
        step_start = time.time()
        brain.step(step)
        step_end = time.time()
        
        total_time += step_end - step_start
        
        if step % interval == 0:
            neurons_per_sec = brain.getTotalNeuronCount() / (step_end - start_time)
            spikes_per_sec = brain.getTotalSpikeCount() / (step_end - start_time)
            memory_mb = brain.getTotalNeuronCount() * 44 / (1024 * 1024)
            
            print(f"{step:6d} {total_time:8.3f} {neurons_per_sec:12.0f} {spikes_per_sec:12.0f} {memory_mb:12.2f}")
    
    average_time = total_time / num_steps
    average_neurons_per_sec = brain.getTotalNeuronCount() / total_time
    average_spikes_per_sec = brain.getTotalSpikeCount() / total_time
    
    print("-" * 60)
    print(f"{'AVERAGE':>6} {total_time:8.3f} {average_neurons_per_sec:12.0f} {average_spikes_per_sec:12.0f} {brain.getTotalNeuronCount() * 44 / (1024 * 1024):12.2f}")
    
    return {
        'average_steps_per_sec': num_steps / total_time,
        'average_neurons_per_sec': average_neurons_per_sec,
        'average_spikes_per_sec': average_spikes_per_sec,
        'total_time': total_time
    }
```

**Solution 2: Memory Optimization**
```python
# Memory optimization tips
def optimize_memory_usage(brain, config):
    print("=== Memory Optimization ===")
    
    # Check current memory usage
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    
    print(f"Current memory usage:")
    print(f"  Neurons: {neurons} x 44 bytes = {neurons * 44 / (1024**2):.2f} MB")
    print(f"  Synapses: {synapses} x 24 bytes = {synapses * 24 / (1024**2):.2f} MB")
    print(f"  Estimated total: {(neurons * 44 + synapses * 24) / (1024**2):.2f} MB")
    
    # Optimization suggestions
    if neurons > 10000:
        print(f"\nMemory optimization suggestions:")
        print(f"  - Disable episodic memory if not needed")
        print(f"  - Disable concept formation if not needed")
        print(f"  - Reduce neuron count for testing")
        print(f"  - Use memory pools for production")
    
    # Check configuration
    print(f"\nConfiguration check:")
    if config.has("performance.memory_pool_size"):
        pool_size = config.getInt("performance.memory_pool_size")
        print(f"  Memory pool: {pool_size / (1024*1024):.2f} MB")
    
    # Estimate memory requirements for different scales
    print(f"\nMemory requirements for different scales:")
    for scale, scale_neurons in [("Micro", 100), ("Small", 1000), ("Medium", 10000), ("Large", 100000)]:
        memory_mb = scale_neurons * 44 / (1024 * 1024)
        print(f"  {scale:8s} ({scale_neurons:6d} neurons): {memory_mb:8.2f} MB")
```

**Solution 3: Parallel Performance**
```python
# Parallel performance optimization
def optimize_parallel_performance(brain, config):
    print("=== Parallel Performance Optimization ===")
    
    # Check thread configuration
    if config.has("performance.parallel_threads"):
        threads = config.getInt("performance.parallel_threads")
        print(f"Thread configuration: {threads} threads")
        
        # Check for SIMD support
        import cpuinfo
        cpu_info = cpuinfo.get_cpu_info()
        
        flags = cpu_info.get('flags', [])
        
        if 'avx512f' in flags:
            print("  AVX-512: Full vectorization supported")
            config.set("performance.simd_level", "AVX-512")
        elif 'avx2' in flags:
            print("  AVX2: Vectorization supported")
            config.set("performance.simd_level", "AVX2")
        elif 'sse4.2' in flags:
            print("  SSE4.2: Basic vectorization supported")
            config.set("performance.simd_level", "SSE4.2")
        
        # Check CPU core count
        cpu_count = psutil.cpu_count()
        print(f"  CPU cores: {cpu_count}")
        
        if threads > cpu_count:
            print(f"  Warning: Threads ({threads}) > CPU cores ({cpu_count})")
            print(f"  Suggestion: Set threads to {cpu_count} or use 0 for auto-detect")
```

### 6. Advanced Troubleshooting

#### Problem: Intermittent Issues
**Symptoms**:
- Problems occur randomly
- Issues depend on system load
- Memory-related problems

**Solutions**:

**Solution 1: Reproducibility**
```python
# Ensure reproducible results
import random
import numpy as np

# Set random seeds for reproducibility
random.seed(42)
np.random.seed(42)

# Save configuration with seeds
config = pynlm.createDefaultConfig()
config.set("brain.random_seed", 42)
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Run multiple times to check consistency
results = []
for run in range(5):
    # Reset random state
    random.seed(42 + run)
    np.random.seed(42 + run)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Record results
    results.append({
        'run': run,
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'rate': brain.getAverageFiringRate()
    })

# Check consistency
print("Consistency check:")
for i, result in enumerate(results):
    print(f"  Run {result['run']}: {result['spikes']} spikes, {result['rate']:.2f} Hz")

# Calculate variance
spike_counts = [r['spikes'] for r in results]
spike_variance = np.var(spike_counts)
print(f"Spike variance: {spike_variance:.2f}")
```

**Solution 2: System Stress Testing**
```python
# Stress test system under load
import subprocess
import time
import os

# Create stress test script
stress_test_script = """
import pynlm
import time
import sys

# Reduce output for stress test
import sys
sys.stdout = None

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run intensive simulation
for step in range(1000):
    brain.step(step)

# Print final stats
sys.stdout = sys.__stdout__
print(f"Stress test completed: {brain.getTotalSpikeCount()} spikes")
"""

# Write and run stress test
with open("stress_test.py", "w") as f:
    f.write(stress_test_script)

# Run stress test
result = subprocess.run([sys.executable, "stress_test.py"], 
                       capture_output=True, text=True, timeout=60)

if result.returncode == 0:
    print("Stress test completed successfully")
    print(f"Output: {result.stdout}")
else:
    print(f"Stress test failed: {result.stderr}")
```

**Solution 3: Continuous Monitoring**
```python
# Continuous system monitoring
def monitor_system_continuously(brain, duration_minutes=10):
    print(f"=== Continuous System Monitoring ({duration_minutes} minutes) ===")
    
    start_time = time.time()
    end_time = start_time + (duration_minutes * 60)
    
    while time.time() < end_time:
        # Get current brain state
        neurons = brain.getTotalNeuronCount()
        active = brain.getActiveNeuronCount()
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount()
        rate = brain.getAverageFiringRate()
        
        # Get system information
        process = psutil.Process()
        memory_rss = process.memory_info().rss / (1024**2)
        memory_vms = process.memory_info().vms / (1024**2)
        
        # Check CPU
        cpu_percent = psutil.cpu_percent(interval=0.1)
        
        # Print status
        current_time = time.time()
        print(f"{current_time - start_time:6.1f}s: "
              f"Neurons:{neurons:5d} Active:{active:5d} "
              f"Firing:{firing:5d} Spikes:{spikes:8d} "
              f"Rate:{rate:6.2f}Hz "
              f"Mem:{memory_rss:8.2f}MB "
              f"CPU:{cpu_percent:5.1f}%")
        
        # Wait for next update
        time.sleep(5)
    
    print(f"Monitoring completed after {duration_minutes} minutes")
```

### 7. Documentation and Resources

#### Problem: Lack of Information
**Symptoms**:
- Need more information about specific topics
- Cannot find examples or documentation
- Need help with advanced features

**Solutions**:

**Solution 1: Documentation Resources**
```python
# Comprehensive documentation resources
resources = {
    "Python API Reference": "https://nlm.readthedocs.io/en/latest/python-api.html",
    "Architecture Documentation": "https://nlm.readthedocs.io/en/latest/architecture/index.html",
    "Configuration Guide": "https://nlm.readthedocs.io/en/latest/configuration/index.html",
    "Performance Guide": "https://nlm.readthedocs.io/en/latest/performance/index.html",
    "Tutorial Series": "https://nlm.readthedocs.io/en/latest/tutorials/index.html",
    "GitHub Repository": "https://github.com/nlm-project/nlm",
    "Issue Tracker": "https://github.com/nlm-project/nlm/issues",
    "Discussions Forum": "https://github.com/nlm-project/nlm/discussions",
    "Examples Gallery": "https://github.com/nlm-project/nlm/tree/main/examples",
    "API Documentation": "https://nlm.readthedocs.io/en/latest/api-reference.html"
}

print("Documentation and Resources:")
for title, url in resources.items():
    print(f"  {title}: {url}")
```

**Solution 2: Online Help**
```python
# Online help resources
def get_help(topic):
    help_resources = {
        "installation": [
            "NLM Installation Guide",
            "Python Dependencies",
            "Build Instructions"
        ],
        "configuration": [
            "Configuration Reference",
            "Parameter Documentation",
            "Preset Configurations"
        ],
        "python_api": [
            "Python API Reference",
            "Code Examples",
            "Error Handling"
        ],
        "troubleshooting": [
            "Common Issues",
            "Debugging Techniques",
            "Performance Optimization"
        ]
    }
    
    if topic in help_resources:
        print(f"Help for '{topic}':")
        for resource in help_resources[topic]:
            print(f"  - {resource}")
    else:
        print(f"Help topic '{topic}' not found")
    
    print(f"\nAdditional resources:")
    print(f"  - GitHub Issues: Search for similar problems")
    print(f"  - Stack Overflow: Tag 'nlm' for questions")
    print(f"  - Documentation: https://nlm.readthedocs.io")
    print(f"  - Community: https://github.com/nlm-project/nlm/discussions")
```

## Troubleshooting Tools and Utilities

### System Monitoring Tools

#### 1. Performance Monitoring
```bash
# Install monitoring tools
sudo apt-get install htop iotop nethogs

# Monitor system resources
htop

# Monitor I/O
iotop

# Monitor network usage
nethosts
```

#### 2. Memory Monitoring
```bash
# Install memory monitoring tools
sudo apt-get install psmisc valgrind

# Monitor memory usage
free -h
vmstat 1 10

# Check for memory leaks
valgrind --tool=massif ./nlm --benchmark
```

#### 3. CPU Profiling
```bash
# Install CPU profiling tools
sudo apt-get install perf linux-tools-common

# Profile CPU usage
perf record -g ./nlm --benchmark
perf report

# Profile specific functions
perf record -g -p <PID> perf report
```

### Application Monitoring Tools

#### 1. NLM-Specific Monitoring
```python
# Custom NLM monitoring script
import pynlm
import time
import threading

class NLMMonitor:
    def __init__(self, brain, output_file="nlm_monitor.csv"):
        self.brain = brain
        self.output_file = output_file
        self.monitoring = False
        self.thread = None
        self.data = []
    
    def collect_data(self):
        while self.monitoring:
            data_point = {
                'timestamp': time.time(),
                'neurons': self.brain.getTotalNeuronCount(),
                'active': self.brain.getActiveNeuronCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'rate': self.brain.getAverageFiringRate()
            }
            self.data.append(data_point)
            time.sleep(1)
    
    def start(self):
        self.monitoring = True
        self.thread = threading.Thread(target=self.collect_data)
        self.thread.start()
    
    def stop(self):
        self.monitoring = False
        if self.thread:
            self.thread.join()
        
        # Save data to CSV
        import csv
        with open(self.output_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['timestamp', 'neurons', 'active', 'firing', 'spikes', 'rate'])
            for point in self.data:
                writer.writerow([point['timestamp'], point['neurons'], 
                               point['active'], point['firing'], 
                               point['spikes'], point['rate']])
    
    def analyze(self):
        import pandas as pd
        import matplotlib.pyplot as plt
        
        df = pd.DataFrame(self.data)
        
        # Create plots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Neurons over time
        axes[0, 0].plot(df['timestamp'], df['neurons'])
        axes[0, 0].set_title('Total Neurons')
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Neurons')
        
        # Firing rate over time
        axes[0, 1].plot(df['timestamp'], df['rate'])
        axes[0, 1].set_title('Firing Rate')
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Hz')
        
        # Active neurons over time
        axes[1, 0].plot(df['timestamp'], df['active'])
        axes[1, 0].set_title('Active Neurons')
        axes[1, 0].set_xlabel('Time (s)')
        axes[1, 0].set_ylabel('Neurons')
        
        # Spikes over time
        axes[1, 1].plot(df['timestamp'], df['spikes'])
        axes[1, 1].set_title('Total Spikes')
        axes[1, 1].set_xlabel('Time (s)')
        axes[1, 1].set_ylabel('Spikes')
        
        plt.tight_layout()
        plt.savefig('nlm_monitor_analysis.png')
        plt.show()
        
        # Statistical analysis
        print("Statistical Analysis:")
        print(f"  Average firing rate: {df['rate'].mean():.2f} Hz")
        print(f"  Standard deviation: {df['rate'].std():.2f} Hz")
        print(f"  Maximum firing rate: {df['rate'].max():.2f} Hz")
        print(f"  Minimum firing rate: {df['rate'].min():.2f} Hz")
        print(f"  Neurons variation: {df['neurons'].std():.0f}")
```

#### 2. Automated Testing
```bash
# Install testing frameworks
sudo apt-get install python3-pytest python3-nose

# Run NLM tests
pytest tests/ --verbose --tb=short

# Run performance tests
pytest tests/performance/ --benchmark-only

# Run integration tests
pytest tests/integration/ --integration

# Run all tests
pytest tests/ --all
```

### Emergency Procedures

#### Immediate Response to Critical Issues

1. **System Crash**
   ```bash
   # Check system status
   systemctl status nlm
   ps aux | grep nlm
   
   

2. **Memory Exhaustion**
   ```bash
   # Check memory usage
   free -h
   ps aux --sort=-%mem | head -10
   
   # Kill processes
   sudo pkill -f "nlm"
   ```

3. **File System Issues**
   ```bash
   # Check disk space
   df -h
   
   # Clean up temporary files
   sudo find /tmp -name "*nlm*" -delete
   ```

#### Recovery Procedures

1. **Configuration Recovery**
   ```bash
   # Backup current configuration
   cp config.json config.json.backup
   
   # Restore to defaults
   cp /path/to/default_config.json config.json
   ```

2. **State Recovery**
   ```bash
   # Save brain state before changes
   brain.save("before_state.bin")
   
   # Restore if needed
   brain.load("before_state.bin")
   ```

3. **System Recovery**
   ```bash
   # Reinstall NLM
   sudo apt-get remove nlm
   sudo apt-get purge nlm
   pip uninstall pynlm
   
   # Reinstall
   pip install -e .
   ```

## Conclusion

Effective troubleshooting combines systematic debugging, comprehensive documentation, and practical problem-solving techniques. By following the approaches outlined in this guide, users can:

1. **Identify the root cause** of issues quickly and accurately
2. **Apply appropriate solutions** based on the problem category
3. **Prevent recurrence** through proper configuration and validation
4. **Monitor system health** to detect issues early
5. **Utilize available resources** for additional assistance

The key to successful troubleshooting is to:

- **Start simple** and work through problems systematically
- **Use available tools** and documentation effectively
- **Document solutions** for future reference
- **Share knowledge** with the broader NLM community
- **Contribute improvements** to the troubleshooting process

By mastering these troubleshooting techniques, users can maximize their productivity with NLM and resolve issues efficiently while learning about the system's capabilities and limitations.

---

*Troubleshooting Guide Version*: 2.0
*Last Updated*: 2026-10-09
*Troubleshooting Status*: Complete

**Related Documentation**:
- [Python API Reference](python-api/index.md) - Function details and error codes
- [Configuration Guide](configuration/index.md) - Valid parameter values
- [Performance Guide](performance/index.md) - Performance optimization
- [Tutorial Series](tutorials/index.md) - Practical examples and exercises

**Additional Resources**:
- [GitHub Issues](https://github.com/nlm-project/nlm/issues) - Bug reports and feature requests
- [Discussions Forum](https://github.com/nlm-project/nlm/discussions) - Community support
- [Documentation Wiki](https://github.com/nlm-project/nlm/wiki) - Community contributions
- [Release Notes](https://github.com/nlm-project/nlm/releases) - Version history and changes

**Emergency Support**:
- Report critical issues to the development team
- Use issue templates for faster resolution
- Include system information and configuration details
- Provide reproduction steps and expected vs. actual results