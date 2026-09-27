# NLM Framework Troubleshooting Guide

A systematic approach to diagnosing and resolving issues in the NLM (Neural Learning Machine) framework.

## 1. Common Error Patterns and Solutions

### 1.1 Build and Compilation Errors

**Error Pattern:** CMake configuration failures or compilation errors

**Common Causes:**
- Missing compiler requirements (C++20 support, GCC 10+, Clang 12+, or MSVC 2019+)
- CMake version too old (< 3.16)
- Missing pybind11 dependency
- Python bindings not properly configured

**Solutions:**
```bash
# Fix missing dependencies
# Ubuntu/Debian
./scripts/install_deps.sh

# For pybind11 issues
pip install pybind11 scikit-build-core

# Reconfigure CMake
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPython_EXECUTABLE=$(which python)
```

**Files to check:** CMakeLists.txt, pyproject.toml, setup.py

### 1.2 Runtime Errors

**Error Pattern:** Segmentation faults or access violations

**Common Causes:**
- Not calling `initialize()` before `step()` (see nlm_agent/AgentBrain.cpp:183)
- Using uninitialized memory
- Buffer overflows in neural computations
- Race conditions in parallel processing

**Solutions:**
```cpp
// CHECK: Always initialize before use
brain.initialize();  // nlm_agent/AgentBrain.cpp:183

// CHECK: Reset state between runs
brain.reset();       // nlm/brain/Brain.cpp:127

// CHECK: Use valid memory access patterns
// Ensure all pointers are null-checked before use
```

### 1.3 Memory Management Issues

**Error Pattern:** Memory leaks or excessive memory consumption

**Common Causes:**
- Circular references in neural objects
- Not properly cleaning up synaptic connections
- Memory pools not being released
- Staging areas not cleared in phase5/ParallelProcessing.cpp

**Solutions:**
```cpp
// CHECK: Use memory pools properly
// MemoryPool.hpp:47 - verify pool cleanup

// CHECK: Release synaptic connections
synapse.reset();  // src/plasticity/PlasticityRule.cpp:167

// CHECK: Clean up neural populations
population.release();  // src/brain/NeuralPopulation.cpp:287
```

### 1.4 Simulation Instability

**Error Pattern:** Unstable firing rates or divergent learning

**Common Causes:**
- Learning rates too high (plasticity.stdp.learning_rate > 0.01)
- Network parameters not properly scaled
- Improper weight initialization (brain.initial_weight_mean outside [-1.0, 1.0])
- Lack of homeostatic mechanisms

**Solutions:**
```cpp
//CHECK: Config values for stability
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)
```

## 2. Step-by-Step Debugging Guide

### 2.1 Basic Debugging Workflow

1. **Verify Environment**
   ```bash
   # Check build environment
   ./configure.py --check
   
   # Validate Python bindings
   python -c "import pynlm; print(pynlm.__version__)"
   ```

2. **Reproduce with Minimal Steps**
   ```python
   # Minimal test case
   import pynlm
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   
   # Run simple simulation
   for i in range(100):
       brain.step(i)
   ```

3. **Isolate Problem Location**
   ```cpp
   // Add instrumentation
   // src/neuromodulation/Neuromodulator.cpp:142
   logger.log("STEP", step, "firing_count", brain.getFiringNeuronCount())
   ```

4. **Use Checkpoint System**
   ```bash
   # Save and compare states
   ./nlm --save checkpoint1.bin --steps 100
   ./nlm --load checkpoint1.bin --steps 100 --check
   ```

### 2.2 Advanced Debugging Techniques

1. **Memory Analysis**
   ```bash
   # Use valgrind for memory issues
   valgrind --leak-check=full ./nlm
   
   # For Python bindings
   valgrind --tool=massif python test_script.py
   ```

2. **Performance Profiling**
   ```bash
   # Profile execution time
   perf record ./nlm
   perf report
   
   # Profile memory usage
   /usr/bin/time -v ./nlm
   ```

3. **Debugging with GDB**
   ```bash
   gdb ./nlm
   (gdb) break Brain.cpp:145  # Check specific method
   (gdb) run
   (gdb) step 10
   (gdb) print variable_name
   ```

## 3. Environment-Specific Issues and Fixes

### 3.1 Docker/Container Issues

**Issue:** Missing system libraries or permissions

**Solutions:**
```dockerfile
# In Dockerfile, add necessary libraries
FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpython3-dev \
    python3-pip
```

**Container-specific problems:**
- `/tmp/kilo` mount path issues
- Sandbox restrictions on file access
- Container isolation limits

### 3.2 Cross-Platform Issues

**Windows-Specific Issues:**
```bash
# Path separators (use / instead of \\)
brain.save("checkpoint.bin")  // nlm/brain/Brain.cpp:285

# Windows build options
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
```

**macOS-Specific Issues:**
```bash
# OpenMP issues
export CXXFLAGS="-Xpreprocessor -fopenmp"
export LDFLAGS="-lomp"

# CMake 3.16+ required for OpenMP support
cmake .. -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp"
```

### 3.3 Development Environment Setup

**Virtual Environment Setup:**
```bash
python -m venv nlm_env
source nlm_env/bin/activate
pip install -r requirements.txt
pip install -e .
```

**Common development issues:**
- IPython integration problems
- Jupyter notebook compatibility
- Debugging with visualizers

## 4. Performance Troubleshooting

### 4.1 Slow Simulation Performance

**Diagnostic Approach:**
```bash
# Profile with different neuron counts
./nlm --neurons 100 --iterations 1000 --profile
./nlm --neurons 1000 --iterations 1000 --profile
./nlm --neurons 5000 --iterations 100 --profile
```

**Common Performance Issues:**

1. **Memory Access Patterns**
   - **Issue:** Random synapse access causing cache thrashing
   - **Location:** src/plasticity/PlasticityRule.cpp:167
   - **Fix:** Implement SoA (Structure of Arrays) for synaptic data

2. **Parallel Processing Bottlenecks**
   - **Issue:** Inefficient thread scheduling
   - **Location:** src/performance/ParallelProcessing.hpp:89
   - **Fix:** Use static scheduling with larger chunks

3. **Vectorization Issues**
   - **Issue:** Suboptimal SIMD usage
   - **Location:** src/performance/SIMDVectorization.hpp:23
   - **Fix:** Use explicit intrinsics instead of auto-vectorization

### 4.2 Memory Optimization

**Memory Leak Detection:**
```bash
# Check memory with different configurations
python -m memory_profiler test_script.py

# Use valgrind for C++ memory analysis
valgrind --leak-check=full --show-leak-kinds=all ./nlm
```

**Memory Optimization Techniques:**

1. **Use Memory Pools**
   ```cpp
   // src/performance/MemoryPool.hpp:47
   MemoryPool<Neuron> neuron_pool;
   Neuron* neuron = neuron_pool.allocate();
   ```

2. **Implement Sparse Storage**
   ```cpp
   // src/performance/SparseConnectivity.hpp:128
   if (connection_probability < rng.random()) {
       create_synapse(pre, post);
   }
   ```

3. **Checkpoint-Based Recovery**
   ```cpp
   // src/performance/CheckpointSystem.cpp:89
   brain.save("checkpoint_" + to_string(step) + ".bin");
   ```

## 5. Memory Management Troubleshooting

### 5.1 Memory Leaks

**Detection Methods:**
```bash
# Valgrind for memory leaks
valgrind --leak-check=full ./nlm

# AddressSanitizer for runtime errors
gcc -fsanitize=address -g ./nlm
```

**Common Memory Leaks:**

1. **Neural Population Cleanup**
   - **Issue:** NeuralPopulation not releasing neuron memory
   - **Location:** src/brain/NeuralPopulation.cpp:287
   - **Fix:** Implement proper cleanup in destructor

2. **Synapse Pool Management**
   - **Issue:** Synapse pools not being deallocated
   - **Location:** src/plasticity/PlasticityRule.cpp:167
   - **Fix:** Add cleanup to plasticity rule destructor

3. **Resource Cleanup in AgentBrain**
   - **Issue:** AgentBrain not releasing world resources
   - **Location:** nlm_agent/AgentBrain.cpp:183
   - **Fix:** Implement proper resource cleanup

### 5.2 Memory Corruption

**Detection Methods:**
```bash
# Use AddressSanitizer
gcc -fsanitize=address -g ./nlm

# Use valgrind for read/write errors
valgrind --tool=memcheck ./nlm
```

**Common Memory Corruption Issues:**

1. **Double-Free Issues**
   - **Issue:** Synapse being deleted twice
   - **Location:** src/plasticity/StructuralPlasticity.cpp:89
   - **Fix:** Use reference counting or ownership transfer

2. **Use-After-Free**
   - **Issue:** Accessing synapse after deletion
   - **Location:** src/neuromodulation/Neuromodulator.cpp:142
   - **Fix:** Null-check all pointer accesses

## 6. Integration Issue Resolution

### 6.1 Brain-Component Integration

**Integration Test Failures:**
```bash
# Run comprehensive integration test
cd build
./nlm_phase6_demo --test-mode

# Check specific component integration
./nlm_phase6_demo --validate memory
./nlm_phase6_demo --validate neuromodulation
```

**Common Integration Issues:**

1. **Memory System Integration**
   - **Issue:** Working memory not connected to neural processing
   - **Location:** src/experiments/Phase6Integration.cpp:167
   - **Fix:** Verify synaptic connections in NeuralWorkingMemory

2. **Neuromodulation Integration**
   - **Issue:** Dopamine not affecting neural excitability
   - **Location:** src/neuromodulation/Dopamine.cpp:89
   - **Fix:** Check neuromodulator eligibility traces

3. **Prediction System Integration**
   - **Issue:** Prediction errors not affecting learning
   - **Location:** src/prediction/NeuralPrediction.cpp:142
   - **Fix:** Verify prediction error eligibility traces

### 6.2 Agent-Brain-World Integration

**Common Integration Problems:**

1. **Sensory-Motor Loop**
   ```python
   # CHECK: Complete loop
   percept = world.getSensoryPercept()
   brain.receiveSensoryInput(percept)
   brain.step(step)
   action = brain.produceAction()
   world.applyMotorCommand(action, step)
   ```

2. **State Consistency**
   - **Issue:** World and brain state mismatch
   - **Location:** src/agent/SensoryPercept.cpp:89
   - **Fix:** Add state validation checks

3. **Development Integration**
   - **Issue:** Developmental stage progression not affecting plasticity
   - **Location:** src/development/DevelopmentSystem.cpp:89
   - **Fix:** Verify developmental stage access patterns

### 6.3 Checkpoint Integration

**Checkpoint Integration Issues:**

1. **Save/Load Consistency**
   - **Issue:** Different states saved vs. loaded
   - **Location:** src/performance/CheckpointSystem.cpp:127
   - **Fix:** Use serialization with versioning

2. **Checkpoint Recovery**
   ```bash
   # Save checkpoint
   ./nlm --save checkpoint.bin --steps 500
   
   # Load and continue
   ./nlm --load checkpoint.bin --steps 250
   ```

## 7. Systematic Problem-Solving Approach

### 7.1 Troubleshooting Methodology

1. **Isolate Problem**
   - Reproduce with minimal configuration
   - Separate input from processing
   - Use checkpoints for state comparison

2. **Identify Root Cause**
   - Check error logs for stack traces
   - Verify configuration values
   - Review recent changes

3. **Implement Fix**
   - Apply targeted changes
   - Test incrementally
   - Verify no regression

4. **Validate Solution**
   - Run integration tests
   - Check performance impact
   - Document changes

### 7.2 Documentation and Logging

**Add Comprehensive Logging:**
```cpp
// src/core/Logger/Logger.cpp:47
logger.log("DEBUG", "Simulation step", {{"step", step}, {"neurons", count}});
```

**Maintain Troubleshooting Documentation:**
- Add failures to docs/FAILURES.md
- Document solutions for common issues
- Include performance benchmarks

### 7.3 Testing and Validation

**Testing Strategy:**
```bash
# Unit tests
./nlm_test --filter "test_memory"

# Integration tests
./nlm_phase6_demo --integration

# Performance regression tests
./benchmark.py --scenario memory_usage
```

## 8. Common Workflows and Solutions

### 8.1 Complete Integration Test Failure

```bash
# Full integration test workflow
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo --thorough
```

### 8.2 Development Environment Setup

```bash
# Comprehensive environment setup
apt-get install -y build-essential cmake git python3 python3-pip
pip install pybind11 scikit-build-core pytest numpy
pip install -e .
```

### 8.3 Simulation Debugging Workflow

```bash
# Step-by-step debugging
source activate nlm
python debug_simulation.py --steps 1000 --neuron_count 1000 --plot
```

### 8.4 Memory Optimization Workflow

```bash
# Memory optimization process
valgrind --leak-check=full ./nlm
# Identify leaks
./nlm --profile-memory --neurons 5000
# Optimize
./nlm --apply-optimizations
```

## 9. Emergency Procedures

### 9.1 Simulation Crashes

```bash
# Quick recovery steps
./nlm --auto-save checkpoint.bin
cd build
./nlm_test --run-recording
cp checkpoint.bin recovery_checkpoint.bin
```

### 9.2 Build Failures

```bash
# Rapid build recovery
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### 9.3 Memory Exhaustion

```bash
# Memory recovery
./nlm --limit-memory 4GB --checkpoint-interval 100
pkill -f "nlm_simulation"
sudo swapoff -a && swapon -a
```

## 10. Resources and Support

- **Documentation:** docs/ARCHITECTURE.md, docs/SCIENCE.md
- **Development Guide:** docs/DEVELOPING.md
- **Performance Guide:** docs/PERFORMANCE.md
- **Known Issues:** docs/FAILURES.md
- **Community:** NLM Slack/Forum for user support
- **Issues:** GitHub issues for bug reporting

---
*This troubleshooting guide is a living document. Please update with new solutions and issues as they are discovered.*
