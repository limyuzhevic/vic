# NLM Unified Demo Launcher

A unified command-line launcher for NLM demos that provides:
- Consistent argument parsing
- Mode switching
- Configuration management
- Help and version information
- Integration between different demo executables

## Features

### Demo Modes
- **demo**: Run Phase 6 integration demo (recommended for new users)
- **benchmark**: Run performance benchmarks
- **experiment**: Run comprehensive experiment suite
- **analysis**: Run detailed system analysis
- **replay**: Run memory replay demonstration
- **performance**: Run performance profiling

### Configuration
- Loads configuration from configs/default.cfg
- Supports command-line overrides
- Provides predefined configuration presets
- Validates configuration settings

### Output
- Color-coded status messages
- Progress indicators
- Performance metrics
- Integration verification reports

## Usage

```bash
# Run the main integration demo (recommended)
nlmdemo demo --quick

# Run a specific benchmark
nnlmdemo benchmark --threads 4 --duration 60

# Run analysis with custom configuration
nnlmdemo analysis --config my_config.cfg --neurons 2000

# List all available demos
nnlmdemo list

# Get help information
nnlmdemo help
nnlmdemo help demo
nnlmdemo help benchmark
```

## Configuration Files

### Default Configuration
`configs/default.cfg` contains default settings for all demos.

### Demo-Specific Configurations
- `configs/demo.cfg`: Settings for main demo
- `configs/benchmark.cfg`: Performance test settings
- `configs/analysis.cfg`: System analysis settings

### User Configurations
- `~/.nlm/config.cfg`: User-specific configuration
- `./config.cfg`: Project-specific configuration

## Exit Codes
- `0`: Success
- `1`: Demo failed
- `2`: Configuration error
- `3`: Missing dependencies
- `4`: User interrupted

## Logging

### Console Output
- INFO: General information and progress
- WARNING: Potential issues or limitations
- ERROR: Failures or errors that prevent completion
- DEBUG: Detailed debugging information

### Log Files
- `nlm_demo.log`: Demo execution logs
- `nlm_performance.log`: Performance metrics
- `nlm_integration.log`: Integration test results

## Examples

### Quick Demo Run
```bash
nnlmdemo demo --quick --verbose
```

### Performance Benchmark
```bash
nnlmdemo benchmark --thorough --threads 8 --output benchmark_results.json
```

### System Analysis
```bash
nnlmdemo analysis --neurons 5000 --regions 4 --plasticity --neuromodulation
```

### Replay Demonstration
```bash
nnlmdemo replay --checkpoint demo_checkpoint.bin --steps 1000
```

## Configuration Presets

### --quick
- Small scale (100 neurons)
- Short duration (1000 steps)
- Minimal features
- Fast execution

### --thorough
- Large scale (5000 neurons)
- Long duration (10000 steps)
- All features enabled
- Comprehensive testing

### --learning
- Focus on plasticity and learning
- Reinforcement learning emphasis
- Novelty detection enabled
- Curiosity-driven exploration

### --stability
- Stress testing
- Long-term stability checks
- Memory leak detection
- Performance under load

## Installation

### System Requirements
- C++20 compiler
- CMake 3.16 or higher
- At least 1GB RAM (recommended 4GB+)

### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Installation
```bash
make install
```

The launcher will be installed to:
- `/usr/local/bin/nnlmdemo` (system-wide)
- `~/.local/bin/nnlmdemo` (user-specific)

## File Structure

```
nlmdemo/
├── include/
│   ├── launcher/
│   ├── config/
│   └── demo/
├── src/
│   ├── main.cpp
│   ├── launcher.cpp
│   ├── config_manager.cpp
│   └── demo_runner.cpp
├── demos/
│   ├── phase6_integration.cpp
│   ├── benchmark.cpp
│   ├── analysis.cpp
│   └── replay.cpp
├── tests/
│   ├── launcher_test.cpp
│   └── config_test.cpp
└── examples/
    ├── quick_demo.cpp
    ├── learning_demo.cpp
    └── stability_test.cpp
```

## Development

### Building the Launcher
```bash
cd /path/to/nlm
mkdir build
 cd build
cmake .. -DNLM_BUILD_LAUNCHER=ON
make
```

### Adding New Demos
1. Create a new demo source file in `demos/`
2. Implement the demo interface
3. Add to `demo_registry.h`
4. Update documentation

### Configuration Schemas
1. Add new configuration parameters to `config_schema.h`
2. Implement validation functions
3. Update default.cfg

### Testing
```bash
# Run unit tests
make test

# Run demo tests
./build/nnlmdemo demo --test

# Run integration tests
./build/nnlmdemo test --integration
```

## Troubleshooting

### Common Issues

#### "Demo failed to initialize"
- Check system resources
- Verify all dependencies are installed
- Try with `--quick` mode first
- Check log files for detailed error messages

#### "Configuration file not found"
- Create the configuration file
- Use absolute paths
- Check file permissions

#### "Performance benchmarks failing"
- Reduce system load
- Check available memory
- Try with fewer threads

### Getting Help

- **Command line help**: `nnlmdemo help`
- **Demo-specific help**: `nnlmdemo help <demo_name>`
- **Configuration help**: `nnlmdemo help config`
- **Examples**: `nnlmdemo examples`
- **Troubleshooting**: `nnlmdemo troubleshooting`

## Future Enhancements

### Planned Features
- **Web interface**: HTTP API for remote control
- **GUI**: Graphical user interface
- **Plugin system**: Extend with third-party plugins
- **Remote monitoring**: Real-time system monitoring
- **Automated testing**: CI/CD integration

### Performance Improvements
- **Parallel processing**: Multi-core optimization
- **Memory pooling**: Reduced allocation overhead
- **Asynchronous I/O**: Non-blocking operations
- **Hardware acceleration**: GPU support

## License

MIT License - See LICENSE file for details.