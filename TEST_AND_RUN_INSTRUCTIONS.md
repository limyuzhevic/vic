# NLM Project - Test and Run Instructions

This document provides instructions for running the NLM (Neural Learning Machine) project, including tests for new functionality.

## Quick Start

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pip**: Python package installer
- **Virtual environment** (recommended)

### Installation Steps

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd nlm-project
   ```

2. **Build the C++ project**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Install Python bindings**
   ```bash
   # Using the system's Python
   pip install .
   
   # Or in a virtual environment (recommended)
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   pip install -e .
   ```

4. **Verify installation**
   ```python
   import pynlm
   print(f"NLM version: {pynlm.__version__}")
   
   # Create a simple brain
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
   ```

## Running Tests

### Option 1: Run All Tests (Recommended)

This will run all C++ unit tests and Python tests:

```bash
# Navigate to the build directory
cd build

# Run C++ tests
ctest --output-on-failure

# Return to project root
cd ..

# Run Python tests
python test_nlm_improvements.py
```

### Option 2: Run Only Python Tests

```bash
# If you only want to test Python functionality
python test_nlm_improvements.py
```

### Option 3: Build and Run Examples

```bash
# Build the project (if not already built)
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run the integration demo
./nlm_phase6_demo
```

## Running the Test Suite

### Test File: `test_nlm_improvements.py`

This test file demonstrates all new functionality added to the NLM project. It includes:

1. **Basic Agent Testing**: Tests agent creation and basic functionality
2. **Advanced Sensory Processing**: Tests sensory validation and signal processing
3. **Debugging Capabilities**: Tests monitoring and debugging features
4. **AgentBrain Enhancements**: Tests specific AgentBrain improvements
5. **Performance Monitoring**: Tests performance tracking features
6. **System Calibration**: Tests system calibration capabilities

### Test Output

When you run the test suite, you should see output like this:

```
NLM Improvements Test Suite
============================================================
Testing new functionality and enhancements...

============================================================
Test 1: Basic Agent with Enhanced Features
============================================================

Step 0:
  Firing neurons: 23
  Total spikes: 45
  Exploration rate: 0.100
  Exploitation rate: 0.900
  Curiosity level: 0.000
  Novelty level: 0.000
  Neuromodulation level: 0.000
  ...

✓ Basic agent test passed!

============================================================
Test 2: Advanced Sensory Processing
============================================================
✓ Percept validation passed
Max value: 1.000
Min value: 0.000
Mean value: 0.502
Variance: 0.083
✓ Normalization applied
✓ Clamping applied
✓ Noise added
✓ Smoothing applied
✓ Copy successful
✓ Serialization/deserialization successful
Difference between original and loaded: 0.000001

✓ Advanced sensory processing test passed!

✓ All tests passed!
```

## New Features Demonstration

### 1. Enhanced AgentBrain

The AgentBrain class now includes:

- **Organized neuron grouping**: Motor and sensory neurons are organized by function
- **Advanced configuration**: Fine-grained control over brain subsystems
- **Exploration-exploitation balance**: Configurable learning behavior
- **Multi-time-scale learning**: Immediate and delayed learning effects
- **Activity-dependent plasticity**: Synaptic changes based on neural activity

### 2. Enhanced SensoryPercept

The SensoryPercept class now includes:

- **Input validation**: Comprehensive validation of sensory data
- **Signal processing**: Normalization, clamping, smoothing, noise addition
- **Statistics calculation**: Max, min, mean, and variance calculations
- **Serialization**: Save and load functionality for persistence
- **Copy operations**: Deep copy functionality for checkpointing

### 3. Debugging and Monitoring

The project now includes:

- **Performance monitoring**: Real-time tracking of system metrics
- **Event logging**: Detailed logs of system events and state changes
- **System health monitoring**: Continuous health assessment
- **Calibration support**: System calibration with error reporting
- **System status**: Comprehensive status reporting

## Configuration and Usage

### Creating a Custom Agent

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Optionally modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Configure behavior
agent.setTargetExplorationRates(0.15, 0.85)
agent.setExplorationDecay(0.98)
agent.setExploitationDecay(0.98)

# Enable debugging
agent.enablePerformanceMonitoring(True)
agent.enableEventLogging(True)
agent.setLogLevel(pynlm.LogLevel.Info)
```

### Running a Simulation

```python
# Run the agent simulation
def run_agent_simulation(num_steps=1000):
    for step in range(num_steps):
        world.update(0.1)  # Update world state
        
        percept = world.getSensoryPercept()  # Get sensory input
        agent.processSensoryInput(percept)  # Process in brain
        
        brain.step(step)  # Run brain computation
        
        motor_cmd = agent.decodeMotorCommand()  # Get action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())  # Execute action
        
        # Apply reward and learning
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"{brain.getFiringNeuronCount()} firing, "
                  f"{agent.getCuriosityLevel():.3f} curiosity, "
                  f"{agent.getNoveltyLevel():.3f} novelty")

# Run the simulation
run_agent_simulation(1000)
```

### Using Checkpoints

```python
# Save checkpoint for recovery
agent.checkpoint("simulation_checkpoint.dat")

# To load a checkpoint, you would typically:
# agent.loadCheckpoint("simulation_checkpoint.dat")
```

## Troubleshooting

### Common Issues

1. **"ModuleNotFoundError: No module named 'pynlm'"**
   - Ensure the Python package is installed: `pip install .`
   - Try in a fresh virtual environment

2. **"ImportError: libnlm.so: cannot open shared object file"**
   - Ensure C++ libraries are built and in the library path
   - On Linux: `LD_LIBRARY_PATH=./build/src` before running

3. **"Segmentation fault"**
   - Ensure `initialize()` is called before `step()`
   - Check configuration parameters
   - Try running with simpler configurations

4. **Performance issues**
   - Use Release build: `cmake .. -DCMAKE_BUILD_TYPE=Release`
   - Limit the number of neurons for testing
   - Disable advanced features if not needed

### Debugging Tips

1. **Enable verbose logging**:
   ```python
   agent.setLogLevel(pynlm.LogLevel.Debug)
   agent.enableEventLogging(True)
   ```

2. **Check system status**:
   ```python
   status = agent.getSystemStatus()
   print(status)
   ```

3. **Monitor performance**:
   ```python
   health = agent.getSystemHealth()
   uptime = agent.getUptime()
   ```

## Documentation

### Key Documentation Files

- **`README.md`**: Main project documentation
- **`HOW_TO_USE.md`**: Beginner-friendly usage guide
- **`easy_usage.md`**: Simple examples for quick start
- **`NLM_IMPROVEMENTS.md`**: Detailed improvements documentation
- **`docs/ARCHITECTURE.md`**: System architecture overview

### Python API Reference

The Python API provides high-level access to NLM functionality:

- **Config class**: Configuration management
- **Brain class**: Core neural computation
- **AgentBrain class**: Agent interface
- **SimpleWorld class**: Environment simulation
- **Constants**: Neuron types, developmental stages, action types

## Build Options

### CMake Configuration

```bash
# Configure with different options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=/path/to/python \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_TESTS=ON

# Build options
make -j$(nproc)          # Parallel build
make install             # Install to system
ctest                    # Run tests
```

### Python Build Options

```bash
# Build and install
pip install .

# Install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build
```

## Next Steps

1. **Run the tests** to verify new functionality
2. **Study the examples** in the test file
3. **Read the documentation** for detailed information
4. **Experiment with configurations** to find optimal settings
5. **Extend the code** with new features and functionality

## Version Information

- **Project**: NLM (Neural Learning Machine)
- **Version**: 0.1.0
- **Phase**: 6 (Final Integration)
- **License**: MIT
- **Python Compatibility**: 3.8+
- **C++ Standard**: C++20

## Support

For questions, issues, or enhancements:

1. **Check documentation** in this repository
2. **Run tests** to identify specific issues
3. **Report bugs** with detailed error information
4. **Submit feature requests** for new functionality
5. **Contribute code** improvements and extensions

---

*Created by: AI Assistant*
*Date: 2026-09-29*
*Purpose: NLM Project Improvements Documentation*