# NLM Python Troubleshooting Guide

## Overview

This guide covers common issues encountered when using the NLM Python bindings and provides step-by-step solutions. It includes real-world debugging scenarios and troubleshooting workflows.

## Table of Contents

- [Common Errors](#common-errors)
- [Installation Issues](#installation-issues)
- [Runtime Errors](#runtime-errors)
- [Performance Issues](#performance-issues)
- [Memory Problems](#memory-problems)
- [Debugging Techniques](#debugging-techniques)
- [Support and Resources](#support-and-resources)

## Common Errors

### Error 1: Module Not Found

```python
import pynlm
# Traceback:
# ModuleNotFoundError: No module named 'pynlm'
```

**Solution:**
1. Verify installation: `pip list | grep pynlm`
2. Install if needed: `pip install pynlm`
3. Build from source if using development version:
   ```bash
   pip install -e .
   ```

### Error 2: RuntimeError in Brain Initialization

```python
brain = pynlm.createBrain(config)
brain.initialize()
# Traceback:
# RuntimeError: Failed to initialize brain
```

**Solution:**
1. Check configuration validity:
   ```python
   if not config.isValid():
       print("Invalid configuration:", config.getValidationErrors())
   ```

2. Verify configuration parameters:
   ```python
   config = pynlm.createDefaultConfig()
   # Check required parameters
   if not config.has("brain.neuron_count"):
       config.set("brain.neuron_count", 1000)
   ```

3. Check system resources:
   ```python
   import psutil
   available_memory = psutil.virtual_memory().available
   required_memory = 1000 * 100  # bytes per neuron
   if available_memory < required_memory:
       print("Insufficient memory. Consider reducing neuron count.")
   ```

### Error 3: Zero-Length Simulation Results

```python
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
# After simulation: All values are 0
```

**Solution:**
1. Verify brain is properly initialized:
   ```python
   if not brain.isInitialized():
       print("Brain not initialized properly")
       return
   ```

2. Check simulation steps:
   ```python
   print(f"Steps executed: {len(range(100))}")
   print(f"Neurons firing: {brain.getFiringNeuronCount()}")
   ```

3. Ensure proper configuration:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.excitability", 1.5)  # Increase excitability
   ```

### Error 4: Agent Not Moving in World

```python
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

# Run simulation
for i in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Problem: Agent stays in place
```

**Solution:**
1. Check action decoding:
   ```python
   action = agent.decodeMotorCommand()
   if action is None:
       print("No action decoded - check neural connections")
   ```

2. Verify sensory processing:
   ```python
   percept = world.getSensoryPercept()
   if percept is None:
       print("No perception available - check world setup")
   else:
       agent.processSensoryInput(percept)
   ```

3. Check action application:
   ```python
   world.applyMotorCommand(action, world.getSimulationTime())
   body = world.getAgentBody()
   print(f"Agent position: {body.getPosition()}")
   ```

### Error 5: Memory Exhaustion

```python
brain = pynlm.createBrain(config)
# ... simulation code ...
# Traceback:
# RuntimeError: Out of memory
```

**Solution:**
1. Monitor memory usage:
   ```python
   import psutil
   memory = psutil.virtual_memory()
   print(f"Memory usage: {memory.percent}%")
   ```

2. Reduce configuration:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 500)  # Reduce from 1000
   config.set("memory.working_memory_size", 100)  # Reduce from 1000
   ```

3. Use checkpointing for long simulations:
   ```python
   if step % 1000 == 0:
       brain.saveCheckpoint(f"checkpoint_{step}.bin")
   ```

## Installation Issues

### Issue 1: pybind11 Not Found

**Symptoms:**
```
ImportError: libpynlm.so: cannot open shared object file: No such file or directory
```

**Solution:**
1. Reinstall with development headers:
   ```bash
   pip uninstall pynlm -y
   pip install -e .
   ```

2. Check build dependencies:
   ```bash
   # On Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential cmake
   ```

### Issue 2: Python Version Incompatibility

**Symptoms:**
```
ValueError: Python version 3.12 not supported
```

**Solution:**
1. Check supported versions:
   ```python
   import pynlm
   print(pynlm.__python_version__)
   print(pynlm.__python_compatibility__)
   ```

2. Upgrade or downgrade Python:
   ```bash
   # For newer Python
   python -m pip install --upgrade pip
   python -m pip install pynlm
   
   # For older Python
   sudo apt-get install python3.8
   pip3 install pynlm
   ```

### Issue 3: Build Failures

**Symptoms:**
```
error: command 'gcc' failed with exit status 1
```

**Solution:**
1. Ensure CMake is recent enough:
   ```bash
   cmake --version
   # Need CMake 3.16+
   ```

2. Install compiler requirements:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gcc g++ clang
   
   # macOS
   xcode-select --install
   
   # Windows
   Install Visual Studio Build Tools
   ```

3. Clean and rebuild:
   ```bash
   rm -rf build
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

## Runtime Errors

### Issue 1: Neural Network Overloading

**Symptoms:**
```python
# Simulation running slowly
print(f"Step {step}: Processing took {step_time:.3f}s")
```

**Solution:**
1. Monitor performance:
   ```python
   import time
   start_time = time.time()
   brain.step(step)
   step_time = time.time() - start_time
   
   if step_time > 0.1:  # Step taking too long
       print(f"Step {step}: Slow performance ({step_time:.3f}s)")
   ```

2. Optimize configuration:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("simulation.time_step", 0.05)  # Smaller timestep
   config.set("brain.excitability", 1.0)    # Optimize neuron dynamics
   ```

3. Use batch processing for large simulations:
   ```python
   def run_simulation_in_batches(brain, total_steps, batch_size=1000):
       for batch_start in range(0, total_steps, batch_size):
           batch_end = min(batch_start + batch_size, total_steps)
           for step in range(batch_start, batch_end):
               brain.step(step)
           
           # Save checkpoint between batches
           if batch_end % 5000 == 0:
               brain.saveCheckpoint(f"checkpoint_{batch_end}.bin")
   ```

### Issue 2: Data Corruption

**Symptoms:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run multiple simulations
for run in range(10):
    brain.step(run)
    # ... run simulation ...
    
# Results are corrupted across runs
```

**Solution:**
1. Always reset between runs:
   ```python
   for run in range(10):
       # Reset brain state
       brain = pynlm.createBrain(config)  # Fresh brain
       brain.initialize()
       
       # Run simulation
       for step in range(100):
           brain.step(step)
           # ... simulation code ...
   ```

2. Use checkpointing for state preservation:
   ```python
   def run_simulation_with_checkpoint(brain, max_steps):
       checkpoint_interval = 100
       
       try:
           brain.initialize()
           
           for step in range(max_steps):
               brain.step(step)
               
               if step % checkpoint_interval == 0:
                   brain.saveCheckpoint(f"checkpoint_{step}.bin")
                   
       except Exception as e:
           # Restore from checkpoint
            if step > checkpoint_interval:
                brain.loadCheckpoint(f"checkpoint_{step - checkpoint_interval}.bin")
           raise
   ```

### Issue 3: Configuration Validation Failures

**Symptoms:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -1)  # Invalid value
# No error thrown
```

**Solution:**
1. Use configuration validation:
   ```python
   config = pynlm.createDefaultConfig()
   
   # Set values with validation
   config.set("brain.neuron_count", 1000)
   config.set("learning.rate", 0.01)
   config.set("brain.excitability", 1.5)
   
   # Validate configuration
   if not config.isValid():
       errors = config.getValidationErrors()
       for error in errors:
           print(f"Configuration error: {error}")
   ```

2. Use recommended configuration ranges:
   ```python
   RECOMMENDED_CONFIG = {
       "brain.neuron_count": (100, 10000),      # neurons
       "learning.rate": (0.001, 0.1),          # learning rate
       "brain.excitability": (0.5, 2.0),      # excitability
       "simulation.time_step": (0.01, 0.5),   # timestep
       "memory.working_memory_size": (10, 1000),    # memory size
   }
   ```

## Performance Issues

### Issue 1: Slow Step Processing

**Symptoms:**
```python
# Monitoring simulation speed
import time
start = time.time()
brain.step(step)
step_time = time.time() - start
print(f"Step {step}: {step_time:.4f}s")
```

**Solution:**
1. Profile performance bottlenecks:
   ```python
   import cProfile, pstats
   
   def profile_simulation():
       profiler = cProfile.Profile()
       profiler.enable()
       
       brain = pynlm.createBrain(config)
       brain.initialize()
       
       for step in range(1000):
           brain.step(step)
       
       profiler.disable()
       
       # Save profile results
       profiler.dump_stats("simulation_profile.prof")
       
       # Print summary
       stats = pstats.Stats("simulation_profile.prof")
       stats.sort_stats('cumulative')
       stats.print_stats(20)
   ```

2. Optimize configuration:
   ```python
   config = pynlm.createDefaultConfig()
   
   # Optimize for speed
   config.set("brain.neuron_count", 500)      # Fewer neurons
   config.set("simulation.time_step", 0.05)   # Smaller timestep
   config.set("brain.excitability", 1.0)     # Standard excitability
   config.set("learning.rate", 0.01)         # Standard learning rate
   ```

3. Use SIMD optimizations:
   ```python
   # Enable SIMD if available
   config.set("simulation.use_simd", True)
   
   # Check SIMD support
   if config.getBool("simulation.simd_available"):
       print("SIMD optimizations enabled")
   ```

### Issue 2: Memory Leaks

**Symptoms:**
```python
# Memory usage increasing over time
import psutil
import os

process = psutil.Process(os.getpid())
memory_before = process.memory_info().rss

# Run simulation
for step in range(10000):
    brain.step(step)

memory_after = process.memory_info().rss
print(f"Memory increase: {(memory_after - memory_before) / 1024 / 1024:.2f} MB")
```

**Solution:**
1. Monitor memory usage:
   ```python
   import psutil
   import gc
   
   def monitor_memory_usage(brain, steps):
       process = psutil.Process()
       
       memory_samples = []
       
       for step in range(steps):
           brain.step(step)
           
           if step % 100 == 0:
               memory = process.memory_info().rss / 1024 / 1024  # MB
               memory_samples.append(memory)
               print(f"Step {step}: Memory usage = {memory:.2f} MB")
       
       # Check for memory leaks
       if len(memory_samples) > 10:
           recent_memory = memory_samples[-5:]
           earlier_memory = memory_samples[:-5]
           
           if sum(recent_memory) > sum(earlier_memory) * 1.1:
               print("Possible memory leak detected!")
   ```

2. Force garbage collection:
   ```python
   import gc
   
   # Clean up references
   del brain
   del world
   del agent
   gc.collect()
   
   # Check memory after collection
   gc_stats = gc.get_stats()
   print(f"GC stats: {gc_stats}")
   ```

3. Use resource pools:
   ```python
   # Enable memory pools
   config.set("simulation.use_memory_pools", True)
   config.set("simulation.pool_size", 1000)
   
   # Monitor pool usage
   if config.getBool("simulation.pool_full"):
       print("Memory pool full - consider reducing simulation size")
   ```

## Memory Problems

### Issue 1: Out of Memory

**Symptoms:**
```python
# Error during simulation
brain = pynlm.createBrain(config)
# ... simulation code ...
# Traceback:
# RuntimeError: Memory allocation failed
```

**Solution:**
1. Reduce memory usage:
   ```python
   config = pynlm.createDefaultConfig()
   
   # Reduce memory-intensive parameters
   config.set("brain.neuron_count", 500)           # 1000 -> 500
   config.set("memory.working_memory_size", 100)  # 1000 -> 100
   config.set("memory.episodic_capacity", 500)    # 1000 -> 500
   ```

2. Use checkpointing for long simulations:
   ```python
   def memory_safe_simulation(brain, max_steps, checkpoint_interval=1000):
       import psutil
       import os
       
       process = psutil.Process(os.getpid())
       
       for step in range(max_steps):
           brain.step(step)
           
           # Check memory usage
           memory_mb = process.memory_info().rss / 1024 / 1024
           print(f"Step {step}: Memory = {memory_mb:.2f} MB")
           
           if memory_mb > 1000:  # > 1 GB
               print("High memory usage - saving checkpoint")
               brain.saveCheckpoint(f"checkpoint_{step}.bin")
   ```

3. Use efficient data structures:
   ```python
   # Use sparse matrices for connectivity
   config.set("brain.use_sparse_connectivity", True)
   
   # Use compressed storage
   config.set("brain.use_compressed_storage", True)
   ```

### Issue 2: Checkpoint Failures

**Symptoms:**
```python
brain.saveCheckpoint("test.bin")
# Traceback:
# RuntimeError: Failed to save checkpoint
```

**Solution:**
1. Check disk space:
   ```python
   import shutil
   disk_space = shutil.disk_usage("/")
   print(f"Available disk space: {disk_space.free / 1024 / 1024 / 1024:.2f} GB")
   
   if disk_space.free < 100 * 1024 * 1024:  # < 100 MB
       print("Insufficient disk space for checkpointing")
   ```

2. Check file permissions:
   ```python
   import os
   
   checkpoint_file = "checkpoint.bin"
   if os.path.exists(checkpoint_file):
       os.remove(checkpoint_file)  # Clear existing file
       
   # Ensure write permissions
   with open(checkpoint_file, 'w') as f:
       f.write("test")
   ```

3. Handle checkpoint errors:
   ```python
   import os
   import traceback
   
   def safe_checkpoint(brain, filename):
       try:
           brain.saveCheckpoint(filename)
           print(f"Checkpoint saved to {filename}")
       except Exception as e:
           print(f"Failed to save checkpoint: {e}")
           # Try to save to alternative location
           alternative_path = f"/tmp/{os.path.basename(filename)}"
           try:
               brain.saveCheckpoint(alternative_path)
               print(f"Checkpoint saved to alternative location: {alternative_path}")
           except Exception as e2:
               print(f"Failed to save checkpoint to alternative location: {e2}")
               raise
   ```

## Debugging Techniques

### Technique 1: Step-by-Step Debugging

```python
import pynlm

def debug_simulation():
    """Debug simulation step by step"""
    
    # 1. Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Run simulation with detailed output
    for step in range(10):
        print(f"\n=== Step {step} ===")
        
        # Update world
        world.update(0.1)
        print(f"World time: {world.getSimulationTime()}")
        
        # Get perception
        percept = world.getSensoryPercept()
        print(f"Perception: {percept}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        print(f"Agent curiosity: {agent.getCuriosityLevel()}")
        print(f"Agent novelty: {agent.getNoveltyLevel()}")
        
        # Brain computation
        brain.step(step)
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
        
        # Decode action
        action = agent.decodeMotorCommand()
        print(f"Action: {action}")
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check agent state
        body = world.getAgentBody()
        print(f"Agent position: {body.getPosition()}")
        print(f"Agent energy: {body.getEnergy()}")
    
    return brain, world, agent

# Run debug simulation
debug_simulation()
```

### Technique 2: State Inspection

```python
import pynlm

def inspect_simulation_state():
    """Inspect simulation state for debugging"""
    
    # Create simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Inspect brain state
    print("=== Brain State ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate()}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Inspect memory state
    print("\n=== Memory State ===")
    working_memory = brain.getWorkingMemory()
    print(f"Working memory size: {working_memory.getSize()}")
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Working memory full: {working_memory.isFull()}")
    
    episodic_memory = brain.getEpisodicMemory()
    print(f"Episodic memory usage: {episodic_memory.getUsage()}")
    print(f"Episodic memory capacity: {episodic_memory.getCapacity()}")
    
    # Inspect configuration
    print("\n=== Configuration ===")
    config_keys = config.getKeys()
    for key in config_keys:
        if key.startswith("brain.") or key.startswith("learning."):
            print(f"{key}: {config.getString(key)}")
    
    return brain

# Inspect simulation state
inspect_simulation_state()
```

### Technique 3: Performance Monitoring

```python
import pynlm
import time
import psutil
import os

class SimulationProfiler:
    """Profile simulation performance"""
    
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        self.start_time = None
        
    def start(self):
        """Start profiling"""
        self.start_time = time.time()
        
    def record_step(self, step):
        """Record step performance metrics"""
        process = psutil.Process(os.getpid())
        
        # Record step time
        step_time = time.time() - self.start_time
        self.step_times.append(step_time)
        
        # Record memory usage
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.memory_usage.append(memory_mb)
        
        # Record CPU usage
        cpu_percent = process.cpu_percent()
        self.cpu_usage.append(cpu_percent)
        
        # Update start time for next step
        self.start_time = time.time()
        
    def get_stats(self):
        """Get performance statistics"""
        if not self.step_times:
            return {}
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        avg_memory = sum(self.memory_usage) / len(self.memory_usage)
        avg_cpu = sum(self.cpu_usage) / len(self.cpu_usage)
        
        return {
            "total_steps": len(self.step_times),
            "avg_step_time": avg_step_time,
            "min_step_time": min(self.step_times),
            "max_step_time": max(self.step_times),
            "avg_memory_mb": avg_memory,
            "avg_cpu_percent": avg_cpu,
            "throughput_steps_per_sec": len(self.step_times) / sum(self.step_times) if sum(self.step_times) > 0 else 0
        }

# Use profiler in simulation
profiler = SimulationProfiler()
profiler.start()

brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    profiler.record_step(step)

# Get performance statistics
stats = profiler.get_stats()
print("Performance statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

## Support and Resources

### Community Support

- **GitHub Issues:** Report bugs and feature requests
- **Stack Overflow:** Use #nlm tag for questions
- **Discord:** Join the NLM community
- **Email:** support@nlm.org

### Documentation Resources

- **API Reference:** `src/docs/python_api.md`
- **User Guides:** `easy_usage.md`, `HOW_TO_USE.md`
- **Architecture:** `docs/ARCHITECTURE.md`
- **Performance:** `docs/PERFORMANCE.md`

### Troubleshooting Links

- **Installation Troubleshooting:** https://github.com/nlm/docs/install
- **Runtime Troubleshooting:** https://github.com/nlm/docs/runtime
- **Performance Troubleshooting:** https://github.com/nlm/docs/performance

### Additional Resources

- **Phase 6 Audit:** `docs/PHASE6_FINAL_AUDIT.md`
- **Scientific Papers:** See `docs/SCIENCE.md`
- **Experiments:** See `docs/EXPERIMENTS.md`

This comprehensive troubleshooting guide provides solutions for common NLM Python usage issues, debugging techniques, and performance optimization strategies. Use it to quickly diagnose and resolve simulation problems.
