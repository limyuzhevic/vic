# NLM Phase 6 Demo System Documentation

## Overview

This document describes the improved Phase 6 demo system for the NLM (熙然) artificial brain simulation project.

## Quick Start

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Demos

The project now provides multiple demo executables:

1. **Basic Phase 6 Demo** (`./nlm_phase6_demo`)
2. **Advanced Phase 6 Demo** (`./nlm_phase6_demo_advanced`)

## Basic Phase 6 Demo

The basic demo (`nlm_phase6_demo`) provides:

- Integration verification
- Individual system tests
- Full integration experiment
- Configurable parameters

### Usage

```bash
./nlm_phase6_demo [options]
```

### Options

- `--help, -h`: Show help message
- `--quick`: Run only integration verification (skip system tests and full experiment)
- `--skip-verification`: Skip integration verification
- `--neuron-count N`: Number of neurons (default: 500)
- `--max-steps S`: Maximum simulation steps (default: 2000)
- `--region-count R`: Number of neural regions (default: 1)
- `--connection-prob P`: Connection probability (default: 0.1)
- `--checkpoint-path P`: Checkpoint file path (default: ./checkpoint_test.bin)
- `--enable-checkpointing`, `--disable-checkpointing`: Enable/disable checkpointing
- `--enable-replay`, `--disable-replay`: Enable/disable replay system
- `--enable-development`, `--disable-development`: Enable/disable development system
- `--verbose, -v`: Enable verbose output
- `--quiet, -q`: Enable quiet output (errors only)

### Example

```bash
./nlm_phase6_demo --neuron-count 1000 --max-steps 5000 --verbose
./nlm_phase6_demo --quick --quiet
./nlm_phase6_demo --enable-checkpointing --checkpoint-path ./my_checkpoint.bin
```

## Advanced Phase 6 Demo

The advanced demo (`nlm_phase6_demo_advanced`) provides:

- Multiple demo modes (Quick, Thorough, Benchmark, Comparison)
- Performance benchmarking
- Detailed metrics collection
- Export capabilities
- Progress tracking
- Comparison testing

### Usage

```bash
./nlm_phase6_demo_advanced [options]
```

### Modes

#### Quick Mode (`--demo-mode quick`)
Runs only integration verification:
- Checks if all brain systems are connected
- No individual system tests
- No full integration experiment
- Fastest mode for validation

#### Thorough Mode (`--demo-mode thorough`)
Runs all components:
- Integration verification
- Individual system tests
- Full integration experiment
- Collects detailed metrics
- Default mode

#### Benchmark Mode (`--demo-mode benchmark`)
Performance benchmarking:
- Runs experiment with optimized settings
- Measures performance metrics
- Calculates throughput
- Reports efficiency metrics

#### Comparison Mode (`--demo-mode comparison`)
Comparison testing:
- Runs current configuration
- Compares with previous results (if available)
- Highlights improvements/differences

### Additional Options

- `--benchmark`: Enable benchmark mode
- `--benchmark-time S`: Benchmark time limit in seconds (default: 120)
- `--benchmark-neurons N`: Neuron count for benchmarking (default: 200)
- `--benchmark-steps S`: Step count for benchmarking (default: 1000)
- `--output FILE`: Write output to file
- `--export-results`: Export results to files
- `--collect-benchmark`: Collect benchmark data
- `--compare-with-prev`: Compare with previous results

### Example

```bash
# Thorough mode with export
./nlm_phase6_demo_advanced --demo-mode thorough --neuron-count 1000 --max-steps 5000 --export-results --output results.json

# Benchmark mode
./nlm_phase6_demo_advanced --demo-mode benchmark --benchmark-time 60

# Quick mode for validation
./nlm_phase6_demo_advanced --demo-mode quick --quiet

# Comparison mode
./nlm_phase6_demo_advanced --demo-mode comparison --compare-with-prev
```

## Performance Optimization

The improved demo system includes several performance optimizations:

### Configuration Validation

All configuration parameters are validated before execution:
- Neuron count must be > 0
- Max steps must be > 0
- Connection probability must be between 0.0 and 1.0
- Region count must be > 0

### Efficient Resource Management

- Configurable resource usage through command-line parameters
- Optional checkpointing to resume interrupted experiments
- Replay system for testing memory consolidation
- Development system for learning rate adaptation

### Benchmarking Features

The benchmark mode provides detailed performance analysis:
- Neurons per second calculation
- Steps per second calculation
- Memory efficiency metrics
- Convergence rate analysis

## System Integration Tests

The Phase 6 demo verifies integration of all major brain systems:

### Memory Systems
- Working Memory: Persistent activity and competition
- Episodic Memory: Experience encoding and replay
- Associative Memory: Hebbian pattern associations

### Neuromodulation Integration
- Dopamine: Neural excitability and plasticity
- Curiosity: Exploration behavior
- Novelty: Surprise detection

### Prediction System
- Temporal prediction
- Prediction error signals
- Confidence tracking

### Cognition Systems
- Neural planner: Action sequence evaluation
- Concept formation: Pattern discovery
- Attention: Competitive selection

### Development Integration
- Developmental stages: Plasticity rate modulation
- Structural plasticity: Age-dependent changes
- Neural excitability: Maturation effects

## Troubleshooting

### Common Issues

#### "Integration verification failed"
- Check if all required libraries are linked
- Verify that Brain initialization works correctly
- Check for compilation errors

#### "Failed to save checkpoint"
- Ensure write permissions for checkpoint directory
- Check available disk space
- Verify checkpoint file path

#### "No memory episodes available for replay"
- This is normal for short experiments
- Increase experiment duration
- Enable more detailed logging

### Help and Support

For issues, refer to:
- Project documentation (docs/ directory)
- Source code comments
- CMake build logs
- NLM project wiki (if available)

## Comparison with Previous Versions

### Phase 3 Demo (`./nlm_phase3_demo`)
- Focus on world interaction
- Simpler configuration
- Fewer test modes
- Less detailed output

### Phase 4 Demo (`./nlm_phase4_demo`)
- Comprehensive cognitive experiments
- Multiple experiment types
- Detailed metrics collection
- Advanced output formatting

### Phase 6 Demo (Basic)
- Final integration test
- System verification
- Configurable parameters
- Progress tracking

### Phase 6 Demo (Advanced)
- Enhanced integration test
- Multiple demo modes
- Advanced benchmarking
- Export capabilities
- Comparison testing

## Future Improvements

The demo system can be further enhanced with:

1. **Web Interface**: Interactive web-based visualization
2. **Real-time Monitoring**: Live performance metrics
3. **Custom Experiments**: User-defined experiment templates
4. **Distributed Computing**: Parallel execution across multiple machines
5. **Machine Learning**: AI-assisted parameter optimization
6. **Simulation Replay**: Recording and playback of entire simulations

## License

This documentation is part of the NLM project, licensed under the MIT License.

## Authors

NLM Research Project - See README.md for scientific background.