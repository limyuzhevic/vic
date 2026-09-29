# NLM - Neural Learning Machine
# Advanced User Guide

This document provides comprehensive guidance for using NLM (熙然), an experimental artificial developmental brain system.

## Overview

NLM is a sophisticated computational brain project designed to study how neural systems develop and learn through interaction with their environment. It implements brain-inspired architectures with neurons, synapses, and various cognitive systems that emerge from neural dynamics.

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm

cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Basic Experiments

```bash
# Run the main NLM simulation
./nlm

# Run Phase 6 integration demo (comprehensive test)
./nlm_phase6_demo

# Run Phase 3 experiment (basic world interaction)
./nlm_phase3_demo

# Run Phase 4 experiment (cognitive capabilities)
./nlm_phase4_demo
```

## Command Line Interface

NLM provides a comprehensive command-line interface for advanced control:

```bash
# Full help documentation
./nlm help

# Run with verbose logging
./nlm run --verbose --log-level debug

# Run specific experiment with custom settings
./nlm experiment phase6 --output my_results --performance

# Configure NLM interactively
./nlm configure

# Show system statistics
./nlm stats

# Show version information
./nlm --version
```

## Advanced Configuration

### Configuration File

NLM supports persistent configuration through `~/.nlm_config`:

```yaml
# Generated configuration
NLM Configuration
=================
Generated on: Tue Sep 29 2026 04:01:15
Iterations: 1000
Random Seed: 12345
Verbose: true
Log Level: debug
Output Directory: ./output
Development Stages: true
Performance Monitoring: true
Async Logging: true
```

### Key Configuration Options

1. **Iterations**: Number of simulation steps (default: 1000)
2. **Random Seed**: For reproducible results (default: 12345)
3. **Log Level**: debug, info, warning, error, critical (default: info)
4. **Development Stages**: Enable developmental plasticity (default: true)
5. **Performance Monitoring**: Track system performance (default: false)
6. **Async Logging**: Enable non-blocking logging (default: true)

## System Architecture

### Core Components

1. **Neurons**: Leaky Integrate-and-Fire (LIF) models with complex dynamics
2. **Synapses**: Chemical and electrical connections with plasticity
3. **Brain Regions**: Specialized processing areas (sensory, motor, association)
4. **Memory Systems**: Working, episodic, and semantic memory
5. **Neuromodulation**: Dopamine, novelty, and curiosity signals
6. **Prediction System**: Forward models and prediction errors
7. **Cognitive Functions**: Planning, attention, and concept formation
8. **Development**: Structural and functional maturation

### Memory Systems

#### Working Memory
- Persistent activity for temporary information storage
- Competitive dynamics for information selection
- Integrated with neural processing loops

#### Episodic Memory
- Experience encoding and replay mechanisms
- Temporal associative memory
- Consolidation during rest phases

#### Semantic Memory
- Pattern associations and knowledge representation
- Conceptual learning from experience
- Distributed storage across neural populations

### Neuromodulation

#### Dopamine
- Reward prediction error signaling
- Reinforcement learning modulation
- Synaptic weight adaptation

#### Novelty Detection
- Surprise-based exploration
- Unsupervised learning activation
- Curiosity drive

#### Curiosity
- Intrinsic motivation
- Exploration-exploitation balance
- Behavioral flexibility

## Running Experiments

### Phase 3: World Interaction
- Basic sensory-motor capabilities
- Reward prediction error learning
- Simple developmental stages

### Phase 4: Cognitive Capabilities
- Neural prediction systems
- Working and episodic memory
- Concept formation
- Neural attention mechanisms
- Predictive planning
- Self-model development
- Social learning
- Continual learning

### Phase 5: Performance Optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Complete brain loop functions coherently

## Performance Monitoring

### Available Metrics

1. **Logs Per Second**: Throughput measurement
2. **Average Log Time**: Processing latency
3. **Memory Usage**: System memory consumption
4. **Worker Queue Size**: Async logging backlog

### Performance Commands

```bash
# Run with performance monitoring
./nlm run --performance

# Check current performance stats
./nlm stats

# Configure performance monitoring
./nlm configure
```

## Advanced Features

### Async Logging

NLM implements thread-safe asynchronous logging to prevent blocking during simulation:

```cpp
// Enable async logging
nlm::Logger::getGlobal().setAsyncLogging(true, 1000, 4);

// Log with async option
logger.info("Message", "file.cpp", 123, "function", true);
```

### Multi-Logger Support

Multiple logger implementations can be active simultaneously:

```cpp
// Add console logger
logger.addLogger(std::make_shared<nlm::ConsoleLogger>());

// Add file logger with rotation
logger.addLogger(std::make_shared<nlm::FileLogger>("output/nlm.log"));

// Log to all configured loggers
logger.info("This goes to all loggers");
```

### Log Rotation

File loggers support automatic rotation to prevent disk space issues:

```cpp
// Configure rotation (100MB per file, keep 5 files)
fileLogger.setFileRotation(100, 5);
```

## Development and Extension

### Adding New Neuron Types

1. Implement new neuron type in `src/brain/Neuron.hpp`
2. Update `src/brain/Neuron.cpp` with specific dynamics
3. Register in brain region population creation

### Adding New Memory Systems

1. Implement memory interface in `src/memory/Memory.hpp`
2. Create specific implementation (e.g., `NeuralEpisodicMemory.cpp`)
3. Integrate with agent brain system

### Extending Neuromodulation

1. Create new neuromodulator class inheriting from `Neuromodulator`
2. Implement specific signal dynamics
3. Integrate with plasticity rules

## Troubleshooting

### Common Issues

#### "Executable not found"
```bash
# Build the project first
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### "Permission denied"
```bash
# Make executables executable
chmod +x nlm
chmod +x nlm_*
```

#### "Memory allocation failed"
```bash
# Reduce memory usage by decreasing iterations
./nlm run --iterations 500
```

#### "Log file permission denied"
```bash
# Ensure output directory exists and is writable
mkdir -p output
chmod 755 output
```

### Debug Mode

For detailed debugging, use:

```bash
# Enable debug logging
./nlm run --verbose --log-level debug

# Configure interactive debugging
./nlm configure
```

## Example Scripts

### Performance Test Script

```bash
#!/bin/bash
# perf_test.sh

echo "Running NLM performance test..."
start_time=$(date +%s)

./nlm run --performance --iterations 500

end_time=$(date +%s)
 duration=$((end_time - start_time))

echo "Test completed in ${duration} seconds"
./nlm stats
```

### Batch Experiment Script

```bash
#!/bin/bash
# batch_experiments.sh

phases=("phase3" "phase4" "phase5" "phase6")

for phase in "${phases[@]}"; do
    echo "Running $phase experiment..."
    ./nlm experiment $phase --output results_$phase --performance
    echo "$phase completed"
done

echo "All experiments completed!"
```

## License

MIT License

Copyright (c) 2024 NLM Authors

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
LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.