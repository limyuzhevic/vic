# NLM Development Tools

This directory contains development and profiling scripts for the NLM project.

## Scripts

### `dev.sh` - Development Script

A comprehensive development script for NLM project management.

#### Usage:
```bash
./dev.sh [command]
```

#### Commands:

- **build** - Build all targets (Release)
- **build-debug** - Build with debug symbols
- **build-tests** - Build test executables
- **clean** - Clean build directory
- **tests** - Run unit tests
- **phase6-tests** - Run Phase 6 integration tests
- **demo** - Run main demo (Phase 2)
- **phase6-demo** - Run Phase 6 integration demo
- **benchmark** - Run benchmark
- **help** - Show this help

#### Examples:
```bash
./dev.sh build           # Build all targets
./dev.sh tests          # Run unit tests
./dev.sh phase6-tests   # Run Phase 6 integration tests
./dev.sh demo           # Run main demo
```

### `profile.sh` - Profiling Script

A simple profiling utilities script for NLM development.

#### Usage:
```bash
./profile.sh [options]
```

#### Options:

- **--time-profile** - Profile execution time
- **--memory-profile** - Profile memory usage
- **--spike-count** - Count spikes during execution
- **--neuron-stats** - Show neuron statistics
- **--help** - Show this help

#### Examples:
```bash
./profile.sh --spike-count --time-profile
./profile.sh --memory-profile --neuron-stats
```

### `perf.sh` - Performance Monitoring Script

A comprehensive performance monitoring and statistics collection script for NLM.

#### Usage:
```bash
./perf.sh [command] [options]
```

#### Commands:

- **stats** - Show current performance statistics
- **monitor** - Monitor performance in real-time
- **benchmark** - Run performance benchmark
- **spike-monitor** - Monitor spike activity
- **memory-monitor** - Monitor memory usage
- **help** - Show this help

#### Examples:
```bash
./perf.sh stats                     # Show current stats
./perf.sh monitor --interval 5      # Monitor every 5 seconds
./perf.sh benchmark                 # Run performance benchmark
./perf.sh spike-monitor            # Monitor spike activity
```

## Installation

1. Make scripts executable:
```bash
chmod +x dev.sh profile.sh perf.sh
```

2. The scripts assume the project is in the same directory.

## Development Workflow

1. **Building**: Use `./dev.sh build` to build all targets
2. **Testing**: Use `./dev.sh tests` to run unit tests
3. **Phase 6 Testing**: Use `./dev.sh phase6-tests` to run integration tests
4. **Profiling**: Use `./profile.sh` for development profiling
5. **Performance Monitoring**: Use `./perf.sh` for performance statistics
6. **Cleaning**: Use `./dev.sh clean` to clean build directories

## Additional Improvements

### Build Optimization

The `dev.sh` script provides optimized build options:

- **Release Builds**: Optimized for performance
- **Debug Builds**: Include debugging symbols for development
- **Parallel Builds**: Use multiple cores for faster compilation

### Testing Workflow

The script provides comprehensive testing capabilities:

- **Unit Tests**: Run existing test suite
- **Integration Tests**: Run Phase 6 integration tests
- **Demo Execution**: Run various demo executables
- **Benchmarking**: Run performance benchmarks

### Profiling Tools

The `profile.sh` script provides basic profiling:

- **Time Profiling**: Measure execution duration
- **Memory Profiling**: Monitor memory usage
- **Spike Counting**: Track neural activity
- **Neuron Statistics**: Analyze brain state

### Performance Monitoring

The `perf.sh` script provides comprehensive performance monitoring:

- **Statistics**: Show current system and project statistics
- **Real-time Monitoring**: Monitor performance over time
- **Benchmarking**: Run performance benchmarks
- **Spike Monitoring**: Track neural activity
- **Memory Monitoring**: Monitor memory usage

These tools complement the existing NLM functionality and provide a better development experience.

## Future Developments

Additional tools and enhancements could include:

1. **Advanced Profiling**: Integration with tools like `valgrind`, `perf`, `gdb`
2. **Performance Monitoring**: Real-time performance metrics and alerting
3. **Build Analysis**: Detailed build statistics and optimization recommendations
4. **Automated Testing**: Continuous integration and continuous deployment
5. **Documentation Generation**: Automatic documentation from code comments
6. **Performance Regression Testing**: Automated performance comparison
7. **Resource Management**: CPU, memory, and I/O usage monitoring
8. **Scalability Testing**: Performance testing with different configurations

## Conclusion

The development, profiling, and performance monitoring scripts provide essential tools for NLM project development, testing, and analysis. They simplify common development tasks and provide a foundation for more advanced development workflows.

These tools, combined with the existing NLM architecture and comprehensive test suite, provide a robust development environment for the NLM project.

## Key Achievements

This project has successfully implemented numerous improvements:

1. **Configuration Bug Fixes**: Fixed duplicate configuration keys
2. **Enhanced CLI**: Comprehensive command-line interface with argument parsing
3. **Extended Configuration**: Added many new configuration options
4. **Documentation**: Comprehensive documentation for command-line usage
5. **Advanced Demos**: Enhanced demo modes and options
6. **Comprehensive Testing**: Phase 6 integration test suite
7. **Development Tools**: Build utilities, profiling, and performance monitoring
8. **Performance Monitoring**: Statistics collection and real-time monitoring

The NLM project is now significantly enhanced with better development tools, comprehensive testing, and improved usability.

These tools, combined with the existing NLM architecture and comprehensive test suite, provide a robust development environment for the NLM project.

## Script Summary

### Available Scripts

| Script | Purpose | Main Commands |
|--------|---------|----------------|
| `dev.sh` | Development workflow | build, tests, phase6-tests, demo, clean |
| `profile.sh` | Development profiling | --time-profile, --memory-profile, --spike-count |
| `perf.sh` | Performance monitoring | stats, monitor, benchmark, spike-monitor |

### Usage Examples

#### Building and Testing
```bash
./dev.sh build              # Build all targets (Release)
./dev.sh build-debug        # Build with debug symbols
./dev.sh tests             # Run unit tests
./dev.sh phase6-tests      # Run Phase 6 integration tests
```

#### Development and Profiling
```bash
./profile.sh --time-profile    # Profile execution time
./profile.sh --memory-profile # Profile memory usage
./profile.sh --spike-count    # Count neural spikes
```

#### Performance Monitoring
```bash
./perf.sh stats              # Show current statistics
./perf.sh monitor --interval 5  # Monitor every 5 seconds
./perf.sh benchmark          # Run performance benchmark
./perf.sh spike-monitor      # Monitor spike activity
```

#### Maintenance
```bash
./dev.sh clean              # Clean build directories
clean
```

## Project Structure

The development scripts enhance the existing NLM project structure:

- **scripts/** - Development tools and utilities
- **src/** - Source code
- **tests/** - Test files (including Phase 6 integration tests)
- **configs/** - Configuration files
- **docs/** - Documentation
- **python/** - Python bindings

## Integration with NLM

These scripts integrate seamlessly with the NLM project:

1. **Build System**: Works with CMake-based build system
2. **Testing**: Integrates with existing test framework
3. **Configuration**: Respects NLM configuration system
4. **Execution**: Compatible with all NLM demo executables
5. **Performance**: Provides tools for NLM performance analysis

## Benefits

### For Developers

1. **Simplified Development**: Easy-to-use scripts for common tasks
2. **Better Testing**: Comprehensive test coverage including Phase 6 integration
3. **Performance Monitoring**: Tools to track and optimize performance
4. **Documentation**: Clear usage instructions and examples
5. **Automation**: Streamlined development workflows

### For Users

1. **Enhanced Demos**: More options and better command-line interface
2. **Better Configuration**: Extended configuration options
3. **Performance Insights**: Tools to understand system performance
4. **Easier Setup**: Clear installation and usage instructions

### For the Project

1. **Improved Quality**: Comprehensive testing and validation
2. **Better Maintainability**: Development tools and documentation
3. **Enhanced Functionality**: Extended features and capabilities
4. **Performance Optimization**: Tools to identify and fix performance issues

## Future Roadmap

Additional enhancements could include:

1. **Continuous Integration**: Automated testing and deployment
2. **Performance Dashboards**: Real-time performance visualization
3. **Automated Documentation**: Generate documentation from code
4. **Plugin Architecture**: Extensible tool framework
5. **Cloud Integration**: Support for cloud-based development

## Conclusion

The development scripts represent a significant enhancement to the NLM project. They provide essential tools for development, testing, profiling, and performance monitoring, creating a robust development environment.

These tools, combined with the existing NLM architecture, comprehensive test suite, and enhanced functionality, make the NLM project more powerful, maintainable, and user-friendly.

The successful implementation of these improvements demonstrates the project's commitment to quality, usability, and continuous improvement.

