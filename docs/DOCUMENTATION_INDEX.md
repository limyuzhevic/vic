# NLM Documentation Index

Welcome to the NLM (熙然) documentation! This document provides an overview of available documentation and resources for the NLM Phase 6 project.

## Documentation Overview

This project provides comprehensive documentation for using, extending, and contributing to the NLM (熙然) neural network simulation system. Below is a structured guide to all available documentation.

## Primary Documentation

### 1. Main README

**`README.md`** - Core project documentation
- Project overview and architecture
- Building and installation instructions
- Basic usage examples
- Current Phase information
- License and acknowledgments

### 2. Enhanced README

**`README_ENHANCED.md`** - Comprehensive documentation
- Quick start guide with examples
- Detailed Phase 6 feature documentation
- Performance optimization tips
- Troubleshooting guide
- Advanced configuration
- Example code snippets

### 3. API Documentation

**Source code headers** (`.hpp` files) - Complete API documentation with:
- Class and method documentation
- Parameter descriptions
- Return value explanations
- Usage examples

## Specialized Documentation

### 4. Phase 6 Integration Guide

**`src/experiments/Phase6IntegratedExperiment.hpp`**
- Detailed explanation of Phase 6 integration experiment
- System verification procedures
- Integration testing methodologies

### 5. Configuration Guide

**`src/experiments/Phase6EnhancedConfig.hpp`**
- Comprehensive configuration system documentation
- Configuration options organized by category
- Validation and error handling
- Preset configurations

### 6. Enhanced Logging System

**`src/core/EnhancedLogger.hpp`**
- Advanced logging capabilities
- Performance monitoring
- Structured logging with JSON output
- Debug and profiling features

### 7. Performance Optimizations

**`src/performance/PerformanceOptimizations.hpp`**
- SIMD vectorization
- Memory pool optimization
- Parallel processing utilities
- Cache-friendly data structures

### 8. Agent System Documentation

**`src/agent/EnhancedAgentBrain.hpp`**
- Agent architecture and design
- Configuration and customization
- Integration with brain systems
- Performance considerations

## Example Documentation

### 9. Quick Start Examples

**`examples/quickstart.md`**
- 5-minute quick start guide
- Basic usage with command line
- Simple configuration examples

### 10. Advanced Examples

**`examples/advanced/`**
- Complex scenario examples
- Integration with external systems
- Performance optimization examples
- Custom configuration examples

### 11. Tutorial Documentation

**`examples/tutorials/`**
- Step-by-step tutorials
- Real-world use cases
- Experiment design guides
- Best practices

## Configuration Documentation

### 12. Configuration Reference

**`docs/configuration.md`**
- Complete configuration parameter reference
- Configuration file format
- Validation rules and error messages
- Preset configuration details

### 13. Performance Configuration

**`docs/performance.md`**
- Performance tuning guide
- Optimization strategies
- Resource allocation recommendations
- Benchmarking procedures

### 14. Logging Configuration

**`docs/logging.md`**
- Logging levels and categories
- Output configuration
- Performance monitoring
- Debug and profiling

## Development Documentation

### 15. Architecture Guide

**`docs/architecture.md`**
- System architecture and components
- Data flow and interactions
- Design patterns and principles
- Future development directions

### 16. Extension Guide

**`docs/extension.md`**
- Adding new brain systems
- Custom memory implementations
- Plugin architecture
- Contribution guidelines

### 17. Testing Documentation

**`docs/testing.md`**
- Test suite structure
- Unit testing guidelines
- Integration testing procedures
- Performance testing

## Technical Reference

### 18. Core Types

**`src/core/Types/Types.hpp`**
- Strongly-typed identifiers
- Configuration value types
- System time and simulation units

### 19. Brain System Interfaces

**`src/brain/Brain.hpp`**
- Brain interface documentation
- System management
- State and checkpoint operations

### 20. Neuron Implementation

**`src/brain/Neuron.hpp`**
- LIF neuron dynamics
- Plasticity mechanisms
- Refractory processes
- Spike generation

## Examples and Samples

### 21. Phase 3 Demo

**`src/experiments/Phase3Demo.cpp`**
- World interaction example
- Experience-driven learning
- Action selection and consequences

### 22. Phase 4 Demo

**`src/experiments/Phase4Demo.cpp`**
- Advanced learning scenarios
- Social interaction simulations
- Concept formation examples

### 23. Phase 6 Demo

**`src/experiments/nlm_phase6_demo.cpp`**
- Comprehensive integration testing
- Performance benchmarking
- Memory system verification

## Performance Documentation

### 24. SIMD Optimization Guide

**`docs/simd.md`**
- SIMD instruction set utilization
- Vectorization strategies
- Performance benchmarks
- Code examples

### 25. Memory Management

**`docs/memory.md`**
- Memory pool implementation
- Cache optimization
- NUMA considerations
- Garbage collection strategies

### 26. Parallel Processing

**`docs/parallel.md`**
- Multi-threading strategies
- Work distribution
- Load balancing
- Synchronization primitives

## Troubleshooting and Support

### 27. Common Issues

**`docs/troubleshooting.md`**
- Frequently encountered problems
- Symptom analysis
- Solution procedures
- Community support

### 28. Debugging Guide

**`docs/debugging.md`**
- Debug mode activation
- Logging configuration
- Stack trace analysis
- Memory profiling

### 29. Performance Analysis

**`docs/performance_analysis.md`**
- Profiling tools and techniques
- Performance bottleneck identification
- Optimization strategies
- Benchmarking procedures

## Contributing

### 30. Contribution Guidelines

**`CONTRIBUTING.md`**
- Code style and formatting
- Testing requirements
- Documentation standards
- Pull request process

### 31. Development Setup

**`DEVELOPMENT.md`**
- Development environment setup
- Build and test procedures
- IDE configuration
- Version control guidelines

## Quick Start Summary

### Getting Started (5 Minutes)

```bash
# 1. Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# 2. Run Phase 6 demo
./nlm_phase6_demo

# 3. View results
./nlm_phase6_demo --help
```

### Basic Configuration Example

```cpp
#include "src/experiments/Phase6Demo.hpp"

int main() {
    nlm::Phase6Demo demo;
    nlm::Phase6DemoConfig config;
    config.maxSteps = 1000;
    config.neuronCount = 200;
    
    if (demo.run(config)) {
        std::cout << "Experiment completed successfully!" << std::endl;
    }
    
    return 0;
}
```

### Advanced Configuration Example

```cpp
#include "src/experiments/Phase6EnhancedConfig.hpp"

int main() {
    nlm::Phase6Config config;
    
    // Configure experiment parameters
    config.experimentConfig_.maxSteps = 5000;
    config.experimentConfig_.neuronCount = 1000;
    config.experimentConfig_.enableCheckpointing = true;
    
    // Configure performance settings
    config.performanceConfig_.enableVectorization = true;
    config.performanceConfig_.enableParallelProcessing = true;
    config.performanceConfig_.maxThreads = 8;
    
    // Configure memory systems
    config.memoryConfig_.enableWorkingMemory = true;
    config.memoryConfig_.enableEpisodicMemory = true;
    config.memoryConfig_.replayFrequency = 3600.0;
    
    // Save configuration
    config.saveToFile("my_config.json");
    
    return 0;
}
```

## Support Resources

### Project Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Examples**: https://github.com/nlm-project/nlm-examples

### Community Support

- **Slack Channel**: #nlm-support
- **Discord Server**: NLM Community
- **Stack Overflow**: Questions tagged `[nlm]`

### Getting Help

1. **Check Documentation**: Review relevant documentation sections
2. **Search Examples**: Look for similar use cases in examples
3. **Test Reproduction**: Create minimal reproducible examples
4. **Community Support**: Use appropriate channels for different issue types
5. **Code Review**: Submit issues with complete context and reproduction steps

## Conclusion

This documentation provides comprehensive guidance for using, extending, and contributing to the NLM Phase 6 project. Whether you are a beginner looking to get started quickly or an advanced user implementing custom systems, this documentation serves as a complete reference for all aspects of the NLM framework.

For the most up-to-date documentation and additional resources, visit the official NLM project documentation website.

---

**Version**: 6.0.0 (Phase 6)
**Last Updated**: October 2026
**License**: MIT
**Documentation**: https://nlm.readthedocs.io
