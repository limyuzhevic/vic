# NLM (Neural Learning Machine) - Comprehensive Build and Usage Documentation

This document provides comprehensive information for building, installing, and using the NLM brain simulation framework.

## Table of Contents

1. [Building the Project](#building-the-project)
2. [Installing Python Libraries](#installing-python-libraries)
3. [Running Demos](#running-demos)
4. [Configuration Options](#configuration-options)
5. [Running Tests](#running-tests)
6. [Troubleshooting](#troubleshooting)
7. [Advanced Usage](#advanced-usage)
8. [Documentation Index](#documentation-index)

---

## Building the Project

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (optional, for Python bindings)
- **pybind11**: Version 2.11.0+ (optional, for Python bindings)

### Standard Build (C++ Only)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build with Python Bindings

```bash
# Install Python dependencies first
pip install pybind11 scikit-build-core pytest numpy

# Build with Python bindings enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)

# Install Python module
pip install .
```

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Disable Python bindings (default for plain C++ builds)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=OFF
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Phase 2 demo - basic neural computation with STDP and Hebbian learning |
| `nlm_phase3_demo` | Executable | Phase 3 demo - world interaction, reward prediction, and basic cognition |
| `nlm_phase4_demo` | Executable | Phase 4 demo - full cognition systems with concepts, self-model, and social learning |
| `nlm_phase5_demo` | Executable | Phase 5 demo - brain-personality, sandbox learning, and checkpoint persistence |
| `nlm_phase6_demo` | Executable | Phase 6 demo - final integration - comprehensive connectivity test of all brain systems |
| `nlm_phase6_text_demo` | Executable | Phase 6 text demo - text processing, concept formation, and social learning |
| `nlm_benchmark` | Executable | Performance benchmark measuring simulation speed at different scales |
| `nlm_test` | Executable | Unit test suite |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `pynlm` | Shared Library | Python bindings for NLM (if built with Python bindings) |

---

## Installing Python Libraries

### From Source (Recommended for Development)

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .

# For standalone python directory build
cmake -S python -B build
cmake --build build
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verification

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

### Troubleshooting Python Installation

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

---

## Running Demos

### Phase 2 Demo (Basic Neural Computation)

```bash
# Run the basic neural computation demo
./nlm

# With custom configuration
./nlm --config myconfig.cfg --neuron_count 2000
```

**What you see:**
- Basic LIF neuron dynamics
- STDP and Hebbian plasticity
- Event-driven spike propagation
- Structural plasticity (synaptogenesis/pruning)

### Phase 3 Demo (World Interaction)

```bash
# Run Phase 3 demo
./nlm_phase3_demo
```

**What you see:**
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 Demo (Full Cognition)

```bash
# Run Phase 4 demo
./nlm_phase4_demo
```

**What you see:**
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning

### Phase 5 Demo (Brain-Personality)

```bash
# Run Phase 5 demo
./nlm_phase5_demo

# With help flag
./nlm_phase5_demo --help
```

**What you see:**
- Pattern probe (fresh brain responses to different percepts)
- Sandbox run (closed-loop learning)
- Checkpoint round-trip (state persistence)

### Phase 6 Demo (Final Integration)

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# With help flag
./nlm_phase6_demo --help
```

**What you see:**
- Integration verification of all brain systems
- Memory integration test
- Neuromodulation integration test
- Checkpoint and replay tests
- Full integration experiment with results summary

### Phase 6 Text Demo (Text Processing)

```bash
# Run Phase 6 text demo
./nlm_phase6_text_demo

# With help flag
./nlm_phase6_text_demo --help
```

**What you see:**
- Text as a sensory channel
- Text association learning
- Concept formation from patterns
- Social learning (observation and imitation)
- Sandbox with teacher labels

### Performance Benchmark

```bash
# Run performance benchmarks
./nlm_benchmark
```

**What you see:**
- Performance measurements for different network sizes:
  - 100 neurons, 1000 steps
  - 500 neurons, 1000 steps
  - 1000 neurons, 500 steps
  - 2000 neurons, 200 steps
- Results summary showing steps/sec, spikes/step, synapses/neuron

---

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | Environment type |
| `environment.width` | int | 10 | World width |
| `environment.height` | int | 10 | World height |
| `environment.max_energy` | float | 100.0 | Maximum energy for agent |
| `environment.energy_decay_rate` | float | 0.01 | Energy decay rate per step |

### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log.level` | string | INFO | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `log.to_file` | bool | false | Enable file logging |
| `log.filename` | string | nlm.log | Log file name |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step (s) |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Simulation time limit (s)

---

## Running Tests

### Unit Tests

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory if not exists
mkdir -p build
cd build

# Configure with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

# Build tests
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Run specific test
cTest -R nlm_test
```

### Test Categories

- **test_types.cpp**: Core type tests
- **test_config.cpp**: Configuration system tests
- **test_random.cpp**: Random number generator tests
- **test_clock.cpp**: Simulation clock tests
- **test_neuron.cpp**: Neuron dynamics tests
- **test_synapse.cpp**: Synapse and plasticity tests
- **test_stdp.cpp**: STDP rule tests
- **test_brain.cpp**: Brain integration tests

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Missing Python dependencies:**
```bash
pip install pybind11 scikit-build-core pytest numpy
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Check disk space

### Runtime Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected
- Verify world configuration is correct

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent's motor neurons are active
- Check that agent's brain has output

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check neuron parameters (threshold, rest potential, etc.)
- Verify synaptic weights are non-zero

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for null pointer dereferences
- Verify array bounds

### Common Solutions

1. **Always call `brain.initialize()` before using brain**
2. **Use `brain.reset()` before re-running simulations**
3. **Check log level for detailed output**: `Logger::getGlobal().setLevel(LogLevel::Info)`
4. **Use smaller network sizes for debugging**: 100-500 neurons
5. **Add visualization**: Enable visualization for debugging (Phase 2 only)

---

## Advanced Usage

### Custom Configuration

```bash
# Create custom config file
cat > myconfig.cfg << EOF
# Custom NLM configuration
brain.neuron_count = 2000
brain.connection_probability = 0.15
plasticity.stdp.learning_rate = 0.005
neuromod.dopamine.scale = 2.0
environment.width = 50
environment.height = 50
log.level = DEBUG
EOF

# Use custom config
./nlm --config myconfig.cfg
```

### Batch Processing

```bash
# Create multiple config files
for size in 100 500 1000 2000; do
    cat > config_${size}.cfg << EOF
brain.neuron_count = ${size}
brain.connection_probability = 0.1
EOF
done

# Run benchmarks with different sizes
for size in 100 500 1000 2000; do
    ./nlm_benchmark --config config_${size}.cfg
    echo "--- Results for ${size} neurons ---"
done
```

### Python Scripting

```python
import pynlm
import time

# Batch processing multiple simulations
def run_batch_simulation(neuron_counts, steps):
    results = []
    
    for neuron_count in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        results.append({
            "neurons": neuron_count,
            "steps": steps,
            "time": end_time - start_time,
            "spikes": brain.getTotalSpikeCount(),
           -productive: brain.getAverageFiringRate()
        })
    
    return results

# Run batch
configs = [100, 500, 1000, 2000]
results = run_batch_simulation(configs, 1000)

for result in results:
    print(f"Size {result['neurons']} neurons: "
          f"{result['steps']/result['time']:.0f} steps/sec, "
          f"{result['spikes']/result['steps']:.1f} spikes/step")
```

### Parallel Processing

```bash
# Run multiple demos in parallel (if system supports it)
# Using GNU parallel
./nlm_phase3_demo &
./nlm_phase4_demo &
./nlm_phase5_demo &
./nlm_phase6_demo &

# Wait for all to complete
wait
```

---

## Documentation Index

- **Core Documentation**: `README.md`, `easy_usage.md`, `HOW_TO_USE.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Scientific Background**: `docs/SCIENCE.md`
- **Experiment Descriptions**: `docs/EXPERIMENTS.md`
- **Phase 6 Final Audit**: `docs/PHASE6_FINAL_AUDIT.md`
- **Limitations**: `docs/LIMITATIONS.md`
- **Roadmap**: `docs/ROADMAP.md`

### Quick Reference Commands

```bash
# Build project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run all demos
./nlm
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo
./nlm_phase6_demo
./nlm_phase6_text_demo
./nlm_benchmark

# Run tests
ctest --output-on-failure

# Install Python bindings
pip install .
```

---

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
