# NLM (Neural Learning Machine) - Installation and Usage Guide

**An Experimental Artificial Developmental Brain**

## Quick Start

### Prerequisites

```bash
# Operating Systems:
- Linux (Ubuntu 20.04+, CentOS 8+)
- macOS 13+ (with Xcode command line tools)
- Windows 10+ (Windows Subsystem for Linux 2)

# Required Tools:
- C++ compiler supporting C++20 (GCC 11+, Clang 14+, MSVC 2022+)
- CMake 3.16 or higher
- Python 3.8 or higher
- pip (Python package installer)

# Optional but recommended:
- Build tool (make/ninja)
- Git
- VS Code or any C++ IDE
```

### Installation Methods

#### Option 1: Full Source Installation (Recommended)

```bash
# Clone the repository
cd /path/to
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the C++ core library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20
make -j$(nproc)

# Install Python bindings
pip install .
```

#### Option 2: Python Package Only (Quick Start)

```bash
# Install Python bindings directly
pip install nlm

# This will build the extensions automatically
```

#### Option 3: Development Installation

```bash
# Install in development mode for editing
pip install -e .

# Then you can edit the Python bindings and recompile
```

### Verify Installation

```python
import pynlm

print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Building from Source (Advanced)

### Detailed Build Options

#### Basic Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### Debug Build
```bash
mkdir debug-build
cd debug-build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

#### Custom Build Options
```bash
cd build
cmake .. \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DZERO_TO_ONE_PLATINUM_BUILD=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON
```

### Build Outputs

After building, you'll have:

| Component | File | Purpose |
|-----------|------|---------|
| `nlm` | Executable | Main simulation executable |
| `libnlm_core.so` | Static Library | Core brain components |
| `libnlm_agent.so` | Static Library | Agent system |
| `libnlm_world.so` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |

### Building Python Package

#### Using scikit-build (Default)
```bash
# Install dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install
pip install .

# Build only (without installing)
pip install build
python -m build
```

#### Using Modern Python Packaging
```bash
# Direct pip install handles everything automatically
pip install .

# For development with editable installs
pip install -e .
```

### Running Tests

#### C++ Unit Tests
```bash
# In build directory
cd build
ctest --output-on-failure
```

#### Python Tests
```bash
# Run Python unit tests
pytest

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_integration.py
```

#### Integration Tests
```bash
# Run Phase 6 integration test
./build/nlm_phase6_demo

# Run demonstration executables
./build/nlm_phase3_demo
./build/nlm_phase4_demo
```

## Basic Usage

### 1. Simple Brain Simulation

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Simple Brain Simulation ===")
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing neurons: {brain.getFiringNeuronCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Final spike count: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 2. Complete Agent in World

```python
import pynlm
import random

def run_complete_simulation(steps=500):
    """Run a complete brain-world-agent simulation."""
    
    print("=== Complete Agent Simulation ===")
    
    # 1. Create brain and initialize
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30.0,
        height=30.0,
        visionWidth=16,
        visionHeight=16
    )
    world.reset()
    
    # Set random agent start position
    brainPos = brain.getBrainPosition()
    world.setAgentStart(brainPos.x, brainPos.y)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Add some resources and hazards to world
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=5.0))
    world.addObject(pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Hazard, value=3.0))
    
    # 5. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motorCmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motorCmd, world.getSimulationTime())
        
        # Apply reward modulation
        actionResult = world.getAgentBody().getLastActionResult()
        if actionResult:
            reward = actionResult.reward
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("=== Simulation Complete ===")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(500)
```

### 3. Learning Experiment

```python
import pynlm
import matplotlib.pyplot as plt

def demonstrate_learning():
    """Demonstrate synaptic plasticity and learning."""
    
    print("=== Learning Demonstration ===")
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    
    # Configure for learning experiment
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource::Runtime)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource::Runtime)
    config.set("plasticity.structural.enable", True, pynlm.ConfigSource::Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getFiringNeuronCount()
    
    print(f"Initial spikes: {initial_spikes}")
    print(f"Initial active neurons: {initial_neurons}")
    
    # Apply learning stimulus
    for step in range(1000):
        # Create correlated input to stimulate STDP
        for i in range(5):
            brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 40.0f)
        
        brain.step(step)
        
        # Log learning progress
        if step % 200 == 0:
            current_spikes = brain.getTotalSpikeCount() - initial_spikes
            print(f"Step {step}: {current_spikes} new spikes")
    
    # Final state
    final_spikes = brain.getTotalSpikeCount()
    final_neurons = brain.getFiringNeuronCount()
    
    print(f"Final spikes: {final_spikes}")
    print(f"Final active neurons: {final_neurons}")
    print(f"Total learning events: {final_spikes - initial_spikes}")
    
    return brain

# Run learning demonstration
brain = demonstrate_learning()
```

### 4. Development Simulation

```python
import pynlm

def demonstrate_development():
    """Demonstrate brain development over time."""
    
    print("=== Development Simulation ===")
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent to handle development
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20.0, height=20.0, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Initial developmental stage:", brain.getDevelopmentalStage())
    
    # Simulate development over time
    for time_step in range(0, 1000, 100):
        # Run a few simulation steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(time_step + step)
            
            # Decode action and update world
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(1.0)
        
        print(f"Time {time_step}: {brain.getDevelopmentalStage()} "
              f"(plasticity: {agent.getBrain()->getPlasticityLevel():.3f})")
    
    print("Final developmental stage:", brain.getDevelopmentalStage())
    print(f"Final plasticity level: {agent.getBrain()->getPlasticityLevel():.3f}")
    
    return brain, agent, world

# Run development simulation
brain, agent, world = demonstrate_development()
```

## Advanced Usage

### 1. Configuration Management

```python
import pynlm
import json

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Access configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Current neuron count: {neuron_count}")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration summary
summary = config.summary()
print(f"Configuration summary:\n{summary}")
```

### 2. Error Handling

```python
import pynlm
import traceback

def safe_simulation():
    """Run simulation with proper error handling."""
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Initialize (check for errors)
        if not brain.initialize():
            print("ERROR: Failed to initialize brain")
            return False
        
        # Run simulation
        for step in range(100):
            brain.step(step)
            
            # Check for abnormal activity
            firing_rate = brain.getAverageFiringRate()
            if firing_rate > 100.0:  # Suspiciously high firing rate
                print(f"WARNING: High firing rate at step {step}: {firing_rate}")
        
        print("Simulation completed successfully")
        return True
        
    except Exception as e:
        print(f"ERROR: Simulation failed with exception: {e}")
        traceback.print_exc()
        return False

# Run safe simulation
safe_simulation()
```

### 3. Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.step_times = []
        self.memory_usage = []
    
    def record_step(self, step_time):
        self.step_times.append(step_time)
    
    def get_statistics(self):
        if not self.step_times:
            return None
        
        avg_time = sum(self.step_times) / len(self.step_times)
        min_time = min(self.step_times)
        max_time = max(self.step_times)
        
        return {
            'average_step_time': avg_time,
            'min_step_time': min_time,
            'max_step_time': max_time,
            'steps_per_second': 1.0 / avg_time if avg_time > 0 else 0,
            'total_steps': len(self.step_times)
        }

def run_performance_test(steps=1000):
    """Run performance test and report metrics."""
    
    print("=== Performance Test ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    monitor = PerformanceMonitor(brain)
    
    start_time = time.time()
    
    for step in range(steps):
        step_start = time.time()
        
        # Run one simulation step
        brain.step(step)
        
        step_end = time.time()
        step_time = step_end - step_start
        
        monitor.record_step(step_time)
        
        # Progress reporting
        if step % 200 == 0:
            stats = monitor.get_statistics()
            if stats:
                print(f"Step {step}: {stats['steps_per_second']:.1f} steps/sec")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\nPerformance Results:")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average step time: {total_time/steps*1000:.2f} ms")
    print(f"Steps per second: {steps/total_time:.1f}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    return monitor.get_statistics()

# Run performance test
stats = run_performance_test(1000)
```

### 4. Checkpoint and Recovery

```python
import pynlm
import os

def save_checkpoint(brain, filepath):
    """Save brain state to checkpoint file."""
    try:
        brain.save(filepath)
        print(f"Checkpoint saved to: {filepath}")
        return True
    except Exception as e:
        print(f"ERROR: Failed to save checkpoint: {e}")
        return False

def load_checkpoint(filepath):
    """Load brain state from checkpoint file."""
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.load(filepath)
        print(f"Checkpoint loaded from: {filepath}")
        return brain
    except Exception as e:
        print(f"ERROR: Failed to load checkpoint: {e}")
        return None

def demonstrate_checkpoint():
    """Demonstrate checkpoint save/load functionality."""
    
    print("=== Checkpoint Demonstration ===")
    
    # Create and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for step in range(100):
        brain.step(step)
    
    # Save checkpoint
    checkpoint_file = "simulation_checkpoint.bin"
    if save_checkpoint(brain, checkpoint_file):
        # Run more simulation
        for step in range(100, 200):
            brain.step(step)
        
        # Load checkpoint
        saved_brain = load_checkpoint(checkpoint_file)
        if saved_brain:
            print(f"Loaded brain has {saved_brain.getTotalNeuronCount()} neurons")
            print(f"Loaded brain has {saved_brain.getTotalSpikeCount()} spikes")
        
        # Clean up
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print("Checkpoint file cleaned up")
    
    return brain

# Run checkpoint demonstration
demonstrate_checkpoint()
```

## Platform-Specific Notes

### Linux

```bash
# Install system dependencies
apt-get update
apt-get install -y g++ cmake python3-dev python3-pip

# Build and install
mkdir build && cd build
cmake ..
make -j$(nproc)
pip install ..
```

### macOS

```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake python3

# Build and install
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
pip install ..
```

### Windows

```powershell
# Using WSL (Windows Subsystem for Linux)
# Follow Linux instructions above in WSL

# Or using native Windows with Visual Studio
try {
    # Install Visual Studio with C++ workload
    # Install CMake from https://cmake.org/download/
    # Install Python from https://www.python.org/downloads/
    
    # Build using CMake GUI or command line
    mkdir build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Release
    
    # Install Python bindings
    pip install ..
} catch {
    Write-Error "Installation failed: $_"
    exit 1
}
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Python Module Not Found**
```bash
pip install --force-reinstall pynlm
```

**Issue 2: CMake Cannot Find Python**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Issue 3: Compilation Errors**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**Issue 4: Runtime Errors**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration is valid

**Issue 5: Memory Issues**
- Monitor memory usage during long simulations
- Consider reducing brain size for memory constraints
- Use checkpointing for long-running simulations

### Getting Help

1. **Documentation**: Read `docs/ARCHITECTURE.md` for technical details
2. **Examples**: Check the examples in the repository
3. **Issues**: Report issues at GitHub repository
4. **Community**: Join the NLM Discord server for discussions

## Contributing

### Project Structure

```
NLM/
├── src/                    # C++ source code
│   ├── brain/            # Neural components
│   ├── agent/            # Agent system
│   ├── world/            # World simulation
│   ├── core/             # Core utilities
│   └── ...
├── python/                # Python bindings
├── tests/                 # Unit tests
├── docs/                  # Documentation
├── CMakeLists.txt         # C++ build system
├── pyproject.toml        # Python package configuration
├── README.md              # Main documentation
├── easy_usage.md          # Beginner guide
└── HOW_TO_USE.md          # Detailed API reference
```

### Code Style Guidelines

- Follow C++ Core Guidelines for C++ code
- Use pybind11 documentation for Python bindings
- Write comprehensive comments for complex algorithms
- Include error checking and validation
- Use consistent naming conventions

### Testing Guidelines

- Write unit tests for all major components
- Include integration tests for system interactions
- Add performance benchmarks
- Include edge case tests

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM is a research project by:
- Neural Learning Machine Research Team
- Institute for Artificial Intelligence

See `docs/RESEARCH_METHODS.md` for scientific background and references.

## Acknowledgements

Special thanks to:
- Contributors who have helped improve the system
- Users who have provided feedback and reported issues
- Open source community for pybind11 and other dependencies

---

*This documentation is continuously updated. Check the repository for the latest version.*
