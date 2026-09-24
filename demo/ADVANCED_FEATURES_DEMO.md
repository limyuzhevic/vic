// Advanced Features Demo
// ====================

## Overview
This demo showcases the new advanced features added to the NLM (熙然) neural simulation framework in Phase 6. It demonstrates:

1. Enhanced API with new simulation control methods
2. Event handlers for custom behavior
3. Performance profiling and monitoring
4. Memory management and optimization
5. Simulation pause/resume functionality
6. Configuration validation
7. Checkpoint and snapshot functionality

## Building and Running the Demo

### Prerequisites
- NLM project built with CMake
- C++ compiler with C++20 support

### Building the Demo
1. Navigate to the demo directory:
   ```bash
   cd demo
   ```

2. Build the demo executable:
   ```bash
   g++ -std=c++20 -I../src advanced_features_demo.cpp -o advanced_features_demo
   ```

### Running the Demo
```bash
./advanced_features_demo
```

## Features Demonstrated

### 1. Enhanced API
The demo shows the new API methods added to the Brain class:
- `pause()` and `resume()` for simulation control
- `isRunning()` to check simulation state
- `getCurrentTime()`, `getCurrentStep()`, `getCurrentTimestep()` for time tracking
- `getActiveNeuronCount()`, `getTotalSpikeCount()`, etc. for statistics
- `validateConfiguration()` for configuration validation

### 2. Event Handlers
The demo demonstrates custom event handlers:
- **Spike Handler**: Logs neuron activity and detects burst firing
- **Memory Handler**: Monitors memory activation and detects consolidation events
- **Neuromodulator Handler**: Tracks neuromodulator level changes and plasticity factors

### 3. Performance Profiling
The demo includes performance benchmarking:
- Enables performance profiling with `enablePerformanceProfiling()`
- Measures simulation speed (steps per second)
- Generates detailed performance statistics
- Compares performance across different feature configurations

### 4. Memory Management
The demo shows memory optimization features:
- Sets memory limits with `setMemoryLimit()`
- Enables debug mode with `enableDebugMode()`
- Monitors memory usage during simulation
- Demonstrates memory pool functionality

### 5. Simulation Control
The demo showcases simulation control features:
- Pauses simulation with `pause()`
- Resumes simulation with `resume()`
- Checks simulation state with `isRunning()`
- Monitors current simulation time and step

### 6. Configuration Validation
The demo validates configuration:
- Calls `validateConfiguration()` to check for issues
- Shows current configuration parameters
- Demonstrates how invalid configurations are handled

### 7. Checkpoint System
The demo demonstrates checkpoint functionality:
- Creates checkpoints with `saveCheckpoint("name")`
- Creates snapshots with `saveSnapshot("filename")`
- Shows how to save and potentially resume brain states

## Expected Output

When you run the demo, you should see output similar to:

```
=== NLM Advanced Features Demo ===
This demo showcases the new advanced features in NLM Phase 6.

Brain created with configuration:
  Neurons: 100
  Regions: 2
  Timestep: 0.001 s

Initializing brain...
Brain initialized successfully.

=== Configuration Validation Demo ===
Configuration validation result: Valid

=== Simulation Control Demo ===
Initial state: Running
After pause: Paused
After resume: Running
Current step: 1
Current time: 0.001 ms
Current timestep: 0.001 ms

=== Event Handlers Demo ===
Event handlers registered:
  - Spike handler (logs neuron activity)
  - Memory handler (detects high activity)
  - Neuromodulator handler (logs significant changes)
Running simulation to trigger events...
Step 0: 15 active neurons
...

=== Memory Management Demo ===
Memory limit set to 10MB
Debug mode enabled
Step 0: Active neurons = 8
...

=== Performance Benchmark ===
Benchmark completed in 125 ms
Performance: 1600 steps/second
...

=== Checkpoint System Demo ===
Checkpoint created: Yes
Checkpoint saved as: demo_checkpoint
Snapshot created: Yes
Snapshot saved as: demo_brain.chk

=== Demo Summary ===
Brain status:
  Total neurons: 100
  Total synapses: 2,500
  Total spikes: 1,234
  Active neurons: 23
  Average firing rate: 12.34 Hz
  E/I ratio: 2.1

=== All demonstrations completed successfully! ===
```

## Key Learnings

1. **API Enhancement**: The new API makes it easier to control and monitor simulations
2. **Event-Driven Architecture**: Event handlers enable custom behavior without modifying core code
3. **Performance Optimization**: Profiling helps identify bottlenecks and optimize performance
4. **Memory Management**: Memory limits and pools prevent out-of-memory issues
5. **Simulation Control**: Pause/resume functionality enables flexible simulation control
6. **Configuration Validation**: Validation helps catch configuration errors early
7. **Persistence**: Checkpoint system enables saving and resuming simulations

## Advanced Usage Tips

1. **For Performance**: Use `--enable-memory-pools --enable-simd --thread-count=4` for optimal performance
2. **For Debugging**: Use `--debug-level=verbose --profile` for detailed debugging
3. **For Long Simulations**: Use `--max-steps=1000000 --memory-limit=1000000000` for large-scale simulations
4. **For Research**: Use `--validate-config` to ensure your configuration is valid before starting

## Integration with Existing Code

The advanced features can be easily integrated into existing NLM applications:

```cpp
// Basic usage with advanced features
nlm::Brain brain(config);
brain.initialize();
brain.enablePerformanceProfiling();
brain.enableDebugMode();

// Register custom event handlers
brain.registerSpikeHandler(customSpikeHandler);
brain.registerMemoryUpdatedHandler(customMemoryHandler);

// Run simulation with control
brain.pause(); // Pause for manual intervention
brain.resume(); // Resume simulation

// Save state for later
brain.saveCheckpoint("research_run_1");

// Continue simulation
for (int i = 0; i < 1000; ++i) {
    brain.step(i, i * 0.001);
}

brain.saveCheckpoint("research_run_2");
```

## Conclusion

The advanced features demo showcases how NLM Phase 6 provides a comprehensive, feature-rich neural simulation framework that is:

- **Power User Friendly**: Advanced commands and API methods
- **Research Ready**: Performance profiling, checkpointing, and event handlers
- **Developer Extensible**: Easy to extend with custom event handlers and features
- **Production Ready**: Memory management, validation, and debugging tools

These features make NLM suitable for both basic educational demonstrations and advanced neuroscience research applications.