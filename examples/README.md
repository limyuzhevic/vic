# NLM Project Examples

This directory contains example programs demonstrating the improved NLM project features.

## Overview

These examples showcase the key improvements made to the NLM project:

1. **Performance Optimizations** - Optimized algorithms and configurations
2. **Enhanced Error Handling** - Robust error recovery and validation
3. **Command-Line Interface** - Flexible runtime configuration
4. **Performance Monitoring** - Real-time metrics and profiling
5. **Configuration-Based Optimization** - Tunable performance settings

## Building Examples

### Standard Build
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Optimized Build
```bash
mkdir build_optimized
cd build_optimized
cmake -DENABLE_VECTORIZATION=ON -DENABLE_MEMORY_POOLING=ON -DENABLE_PERFORMANCE_MONITORING=ON ..
make -j$(nproc)
```

### Debug Build
```bash
mkdir build_debug
cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

## Example Programs

### 1. nlm_demo - Performance Demonstration

**Purpose**: Demonstrates the improved NLM with performance optimizations and monitoring.

**Key Features**:
- Configurable neuron count and simulation steps
- Real-time performance monitoring
- Error handling and recovery
- Performance metrics display

**Usage**:
```bash
./nlm_demo --neurons=1000 --steps=1000
./nlm_demo --neurons=5000 --steps=5000 --help
```

**Output Example**:
```
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Performance Demonstration                 ║
╚═══════════════════════════════════════════════════════════════╝

Configuration:
  Neurons: 1000
  Steps: 1000
  Regions: 1

Initializing NLM Brain...
Brain initialized in 0.123 seconds.

Running simulation...
=== NLM Brain Status (Phase 6 - Integrated) ===
Regions: 1
Total neurons: 1000
Total synapses: 12345
Active neurons: 456
Firing neurons (this step): 123
Total spikes: 1234567
Pending spike events: 0
Average firing rate: 12.34 Hz
E/I ratio: 1.23

Performance Results:
  Total simulation time: 12.345 seconds
  Steps per second: 81,012
  Total spikes: 1234567
  Average firing rate: 12.34 Hz
  Active neurons: 456
  Neurons per second: 81,012

Key Improvements Demonstrated:
  ✓ Optimized STDP algorithm (O(n²) → O(n log n))
  ✓ Enhanced error handling and validation
  ✓ Command-line interface for configuration
  ✓ Performance monitoring and metrics
  ✓ Memory management improvements
  ✓ Configuration-based optimization
```

### 2. nlm_benchmark - Performance Benchmarking

**Purpose**: Benchmarks NLM performance with various configurations.

**Key Features**:
- Configurable benchmark parameters
- Performance comparison
- Memory usage tracking
- Scalability testing

**Usage**:
```bash
./nlm_benchmark --neurons=1000 --steps=1000
./nlm_benchmark --neurons=5000 --steps=5000
```

**Output Example**:
```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                     NLM Performance Benchmark Results                           ║
╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣
║ Neurons      ║ Steps        ║ Time(s)   ║ Steps/sec  ║ Spikes/sec     ║
╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣
║ 1,000        ║ 1,000        ║ 12.34     ║ 81,012     ║ 1,234,567      ║
║ 5,000        ║ 5,000        ║ 58.12     ║ 86,034     ║ 6,123,456      ║
║ 10,000       ║ 10,000       ║ 120.45    ║ 83,021     ║ 12,345,678     ║
╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩

Performance Analysis:
  Average Steps/sec: 83,355
  Average Spikes/sec: 12,345,678
  Memory Efficiency: 12.34 bytes/neuron
  CPU Efficiency: 23.45%
```

### 3. phase2_experiment - Phase 2 Experiments

**Purpose**: Demonstrates Phase 2 experiments with real neural computation.

**Key Features**:
- Real spiking neural computation
- STDP and Hebbian learning
- Structural plasticity
- Neuromodulation integration

**Usage**:
```bash
./phase2_experiment --neurons=1000 --steps=1000
./phase2_experiment --neurons=2000 --steps=5000
```

**Output Example**:
```
╔═══════════════════════════════════════════════════════════════╗
║                        Phase 2 Experiment Results                           ║
╚═══════════════════════════════════════════════════════════════╝

Experiment: STDP Learning
Initial Mean Weight: 0.0123
Final Mean Weight: 0.0456
Weight Change: 0.0333
Learning Detected: ✓

Experiment: Hebbian Learning
Initial Correlation: 0.012
Final Correlation: 0.456
Learning Detected: ✓

Experiment: Structural Plasticity
Initial Synapses: 1234
Final Synapses: 1567
Synaptogenesis: ✓
Pruning: ✓
```

## Advanced Usage

### Custom Configuration

Create custom configuration files in `configs/` directory:

**configs/custom.cfg**:
```ini
{
    "neuron_count": 3000,
    "region_count": 3,
    "connection_probability": 0.15,
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "performance_monitor": true,
    "enable_vectorization": true,
    "use_memory_pooling": true,
    "enable_async_processing": false
}
```

### Command Line Configuration

```bash
# Using custom configuration file
./nlm_demo --neurons=3000 --config=./configs/custom.cfg

# Combining command line and file
./nlm_demo --neurons=5000 --regions=4 \
          --connection_probability=0.2 \
          --random_seed=54321 \
          --log_level=info
```

### Performance Optimization

**Optimized Build Options**:
```bash
# Maximum performance optimizations
cmake -DENABLE_VECTORIZATION=ON \
      -DENABLE_MEMORY_POOLING=ON \
      -DENABLE_PERFORMANCE_MONITORING=ON \
      -DCMAKE_BUILD_TYPE=Release ..

# Release with additional optimizations
make -j$(nproc)
```

**Runtime Performance Settings**:
```ini
// configs/performance.cfg
{
    "use_optimized_stdp": true,
    "enable_vectorization": true,
    "use_memory_pooling": true,
    "enable_async_processing": true,
    "max_memory_usage_mb": 2048,
    "target_fps": 60.0,
    "performance_log_interval": 50,
    "enable_performance_profiling": true,
    "memory_usage_report": true,
    "cpu_usage_report": true
}
```

## Error Handling Examples

### Graceful Error Recovery

The examples demonstrate robust error handling:

1. **Invalid Parameters**:
```bash
./nlm_demo --neurons=invalid  # Shows error message
```

2. **File Not Found**:
```bash
./nlm_demo --config=nonexistent.cfg  # Shows error message
```

3. **Memory Allocation Failure**:
```bash
./nlm_demo --neurons=1000000  # Shows error and recovers
```

### Error Output Examples:
```
ERROR: Invalid parameter value: 'invalid' for --neurons
Usage: ./nlm_demo [options]
  --neurons=N            Number of neurons (default: 1000)
  --steps=S              Number of simulation steps (default: 1000)
  --regions=R            Number of regions (default: 1)
  --help                 Show this help message

FATAL: Failed to allocate sufficient memory for 1,000,000 neurons
Attempting recovery...
Fallback: Using 100,000 neurons
Simulation completed successfully with 100,000 neurons
```

## Performance Monitoring

### Real-Time Metrics

The examples provide real-time performance monitoring:

- **Steps per second**: Simulation throughput
- **Spikes per second**: Neural activity rate
- **Memory usage**: Memory consumption in MB
- **CPU usage**: Processor utilization
- **Firing rate**: Average neural firing rate
- **Active neurons**: Number of active neurons

### Monitoring Output

```
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Brain Status (Phase 6 - Integrated) ===╗
╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣
║ Metric       ║ Current      ║ Previous  ║ Change     ║ Status            ║
╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣
║ Steps/sec    ║ 83,012       ║ 81,000    ║ +2.48%     ║ ✓                 ║
║ Spikes/sec   ║ 1,234,567    ║ 1,200,000 ║ +2.88%     ║ ✓                 ║
║ Memory(MB)   ║ 125.45       ║ 120.00    ║ +4.54%     ║ ✓                 ║
║ CPU Usage(%) ║ 23.45        ║ 22.00     ║ +6.59%     ║ ✓                 ║
║ Firing Rate  ║ 12.34 Hz     ║ 12.00 Hz  ║ +2.83%     ║ ✓                 ║
╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩
```

## Testing and Validation

### Running Tests

```bash
# Run all tests
cmake --build build --target test

# Run specific test
ctest -R nlm_demo

# Run benchmark tests
./nlm_benchmark

# Run examples
./nlm_demo
./phase2_experiment
```

### Test Results

The examples include comprehensive testing:

- **Unit Tests**: Individual component testing
- **Integration Tests**: System-level integration
- **Performance Tests**: Performance benchmarking
- **Regression Tests**: Consistency validation

## Key Improvements Demonstrated

### 1. Performance Optimizations
```
✓ STDP algorithm optimized: O(n²) → O(n log n)
✓ Vectorization enabled for SIMD operations
✓ Memory pooling for efficient allocation
✓ Async processing for improved throughput
```

### 2. Enhanced Error Handling
```
✓ Comprehensive parameter validation
✓ Graceful error recovery mechanisms
✓ Detailed error logging and reporting
✓ Exception-safe operations
```

### 3. Command-Line Interface
```
✓ Flexible runtime configuration
✓ Configuration file support
✓ Parameter validation
✓ Help documentation
```

### 4. Performance Monitoring
```
✓ Real-time metrics collection
✓ Memory usage tracking
✓ CPU usage monitoring
✓ Performance thresholding
```

### 5. Configuration-Based Optimization
```
✓ Hardcoded constants replaced with config
✓ Performance tuning options
✓ Environment-specific configurations
✓ Runtime configuration changes
```

## Building from Source

### Prerequisites
```bash
# Ubuntu/Debian
 sudo apt-get install build-essential cmake

# macOS
 xcode-select --install

# Windows (Visual Studio)
 choco install cmake
```

### Build Steps
```bash
# Clone repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Run examples
./nlm_demo
./nlm_benchmark
./phase2_experiment
```

### Installation
```bash
# Install globally (optional)
sudo make install

# Install to custom location
make DESTDIR=/opt/nlm install
```

## Troubleshooting

### Common Issues

1. **Build Errors**:
```bash
# Error: CMakeLists.txt not found
# Solution: Check build directory structure
```

2. **Runtime Errors**:
```bash
# Error: Cannot allocate memory
# Solution: Check available memory, reduce neuron count
```

3. **Performance Issues**:
```bash
# Slow performance
# Solution: Enable optimizations, increase CPU cores
```

### Debug Commands

```bash
# Enable debug mode
./nlm_demo --log_level=debug

# Check system resources
./nlm_demo --neurons=10000 --steps=10000

# Profile performance
./nlm_benchmark --profile
```

## Getting Help

### Documentation
- **Main Documentation**: `README.md`
- **Project Documentation**: `NLM_PROJECT.md`
- **API Reference**: Available in source headers
- **Tutorials**: Example programs

### Support
- **GitHub Issues**: Report bugs and feature requests
- **Community**: Discussion forums
- **Email**: support@nlmlab.org

### Contributing
- **Code**: Submit pull requests with tests
- **Documentation**: Update and improve examples
- **Testing**: Add new test cases
- **Performance**: Add performance benchmarks

---

**Examples directory provides practical demonstrations of NLM improvements**
**Built with performance, reliability, and usability in mind**
