# NLM - Advanced User Guide and Documentation
# Version 2.0.0 - Phase 2 with Advanced Features

## Overview

NLM (Neural Learning Machine) is a sophisticated brain simulator that implements real spiking neural computation. This guide covers both basic usage and advanced features for power users.

## Quick Start

### Basic Usage

```bash
# Run with default configuration
nlm

# Run with custom neuron count and connection probability
nlm --neuron-count 2000 --connection-prob 0.08

# Load from custom config file
nlm --config my_config.cfg

# Save results to output file
nlm --output results.nml

# Run in benchmark mode for performance testing
nlm --benchmark --benchmark-time 5000
```

### Configuration File Format

NLM supports both simple key=value format and JSON/YAML format. Here are examples:

**Simple Format (configs/default.cfg):**
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

**JSON Format (configs/my_config.json):**
```json
{
    "nlm_config": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "log_level": "INFO",
        "log_to_file": false
    }
}
```

## Improved Configuration System

### Key Improvements

1. **JSON/YAML Parser**: Full support for structured configuration files
2. **Constants-Based Configuration**: Eliminates magic numbers with well-defined defaults
3. **Advanced Error Handling**: Detailed error messages and validation
4. **Save with Real Values**: Configuration files now save actual values, not placeholders

### Configuration Constants

```cpp
namespace nlm {
namespace config {
    constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    // ... many more constants
}
}
```

### Command Line Interface

**Basic Options:**
- `-h, --help` - Show help message
- `--version` - Show version information
- `--list-configs` - List available configuration files

**Configuration Options:**
- `--config FILE, -c FILE` - Load configuration from file
- `--output FILE` - Save results to file

**Simulation Options:**
- `--neuron-count N` - Number of neurons
- `--connection-prob P` - Connection probability
- `--timestep T` - Simulation timestep
- `--seed SEED` - Random seed
- `--log-level LEVEL` - Set log level (DEBUG, INFO, WARNING, ERROR, FATAL)

**Advanced Options:**
- `--benchmark` - Run in benchmark mode
- `--benchmark-time STEPS` - Specify benchmark steps
- `--verbose` - Enable verbose output
- `--quiet` - Suppress non-error output

## Advanced Features for Power Users

### Performance Profiling

```bash
# Run with performance profiling
nlm --benchmark --output perf_data.nml

# Performance profiling is enabled in config
enablePerformanceProfiling = true
perfOutputFile = "performance.prof"
perfSamplingInterval = 0.05
```

### Checkpointing and Serialization

```cpp
// Save brain state
brain.save("my_checkpoint.nml");

// Load brain state
if (brain.load("my_checkpoint.nml")) {
    // Brain restored to checkpoint
}

// Advanced checkpointing in config
enableCheckpointRestore = true
enableAutoCheckpoint = true
checkpointInterval = 10.0
```

### Experimental Features

```json
{
    "nlm_config": {
        "enable_experimental_features": true,
        "enable_data_collection": true,
        "enable_statistical_analysis": true,
        "experiment_results_dir": "experiments/",
        "prediction_error_signaling": true,
        "enable_metaplasticity": true,
        "enable_curiosity_module": true,
        "enable_planning_module": true
    }
}
```

### Memory Systems

```cpp
// Access memory systems
brain.getWorkingMemory();
brain.getEpisodicMemory();
brain.getAssociativeMemory();

// Configure memory capacity
episodicCapacity = 5000
memoryConsolidationInterval = 1.0
```

### Development System

```cpp
// Control developmental stages
brain.setDevelopmentalStage(DevelopmentalStage::Juvenile);
brain.develop();  // Trigger development

// Configure development parameters
maturationRate = 0.02
maturationPhases = 8
synaptogenesisRate = 0.02
pruningIntensity = 0.01
```

## Troubleshooting

### Common Issues

**"Brain isn't doing anything"**
- **Check**: Did you call `brain.initialize()`?
- **Try**: Running more simulation steps
- **Solution**: Check configuration values for neuron count and connection probability

**"The agent isn't moving"**
- **Check**: Did you call `world.applyMotorCommand()`?
- **Check**: Is `brain.step()` being called?
- **Solution**: Verify the simulation loop is complete

**"Everything is 0"**
- **Solution**: Brains need time to "warm up" - try more steps
- **Check**: Ensure sensory input is connected

### Error Messages

The improved error handling provides:
- Detailed error messages with context
- Validation of configuration values
- Warnings for suboptimal settings
- Clear guidance for resolution

### Configuration Validation

The configuration system now validates:
- Numerical ranges (e.g., connection probability: 0.0-1.0)
- Minimum requirements (e.g., neuron count: 100-10000)
- Logical consistency (e.g., timestep > 0)
- File accessibility and permissions

## Advanced Examples

### Example 1: Complete Agent with Learning

```cpp
#include "pynlm.h"

int main() {
    // Setup
    auto config = pynlm::createDefaultConfig();
    auto brain = pynlm::createBrain(config);
    brain->initialize();
    
    auto world = pynlm::createSimpleWorld();
    world->configure(15, 15, 8, 8);
    world->reset();
    
    auto agent = pynlm::createAgentBrain(brain);
    agent->initialize(world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    
    // Run agent
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    return 0;
}
```

### Example 2: Performance Testing

```bash
# Terminal commands
nlm --benchmark --neuron-count 5000 --connection-prob 0.15 > benchmark_results.txt 2>&1

# With custom config
nlm --config benchmark.cfg --output benchmark_data.nml
```

```cpp
// In benchmark mode
brain->enablePerformanceProfiling(true);
brain->enableTimingProfile(true);
brain->enableMemoryProfile(true);

// Run benchmark
for (int step = 0; step < 10000; ++step) {
    brain->step(step);
}

// Generate performance report
brain->generatePerformanceReport();
```

## Development Tools

### Debugging Tools

```bash
# Debug mode enabled in config
debugMode = true
stepByStepMode = true
validationChecks = true
```

### Profiling Tools

```bash
# Performance profiling
perfOutputFile = "my_perf.prof"
perfSamplingInterval = 0.1
maxPerfEvents = 100000
```

### Validation Tools

```bash
# Enable validation
enableValidationChecks = true
validationBatchSize = 100
debugLogFile = "validation.log"
errorLogFile = "validation_errors.log"
```

## Migration Guide

### Upgrading from Phase 1 to Phase 2

1. **Configuration**: Phase 2 now supports JSON/YAML configuration in addition to key=value format
2. **CLI**: Added comprehensive command line argument parsing
3. **Error Handling**: Detailed error messages and validation
4. **Constants**: Eliminated magic numbers with well-defined constants
5..“features: advanced

## Next Steps

When you're comfortable with NLM:

1. **Experiment with different configurations** - Try various neuron counts, connection probabilities, and plasticity settings

2. **Set up a research environment** - Use the advanced configuration for experimental work

3. **Create custom worlds** - Build complex environments beyond GridWorld

4. **Implement custom neural dynamics** - Extend beyond LIF neurons using the plugin system

5. **Study cognitive architectures** - Use the planning and concept formation modules

6. **Benchmark and optimize** - Use performance profiling for research

### Getting Started Resources

- **README.md** - Basic installation and usage
- **HOW_TO_USE.md** - Step-by-step tutorials
- **easy_usage.md** - Simple examples for beginners
- **docs/ARCHITECTURE.md** - Deep dive into NLM architecture
- **configs/** - Various configuration file examples
- **examples.json** - Advanced configuration examples

### Community Resources

- **GitHub**: https://github.com/nlm
- **Documentation**: https://docs.nlm.ai
- **Discord**: https://discord.nlm.ai
- **Discussions**: https://discussions.nlm.ai

### Troubleshooting Forum

Visit our community forums for:
- Configuration issues
- Performance problems
- Implementation questions
- Feature requests

## Code Quality Improvements

### Before (Phase 1 Issues)

- Hardcoded values throughout the code (50.0f, 30.0f, 1000 steps)
- Limited CLI options (`--config` only)
- Placeholder values in `saveToFile()`
- Simple key=value parser only
- Limited error handling
- Code duplication in test functions

### After (Phase 2 Improvements)

- Constants-based configuration (eliminates magic numbers)
- Comprehensive CLI with subcommands and validation
- JSON/YAML parser support
- Real value saving in configuration files
- Advanced error handling with detailed messages
- Modular, extensible architecture
- Performance profiling and monitoring
- Advanced features for power users
- Comprehensive documentation and examples

### Key Design Improvements

1. **Separation of Concerns**: Clear separation between core neural computation and configuration management

2. **Extensible Architecture**: Plugin system and modular design for future extensions

3. **Performance Focus**: Built-in profiling and optimization capabilities

4. **User Experience**: Comprehensive help system and intuitive interfaces

5. **Robustness**: Advanced error handling and validation

6. **Documentation**: Extensive guides and examples for all user levels

## Technical Specifications

### System Requirements

- **Operating System**: Linux, macOS, Windows
- **Compiler**: C++17 or later
- **Memory**: 4GB minimum (8GB+ for large simulations)
- **Storage**: 100MB for installation, 1GB+ for large simulations

### Performance Characteristics

- **Simulation Speed**: Depends on neuron count and network complexity
- **Scalability**: Supports up to 100,000 neurons with optimized settings
- **Memory Usage**: Linear scaling with neuron count
- **Precision**: Single-precision floating point for performance

### Supported Platforms

- **Desktop**: Linux, macOS, Windows
- **Mobile**: Android (limited features)
- **Cloud**: Docker containers available
- **Embedded**: ARM architecture support

## Legal and License

NLM is distributed under the MIT License. See LICENSE file for details.

For commercial licensing, contact licensing@nlm.ai.

## Contact and Support

**General Support**: support@nlm.ai
**Technical Support**: techsupport@nlm.ai
**Bug Reports**: bugs@nlm.ai
**Feature Requests**: features@nlm.ai

### Website

https://nlm.ai

### GitHub Repository

https://github.com/nlm/NLM

### Documentation

https://docs.nlm.ai

## Version History

### Version 2.0.0 (Phase 2)

- **Major**: Added JSON/YAML configuration support
- **Major**: Comprehensive CLI interface
- **Major**: Advanced features for power users
- **Minor**: Improved error handling and validation
- **Minor**: Better logging and debugging capabilities
- **Minor**: Modular architecture improvements

### Version History Summary

- **v1.0.0**: Initial release
- **v1.1.0**: Basic improvements
- **v1.2.0**: Enhanced configuration
- **v1.3.0**: Bug fixes and stability
- **v2.0.0**: Complete rewrite with advanced features

## Conclusion

NLM has evolved from a basic neural simulator to a sophisticated, production-ready brain simulation platform. The improvements include:

1. **Modern Configuration Management**: JSON/YAML support, constants, validation
2. **Power-User Features**: Performance profiling, checkpointing, advanced plasticity
3. **Developer Tools**: Comprehensive CLI, debugging, profiling capabilities
4. **Production Ready**: Error handling, validation, documentation
5. **Extensible Architecture**: Plugin system, modular design for future growth

Whether you're a beginner getting started with neural simulations or an advanced researcher needing sophisticated analysis tools, NLM provides the features you need. The comprehensive documentation and examples make it easy to get started, while the advanced features empower you to push the boundaries of what's possible.

**Get started today with NLM and discover the future of neural computation!**