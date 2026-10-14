#!/usr/bin/env python3

"""
NLM Quick Start Guide
=====================

This guide provides a quick overview of the enhanced NLM (Neural Learning Machine)
project and how to get started with the new features.

---

## Quick Start

### 1. Building and Running

**Build the project:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Run basic connectivity test:**
```bash
./nlm --mode=basic
```

**Run complete test suite:**
```bash
./nlm --mode=all
```

**Run tests only:**
```bash
./nlm_test
```

**Run integration tests:**
```bash
./nlm_phase6_demo
```

### 2. Using the Python CLI Tool

```bash
# Show help
nlm_cli help

# Run neural simulation
nlm_cli run --mode=plasticity --steps=500

# Run tests
nlm_cli test --all --verbose

# Manage configuration
nlm_cli config show

# Run benchmarks
nlm_cli benchmark --type=performance
```

### 3. Using the Test Runner

```bash
# Run all tests
python3 test_runner.py

# Run specific test types
python3 test_runner.py --unit
python3 test_runner.py --integration
python3 test_runner.py --performance

# Run with custom config
python3 test_runner.py --config=myconfig.cfg
```

---

## Key Features

### Command-Line Interface (C++)

| Option | Description |
|--------|-------------|
| `--mode=STRING` | Run specific mode: `basic`, `plasticity`, `stdp`, `all` |
| `--steps=N` | Set simulation steps per test |
| `--neuron=N` | Set number of neurons |
| `--seed=N` | Set random seed |
| `--verbose` | Enable verbose logging |
| `--quiet` | Suppress info messages |
| `--test` | Run tests only |
| `--help` | Show help message |

### Python CLI Interface


| Command | Description |
|---------|-------------|
| `run` | Run neural simulation |
| `test` | Run tests |
| `config` | Manage configuration |
| `benchmark` | Run benchmarks |
| `experiment` | Run experiments |
| `help` | Show help |

### Configuration Options

| Section | Key | Default |
|---------|-----|---------|
| Simulation | `simulation_timestep` | `0.001` |
| Neurons | `neuron_count` | `500` |
| Regions | `region_count` | `1` |
| Connectivity | `connection_probability` | `0.1` |
| Plasticity | `plasticity_learning_rate` | `0.01` |
| Memory | `working_memory_capacity` | `0.1` |
| Checkpointing | `checkpoint_enabled` | `true` |

---

## Testing

### Unit Tests

```bash
# Run all unit tests
./nlm_test

# View test results
python3 test_runner.py --unit
```

### Integration Tests

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# View integration results
python3 test_runner.py --integration
```

### Performance Tests

```bash
# Run performance benchmarks
python3 test_runner.py --performance

# Custom benchmark
nlm_cli benchmark --type=performance --duration=120
```

---

## Configuration

### Default Configuration File

Edit `configs/default.cfg` to customize your simulation:

```ini
# Simulation settings
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Performance settings
enable_parallel_processing = false
enable_vectorization = true
enable_memory_pools = true

# Memory settings
working_memory_capacity = 0.1
episodic_memory_max_episodes = 1000

# Development settings
development_enabled = true
developmental_stage = "adult"

# Checkpoint settings
checkpoint_enabled = true
checkpoint_interval = 1000
checkpoint_path = "./checkpoints"
```

### Command-Line Configuration Override

```bash
# Override configuration values
./nlm --config=myconfig.cfg --neuron=2000 --seed=123
```

---

## Advanced Usage

### Running Specific Tests

```bash
# Basic connectivity test only
./nlm --mode=basic

# Plasticity learning test only
./nlm --mode=plasticity

# STDP verification test only
./nlm --mode=stdp

# Complete test suite
./nlm --mode=all
```

### Custom Simulation Steps

```bash
# Run with custom steps
./nlm --mode=plasticity --steps=2000

# Run with custom neuron count and steps
./nlm --mode=all --neuron=1000 --steps=500
```

# Advanced simulation with custom seed
./nlm --mode=all --neuron=2000 --seed=456 --verbose
```

### Performance Optimization

```bash
# Build with performance optimizations
mkdir build_cmake
cd build_cmake
cmake .. -DCMAKE_BUILD_TYPE=Release
   # Enable all optimizations
```

### Debugging and Logging

```bash
# Enable verbose logging
./nlm --mode=plasticity --verbose

# Run with quiet mode (errors only)
./nlm --mode=all --quiet

# Enable test-only mode (simulation skipped)
./nlm --test
```

---

## File Structure

```
/workspace/app/
├── src/                    # C++ source code
│   ├── main.cpp          # Original main executable
│   ├── main_improved.cpp  # Enhanced main executable
│   ├── brain/            # Brain components
│   ├── core/             # Core utilities
│   ├── sensory/          # Sensory systems
│   └── motor/            # Motor systems
│
├── tests/                 # Test framework
│   ├── test_brain.cpp
│   ├── test_neuron.cpp
│   ├── test_stdp.cpp
│   └── test_main.cpp
│
├── configs/               # Configuration files
│   └── default.cfg
│
├── experiments/           # Experiment framework
│   ├── Phase6Demo.cpp
│   └── Phase6IntegratedExperiment.cpp
│
├── performance/            # Performance optimizations
│   ├── MemoryPool.hpp
│   └── EventQueue.hpp
│
├── docs/                  # Documentation
│   └── ENHANCEMENT_SUMMARY.md
│
├── nlm_cli.py            # Python CLI tool
├── test_runner.py        # Test runner
├── build_and_test.sh     # Build and test script
└── README.md             # Project documentation
```

---

## Getting Help

### Online Documentation

- **Project Documentation**: `README.md`
- **Enhancement Summary**: `ENHANCEMENT_SUMMARY.md`
- **API Documentation**: Generated by Doxygen

### Command-Line Help

```bash
# Show general help
./nlm --help

# Show version information
./nlm --version

# Show command-specific help
./nlm_cli help run
./nlm_cli help test
```

### Python CLI Help

```bash
# Show general help
nlm_cli help

# Show command-specific help
nlm_cli help run
nlm_cli help test
```

---

## Troubleshooting

### Common Issues

**Issue**: Build fails with CMake errors
**Solution**: Ensure you have a C++20 compliant compiler (GCC 11+, Clang 14+, or MSVC 19+)

**Issue**: Running out of memory
**Solution**: Adjust configuration:
```ini
enable_parallel_processing = false
enable_memory_pools = false
working_memory_capacity = 0.05
```

**Issue**: Tests failing
**Solution**: Run with verbose logging:
```bash
./nlm_test --verbose
```

**Issue**: Configuration file not found
**Solution**: Ensure the config file exists and is readable:
```bash
ls -la configs/default.cfg
```

---

## Performance Tips

### Simulation Performance

1. **Optimize neuron count**: Start with smaller simulations:
```bash
./nlm --mode=all --neuron=100
```

2. **Enable vectorization**:
```ini
enable_vectorization = true
```

3. **Use memory pools**:
```ini
enable_memory_pools = true
```

### Memory Management

1. **Monitor memory usage**:
```bash
# Use system monitoring tools
watch -n 1 free -h
```

2. **Adjust memory settings**:
```ini
working_memory_capacity = 0.05
episodic_memory_max_episodes = 100
```

---

## Example Usage Scenarios

### Research Scenario
```bash
# Research with custom parameters
./nlm --mode=all \
     --neuron=5000 \
     --steps=3000 \
     --seed=42 \
     --verbose
```

### Production Scenario
```bash
# Production with optimized settings
./nlm --mode=plasticity \
     --neuron=1000 \
     --steps=500 \
     --quiet
```

### Development Scenario
```bash
# Development with full debugging
./nlm --mode=all \
     --neuron=100 \
     --steps=100 \
     --verbose \
     --test
```

---

## Conclusion

The enhanced NLM project provides:

- **Improved performance** and scalability
- **Better usability** with comprehensive CLI tools
- **Robust testing** framework
- **Advanced configuration** management
- **Comprehensive documentation**
- **Future extensibility** and plugin architecture

Get started today and explore the power of neural network simulation with NLM!

---

**For more information**: Visit the project documentation or contact the development team.

**Last updated**: October 14, 2026
**Version**: 2.0.0 (Phase 2 Enhanced)
