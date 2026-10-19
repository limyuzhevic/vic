# NLM Debugging and Profiling Configuration

This directory contains debugging and profiling configuration files for the NLM project.

## Files in this directory:

### debug/
Debugging tools and utilities.

### profile/
Performance profiling tools.

### scripts/
Development and debugging scripts.

### README.md
Documentation for debugging and profiling tools.

## Usage

### Debugging Tools

The NLM project includes comprehensive debugging tools:

```bash
# Build with debug symbols and sanitizers
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASAN=ON

# Run debugger
gdb --args ./nlm --simulate

# Use Visual Studio Code with C++ extension
code .
```

### Performance Profiling

```bash
# Build with profiling enabled
cmake .. -DNLM_ENABLE_PROFILING=ON

# Run performance benchmark
./nlm_phase6_demo --benchmark

# Profile Python bindings
python -m memory_profiler pynlm/core/brain.py
python -m cProfile -o profile.stats pynlm/core/brain.py
```

### Code Coverage

```bash
# Install coverage tools
pip install coverage pytest-cov

# Run tests with coverage
cd python
pytest --cov=pynlm --cov-report=html

# Generate coverage report
pytest --cov=pynlm --cov-report=term-missing
```

## Debugging Configuration

### C++ Debugging

The C++ debugging configuration includes:

- **Debug symbols**: Full symbol information for debugging
- **Address sanitizer**: Memory error detection
- **Thread sanitizer**: Thread safety issues detection
- **Valgrind**: Memory leak detection
- **GDB integration**: Breakpoints and debugging commands

#### Debug Configuration Options

```cmake
# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Enable address sanitizer
-DNLM_ENABLE_ASAN=ON

# Enable thread sanitizer
-DNLM_ENABLE_TSAN=ON

# Enable valgrind support
-DNLM_ENABLE_VALGRIND=ON

# Enable detailed logging
-DNLM_ENABLE_DEBUG_LOGGING=ON
```

### Python Debugging

The Python debugging configuration includes:

- **Python debugger (pdb)**: Step-by-step debugging
- **ipdb**: IPython-enhanced debugger
- **cProfile**: Performance profiling
- **memory_profiler**: Memory usage profiling
- **line_profiler**: Line-by-line performance profiling

#### Python Debug Configuration

```bash
# Install debugging tools
pip install ipdb memory_profiler line_profiler

# Enable debug mode
export PYNLB_DEBUG=true
export PYNLB_LOG_LEVEL=DEBUG

# Run with debugger
ipdb run script.py
```

## Profiling Configuration

### Performance Profiling

The NLM project includes comprehensive performance profiling capabilities:

#### C++ Profiling

```bash
# Build with profiling enabled
cmake .. -DNLM_ENABLE_PROFILING=ON

# Run with gprof
./nlm --simulate -prof

# Analyze profile data
gprof nlm gmon.out
```

#### Python Profiling

```bash
# Install profiling tools
pip install pytest-benchmark snakeviz

# Run performance benchmarks
cd python
pytest tests/ -v -m benchmark --benchmark-save=benchmarks.json

# Visualize benchmarks
pytest --benchmark-generate-html
```

### Profiling Configuration Options

```python
# Python profiling configuration
profiling_config = {
    'benchmark': {
        'enabled': True,
        'output_dir': 'benchmarks/',
        'format': ['json', 'txt', 'html'],
        'compare_with_previous': True,
        'regression_threshold': 0.05
    },

    'memory': {
        'enabled': True,
        'output_dir': 'memory_reports/',
        'threshold': 100.0,  # MB
        'track_objects': True
    },

    'cpu': {
        'enabled': True,
        'sampling_rate': 100,  # Hz
        'output_dir': 'cpu_reports/',
        'profile_functions': True
    }
}
```

## Debugging Scripts

### Debug Scripts

The debug directory contains scripts for debugging and analyzing NLM:

```
debug/
├── analyze_memory_leaks.py
├── debug_connectivity.py
├── profile_performance.py
├── test_debug_features.py
└── visualize_results.py
```

#### analyze_memory_leaks.py
Memory leak detection and analysis

#### debug_connectivity.py
Debug brain connectivity and communication

#### profile_performance.py
Performance profiling and benchmarking

#### test_debug_features.py
Test debugging features and tools

#### visualize_results.py
Visualize debug and profiling results

### Profiling Scripts

The profile directory contains scripts for performance profiling:

```
profile/
├── benchmark_suite.py
├── generate_report.py
├── performance_regression.py
├── profile_functions.py
└── summarize_results.py
```

#### benchmark_suite.py
Comprehensive benchmark suite

#### generate_report.py
Generate profiling reports

#### performance_regression.py
Detect performance regressions

#### profile_functions.py
Profile individual functions

#### summarize_results.py
Summarize profiling results
```

## Integration with CI/CD

### Debug and Profiling in CI

The NLM CI/CD pipeline includes debug and profiling stages:

```yaml
# .github/workflows/ci.yml
steps:
  # Debug configuration stage
  - name: Configure debug build
    run: |
      cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASAN=ON

  # Build and test debug
  - name: Build debug and test
    run: |
      make -j$(nproc)
      ./nlm_test

  # Profiling stage
  - name: Run performance benchmarks
    run: |
      cd python
      pytest tests/ -v -m benchmark

  # Memory analysis stage
  - name: Memory analysis
    run: |
      cd python
      python -m memory_profiler pynlm/
```

## Debugging Workflow

### Local Development

1. **Setup**: Install debugging tools and configure environment
2. **Build**: Build with debug symbols and sanitizers
3. **Run**: Run application with debugging enabled
4. **Debug**: Use debugger to step through code and identify issues
5. **Profile**: Profile performance and identify bottlenecks
6. **Analyze**: Analyze debug and profiling results
7. **Fix**: Fix issues and repeat

### CI/CD Workflow

1. **Configure**: Configure debug and profiling builds
2. **Build**: Build with appropriate options
3. **Test**: Run tests with debugging enabled
4. **Profile**: Run performance benchmarks
5. **Report**: Generate and analyze reports
6. **Store**: Store results for trend analysis

## Tools and Utilities

### Debugging Utilities

- **log_analysis.py**: Analyze debug logs
- **memory_analyzer.py**: Analyze memory usage
- **performance_tracker.py**: Track performance metrics
- **debug_visualizer.py**: Visualize debug results

### Profiling Utilities

- **benchmark_runner.py**: Run benchmark suite
- **performance_comparator.py**: Compare performance across versions
- **regression_detector.py**: Detect performance regressions
- **report_generator.py**: Generate profiling reports

## Configuration Files

### debug.cfg
Debug configuration file

### profile.cfg
Profiling configuration file

### benchmarks.cfg
Benchmark configuration file

### memory.cfg
Memory analysis configuration file

## Troubleshooting

### Common Debugging Issues

#### Symbol resolution
- Ensure debug symbols are enabled
- Use appropriate debugging tools
- Check library paths

#### Performance profiling
- Ensure profiling is enabled in build configuration
- Use appropriate profiling tools
- Check system resources

#### Memory analysis
- Ensure address sanitizer is enabled
- Check memory limits
- Monitor memory usage

### Getting Help

- **Documentation**: https://nlm.readthedocs.io/debugging
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Slack/Discord**: Contact project maintainers for debugging support

## License

MIT License. See project root for details.