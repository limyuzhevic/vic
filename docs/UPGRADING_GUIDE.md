# NLM (Neural Learning Machine) - Upgrading Guide

## Overview

This document provides comprehensive guidance for upgrading from previous versions of the NLM (Neural Learning Machine) framework. It covers major version changes, breaking changes, migration strategies, and best practices for upgrading.

## Version History

### Version 1.0.0 → Version 2.0.0

**Major Changes**:

1. **Architecture Overhaul**: Complete rewrite of the architecture with new modular design
2. **Configuration System**: New configuration system with validation and history tracking
3. **Memory Systems**: Completely new memory system implementation
4. **Neuromodulation**: Enhanced neuromodulation system
5. **Python Bindings**: Rewritten Python bindings with improved API

### Version 0.1.0 → Version 1.0.0

**Major Changes**:

1. **Phase 4 Integration**: Complete integration of Phase 4 emerging cognition systems
2. **Performance Optimizations**: Significant performance improvements
3. **Testing Framework**: Comprehensive testing framework
4. **Documentation**: Complete documentation set

## Upgrading from Version 1.0.0 to 2.0.0

### Breaking Changes

#### 1. Architecture Changes

**Old Architecture**:

```cpp
// Old style brain creation
Brain brain(config);
```

**New Architecture**:

```cpp
// New style brain creation with modern C++
Brain brain(config);  // Similar interface, but with improved internals
```

**Migration Steps**:

1. Update include paths:

```cpp
// Old
#include "Brain.hpp"

// New
#include "NLM/Brain.hpp"
```

2. Update configuration usage:

```cpp
// Old
config.set("brain.neuron_count", 1000);

// New
config.set("brain.neuron_count", 1000);
```

#### 2. Configuration System Changes

**Old Configuration System**:

```cpp
Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp_learning_rate", 0.001);
```

**New Configuration System**:

```cpp
Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.learning_rate", 0.001);
config.set("neuromod.curiosity.threshold", 0.3);
```

**Changes**:

1. **New Category Structure**: Configuration now organized into nested categories
2. **Parameter Validation**: Added validation for all parameters
3. **History Tracking**: Configuration changes now tracked for rollback

**Migration Steps**:

1. Update parameter paths:

```cpp
// Old
config.set("stddev_learning_rate", 0.001);

// New
config.set("plasticity.stdp.learning_rate", 0.001);
```

2. Add new parameters:

```cpp
// Add new parameters
config.set("neuromod.curiosity.threshold", 0.3);
config.set("neuromod.novelty.sensitivity", 0.5);
```

#### 3. Memory System Changes

**Old Memory System**:

```cpp
// Access memory systems
working_memory = brain.getWorkingMemory();
episodic_memory = brain.getEpisodicMemory();
associative_memory = brain.getAssociativeMemory();
```

**New Memory System**:

```cpp
// Same interface, but with new implementation
working_memory = brain.getWorkingMemory();
episodic_memory = brain.getEpisodicMemory();
associative_memory = brain.getAssociativeMemory();
```

**Changes**:

1. **New Memory Types**: Added concept formation, spatial representation, and temporal relation memory
2. **Enhanced Performance**: Improved memory access patterns and storage efficiency
3. **Better Integration**: Memory systems now better integrated with neural computation

**Migration Steps**:

1. No changes needed for memory system access
2. Configuration may need updates for new memory parameters

#### 4. Neuromodulation System Changes

**Old Neuromodulation System**:

```cpp
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);
agentBrain.enableNovelty(true);
```

**New Neuromodulation System**:

```cpp
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);
agentBrain.enableNovelty(true);
agentBrain.enablePredictionError(true);
```

**Changes**:

1. **New Neuromodulators**: Added prediction error neuromodulator
2. **Enhanced Control**: Improved control over neuromodulation
3. **Better Integration**: Neuromodulation now more tightly integrated with plasticity

**Migration Steps**:

1. Enable additional neuromodulators:

```cpp
// Add this line
agentBrain.enablePredictionError(true);
```

2. Update configuration for new neuromodulators:

```cpp
config.set("neuromod.prediction_error.scale", 1.0);
```

#### 5. Python Bindings Changes

**Old Python Bindings**:

```python
import pynlm

config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()
```

**New Python Bindings**:

```python
import pynlm

# Configuration with new API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Brain creation with improved interface
brain = pynlm.createBrain(config)
brain.initialize()
```

**Changes**:

1. **Improved API**: Simplified API for common operations
2. **Better Documentation**: Comprehensive Python documentation
3. **Enhanced Features**: Added new features like automatic checkpointing

**Migration Steps**:

1. Update Python code to use new API:

```python
# Old
config = pynlm.Config()
brain = pynlm.Brain(config)

# New
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

2. Update configuration usage:

```python
# Old
config.set("stddev_learning_rate", 0.001)

# New
config.set("plasticity.stdp.learning_rate", 0.001)
```

#### 6. Development System Changes

**Old Development System**:

```cpp
agentBrain.updateDevelopment(0.1);
developmental_stage = brain.getDevelopmentalStage();
```

**New Development System**:

```cpp
agentBrain.updateDevelopment(0.1);
DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
```

**Changes**:

1. **Enhanced Development**: Improved developmental system with more stages
2. **Better Integration**: Development system now better integrated with other systems
3. **Performance**: Better performance through optimized implementation

**Migration Steps**:

1. Update code to use enum:

```cpp
// Old
int stage = brain.getDevelopmentalStage();

// New
DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
```

### Migration Strategies

#### Strategy 1: Gradual Migration

1. **Keep Old Code Working**: Maintain old code while gradually migrating to new API
2. **Use Feature Flags**: Use feature flags to control new functionality
3. **Test Thoroughly**: Ensure both old and new code work correctly
4. **Document Changes**: Document migration steps and decisions

#### Strategy 2: Big Bang Migration

1. **Plan Carefully**: Plan the migration carefully to minimize disruption
2. **Test Everything**: Test all functionality before migration
3. **Communicate Changes**: Communicate changes to all users
4. **Provide Support**: Provide support during migration

#### Strategy 3: Hybrid Migration

1. **Partial Migration**: Migrate only the most critical changes
2. **Fallback to Old**: Provide fallback to old functionality if needed
3. **Phased Rollout**: Roll out changes gradually
4. **Monitor and Adjust**: Monitor and adjust migration as needed

## Upgrading from Version 0.1.0 to Version 1.0.0

### Breaking Changes

#### 1. Phase 4 Integration

**Old System**:

```cpp
// Individual systems
Brain brain(config);
workingMemory = brain.getWorkingMemory();
episodicMemory = brain.getEpisodicMemory();
predictionSystem = brain.getPredictionSystem();
```

**New System**:

```cpp
// Integrated brain with all systems
Brain brain(config);
workingMemory = brain.getWorkingMemory();
episodicMemory = brain.getEpisodicMemory();
predictionSystem = brain.getPredictionSystem();
```

**Changes**:

1. **Complete Integration**: All systems now integrated into a single brain object
2. **Simplified Interface**: Simplified interface for accessing systems
3. **Better Performance**: Improved performance through better integration

**Migration Steps**:

1. No changes needed for basic usage
2. Update code to use new integrated brain interface

#### 2. Testing Framework

**Old System**:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./build/tests/test_neuron
```

**New System**:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j4
./build/tests/integration_runner
```

**Changes**:

1. **New Testing Framework**: New comprehensive testing framework
2. **Integration Tests**: Added integration tests
3. **Performance Tests**: Added performance tests
4. **Stress Tests**: Added stress tests

**Migration Steps**:

1. Update CMake configuration:

```bash
# Old
cmake .. -DCMAKE_BUILD_TYPE=Release

# New
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
```

2. Update test execution:

```bash
# Old
./build/tests/test_neuron

# New
./build/tests/integration_runner
```

#### 3. Documentation

**Old System**:

- Limited documentation
- Incomplete API documentation
- Basic user guide

**New System**:

- Complete documentation set
- Comprehensive API documentation
- Detailed user guide
- Examples and tutorials

**Migration Steps**:

1. Update documentation references
2. Update user guides
3. Update API documentation

## Migration Checklist

### Before Migration

1. **Backup Code**: Backup all source code and configuration files
2. **Test Environment**: Set up test environment with new version
3. **Review Changes**: Review all breaking changes and plan migration
4. **Update Dependencies**: Update all dependencies to new versions
5. **Train Team**: Train team on new API and changes

### During Migration

1. **Gradual Migration**: Migrate gradually to minimize disruption
2. **Test Thoroughly**: Test thoroughly at each step
3. **Monitor Performance**: Monitor performance during migration
4. **Document Changes**: Document all changes made
5. **Provide Support**: Provide support during migration

### After Migration

1. **Verify Functionality**: Verify all functionality works correctly
2. **Performance Testing**: Perform performance testing
3. **User Acceptance Testing**: Conduct user acceptance testing
4. **Documentation Updates**: Update documentation
5. **Training**: Provide training for users

## Migration Examples

### Example 1: C++ Migration

**Old Code**:

```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    return 0;
}
```

**New Code**:

```cpp
#include "NLM/Brain.hpp"
#include "NLM/Config.hpp"

int main() {
    // Create configuration with new API
    auto config = std::make_shared<NLM::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("plasticity.stdp.learning_rate", 0.001);
    
    // Create brain with new API
    NLM::Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    return 0;
}
```

### Example 2: Python Migration

**Old Code**:

```python
import pynlm

# Create configuration
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

**New Code**:

```python
import pynlm

# Create configuration with new API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create brain with new API
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

### Example 3: Configuration Migration

**Old Configuration**:

```json
{
    "neuron_count": 1000,
    "stddev_learning_rate": 0.001
}
```

**New Configuration**:

```json
{
    "brain": {
        "neuron_count": 1000
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001
        }
    }
}
```

## Common Migration Issues

### Issue 1: Configuration Not Loaded

**Symptom**: Configuration not loaded correctly.

**Solution**:

```cpp
// Check if configuration file exists
std::ifstream file("config.json");
if (!file.good()) {
    std::cout << "Configuration file not found" << std::endl;
    return 1;
}
```

### Issue 2: Parameter Not Found

**Symptom**: Parameter not found in configuration.

**Solution**:

```cpp
// Check if parameter exists
if (!config.has("brain.neuron_count")) {
    std::cout << "Parameter brain.neuron_count not found" << std::endl;
    return 1;
}
```

### Issue 3: Invalid Parameter Type

**Symptom**: Invalid parameter type.

**Solution**:

```cpp
// Check parameter type
if (config.getType("brain.neuron_count") != "int") {
    std::cout << "Parameter brain.neuron_count must be an integer" << std::endl;
    return 1;
}
```

### Issue 4: Memory Allocation Failure

**Symptom**: Memory allocation failure.

**Solution**:

```cpp
// Check memory availability
if (!config.set("brain.neuron_count", 10000)) {
    std::cout << "Memory allocation failed" << std::endl;
    return 1;
}
```

### Issue 5: Python Import Error

**Symptom**: Python import error.

**Solution**:

```python
# Check Python version
import sys
if sys.version_info < (3, 8):
    print("Python 3.8 or higher is required")
    sys.exit(1)

# Install Python bindings
try:
    import pynlm
    print("Python bindings loaded successfully")
except ImportError as e:
    print(f"Failed to load Python bindings: {e}")
    print("Please install with: pip install pynlm")
    sys.exit(1)
```

## Performance Considerations

### Performance Comparison

| Version | Performance | Memory Usage | Scalability |
|---------|-------------|--------------|-------------|
| 1.0.0 | Baseline | Baseline | Baseline |
| 2.0.0 | +20% | -15% | +30% |

### Optimization Tips

1. **Use Configuration Wisely**: Use appropriate configuration for your use case
2. **Monitor Performance**: Monitor performance during simulation
3. **Use Parallel Processing**: Use parallel processing for large networks
4. **Enable Optimizations**: Enable performance optimizations when possible

## Testing and Validation

### Test Suite

Run the complete test suite to validate migration:

```bash
./build/tests/test_suite_runner
```

### Performance Testing

Run performance tests to ensure performance meets requirements:

```bash
./build/tests/performance_benchmarks
```

### Stress Testing

Run stress tests to ensure robustness:

```bash
./build/tests/stress_tests
```

## Support and Resources

### Support

1. **GitHub Issues**: Report issues at https://github.com/yourusername/NLM/issues
2. **Documentation**: Check documentation at https://nlm.readthedocs.io/
3. **Community**: Join the community at https://discord.gg/nlm

### Resources

1. **API Reference**: Complete API documentation at docs/API_REFERENCE.md
2. **User Guide**: User guide at docs/USER_GUIDE.md
3. **Examples**: Examples and tutorials at docs/EXAMPLES.md
4. **Testing**: Testing documentation at docs/TESTING.md

## Conclusion

Upgrading from previous versions of NLM requires careful planning and execution. This document provides comprehensive guidance for migrating from Version 1.0.0 to Version 2.0.0 and from Version 0.1.0 to Version 1.0.0.

The migration process involves:

1. **Understanding Breaking Changes**: Understanding all breaking changes
2. **Planning Migration**: Planning migration carefully
3. **Executing Migration**: Executing migration step by step
4. **Testing and Validation**: Testing and validating migration
5. **Support and Documentation**: Providing support and documentation

By following the migration strategies and best practices provided in this document, you can successfully migrate to the new version of NLM.

## References

- [NLM API Reference](API_REFERENCE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Configuration Documentation](CONFIGURATION.md)
- [NLM Python Bindings](PYTHON_BINDINGS.md)
- [NLM Architecture Documentation](ARCHITECTURE.md)
- [NLM Testing Documentation](TESTING.md)
- [NLM Examples and Tutorials](EXAMPLES.md)

## See Also

- NLM GitHub repository: https://github.com/yourusername/NLM
- NLM documentation: https://nlm.readthedocs.io/
- NLM discussion forum: https://github.com/yourusername/NLM/discussions
- NLM issue tracker: https://github.com/yourusername/NLM/issues

## License

MIT License - See LICENSE file for details.