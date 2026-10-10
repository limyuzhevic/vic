# NLM README Improvements

## Overview

This document provides enhanced setup instructions and usage guidance for the Neural Learning Machine (NLM) framework.

## Quick Start

### Prerequisites

- C++17 compatible compiler
- CMake 3.15 or higher
- Python 3.8+ (for Python bindings)

### Building

```bash
# Create build directory and configure
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON=ON ..

# Build with multiple threads
make -j$(nproc)

# Or install dependencies
make install
```

### Running Examples

```bash
# Run basic test suite
./nlm --test=all

# Run specific benchmark
./nlm --benchmark --neurons=10000 --steps=10000

# Run scaling benchmark
./nlm --scaling-benchmark
```

## Configuration

### Configuration Files

NLM supports multiple configuration file formats:

1. **JSON** (primary): `config.json`
2. **YAML**: `config.yaml` (experimental)
3. **INI**: `config.ini` (basic)

### Configuration Examples

```json
{
  "simulation": {
    "timestep": 0.001,
    "duration": 100.0,
    "neuron_count": 1000
  },
  "plasticity": {
    "stdp": {
      "enabled": true,
      "ltp_weight": 0.01,
      "ltd_weight": 0.012,
      "time_constant": 20.0
    }
  },
  "logging": {
    "level": "info",
    "file": "nlm.log"
  }
}
```

### Command Line Arguments

```bash
./nlm \
  --neurons=10000 \
  --steps=10000 \
  --config=config.json \
  --benchmark \
  --save_results=results.json
```

## Architecture Overview

### Core Components

1. **Brain Layer**: Neural computation engine with spike dynamics
2. **Memory Layer**: Working, episodic, and associative memory systems
3. **Prediction Layer**: Temporal sequence learning and forecasting
4. **Cognition Layer**: Attention, planning, and self-model
5. **Neuromodulation Layer**: Reward, novelty, and curiosity signals

### Key Features

- **Event-Driven Processing**: Only active neurons consume computation
- **Sparse Connectivity**: Efficient O(k) synapse access (k = connections per neuron)
- **Hardware Optimization**: SIMD vectorization for performance
- **Memory Efficiency**: Pre-allocated pools and cache-friendly layouts
- **Extensible Design**: Plugin architecture for new components

## Performance Guidelines

### Scaling Targets

| Scale | Neurons | Synapses | Memory | Steps/sec |
|-------|---------|----------|--------|-----------|
| Micro | 1,000 | ~10,000 | ~1 MB | 10,000+ |
| Small | 10,000 | ~100,000 | ~10 MB | 5,000+ |
| Medium | 100,000 | ~1,000,000 | ~100 MB | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | ~1 GB | 100+ |

### Performance Tips

1. **Profile First**: Use `perf` or `valgrind` before optimizing
2. **Measure Impact**: Compare before/after benchmarks
3. **Hardware Considerations**: Cache affinity, NUMA, AVX-512
4. **Memory Bound**: Optimize for memory bandwidth, not CPU cycles
5. **Threading**: Use thread pools, avoid global locks

### Common Pitfalls

1. **Excessive Connectivity**: Avoid dense matrices for large networks
2. **Memory Bloat**: Monitor memory usage at target scale
3. **Cache Misses**: Structure data for cache locality
4. **Contention**: Minimize global synchronization
5. **Precision Loss**: Check numerical stability for float operations

## Advanced Usage

### Python Integration

```python
import nlm
import numpy as np

# Create brain configuration
config = nlm.Config({
    "neuron_count": 10000,
    "stdp": {"enabled": True}
})

# Initialize brain
brain = nlm.Brain(config)

# Run simulation
for step in range(1000):
    brain.step(step)

# Access results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Custom Components

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
    }
    
    // Implement pure virtual methods...
};
```

### Development Setup

```bash
# Install development dependencies
cmake -DCMAKE_BUILD_TYPE=Debug -DNLM_DEVELOPMENT=ON ..
make -j$(nproc)

# Run unit tests
./nlm --test=unit

# Run integration tests
./nlm --test=integration

# Profile specific tests
perf record ./nlm --test=benchmark
perf report
```

## Troubleshooting

### Common Issues

#### "Configuration file not found"
```bash
# Check working directory
ls -la config.json

# Or specify full path
cmake -DCONFIG_PATH=/absolute/path/config.json ..
```

#### "Memory allocation failed"
```bash
# Reduce memory usage
./nlm --neurons=1000 --steps=1000

# Or enable compression
./nlm --neurons=10000 --memory_compress=ON
```

#### "Performance below expectations"
```bash
# Profile with hardware counters
perf stat -e cache-references,cache-misses ./nlm --benchmark

# Check implementation issues
./nlm --validate_implementation
```

### Getting Help

- **Documentation**: https://github.com/nlm/docs
- **Community**: #nlm on Discord
- **Issues**: File bugs at GitHub issues
- **Examples**: Check examples/ directory

## Support

For technical support, refer to the NLM documentation website or join the community forums. Include:

- Your platform and version
- Configuration used
- Error messages and stack traces
- Reproduction steps

## Contributing

1. **Fork** the repository
2. **Create** a feature branch
3. **Follow** the code style and conventions
4. **Add** documentation for new features
5. **Run** all tests before submitting
6. **Update** documentation as needed

## License

NLM is distributed under the MIT License. See LICENSE file for details.