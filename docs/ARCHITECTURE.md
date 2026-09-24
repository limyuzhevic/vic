# NLM Command Line Interface

## Overview

This document describes the command line interface for the NLM (Neural Learning Machine) project. The NLM project provides several demo executables that demonstrate different phases of development, each with their own command line options for customization and control.

## Common Command Line Arguments

All NLM executables support the following common arguments:

### `--help` / `-h`
Display help information and available options

### `--version`
Display version information

### `--config <file>`
Load configuration from the specified file (default: `configs/default.cfg`)

### `--verbose` / `-v`
Enable verbose logging output

### `--quiet` / `-q`
Suppress non-essential output

### `--steps <N>`
Number of simulation steps to run (default varies by demo)

### `--neurons <N>`
Number of neurons in the brain (default varies by demo)

### `--regions <N>`
Number of brain regions (default varies by demo)

### `--seed <N>`
Random seed for reproducible experiments (default: `42`)

## Phase 3 Demo: Basic Neural Computation

### Executable
`./nlm_phase3_demo`

### Purpose
Demonstrates basic neural computation including LIF neuron dynamics, event-driven spike propagation, STDP and Hebbian plasticity, and structural plasticity.

### Command Line Options
```bash
./nlm_phase3_demo [--steps N] [--neurons N] [--regions N] [--seed N] [--verbose] [--quiet]
```

### Example Usage
```bash
# Run basic neural computation test
./nlm_phase3_demo

# Run with custom parameters
./nlm_phase3_demo --steps 5000 --neurons 1000 --seed 123

# Enable verbose output
./nlm_phase3_demo --verbose
```

### Expected Output
- Neural dynamics simulation
- Spike propagation visualization
- Plasticity learning demonstration
- Final network statistics

## Phase 4 Demo: Working Memory and Prediction

### Executable
`./nlm_phase4_demo`

### Purpose
Demonstrates Phase 4 capabilities including working memory, episodic memory, concept formation, neural attention, and predictive planning.

### Command Line Options
```bash
./nlm_phase4_demo [--steps N] [--neurons N] [--regions N] [--seed N] [--verbose] [--quiet] [--workload <type>]
```

### Additional Options
`--workload <type>`
Memory workload type: `simple`, `complex`, `mixed` (default: `simple`)

### Example Usage
```bash
# Run working memory and prediction demo
./nlm_phase4_demo

# Run with complex memory workload
./nlm_phase4_demo --workload complex --steps 10000

# Custom configuration
./nlm_phase4_demo --config my_config.cfg --seed 456
```

### Expected Output
- Working memory trace demonstrations
- Concept formation from patterns
- Predictive planning sequences
- Memory capacity measurements

## Phase 5 Demo: Performance Optimizations

### Executable
`./nlm_phase5_demo`

### Purpose
Tests advanced performance optimizations including SIMD vectorization, multithreading, memory pools, event-driven processing, and sparse connectivity.

### Command Line Options
```bash
./nlm_phase5_demo [--steps N] [--neurons N] [--regions N] [--seed N] [--verbose] [--quiet] [--performance <mode>]
```

### Additional Options
`--performance <mode>`
Performance test mode: `simd`, `threads`, `memory`, `sparse`, `event` (default: `all`)

`--thread-count <N>`
Number of threads for multithreading tests (default: auto-detect)

`--output-dir <dir>`
Output directory for test results (default: `./output/phase5_demo`)

### Example Usage
```bash
# Run comprehensive performance test
./nlm_phase5_demo

# Test specific optimization (SIMD)
./nlm_phase5_demo --performance simd --steps 2000

# Custom thread count
./nlm_phase5_demo --thread-count 8 --performance threads

# Save detailed results
./nlm_phase5_demo --output-dir ./results --verbose
```

### Expected Output
- Performance benchmark results
- Optimization effectiveness metrics
- Memory usage statistics
- Threading efficiency measurements

## Phase 6 Demo: Full Integration Test

### Executable
`./nlm_phase6_demo`

### Purpose
Comprehensive integration test verifying that all brain systems (memory, neuromodulation, prediction, cognition, development) are properly connected and functional.

### Command Line Options
```bash
./nlm_phase6_demo [--steps N] [--neurons N] [--regions N] [--seed N] [--verbose] [--quiet] [--integration <test>]
```

### Additional Options
`--integration <test>`
Integration test type: `full`, `memory`, `neuromod`, `prediction`, `cognition`, `development` (default: `full`)

`--enable-checkpointing`
Enable checkpoint save/load tests

`--enable-replay`
Enable memory replay system tests

`--enable-development`
Enable developmental stage tests

### Example Usage
```bash
# Run full integration test
./nlm_phase6_demo

# Test specific integration (memory systems only)
./nlm_phase6_demo --integration memory

# Enable all optional tests
./nlm_phase6_demo --enable-checkpointing --enable-replay --enable-development

# Custom test configuration
./nlm_phase6_demo --steps 5000 --neurons 800 --regions 3
```

### Expected Output
- Integration verification results
- Memory system connectivity tests
- Neuromodulation system functionality
- Prediction system integration
- Cognitive system operation
- Development stage progression
- Checkpoint and replay system tests

## Common Configuration Files

### Default Configuration
`configs/default.cfg`

Basic configuration for all demos with sensible defaults for testing.

### Performance Configuration  
`configs/performance.cfg`

Optimized settings for Phase 5 performance tests with emphasis on speed and efficiency.

### Integration Configuration
`configs/integration.cfg`

Configuration for Phase 6 integration tests with all systems enabled.

### Custom Configuration
Create your own configuration file by copying `configs/default.cfg` and modifying values as needed.

## Output Formats

### Text Output
- Console output during demo execution
- Summary statistics and final results
- Progress updates and logging information

### File Output
- Detailed logs in `./output/<demo_name>/` directory
- Configuration snapshots
- Performance benchmark results
- Integration test reports

## Troubleshooting

### Common Issues

**1. Build Failure**
- Ensure CMake 3.16+ and C++20 compiler
- Check for missing dependencies (pthread, etc.)
- Verify file paths in CMakeLists.txt

**2. Demo Not Found**
- Build the project first: `mkdir build && cd build && cmake .. && make`
- Check executable names: `./nlm_phase3_demo`, `./nlm_phase4_demo`, etc.

**3. Configuration Errors**
- Ensure configuration files exist in `configs/` directory
- Check file permissions
- Use `--config default` to reset to default configuration

**4. Missing Libraries**
- On Ubuntu/Debian: `sudo apt-get install build-essential cmake`
- On macOS: `brew install cmake`
- On Windows: Install Visual Studio with C++ support

### Getting Help

For additional help, refer to:
- `README.md` - Project overview and building instructions
- `docs/` - Technical documentation
- `--help` flag - Command line options for each demo
- `./<demo> --help` - Specific demo options

## Research and Development

### Contributing
1. Fork the repository
2. Create a feature branch
3. Fix any compilation issues first
4. Add new features or demos
5. Update documentation
6. Submit pull request

### Testing
- Unit tests in `tests/` directory (once compilation works)
- Integration tests in demo executables
- Performance benchmarks in Phase 5 demo
- System connectivity tests in Phase 6 demo

### Development Workflow
1. Start with Phase 3 demos for basic testing
2. Progress to Phase 4 for memory and cognition features
3. Use Phase 5 for performance optimization testing
4. Complete with Phase 6 integration tests

## License

MIT License

---

For more detailed information about specific features, parameters, or experimental results, refer to the project documentation in the `docs/` directory.